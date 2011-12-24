#include "server_socket.h" // Header file for server_socket
#include "packet.h" // Header file for packet class
#include "bernoulli_trial.h" // Header file for BernoulliTrial class
#include <sys/time.h>
#include <cerrno>
#include <pthread.h>
#include "mutex_timer.h"

#define PACKET_LOSS_PROBABILITY 0
#define ACK_LOSS_PROBABILITY 0

#define PACKET_TIMEOUT 2000
#define TIMEOUT_COUNT 8

using std::cout;
using std::cerr;
using std::endl;
using std::make_pair;

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

	//@amr: if port number is < 2000, it will be valid, OK?
	valid &= (port_no > 0 && port_no < (1<<16));

	if(connection_type == 'T'){
		this->connection_type = SOCK_STREAM;
	} else if(connection_type == 'U'){
		this->connection_type = SOCK_DGRAM;
	} else{
		valid = false;
	}

	this->bufferSize = buffer_size;
	if(buffer_size > maxBufferSize){
		cerr << "Error! BufferSize is too large! Maximum buffer size = " << maxBufferSize << endl;
		valid = false;
	} else if(buffer_size <= 0){
		cerr << "Error! BufferSize is <= 0" << endl;
		valid = false;
	}

	this->queueSize = queueSize;
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

ServerSocket::ServerSocket(){}

void ServerSocket::closeConnection(int port,unsigned long ip){
    pthread_mutex_t * thread_mutex = mutex_map[make_pair(port,ip)];
    delete(thread_mutex);
    mutex_map.erase(make_pair(port,ip));

    pthread_mutex_t * ack_mutex = ack_mutex_map[make_pair(port,ip)];
    delete(ack_mutex);
    mutex_map.erase(make_pair(port,ip));

    send_sync_map.erase(make_pair(port,ip));
    receive_sync_map.erase(make_pair(port,ip));

    char * buffer = buffers_map[make_pair(port,ip)];
    delete(buffer);
    buffers_map.erase(make_pair(port,ip));

    buffers_lengths_map.erase(make_pair(port,ip));
}

bool ServerSocket::isRunning(){
	return running;
}

int ServerSocket::getBufferSize(){
	return this->bufferSize;
}

int ServerSocket::getQueueSize(){
	return this->queueSize;
}

int ServerSocket::getConnectionType(){
	return this->connection_type;
}

int ServerSocket::read_tcp(char buffer[], int client_file_descriptor){
	memset(buffer, 0, bufferSize);
	return read(client_file_descriptor, buffer, bufferSize);
}

bool ServerSocket::writeToSocket(char buffer[], void *args){
	return writeToSocket(buffer, strlen(buffer) + 1, args);
}

bool ServerSocket::writeToSocket(char buffer[], int size, void *args){
	bool success = true;

	if(buffer == NULL || args == NULL){
		cerr << "Null Argument Exception in writeToScoekt" << endl;
		success = false;
	}

	void **ar = (void **) args;
	int client_file_descriptor = *((int *) ar[1]);

	if(this->connection_type == SOCK_STREAM){
		int data_size = write(client_file_descriptor, buffer, size);
		success &= (data_size >= 0);
	} else if(this->connection_type == SOCK_DGRAM){
        //unreliable send
		//sockaddr_in *client_address = (sockaddr_in *) ar[3];
		//int data_size = sendto(socket_file_descriptor, buffer, size, 0, (struct sockaddr *) client_address, sizeof(*client_address));
		//success &= (data_size >= 0);
		sockaddr_in *client_address = (sockaddr_in *) ar[3];
        success &= (reliableUdpSend(buffer,size,client_address) > 0);
	}

	if(!success){
		cerr << "Error writing to socket" << endl;
	}

	return success;
}

int ServerSocket::readFromSocket(char * buffer,int buffsize,void * args){
    int ret = -1;
	void **ar = (void **) args;
	int client_file_descriptor = *((int *) ar[1]);
    if (this->connection_type == SOCK_STREAM){
        ret = read(client_file_descriptor,buffer,buffsize);
    }else{
        sockaddr_in * client_address = (sockaddr_in *) ar[3];
        int port = client_address->sin_port;
        unsigned long ip = client_address->sin_addr.s_addr;
        pthread_mutex_t * mutex = mutex_map[make_pair(port,ip)];
        pthread_mutex_lock(mutex);
        char * result = buffers_map[make_pair(port,ip)];
        int len = buffers_lengths_map[make_pair(port,ip)];
        memcpy(buffer,result,len);
        ret = len;
    }
    return ret;
}

void ServerSocket::handleTCPRequest(void *args){
	void **ar = (void **) args;
	int socket_new_file_descriptor = *((int *) ar[1]);
	char buffer[this->bufferSize]; //local variable for each thread, No syncronization needed

    void *process_args[4];
    process_args[0] = (void*) (ar[0]); //pointer to the ServerSocket object (this)
    process_args[1] = (void*) (ar[1]); // socket_file descriptor
    process_args[3] = (void*) (ar[2]); //client_socket_address (originally obtained from the accept() method)
    while (1){
        // block wait till the client sends its message
    	memset(buffer, 0, sizeof(buffer));
        int rc = read_tcp(buffer, socket_new_file_descriptor);

        if(rc == 0){
            cerr << "client closed connection" << endl;
            break;
        }else if (rc < 0){
            cerr << "error reading from tcp connection" << endl;
            break;
        }
        process_args[2] = (void*) buffer;
        // handle request
        if(!(*process)(process_args)){
            //server process method chose not to keep the connection
            cerr <<"closing connection" << endl;
            break;
        }
    }

	//free the arguements for this request, as it is terminating...
    delete((int *)ar[1]);
    delete((sockaddr_in*)ar[2]);
	delete(ar);

    //close the socket when tcp session finished
	close(socket_new_file_descriptor);
}

// Sets the server for TCP connection.
bool ServerSocket::handleTcpConnection() {
	sockaddr_in client_address;
	listen(socket_file_descriptor, queueSize);
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

		void **args = new void*[3];
        sockaddr_in * client_address_p = (sockaddr_in *) malloc(sizeof(sockaddr_in));
        *client_address_p = client_address;
		args[0] = (void*)this;
		args[1] = (void *) new int(socket_new_file_descriptor);
		args[2] = (void *) client_address_p;

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

    sockaddr_in * client_address = (sockaddr_in *) ar[3];
    int port = client_address->sin_port;
    unsigned long ip = client_address->sin_addr.s_addr;
    pthread_mutex_t * mutex = mutex_map[make_pair(port,ip)];
    sockaddr_in *cl_copy = (sockaddr_in *) malloc(sizeof(sockaddr_in));
    *cl_copy = *client_address ;
    //load the localBuffer
    char * localBuffer = buffers_map[make_pair(port,ip)];
    while (true){
        //wait till some one signals the lock
        pthread_mutex_lock(mutex);
        
        ar[2] = (void *) localBuffer;
        ar[3] = (void *) cl_copy;
        // handle request
        bool keepConnection = (*process)(ar);
        if (!keepConnection)
            break;
    }
    cout << "CONNECTION CLOSED" << endl;

    //TODO:
    //free args
    closeConnection(cl_copy->sin_port,cl_copy->sin_addr.s_addr);

    delete((int *)ar[1]);
    delete(cl_copy);
    delete(ar);
}

// Sets the server for UDP connection.
bool ServerSocket::handleUDPConnection() {
	sockaddr_in client_address;
	socklen_t client_len = sizeof(sockaddr_in);
	char data_buffer[bufferSize];
	int data_size;
	cerr << "Waiting for UDP requests" << endl;
    BernoulliTrial bernoulli(ACK_LOSS_PROBABILITY);

	while (1) {
		// Initialize data_buffer to zero.
		//pthread_mutex_lock(&buf_udp_mutex);
		memset(data_buffer, 0, bufferSize);

		// Reads kBufferSize - 1 bytes into the data_buffer.
		// Updates the client_address with the senders address.
		data_size = recvfrom(socket_file_descriptor, data_buffer, bufferSize-1, 0, (sockaddr *) &client_address, &client_len);
        cout << "-------------------------------------------------" << endl;
		if (data_size < 0) {
			cerr << "Unable to receive data!" << endl;
			return false;
		}
        int port = client_address.sin_port;
        unsigned long ip = client_address.sin_addr.s_addr;
        Packet packet(data_buffer,data_size);
        cout << "SOCK : " << socket_file_descriptor << " GOT new packet" << endl;
        if (packet.isAck())
            cout << "<---- PACKET : " << packet.getSyncBit() << " Received -- ACK --" << endl;
        else
            cout << "<---- PACKET : " << packet.getSyncBit() << " Received" << endl;

        //check if this a new client or an old client
        if (mutex_map.count(make_pair(port,ip)) == 0){
            //new client
            cout << "NEW CLIENT (port,ip) " << port << " , "  << ip << endl;
            if (packet.isAck() || packet.isDisconnect()){
                //corrupt packet since receiver don't get ACK packets or
                //(disconnect packet for the first time)
                //send ACK with the last bit (1 in this case) since new client
                //is waiting for zero
                //TODO:: send the ack packet
                //int ds = sendto(socket_file_descriptor, ackPacket.getRawData(), ackPacket.getRawDataLength(),
                //       0, (struct sockaddr *) &client_address, sizeof(client_address));
                continue;
            }

            bool sync = packet.getSyncBit();
            //send ack packet
            Packet ackPacket(true,sync,false,(char*)"",0);
            if (!bernoulli.shouldDoIt()){
                sendto(socket_file_descriptor, ackPacket.getRawData(), ackPacket.getRawDataLength(),
                        0, (struct sockaddr *) &client_address, sizeof(client_address));
                cout << "----> PACKET : Ack " << ackPacket.getSyncBit() << " message Sent" << endl;
            }else{
                cout << "-||-> PACKET : Ack " << ackPacket.getSyncBit() << " message Simulated Loss" << endl;
            }

            if (sync == true){
                //unsynchronized packet .. new client is expecting a 0 sync
                //packet so discard packet
                cout << "----- PACKET : unsynced , discarded" << endl;
                continue;
            }

            //void *args[4]; //XXX:THIS LINE WAS A DISASTER
            void **args = new void*[4];
            sockaddr_in * client_address_p = (sockaddr_in *) malloc(sizeof(sockaddr_in));
            *client_address_p = client_address;
            args[0] = (void *)this;
            //XXX: this number will always be constant (since udp use one
            //socket) but kept for the sake of consistency
            args[1] = (void *) new int(socket_file_descriptor);
            //XXX: no need for this as each thread will have its buffer
            //in the buffers array but kept for consistency
            //args[2] = (void *) data_buffer;
            args[3] = (void *) client_address_p;
            
            //Create records of this thread
            //1- save record in mutex_map and init the mutex , also init ack mutex
            mutex_map[make_pair(port,ip)] = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
            pthread_mutex_init(mutex_map[make_pair(port,ip)],NULL);

            ack_mutex_map[make_pair(port,ip)] = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
            pthread_mutex_init(ack_mutex_map[make_pair(port,ip)],NULL);
            pthread_mutex_lock(ack_mutex_map[make_pair(port,ip)]);

            //2- create new buffer for the connection and copy the data to it
            char * thread_buffer = new char[udpBufferSize];
            buffers_map[make_pair(port,ip)] = thread_buffer;
            memcpy(thread_buffer, packet.getData(),packet.getDataLength());
            //3- set the length of msg
            buffers_lengths_map[make_pair(port,ip)] = packet.getDataLength();

            //4- set send sync map to 1 (last sync bit used so the first one
            //to be used is 0)
            send_sync_map[make_pair(port,ip)] = true;
            //5-start the new thread
            pthread_t thrd;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            if(pthread_create(&thrd, NULL, playThread, (void *)args)){
                cerr << "Failed to create thread!";
                return false;
            }
            pthread_attr_destroy(&attr);
        }else{
            //old client 
            cout << "OLD CLIENT (port,ip) " << port << " , "  << ip << endl;
            if (packet.isDisconnect()){
                //TODO:handle disconnect mechanism
                continue;
            }

            if (packet.isAck()){
                //copy the buffer
                char * thread_buffer = buffers_map[make_pair(port,ip)];
                memcpy(thread_buffer, packet.getRawData(),packet.getRawDataLength());
                buffers_lengths_map[make_pair(port,ip)] = packet.getRawDataLength();
                //signal the ack mutex on the old thread
                pthread_mutex_t * ack_mutex = ack_mutex_map[make_pair(port,ip)];
                pthread_mutex_unlock(ack_mutex);
            }else{
                int expectedSyncBit = !receive_sync_map[make_pair(port,ip)];
                //send ack packet
                Packet ackPacket(true,packet.getSyncBit(),false,(char*)"",0);
                if (!bernoulli.shouldDoIt()){
                    sendto(socket_file_descriptor, ackPacket.getRawData(), ackPacket.getRawDataLength(),
                            0, (struct sockaddr *) &client_address, sizeof(client_address));
                    cout << "----> PACKET : Ack " << ackPacket.getSyncBit() << " message Sent" << endl;
                }else{
                    cout << "-||-> PACKET : Ack " << ackPacket.getSyncBit() <<  " message Simulated Loss" << endl;
                }

                if (expectedSyncBit != packet.getSyncBit()){
                    //unsynchronized packet .. discard it
                    cout << "----- PACKET : unsynced , discarded expecting : " << expectedSyncBit << " got : " << packet.getSyncBit() << endl;
                    continue;
                }

                //invert the sync bit
                receive_sync_map[make_pair(port,ip)] = expectedSyncBit;

                //copy the buffer
                char * thread_buffer = buffers_map[make_pair(port,ip)];
                memcpy(thread_buffer, packet.getData(),packet.getDataLength());
                buffers_lengths_map[make_pair(port,ip)] = packet.getDataLength();
                //signal the waiting mutex on the old thread
                pthread_mutex_t * thread_mutex = mutex_map[make_pair(port,ip)];
                pthread_mutex_unlock(thread_mutex);
            }

        }
	}

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

int ServerSocket::recvfromTimeout(int socket_fd,char * buff,int bufflen,struct sockaddr * client,socklen_t* client_len,int msec){
    struct timeval t;
    t.tv_sec = msec/1000;
    t.tv_usec = msec%1000;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&t, sizeof(t)))
        return -1;
    int recv_count = recvfrom(socket_fd,buff,bufflen,0,client,client_len);
    return recv_count;
}

int ServerSocket::reliableUdpSend(char* buffer,int length,struct sockaddr_in * client_address){
    int port = client_address->sin_port;
    unsigned long ip = client_address->sin_addr.s_addr;
    //----
    //0 - create a new packet with syncbit = invertion the last one used
    //1 - send the packet (if bernoulli says ok :) )
    //2 - while not timeout (wait for the correct ACK packet)
    //3 - if timeout go to step 1 else, we are done
    //--------
    //0
    bool sync = !send_sync_map[make_pair(port,ip)];
    bool done = false;
    Packet packet(false,sync,false,buffer,length);
    int ret = -1;
    BernoulliTrial bt(PACKET_LOSS_PROBABILITY);
    int count = 0;
    while (!done && count < TIMEOUT_COUNT){
        //1
        bool packet_lost = bt.shouldDoIt();
        if (!packet_lost){
            cout << "----> PACKET : "<< sync << " sent" << endl;
            int res = sendto(socket_file_descriptor, packet.getRawData(), packet.getRawDataLength(), 0,
                    (const struct sockaddr *)client_address,sizeof(*client_address));
            //an error occurred couldn't send the packet
            if (res <= 0)
                return res;
        }else
            cout << "-||-> PACKET : "<< sync << " Simulated Loss" << endl;
            
        //2
        bool correctACK = false;
        int timeout = PACKET_TIMEOUT;
        struct timeval start,end;
        while (!correctACK){
            //wait for ACK (ack size is very small (2bytes))
            char data[10];
            unsigned int sz = sizeof(*client_address);
            gettimeofday(&start,NULL);

            int port = client_address->sin_port;
            unsigned long ip = client_address->sin_addr.s_addr;
            pthread_mutex_t * mutex = ack_mutex_map[make_pair(port,ip)];
            //start timer
            MutexTimer timer(timeout,mutex);
            pthread_mutex_lock(mutex);
            timer.stop();

            gettimeofday(&end,NULL);


            //check for timeout
            int delta = (end.tv_sec*1000 + end.tv_usec/1000) -
                (start.tv_sec*1000 + start.tv_usec/1000);
            timeout -= delta;
            //timeout
            if (timeout <= 0) 
                break;

            char * result = buffers_map[make_pair(port,ip)];
            int ackret = buffers_lengths_map[make_pair(port,ip)];
            memcpy(data,result,ackret);

            //check packet
            Packet ackPacket(data,ackret);
            if (ackPacket.isAck() && ackPacket.getSyncBit() == sync){
                //we are done
                correctACK = true;
                done = true;
                ret = packet.getDataLength();
            }
        }
        if (!correctACK){
            cout << "----- PACKET : "<< sync << " time out #" << count << endl;
            ret = -1;
        }
        count++;
    }
    send_sync_map[make_pair(port,ip)] = sync;
    return ret;
}

//=============================================================
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

bool go(void *args){
	cerr << "الحمد لله رب العالمين" << endl;
	void **ar = (void **) args;
	ServerSocket *serverSocket = (ServerSocket *) ar[0];
	char *buffer = (char *) ar[2];
	cerr << "Message received from client is: " << buffer << endl;
	int first_num, second_num;
	sscanf(buffer, "%d %d", &first_num, &second_num);
	first_num += second_num;
	char sum[serverSocket->getBufferSize()];
	sprintf(sum, "%d", first_num);
	return serverSocket->writeToSocket(sum, args);
}

/*
// example main function like the one that should start the server
int main(int argc, char** argv) {
//	ServerSocket serverSocket('T', 6060, 1024, 5, &go_tcp);
//	ServerSocket serverSocket('U', 6060, 1024, 5, &go_udp);

//	ServerSocket serverSocket('T', 6060, 1024, 5, &go);
	ServerSocket serverSocket('U', 6060, 1024, 5, &go);

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
//*/
