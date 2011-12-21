#ifndef CLIENT_SOCKET_H_
#define CLIENT_SOCKET_H_

#include <stdio.h> // For standard input and output.
#include <sys/types.h> // Defines types used for system calls. Required by socket.h
#include <sys/socket.h> // For socket definitions.
#include <netinet/in.h> // For internet domain address.
#include <string.h> // For memset.
#include <iostream> // For cout and endl.
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>

class ClientSocket{
	private:
		int server_port_num; // Server port number.
		int connection_type; // Connection type. Either TCP or UDP.
		char server_name[32];
		int socket_file_descriptor;
		struct sockaddr_in server_address;
		struct sockaddr_in sender_address;
		struct hostent *server;
		int client_id;

		bool init(char, int, char *);

        //a recvfrom method that will timeout after the given millisec
        //value , upon time out a -1 will be returned and errno will be set to
        //EAGAIN or EWOULDBLOCK
        int recvfromTimeout(int,char*,int,struct sockaddr*,socklen_t*,int millisec);

	public:
		ClientSocket(char connection_type, int server_port_number);
		ClientSocket(char connection_type, int server_port_number, char *server_name);
		ClientSocket(char connection_type);
		virtual ~ClientSocket();

        int readFromSocket(char *, int);

		int writeToSocket(char *);
        int writeToSocket(char *,int);
};

#endif /* CLIENT_SOCKET_H_ */
