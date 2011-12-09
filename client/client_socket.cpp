#include "client_socket.h" // Header file for server_socket

using std::cout;
using std::cerr;
using std::endl;

bool ClientSocket::init(char connection_type, int server_port_number, char *server_name){

    this->server_port_num = server_port_number;

    if(connection_type == 'T'){
        this->connection_type = SOCK_STREAM;
    } else if(connection_type == 'U'){
        this->connection_type = SOCK_DGRAM;
    } else{
        return false;
    }

    if(server_name){
        strcpy(this->server_name, server_name);
        if(!(server = gethostbyname(server_name))){
            cerr << "Error! Couldn't find server" << endl;
            return false;
        }
    } else{
        cerr << "Error! Null Argument Exception!" << endl;
        return false;
    }

    socket_file_descriptor = socket(AF_INET, this->connection_type, 0);
    if(socket_file_descriptor < 0){
        cerr << "Error Opening Socket" << endl;
        return false;
    }

    bzero((char*) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char*) server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(server_port_number);

    if(this->connection_type == SOCK_STREAM){
        if(connect(socket_file_descriptor, (struct sockaddr *) &server_address, sizeof(server_address)) < 0){
            cerr << "Error connecting to socket!" << endl;
            return false;
        }
    }

    return true;
}

ClientSocket::ClientSocket(char connection_type, int server_port_number){
    if(!init(connection_type, server_port_number, defaultServerName)){
        cerr << "Error Constructing ClientSocket Object" << endl;
        throw -1;
    }
}

ClientSocket::ClientSocket(char connection_type, int server_port_number, char *server_name){
    if(!init(connection_type, server_port_number, server_name)){
        cerr << "Error Constructing ClientScoket Object" << endl;
        throw -1;
    }
}

ClientSocket::ClientSocket(char connection_type){
    if(!init(connection_type, defaultPortNumber, defaultServerName)){
        cerr << "Error Constructing ClientSocket Object" << endl;
        throw -1;
    }
}

ClientSocket::~ClientSocket(){
    if(socket_file_descriptor >= 0)
        close(socket_file_descriptor);
}

int ClientSocket::readFromSocket(char buf[], int buffer_size){
    int ret=-1;
    memset(buf, 0, buffer_size);
    if(connection_type == SOCK_STREAM){
        ret = read(socket_file_descriptor, buf, buffer_size);
    } else if(connection_type == SOCK_DGRAM){
        unsigned int sz = sizeof(sender_address);
        ret = recvfrom(socket_file_descriptor, buf, buffer_size, 0, (struct sockaddr *)&sender_address, &sz);
    }

    return ret;
}

int ClientSocket::writeToSocket(char *msg){
    int ret=-1;
    if(connection_type == SOCK_STREAM){
        ret = write(socket_file_descriptor, msg, strlen(msg));
    } else if(connection_type == SOCK_DGRAM){
        ret = sendto(socket_file_descriptor, msg, strlen(msg), 0, (const struct sockaddr *)&server_address,sizeof(server_address));
    }
    return ret;
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
/*
int main(){
	int a, b;
	ClientSocket clientSocket('T', 6060);

	char buf[256] = "send file";
	char str[256];
	sprintf(str, "sending message: %s", buf);
	cerr << str << endl;
	clientSocket.writeToSocket(buf);


	FILE *fout = fopen("img_copy.jpg", "w");

	char packet[256];
	memset(packet,0,256);
	int cnt=0;
	while(clientSocket.readFromSocket(packet, 256)){
		cerr << ++cnt << endl;
		fwrite(packet, 1, ret, fout);
		memset(packet,0,256);
	}

	fclose(fout);

	cerr << "Done, thank God :)" << endl;


	return 0;
} //*/
