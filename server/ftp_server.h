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
using std::pair;

const int defaultControlPort = 7070;
const int defaultDataPort = 7071;
const int defaultControlBufferSize = 2048;
const int defaultDataBufferSize = 2048;
enum DataTransferType { UPLOAD, DOWNLOAD };

class ftp_state{
    public:
        std::string username;
        std::string current_dir;
        bool is_guest;
        bool is_connection_open;
        bool cancel_transmission;
        int clientfd;
        int port;
        unsigned long ip;
};

class FTPServer{
    private:
        int controlPortNumber;
        int dataPortNumber;
        int controlBufferSize;
        int dataBufferSize;
        std::map<pair<int,long>, ftp_state *> states;

        ServerSocket controlServerSocket;
        pthread_mutex_t states_mutex;
        ServerSocket dataServerSocket;

        void init(int, int, int, int, int, bool (*)(void*), bool (*)(void*));

    public:
        FTPServer(int control_port_no, int data_port_no, int control_buffer_size, int data_buffer_size, int queueSize, bool (*)(void*), bool (*)(void*));
        FTPServer(bool (*)(void*), bool (*)(void*));
        void cancelTransmission();
        bool getTransmitting();
        int getDataBufferSize();
        int getControlBufferSize();
        bool downloadFile(char *,int, void *);
        bool uploadFile(char *,int, void *);

        void run();
        void addState(int,unsigned long, ftp_state *);
        void removeState(int,unsigned long);
        ftp_state * getState(int,unsigned long);
};

#endif /* FTPSERVER_H_ */
