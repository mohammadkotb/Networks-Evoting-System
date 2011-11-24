
#include "client_socket.h" // Header file for server_socket

using std::cout;
using std::cerr;
using std::endl;

bool ClientSocket::init(char connection_type, int server_port_number, char *server_name){
	bool valid = true;

	this->server_port_num = server_port_number;

	if(connection_type == 'T'){
		this->connection_type = SOCK_STREAM;
	} else if(connection_type == 'U'){
		this->connection_type = SOCK_DGRAM;
	} else{
		valid = false;
	}

	if(server_name){
		strcpy(this->server_name, server_name);
		if(!(server = gethostbyname(server_name))){
			cerr << "Error! Couldn't find server" << endl;
			valid = false;
		}
	} else{
		cerr << "Error! Null Argument Exception!" << endl;
		valid = false;
	}

	socket_file_descriptor = socket(AF_INET, this->connection_type, 0);
	if(socket_file_descriptor < 0){
		cerr << "Error Opening Socket" << endl;
		valid = false;
	}

	bzero((char*) &server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	bcopy((char*) server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
	server_address.sin_port = htons(server_port_number);

	if(connection_type == SOCK_STREAM){
		if(connect(socket_file_descriptor, (struct sockaddr *) &server_address, sizeof(server_address)) < 0){
			cerr << "Error connecting to socket!" << endl;
			valid = false;
		}
	}

	return valid;
}

ClientSocket::ClientSocket(char connection_type, int server_port_number){
	if(!init(connection_type, server_port_number, defaultServerName)){
		cerr << "Error Constructing ClientSocket Object" << endl;
	}
}

ClientSocket::ClientSocket(char connection_type, int server_port_number, char *server_name){
	if(!init(connection_type, server_port_number, server_name)){
		cerr << "Error Constructing ClientScoket Object" << endl;
	}
}

ClientSocket::ClientSocket(char connection_type){
	if(!init(connection_type, defaultPortNumber, defaultServerName)){
		cerr << "Error Constructing ClientSocket Object" << endl;
	}
}

ClientSocket::~ClientSocket(){
	if(socket_file_descriptor >= 0)
		close(socket_file_descriptor);
}

bool ClientSocket::readFromSocket(char buf[], int buffer_size){
	int ret=-1;
	memset(buf, 0, buffer_size);
	if(connection_type == SOCK_STREAM){
		ret = read(socket_file_descriptor, buf, buffer_size-1);
	} else if(connection_type == SOCK_DGRAM){
		unsigned int sz = sizeof(sender_address);
	    ret = recvfrom(socket_file_descriptor, buf, buffer_size-1, 0, (struct sockaddr *)&sender_address, &sz);
	}

	return (ret>=0);
}

bool ClientSocket::writeToSocket(char *msg){
	int ret=-1;
	if(connection_type == SOCK_STREAM){
   	    ret = write(socket_file_descriptor, msg, strlen(msg));
	} else if(connection_type == SOCK_DGRAM){
		ret = sendto(socket_file_descriptor, msg, strlen(msg), 0, (const struct sockaddr *)&server_address,sizeof(server_address));
	}

	return (ret>=0);
}

// client main
/*
int main(){
	int a, b;
	ClientSocket clientSocket('U', 6060);

	a = rand()%1000;
	b = rand()%1000;

	char buf[256];
	sprintf(buf, "%d %d", a, b);
	char str[256];
	sprintf(str, "sending message: %s", buf);
	cerr << str << endl;
	clientSocket.writeToSocket(buf);
	clientSocket.readFromSocket(buf, 256);

	cerr << "Returned Message = " << buf << endl;


	return 0;
}
//*/
