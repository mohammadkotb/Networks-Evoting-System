/*
 * FTPServer.h
 *
 *  Created on: Dec 9, 2011
 *      Author: lifemaker
 */

#ifndef FTPSERVER_H_
#define FTPSERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <map>

#include "server_socket.h"

using std::cout;
using std::cerr;
using std::endl;

const int defaultControlPort = 7070;
const int defaultDataPort = 7071;
const int defaultControlBufferSize = 1<<10;
const int defaultDataBufferSize = 1<<20;
enum DataTransferType { UPLOAD, DOWNLOAD };

struct ftp_state{
	std::string username;
	std::string current_dir;
	bool is_guest;
	bool is_connection_open;
	bool cancel_transmission;
	int x;
};

class FTPServer{
	private:
		int controlPortNumber;
		int dataPortNumber;
		int controlBufferSize;
		int dataBufferSize;
		std::map<int, struct ftp_state *> states;
		ServerSocket controlServerSocket;
		pthread_mutex_t states_mutex;
		ServerSocket dataServerSocket;

		void init(int, int, int, int, int, bool (*)(void*));
//		bool downloadFile_aux(char *, void *);
//		bool uploadFile_aux(char *, void *);
		static bool processFileTransfer(void *);

	public:
		FTPServer(int control_port_no, int data_port_no, int control_buffer_size, int data_buffer_size, int queueSize, bool (*)(void*));
		FTPServer(bool (*)(void*));
		void cancelTransmission();
		bool getTransmitting();
		int getDataBufferSize();
		int getControlBufferSize();
		bool downloadFile(char *, int, void *);
		bool uploadFile(char *, int, void *);

		static void *downloadFile(void *);
		static void *uploadFile(void *);
		bool openDataConnection(char *, DataTransferType, void *);
		void run();
		void addState(int, struct ftp_state *);
		void removeState(int);
		struct ftp_state * getState(int);
};

static FTPServer *ftpServer;

#endif /* FTPSERVER_H_ */
