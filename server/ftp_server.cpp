#include "ftp_server.h"
#define DBG

using namespace std;

//bool processFileTransfer(void *args){
//    cout << "IN PROCESS FILE" << endl;
//        void **ar = (void **) args;
//        char *buffer = (char *) ar[2];
//        int client_control_fd;
//        char fileName[1<<10];
//        int transfer_type;
//        sscanf(buffer, "%d %s %d", &client_control_fd, fileName, &transfer_type);
//        if(transfer_type == 0){ //upload
//                uploadFile(fileName, client_control_fd, args);
//        } else if(transfer_type == 1){ //download
//                downloadFile(fileName, client_control_fd, args);
//        }
//        return false;
//}

void FTPServer::init(int control_port_no, int data_port_no, int control_buffer_size, int data_buffer_size, int queueSize, bool (*process_fn)(void*), bool (*data_process)(void*)){
	this->controlPortNumber = control_port_no;
	this->dataPortNumber = data_port_no;
	this->controlBufferSize = control_buffer_size;
	this->dataBufferSize = data_buffer_size;
	pthread_mutex_init(&states_mutex, NULL);

	this->controlServerSocket = ServerSocket('T', controlPortNumber, controlBufferSize, queueSize, process_fn);
        this->dataServerSocket = ServerSocket('T', dataPortNumber, dataBufferSize, queueSize, data_process);
}

FTPServer::FTPServer(int control_port_no, int data_port_no, int control_buffer_size, int data_buffer_size, int queue_size, bool(*process_fn)(void*), bool(*process_data)(void*)){
        init(control_port_no, data_port_no, control_buffer_size, data_buffer_size, queue_size, process_fn, process_data);
}

FTPServer::FTPServer(bool (*process_fn)(void*), bool (*data_process)(void*)){
        init(defaultControlPort, defaultDataPort, defaultControlBufferSize, defaultDataBufferSize, defaultQueueSize, process_fn, data_process);
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

ftp_state * FTPServer::getState(int fileDescriptor){
    //cerr << "in getState" << endl;
    cout << "final file descriptor: " << fileDescriptor << endl;
    cout << "MAP SIZE: " << states.size() << endl;
    for(map<int, ftp_state*>::iterator it = states.begin(); it != states.end() ; ++it){
        cout << it->first << endl;
        cout << it->second->username << endl;
    }
    if (states.find(fileDescriptor) == states.end())
        return 0;
    return states.find(fileDescriptor)->second;
}

bool FTPServer::downloadFile(char *fileName, int control_fd, void *args){
    cout << "In download file" << endl;
        void **ar = (void **) args;
        cout << "HERE-2" << endl;
        ServerSocket *dataServerSocket = (ServerSocket *) ar[0];
        cout << "HERE-1" << endl;
        cout << "contorl_fd @ server = " << control_fd << endl;
        cout << "data_fd @ server = " << (*((int *) ar[1])) << endl;
        //cerr << "this.getDataBufferSize = " << ftpServer->getDataBufferSize() << endl;
        ftp_state * state = getState(control_fd);

        cout << "HERE0" << endl;

        string temp(fileName);
        string u = state->username;
        u = u.substr(1,u.size()-2);
        temp = "../ftdocs/" + u + temp;
        cout << "FILE NAME = " << temp << endl;
        fileName = (char *) temp.c_str();

        state->is_connection_open = true;

        FILE *fin = fopen(fileName, "r");
        if(!fin){
                cerr << "Error! couldn't open the file: " << fileName << endl;
                state->is_connection_open = false;
                state->cancel_transmission = false;
                return false;
        }

        cout<<"HERE1"<<endl;
//	int bufSz = dataServerSocket->getBufferSize();
        int bufSz = getDataBufferSize();
        char packet[bufSz];

        cout<<"HERE2"<<endl;

        int n;
        while(!(state->cancel_transmission) && (n=fread(packet, 1, bufSz, fin))){
                dataServerSocket->writeToSocket(packet, n, args);
//		sleep(1);
        }

        cout<<"HERE3"<<endl;

        cerr << "closing file " << fileName << endl;
        if(fclose(fin)==EOF){
                cerr << "Error! couldn't close the file: " << fileName << endl;
                state->is_connection_open = false;
                state->cancel_transmission = false;
                return false;
        }

        cout<<"HERE4"<<endl;

        state->is_connection_open = false;
        state->cancel_transmission = false;
        cout << "DONE IN DOWNLOAD FILE" << endl;
        return true;
}

bool FTPServer::uploadFile(char *fileName, int control_fd, void *args){
        void **ar = (void **) args;
        ftp_state * state = this->getState(control_fd);

        state->is_connection_open = true;
        ServerSocket* dataServerSocket = (ServerSocket *) ar[0];
        dataServerSocket->writeToSocket((char*)"dummy",args);

        string temp(fileName);
        string u = state->username;
        u = u.substr(1,u.size()-2);
        temp = "../ftdocs/" + u + temp;
        cout << "FILE NAME = " << temp << endl;
        fileName = (char *) temp.c_str();

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



void FTPServer::run(){
	pthread_t thrd;
	pthread_create(&thrd, NULL, &(dataServerSocket.run), (void *)(&dataServerSocket));
	controlServerSocket.run((void *)(&controlServerSocket));
//	dataServerSocket.run((void *)(&dataServerSocket));
	pthread_mutex_destroy(&states_mutex);
}

//==========================================================================================================================================
//*
//bool go_ftp_server(void *args){
//	void **ar = (void **) args;
//	ServerSocket *serverSocket = (ServerSocket *) ar[0];
//	int client_fd = *((int *) ar[1]);
//	char *raw_request = (char *) ar[2];

//	//TODO: parse the request to determine the command type

//	// temp code
//	int command_type = 0;
//        ftp_state state;
//	//TODO: the state should be initialized using client commands: connect / login AND i need a message telling me whether the client is a candidate or a voter (to set the isGuest flag)
//	state.cancel_transmission = false;
//	state.is_connection_open = false;
//	state.current_dir = "/";
//	state.is_guest = true;
//	state.username = "";
//	//TODO: the addState should be called in response to "connect (or any other type of message)" command from the user
//        addState(client_fd, &state);

//	char response[1<<4];

//	switch(command_type){
//		case 0: //upload
////			ftpServer->openDataConnection(buffer_file_name, UPLOAD, args);
////			sleep(5);

//			sprintf(response, "%d", client_fd);
//			serverSocket->writeToSocket(response, args);
//			break;
//		case 1: //download
////			ftpServer->openDataConnection(raw_request, DOWNLOAD, args);
////			sleep(5);
//			sprintf(response, "%d", client_fd);
//			serverSocket->writeToSocket(response, args);
//			break;
//		case 2: //connect

//			break;
//		case 3: //quit
//			return false; //close ftp control connection
////			break;
//	}
////----------------------------------------------------------------------------------------------------------------------------
//	cerr << "File requested by client is:: " << raw_request << endl;

//	//TODO: we should eliminate the sleep() and put the code in a while(1) with the break condition is receiving "bye" command from client

//	return true; //keep connection!
//}

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
