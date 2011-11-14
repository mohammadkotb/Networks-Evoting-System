
#include "server_socket.h" // Header file for server_socket

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;

bool ServerSocket::setupServer(){
	// Create a new socket with the socket system call.
	// Specify the socket domain (unix or internet domain) and the stream type.
	// The third parameter is set to 0 to automatically specify the best suitable protocol.
	socket_file_descriptor = socket(AF_INET, connection_type, 0);
	if(socket_file_descriptor < 0){
		cerr << "Error! Couldn't open socket for server!" << endl;
		return false;
	}

	bzero((char *) &server_address, sizeof(server_address));
	// Initialize server_address.
	server_address.sin_family = AF_INET;
	// Specify the port number. htons converts for host byte order to network byte order.
	// Differnet machines have different byte order ex: little-endian / big-endian.
	server_address.sin_port = htons(port_num);
	// IP address
	server_address.sin_addr.s_addr = INADDR_ANY;
	// sin_zero = 0.
	memset(server_address.sin_zero, 0, kSinZeroSize);
	// bind socket to address using the bind system call.
	if (bind(socket_file_descriptor, (sockaddr *) &server_address, sizeof(server_address)) < 0) {
		cerr << "Error binding socket!" << endl;
		cerr << "Is this port already in use?" << endl;
		return false;
	}

	return true;
}

bool ServerSocket::init(char connection_type, int port_no, int buffer_size, int queueSize, bool (*process_fn)(void*)) {
	cerr << "Initializing server" << endl;

	bool valid = true;

	this->port_num = port_no;
	this->process = process_fn;
	pthread_mutex_init(&buf_udp_mutex, NULL);
	//@amr: if port number is < 2000, it will be valid, OK?
	valid &= (port_no > 0 && port_no < (1<<16));

	if(connection_type == 'T'){
		this->connection_type = SOCK_STREAM;
	} else if(connection_type == 'U'){
		this->connection_type = SOCK_DGRAM;
	} else{
		valid = false;
	}

	this->kBufferSize = buffer_size;
	if(buffer_size > maxBufferSize){
		cerr << "Error! BufferSize is too large! Maximum buffer size = " << maxBufferSize << endl;
		valid = false;
	} else if(buffer_size <= 0){
		cerr << "Error! BufferSize is <= 0" << endl;
		valid = false;
	}

	this->kQueueSize = queueSize;
	if(queueSize < 0 || queueSize > 100){
		cerr << "Error! Queue Size must be in the range [0:" << maxQueueSize <<"]" << endl;
		valid = false;
	}

	if(valid){
		valid &= setupServer();
	}

	return valid;
}

ServerSocket::ServerSocket(char connection_type, int port_no, int buffer_size, int queueSize, bool (*process_fn)(void*)){
	if(!init(connection_type, port_no, buffer_size, queueSize, process_fn)){
		cerr << "Error Constructing ServerSocket Object" << endl;
	}
}

ServerSocket::ServerSocket(char connection_type, bool (*process_fn)(void*)){
	if(!init(connection_type, defaultPortNumber, defaultBufferSize, defaultQueueSize, process_fn)){
		cerr << "Error Constructing ServerSocket Object" << endl;
	}
}

bool ServerSocket::isRunning(){
	return running;
}

int ServerSocket::getBufferSize(){
	return this->kBufferSize;
}

int ServerSocket::getQueueSize(){
	return this->kQueueSize;
}

int ServerSocket::getConnectionType(){
	return this->connection_type;
}

bool ServerSocket::read_tcp(char buffer[], int client_file_descriptor){
	memset(buffer, 0, kBufferSize);
	int data_size = read(client_file_descriptor, buffer, kBufferSize - 1);
	if (data_size < 0) {
		cerr << "Error reading from socket." << endl;
		return false;
	}

	return true;
}

void ServerSocket::handleTCPRequest(void *args){
	void **ar = (void **) args;
	int socket_new_file_descriptor = *((int *) ar[1]);
	char buffer[this->kBufferSize]; //local variable for each thread, No syncronization needed

	// block wait till the client sends its message
	if(!read_tcp(buffer, socket_new_file_descriptor)){
		cerr << "Error reading from buffer in tcp connection" << endl;
		cerr << "Terminating request" << endl;
		pthread_exit(NULL);
	}

	void *process_args[3];
	process_args[0] = (void*) (ar[0]); //pointer to the ServerSocket object (this)
	process_args[1] = (void*) (ar[1]); //client_socket_address (originally obtained from the accept() method)
	process_args[2] = (void *) buffer;

	// handle request
	if(!(*process)(process_args)){
		cerr << "Error! Couldn't process the request correctly" << endl;
	}

	//TODO:
	// should the response be sent from the *process method? i think the response should be built in the server's process method
	// but sent from here

	// because we are implementing HTTTP 1.0 then we will always close the client's file descriptor after processing his ONLY request
	close(socket_new_file_descriptor);
}

// Sets the server for TCP connection.
bool ServerSocket::handleTcpConnection() {
	sockaddr_in client_address;
	listen(socket_file_descriptor, kQueueSize);
	cerr << "Listening to port: " << port_num << endl;
	socklen_t client_size = sizeof(client_address);
	int socket_new_file_descriptor;

	while (1) {
		// Block and wait for client requests. A wait a connection on socket_file_descriptor.
		// Updates the client address and client address size accordingly.
		socket_new_file_descriptor = accept(socket_file_descriptor, (sockaddr *) &client_address, &client_size);
		if (socket_file_descriptor < 0) {
			cerr << "Error! Unable to accept TCP connection" << endl;
			cerr << "Server is shutting down" << endl;
			return false;
		}

		pthread_t thrd;
		pthread_attr_t attr;
		pthread_attr_init(&attr);

		void *args[2];
		args[0] = (void*)this;
		args[1] = (void*) &socket_new_file_descriptor;

		if(pthread_create(&thrd, NULL, playThread, (void *)args)){
			cerr << "Failed to create thread!";
			pthread_exit(NULL);
		}
		pthread_attr_destroy(&attr);
	}

	return true;
}

void ServerSocket::handleUDPRequest(void *args){
	void **ar = (void **) args;
	char *globalBuffer = (char *) ar[2];
	char localBuffer[this->kBufferSize];
	strcpy(localBuffer, globalBuffer);
	pthread_mutex_unlock(&buf_udp_mutex);

	ar[2] = (void *) localBuffer;

	// handle request
	if(!(*process)(ar)){
		cerr << "Error! Couldn't process the request correctly" << endl;
	}

	//TODO:
	// should the response be sent from the *process method? i think the response should be built in the server's process method
	// but sent from here
}

// Sets the server for UDP connection.
bool ServerSocket::handleUDPConnection() {
	sockaddr_in client_address;
	socklen_t client_len = sizeof(sockaddr_in);
	char data_buffer[kBufferSize];
	int data_size;
	cerr << "Waiting for UDP requests" << endl;

	while (1) {
		// Initialize data_buffer to zero.
		pthread_mutex_lock(&buf_udp_mutex);
		memset(data_buffer, 0, kBufferSize);

		// Reads kBufferSize - 1 bytes into the data_buffer.
		// Updates the client_address with the senders address.
		data_size = recvfrom(socket_file_descriptor, data_buffer, kBufferSize-1, 0, (sockaddr *) &client_address, &client_len);
		if (data_size < 0) {
			cerr << "Unable to receive data!" << endl;
			return false;
		}

		pthread_t thrd;
		pthread_attr_t attr;
		pthread_attr_init(&attr);

		void *args[4];
		args[0] = (void *)this;
		args[1] = (void *) &socket_file_descriptor;
		args[2] = (void *) data_buffer;
		args[3] = (void *) &client_address;

		if(pthread_create(&thrd, NULL, playThread, (void *)args)){
			cerr << "Failed to create thread!";
			return false;
		}
		pthread_attr_destroy(&attr);
	}

	pthread_mutex_destroy(&buf_udp_mutex);
	return true;
}

void ServerSocket::playThread_aux(void *args){
	if(connection_type == SOCK_STREAM){
		handleTCPRequest(args);
	} else if(connection_type == SOCK_DGRAM){
		handleUDPRequest(args);
	}
}

void *  ServerSocket::playThread(void *args){
	void **ar = (void **) args;
	((ServerSocket *) ar[0])->playThread_aux(args);
	return NULL;
}

void ServerSocket::run_aux(){
	running = true;
	if(connection_type == SOCK_STREAM){
		handleTcpConnection();
	} else if(connection_type == SOCK_DGRAM){
		handleUDPConnection();
	}
}

void * ServerSocket::run(void * args){
	ServerSocket *serverSocket = (ServerSocket *) args;
	serverSocket->run_aux();
	return NULL;
}

// sample test
/*
// example function for processing tcp requests
bool go_tcp(void * args){
	cerr << "الحمد لله رب العالمين" << endl;
	void **ar = (void **) args;
	ServerSocket *serverSocket = (ServerSocket *) ar[0];
	int client_file_descriptor = *((int *) ar[1]);
	char *buffer = (char *) ar[2];
	cerr << "Message received from client is: " << buffer << endl;
	int first_num, second_num;
	sscanf(buffer, "%d %d", &first_num, &second_num);
	first_num += second_num;
	char sum[serverSocket->getBufferSize()];
	sprintf(sum, "%d", first_num);
	int data_size = write(client_file_descriptor, sum, strlen(sum));
	if (data_size < 0) {
		cerr << "Error writing to socket!";
		return false;
	}
	if(serverSocket->getConnectionType() == SOCK_STREAM) close(client_file_descriptor);
	return true;
}

// example function for processing udp requests
bool go_udp(void * args){
	cerr << "الحمد لله رب العالمين" << endl;
	void **ar = (void **) args;
	ServerSocket *serverSocket = (ServerSocket *) ar[0];
	int client_file_descriptor = *((int *) ar[1]);
	char *buffer = (char *) ar[2];
	sockaddr_in *client_address = (sockaddr_in *) ar[3];
	cerr << "Message received from client is: " << buffer << endl;
	int first_num, second_num;
	sscanf(buffer, "%d %d", &first_num, &second_num);
	first_num += second_num;
	char sum[serverSocket->getBufferSize()];
	sprintf(sum, "%d", first_num);
	int data_size = sendto(client_file_descriptor, sum, strlen(sum), 0, (struct sockaddr *) client_address, sizeof(*client_address));
	if (data_size < 0) {
		cerr << "Error writing to socket!";
		return false;
	}
	if(serverSocket->getConnectionType() == SOCK_STREAM) close(client_file_descriptor);
	return true;
}

// example main function like the one that should start the server
int main(int argc, char** argv) {
//	ServerSocket serverSocket('T', 6060, 1024, 5, &go_tcp);
	ServerSocket serverSocket('U', 6060, 1024, 5, &go_udp);

	pthread_t thrd;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	if(pthread_create(&thrd, NULL, serverSocket.run, (void *) &serverSocket)){
		cerr << "Failed to create thread!";
		pthread_exit(NULL);
	}
	pthread_attr_destroy(&attr);

	void *status;
	if (pthread_join(thrd, &status)) {
		cerr << "Error joining thread" << endl;
	}

	return 0;
}
*/
