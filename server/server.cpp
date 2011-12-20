#include <iostream>
#include <map>
#include <vector>
#include "server_socket.h"
#include "user.h"
#include "http_get_request_parser.h"
#include "server_manager.h"
#include "ftp_server.h"
#include "stdlib.h"

using namespace std;

ServerManager server_manager;
FTPServer* ftpServer;

bool processFileTransfer(void *args){
    cout << "IN PROCESS FILE" << endl;
    void **ar = (void **) args;
    char *buffer = (char *) ar[2];
    int state_port;
    char fileName[1<<10];
    int transfer_type;
    sscanf(buffer, "%d %s %d", &state_port, fileName, &transfer_type);
    cout << "TRANS : " << buffer << endl;
    if(transfer_type == 0){ //upload
        ftpServer->uploadFile(fileName,state_port, args);
    } else if(transfer_type == 1){ //download
        ftpServer->downloadFile(fileName,state_port, args);
    }
    return false;
}

bool handle_web_request(void * args){
    //get arguments
	void **ar = (void **) args;
	ServerSocket *serverSocket = (ServerSocket *) ar[0];
	int client_file_descriptor = *((int *) ar[1]);
	char *buffer = (char *) ar[2];
    //=================================================
    cout << "===========================" << endl;
	cout << "raw data : " << buffer << endl;
    //TODO:: optimize this (copy the whole buffer into string)
    string data(buffer);
    try{
        string response;
        server_manager.handle_request(&response, data);
        std::vector<char> writable(response.size()+1);
        std::copy(response.begin(), response.end(), writable.begin());
        writable[response.size()] = '\0';
        //@Moustafa: please change to const char * instead of char *
        serverSocket->writeToSocket(&writable[0], args);
        HttpGetRequestParser request(data);
        cout << "Required file : " << request.getRequiredFileName() << endl;
        map<string,string> *m = request.getParameters();
        map<string,string>::iterator it = m->begin();
        cout << "parameters : " << endl;
        for (;it != m->end();it++)
            cout << it->first << " = " << request.getParameter(it->first) << endl;
    }catch(int e){
        cerr << "Error! Couldn't process the request correctly" << endl;
    }
    //=================================================
    //since connection is not needed any more (HTTP 1.0) then we don't need to
    //keep the connection so return false;
    return false;
}

bool handle_ftp_request(void *args){
    void **ar = (void **) args;
	ServerSocket *serverSocket = (ServerSocket *) ar[0];
    int client_fd = *((int *) ar[1]);
    char *buffer_file_name = (char *) ar[2];
    sockaddr_in * addr = (sockaddr_in *) ar[3];

    cerr << "RAW FTP REQUEST : " << buffer_file_name << endl;
    cout << "FTP port = " << addr->sin_port<< endl;
    cout << "FTP ip = " << addr->sin_addr.s_addr<< endl;

    ftp_state * state;
    state = ftpServer->getState(addr->sin_port,addr->sin_addr.s_addr);
    if (state == 0){
        state = new ftp_state();
        state->cancel_transmission = false;
        state->is_connection_open = false;
        state->current_dir = "/";
        state->is_guest = true;
        state->username = "";
        state->clientfd = client_fd;
        state->port = addr->sin_port;
        state->ip = addr->sin_addr.s_addr;
        ftpServer->addState(addr->sin_port,addr->sin_addr.s_addr, state);
    }
    string response;
    string command_data(buffer_file_name);
    bool keepConnection = server_manager.handle_ftp_command(&response, command_data,*state);
    serverSocket->writeToSocket((char*)response.c_str(), args);
    cout << response << endl;

    return keepConnection;
}

void * init_web_server(void * arg){
    ServerSocket serverSocket('U', 6060, 1024, 5, &handle_web_request);
    serverSocket.run(&serverSocket);
}
void *init_ftp_server(void * arg){
    cerr << "starting ftp server" << endl;
        ftpServer = new FTPServer(&handle_ftp_request, &processFileTransfer);
	ftpServer->run();
	delete(ftpServer);

	printf("terminated\n");
}

int main() {
    //START WEB SERVER
	pthread_t thrd;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	if(pthread_create(&thrd, NULL, init_web_server, (void *) 0)){
		cerr << "Failed to create thread!";
		pthread_exit(NULL);
	}
	pthread_attr_destroy(&attr);
    //================
    //START FTP SERVER
	pthread_t thrd2;
	pthread_attr_t attr2;
	pthread_attr_init(&attr2);
	pthread_attr_setdetachstate(&attr2, PTHREAD_CREATE_JOINABLE);

	if(pthread_create(&thrd, NULL, init_ftp_server, (void *) 0)){
		cerr << "Failed to create thread!";
		pthread_exit(NULL);
	}
	pthread_attr_destroy(&attr2);
    //==================
    //join on the two threads

	void *status;
	if (pthread_join(thrd, &status)) {
		cerr << "Error joining thread" << endl;
	}
	void *status2;
	if (pthread_join(thrd, &status2)) {
		cerr << "Error joining thread" << endl;
	}

    return 0;
}

