/*
 * server_socket.h
 *
 *  Created on: Nov 5, 2011
 *      Author: lifemaker
 */

#ifndef SERVER_SOCKET_H_
#define SERVER_SOCKET_H_


#include <stdio.h> // For standard input and output.
#include <sys/types.h> // Defines types used for system calls. Required by socket.h
#include <sys/socket.h> // For socket definitions.
#include <netinet/in.h> // For internet domain address.
#include <string.h> // For memset.
#include <iostream> // For cout and endl.
#include <stdlib.h>
#include <time.h>
#include <map>

const int kSinZeroSize = 8; // sin_zero size
const int maxBufferSize = 1<<20; // Maximum size of the data buffer.
const int defaultBufferSize = 1024; // Default size of the data buffer.
const int defaultQueueSize = 5; // Default Queue size
const int maxQueueSize = 100; //Maximum Queue Size
const int defaultPortNumber = 80; // Default port number
const int udpBufferSize = 1024;

class ServerSocket{
	private:
		int queueSize; // Queue size
		int bufferSize; // size of the data buffer.
		int port_num; // Server port number.
		int connection_type; // Connection type. Either TCP or UDP.
		int socket_file_descriptor; // used for storing the values returned by the socket system call and the accept system call.
		sockaddr_in server_address; // Server address.
		bool running; // indicates whether this thread is running or not yet

		// Initialize server socket
		bool init(char connection_type, int port_no, int buffer_size, int queueSize, bool (*)(void*));

		// auxiliary function for initializing the server socket
		bool setupServer();

		// reads message sent by clients in case of TCP and returns number of bytes read
		int read_tcp(char *, int client_file_descriptor);

		// logic for handling a single TCP request
		void handleTCPRequest(void *);

		// logic for handling a single UPD request
		void handleUDPRequest(void *);

		// logic for handling TCP connections
		bool handleTcpConnection();

		// logic for handling UDP connections
		bool handleUDPConnection();

		// auxiliary function for running the server socket in a new thread
		void run_aux();

		// pointer to server function that process client requests
		bool (*process)(void *);

        //============================================
        //Reliable UDP variables and methods

        //udp mutex map
        std::map<std::pair<int,unsigned long>,pthread_mutex_t *> mutex_map;

        //udp ack mutex map
        std::map<std::pair<int,unsigned long>,pthread_mutex_t *> ack_mutex_map;

        //udp buffer map
        std::map<std::pair<int,unsigned long>,char *> buffers_map;

        //udp received message length map
        std::map<std::pair<int,unsigned long>,int> buffers_lengths_map;

        //udp send sync maps (sync bit for each client)
        std::map<std::pair<int,unsigned long>,bool> send_sync_map;

        //udp recieve sync maps (sync bit for each client)
        std::map<std::pair<int,unsigned long>,bool> receive_sync_map;

        int reliableUdpSend(char* buffer,int length,struct sockaddr_in * client_address);

        int recvfromTimeout(int socket_fd,char * buff,int bufflen,struct sockaddr * client,socklen_t* client_len,int msec);
	public:
		// Constructors
		ServerSocket();
		ServerSocket(char connection_type, int port_no, int buffer_size, int queueSize, bool (*)(void*));
		ServerSocket(char connection_type, bool (*)(void*));

		// Getters
		bool isRunning();
		int getBufferSize();
		int getQueueSize();
		int getConnectionType();

		//@depricated: this function only works for writing strings, so it has a more generic overload
		// writes STRINGS ONLY to socket regardless of connection type (tcp/udp)
		bool writeToSocket(char *, void *);

		int readFromSocket(char *,int, void *);

		// writes to socket regardless of connection type (tcp/udp)
		bool writeToSocket(char *, int, void *);

		// two functions needed for creating a thread for each request received
		void playThread_aux(void *);
		static void *playThread(void *);

		// runs the server (should be called after initializing the server)
		static void *run(void *);
};

#endif /* SERVER_SOCKET_H_ */
