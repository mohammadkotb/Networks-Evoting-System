#include "ftp_server.h"
#define DBG

using namespace std;

void FTPServer::init(int control_port_no, int data_port_no, int control_buffer_size, int data_buffer_size, int queueSize, bool (*process_fn)(void*), bool (*data_process)(void*)){
	this->controlPortNumber = control_port_no;
	this->dataPortNumber = data_port_no;
	this->controlBufferSize = control_buffer_size;
	this->dataBufferSize = data_buffer_size;
	pthread_mutex_init(&states_mutex, NULL);

	this->controlServerSocket = ServerSocket('U', controlPortNumber, controlBufferSize, queueSize, process_fn);
    this->dataServerSocket = ServerSocket('U', dataPortNumber, dataBufferSize, queueSize, data_process);
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

void FTPServer::addState(int port,unsigned long ip, ftp_state *state){
	pthread_mutex_lock(&states_mutex);
	states[make_pair(port,ip)] = state;
	pthread_mutex_unlock(&states_mutex);
}

void FTPServer::removeState(int port,unsigned long ip){
	pthread_mutex_lock(&states_mutex);
	states.erase(make_pair(port,ip));
	pthread_mutex_unlock(&states_mutex);
}

ftp_state * FTPServer::getState(int port,unsigned long ip){
    if (states.find(make_pair(port,ip)) == states.end())
        return 0;
    return states.find(make_pair(port,ip))->second;
}

bool FTPServer::downloadFile(char *fileName,int port, void *args){
        void **ar = (void **) args;
        ServerSocket *dataServerSocket = (ServerSocket *) ar[0];
        sockaddr_in * client_address = (sockaddr_in*) ar[3];
        ftp_state * state = getState(port,client_address->sin_addr.s_addr);

        string temp(fileName);
        if (!state->is_guest){
            string u = state->username;
            u = u.substr(1,u.size()-2);
            temp = "../ftdocs/" + u + temp;
        }else{
            temp = "../ftdocs" + temp ;
        }

        fileName = (char *) temp.c_str();
        cout << "FILE NAME = " << temp << endl;

        state->is_connection_open = true;

        FILE *fin = fopen(fileName, "r");
        if(!fin){
                cerr << "Error! couldn't open the file: " << fileName << endl;
                state->is_connection_open = false;
                state->cancel_transmission = false;
                return false;
        }

        int bufSz = getDataBufferSize();
        char packet[bufSz];


        int n;
        cout << "sending file ..." << endl;
        while(!(state->cancel_transmission) && (n=fread(packet, 1, bufSz, fin))){
            dataServerSocket->writeToSocket(packet, n, args);
            cout << "CHUNK" << endl;
            sleep(5);
        }
        cout << "download complete" << endl;

        if (state->cancel_transmission)
            cerr << "Transmission cancelled" << endl;

        //TODO:: remove this line
        //this just sends a dummy packet so if the udp client was blocked
        //it can continue by writing empty string to file and exiting the loop
        //as the number of written bytes will be n=0
        dataServerSocket->writeToSocket((char *)"",0, args);


        cerr << "closing file " << fileName << endl;
        if(fclose(fin)==EOF){
            cerr << "Error! couldn't close the file: " << fileName << endl;
            state->is_connection_open = false;
            state->cancel_transmission = false;
            return false;
        }


        state->is_connection_open = false;
        state->cancel_transmission = false;
        cout << "DONE IN DOWNLOAD FILE" << endl;
        return true;
}

bool FTPServer::uploadFile(char *fileName,int port, void *args){
        void **ar = (void **) args;
        
        sockaddr_in * client_address = (sockaddr_in*) ar[3];

        ftp_state * state = getState(port,client_address->sin_addr.s_addr);

        state->is_connection_open = true;
        ServerSocket* dataServerSocket = (ServerSocket *) ar[0];
        dataServerSocket->writeToSocket((char*)"dummy",args);

        string temp(fileName);
        if (!state->is_guest){
            string u = state->username;
            u = u.substr(1,u.size()-2);
            temp = "../ftdocs/" + u + temp;
        }else{
            temp = "../ftdocs" + temp ;
        }

        fileName = (char *) temp.c_str();
        cout << "FILE NAME = " << temp << endl;

        //int client_fd = *((int *) ar[1]);

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

        cout << "Waiting for file packets to come" << endl;
        while(!(state->cancel_transmission) && ((n = dataServerSocket->readFromSocket(packet, bufSz,args)) > 0)){
                total+=n;
                fwrite(packet, 1, n, fout);
                cout << "chunk" << endl;
                sleep(3);
        }
        cout << "Upload Complete" << endl;

        if (state->cancel_transmission)
            cerr << "Transmission cancelled" << endl;

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
