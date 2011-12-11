#include "ftp_server.h"
#define DBG

void FTPServer::init(int control_port_no, int data_port_no, int control_buffer_size, int data_buffer_size, int queueSize, bool (*process_fn)(void*)){
	this->controlPortNumber = control_port_no;
	this->dataPortNumber = data_port_no;
	this->controlBufferSize = control_buffer_size;
	this->dataBufferSize = data_buffer_size;
	pthread_mutex_init(&states_mutex, NULL);

	this->controlServerSocket = ServerSocket('T', controlPortNumber, controlBufferSize, queueSize, process_fn);
	this->dataServerSocket = ServerSocket('T', dataPortNumber, dataBufferSize, queueSize, &processFileTransfer);
}

FTPServer::FTPServer(int control_port_no, int data_port_no, int control_buffer_size, int data_buffer_size, int queue_size, bool(*process_fn)(void*)){
	init(control_port_no, data_port_no, control_buffer_size, data_buffer_size, queue_size, process_fn);
}

FTPServer::FTPServer(bool (*process_fn)(void*)){
	init(defaultControlPort, defaultDataPort, defaultControlBufferSize, defaultDataBufferSize, defaultQueueSize, process_fn);
}

int FTPServer::getDataBufferSize(){
	return this->dataBufferSize;
}

int FTPServer::getControlBufferSize(){
	return this->controlBufferSize;
}

void FTPServer::addState(int fileDescriptor, ftp_state *state){
	pthread_mutex_lock(&states_mutex);
	states[fileDescriptor] = state;
	pthread_mutex_unlock(&states_mutex);
}

void FTPServer::removeState(int fileDescriptor){
	pthread_mutex_lock(&states_mutex);
	states.erase(fileDescriptor);
	pthread_mutex_unlock(&states_mutex);
}

struct ftp_state * FTPServer::getState(int fileDescriptor){
    if (states.find(fileDescriptor) == states.end())
        return 0;
	return states.find(fileDescriptor)->second;
}

bool FTPServer::downloadFile(char *fileName, int control_fd, void *args){
	void **ar = (void **) args;
	ServerSocket *dataServerSocket = (ServerSocket *) ar[0];
	struct ftp_state * state = this->getState(control_fd);

	state->is_connection_open = true;

	FILE *fin = fopen(fileName, "r");
	if(!fin){
		cerr << "Error! couldn't open the file: " << fileName << endl;
		state->is_connection_open = false;
		state->cancel_transmission = false;
		return false;
	}

//	int bufSz = dataServerSocket->getBufferSize();
	int bufSz = getDataBufferSize();
	char packet[bufSz];

	int n;
	while(!(state->cancel_transmission) && (n=fread(packet, 1, bufSz, fin))){
		dataServerSocket->writeToSocket(packet, n, args);
//		sleep(1);
	}

	cerr << "closing file " << fileName << endl;
	if(fclose(fin)==EOF){
		cerr << "Error! couldn't close the file: " << fileName << endl;
		state->is_connection_open = false;
		state->cancel_transmission = false;
		return false;
	}

	state->is_connection_open = false;
	state->cancel_transmission = false;
	return true;
}

bool FTPServer::uploadFile(char *fileName, int control_fd, void *args){
	void **ar = (void **) args;
	struct ftp_state * state = this->getState(control_fd);

	state->is_connection_open = true;

	int client_fd = *((int *) ar[1]);

	FILE *fout = fopen(fileName, "w");
	if(!fout){
		cerr << "Error! couldn't create the file: " << fileName << endl;
		state->is_connection_open = false;
		state->cancel_transmission = false;
		return false;
	}

	int bufSz = getDataBufferSize();
	char packet[bufSz];
	memset(packet,0,bufSz);

	int n, total=0;

	while(!(state->cancel_transmission) && ((n = read(client_fd, packet, bufSz)) > 0)){
		total+=n;
		fwrite(packet, 1, n, fout);
	}

	if(n<0){
		cerr << "Error receiving data from client!" << endl;
	}

	cerr << "Closing file " << fileName << endl;
	if(fclose(fout)==EOF){
		cerr << "Error! couldn't close the file: " << fileName << endl;
		state->is_connection_open = false;
		state->cancel_transmission = false;
		return false;
	}

	state->is_connection_open = false;
	state->cancel_transmission = false;


	cerr << "total = " << 1.0*total/1000.0 << "Kbyte" << endl;
	cerr << "File successfully uploaded, thank God :)" << endl;

	return true;

}

/*

void *FTPServer::downloadFile(void *args){
	void **ar = (void **) args;
	char *fileName = (char *) ar[2];
	ftpServer->downloadFile_aux(fileName, args);

	return NULL;
}

void *FTPServer::uploadFile(void *args){
	void **ar = (void **) args;
	char *fileName = (char *) ar[2];
	ftpServer->uploadFile_aux(fileName, args);

	return NULL;
}

bool FTPServer::openDataConnection(char *fileName, DataTransferType type, void *args){
	pthread_t thrd;

	bool fail=false;
	if(type == UPLOAD){
		fail = pthread_create(&thrd, NULL, this->uploadFile, args);
	} else if(type == DOWNLOAD){
		fail = pthread_create(&thrd, NULL, this->downloadFile, args);
	}

	if(fail){
		cerr << "Failed to create data thread!";
		return false;
	}

	return true;
}
*/

bool FTPServer::processFileTransfer(void *args){
	void **ar = (void **) args;
	char *buffer = (char *) ar[2];

	int client_control_fd;
	char fileName[1<<10];
	int transfer_type;

	sscanf(buffer, "%d %s %d", &client_control_fd, fileName, &transfer_type);

#ifdef DBG
	int client_fd = *((int *) ar[1]);
	cerr << "dbg: (" << client_fd << ", " << client_control_fd << ")" << endl;
#endif

	if(transfer_type == 0){ //upload
		ftpServer->uploadFile(fileName, client_control_fd, args);
	} else if(transfer_type == 1){ //download
		ftpServer->downloadFile(fileName, client_control_fd, args);
	}

	return false;
}


void FTPServer::run(){
	pthread_t thrd;
	pthread_create(&thrd, NULL, &(dataServerSocket.run), (void *)(&dataServerSocket));
	controlServerSocket.run((void *)(&controlServerSocket));
//	dataServerSocket.run((void *)(&dataServerSocket));
	pthread_mutex_destroy(&states_mutex);
}

//==========================================================================================================================================
//*
bool go_ftp_server(void *args){
	void **ar = (void **) args;
	ServerSocket *serverSocket = (ServerSocket *) ar[0];
	int client_fd = *((int *) ar[1]);
	char *raw_request = (char *) ar[2];

	//TODO: parse the request to determine the command type

	// temp code
	int command_type = 0;
	struct ftp_state state;
	//TODO: the state should be initialized using client commands: connect / login AND i need a message telling me whether the client is a candidate or a voter (to set the isGuest flag)
	state.cancel_transmission = false;
	state.is_connection_open = false;
	state.current_dir = "/";
	state.is_guest = true;
	state.username = "";
	//TODO: the addState should be called in response to "connect (or any other type of message)" command from the user
	ftpServer->addState(client_fd, &state);

	char response[1<<4];

	switch(command_type){
		case 0: //upload
//			ftpServer->openDataConnection(buffer_file_name, UPLOAD, args);
//			sleep(5);

			sprintf(response, "%d", client_fd);
			cerr << "dbg: sending response: " << response << endl;
			serverSocket->writeToSocket(response, args);
			break;
		case 1: //download
//			ftpServer->openDataConnection(raw_request, DOWNLOAD, args);
//			sleep(5);
			sprintf(response, "%d", client_fd);
			cerr << "dbg: sending response: " << response << endl;
			serverSocket->writeToSocket(response, args);
			break;
		case 2: //connect

			break;
		case 3: //quit
			return false; //close ftp control connection
//			break;
	}
//----------------------------------------------------------------------------------------------------------------------------
	cerr << "File requested by client is:: " << raw_request << endl;

	//TODO: we should eliminate the sleep() and put the code in a while(1) with the break condition is receiving "bye" command from client

	return true; //keep connection!
}

/*
int main(){
	cerr << "started" << endl;

	ftpServer = new FTPServer(&go_ftp_server);
	ftpServer->run();
	delete(ftpServer);

	printf("terminated\n");

	return 0;
}
*/
//*/
