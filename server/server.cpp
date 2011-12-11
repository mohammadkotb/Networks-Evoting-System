#include <iostream>
#include <map>
#include <vector>
#include "server_socket.h"
#include "user.h"
#include "http_get_request_parser.h"
#include "server_manager.h"
#include "ftp_server.h"

using namespace std;

ServerManager server_manager;

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
    int client_fd = *((int *) ar[1]);
    char *buffer_file_name = (char *) ar[2];
    cerr << "RAW FTP REQUEST : " << buffer_file_name << endl;

    struct ftp_state state;
    //TODO: the state should be initialized using client commands: connect / login AND i need a
    //message telling me whether the client is a candidate or a voter (to set the isGuest flag)
    state.cancel_transmission = false;
    state.is_connection_open = false;
    state.current_dir = "/";
    state.is_guest = true;
    state.username = "";
    //TODO: the addState should be called in response to "connect (or any other type of message)"
    //command from the user
    ftpServer->addState(client_fd, &state);

    //TODO: the method should parse the request and respond with the proper response
    string response;
    string command_data(buffer_file_name);
    server_manager.handle_ftp_command(&response, command_data);
    cout << response << endl;
    /*
       ftpServer->openDataConnection(buffer_file_name, DOWNLOAD, args);
    */

    return true;
}

void * init_web_server(void * arg){
    ServerSocket serverSocket('T', 6060, 1024, 5, &handle_web_request);
    serverSocket.run(&serverSocket);
}
void *init_ftp_server(void * arg){
    cerr << "starting ftp server" << endl;
	ftpServer = new FTPServer(&handle_ftp_request);
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

