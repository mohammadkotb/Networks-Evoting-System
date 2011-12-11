#include "ftp_server.h"

void FTPServer::init(int control_port_no, int data_port_no, int control_buffer_size, int data_buffer_size, int queueSize, bool (*process_fn)(void*)){
        this->controlPortNumber = control_port_no;
        this->dataPortNumber = data_port_no;
        this->controlBufferSize = control_buffer_size;
        this->dataBufferSize = data_buffer_size;
        pthread_mutex_init(&states_mutex, NULL);

        this->controlServerSocket = ServerSocket('T', controlPortNumber, controlBufferSize, queueSize, process_fn);
}

FTPServer::FTPServer(int control_port_no, int data_port_no, int control_buffer_size, int data_buffer_size, int queue_size, bool(*process_fn)(void*)){
        init(control_port_no, data_port_no, control_buffer_size, data_buffer_size, queue_size, process_fn);
}

FTPServer::FTPServer(bool (*process_fn)(void*)){
        init(defaultControlPort, defaultDataPort, defaultControlBufferSize, defaultDataBufferSize, defaultQueueSize, process_fn);
}

void FTPServer::addState(int fileDescriptor, struct ftp_state *state){
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
        return states.find(fileDescriptor)->second;
}

bool FTPServer::downloadFile_aux(char *fileName, void *args){
        void **ar = (void **) args;
        ServerSocket *dataServerSocket = (ServerSocket *) ar[0];
        int client_fd = *((int *) ar[1]);
        struct ftp_state * state = this->getState(client_fd);
        state->is_connection_open = true;

//	char *buffer_file_name = (char *) ar[2];

        FILE *fin = fopen(fileName, "r");
        if(!fin){
                cerr << "Error! couldn't open the file: " << fileName << endl;
                state->is_connection_open = false;
                state->cancel_transmission = false;
                return false;
        }

        int bufSz = dataServerSocket->getBufferSize();
        char packet[bufSz];

        int n;
        while(!(state->cancel_transmission) && (n=fread(packet, 1, bufSz, fin))){
                dataServerSocket->writeToSocket(packet, n, args);
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

bool FTPServer::uploadFile_aux(char *fileName, void *args){
/*
        ClientSocket clientSocket('T', 6060);

        char file_name_buf[256] = "inceptionTrailer.flv";
        cerr << "Requesting file: " << file_name_buf << endl;
        clientSocket.writeToSocket(file_name_buf);

        FILE *fout = fopen(file_name_buf, "w");

        int bufSz=1<<20; //this MUST BE >= buffer size of the FTP server, so as not to cause buffer over flow, and drop data
        char packet[bufSz];
        memset(packet,0,bufSz);
        int n, total=0;

        while((n = clientSocket.readFromSocket(packet, bufSz))){
                total+=n;
                fwrite(packet, 1, n, fout);
        }

        fclose(fout);

        cerr << "total = " << 1.0*total/1000.0 << "Kbyte" << endl;
        cerr << "File successfully received, thank God :)" << endl;

*/
        return 0;
}

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


void FTPServer::run(){
        controlServerSocket.run((void *)(&controlServerSocket));
        pthread_mutex_destroy(&states_mutex);
}

//==========================================================================================================================================
/*
bool go_ftp_server(void *args){
        void **ar = (void **) args;
        int client_fd = *((int *) ar[1]);
        char *buffer_file_name = (char *) ar[2];
        cerr << "File requested by client is: " << buffer_file_name << endl;

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

        while(1){
                ftpServer->openDataConnection(buffer_file_name, DOWNLOAD, args);
                sleep(5);
                break;
        //TODO: we should eliminate the sleep() and put the code in a while(1)
        //with the break condition is receiving "bye" command from client
        }

        return true;
}

int main(){
        cerr << "started" << endl;

        ftpServer = new FTPServer(&go_ftp_server);
        ftpServer->run();
        delete(ftpServer);

        printf("terminated\n");

        return 0;
}
*/
