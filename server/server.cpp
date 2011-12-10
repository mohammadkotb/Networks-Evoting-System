#include <iostream>
#include <map>
#include <vector>
#include "server_socket.h"
#include "user.h"
#include "http_get_request_parser.h"
#include "server_manager.h"

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
        return false;
    }
    //=================================================
    //close the connection after returning the required object
	if(serverSocket->getConnectionType() == SOCK_STREAM) close(client_file_descriptor);
    return true;
}

void start_server(){
        ServerSocket serverSocket('T', 6060, 1024, 5, &handle_web_request);

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
}

int main() {
    start_server();
    return 0;
}
