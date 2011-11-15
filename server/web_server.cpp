#include <iostream>
#include "server_socket.h"
#include "user.h"

using namespace std;

// example function for processing tcp requests
bool go_tcp(void * args){
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

void start_server(){
	ServerSocket serverSocket('T', 6060, 1024, 5, &go_tcp);

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
    //start_server();
    User user("Ahmed Kotb", "ahmedkotb","root");
    cout << user.getName() << endl;
    cout << user.getUserName() << endl;
    cout << user.getPassword() << endl;
    return 0;
}
