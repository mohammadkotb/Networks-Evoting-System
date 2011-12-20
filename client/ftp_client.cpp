#include <iostream>

#include "ftp_client.h"
#include "ftp_list_parser.h"

using namespace std;

class FileInfo{
    public:
        string source;
        string destination;
};

const char CONNECTION_TYPE = 'U';

FtpClient::FtpClient(const string& hostname, int port)
    : client_socket_(CONNECTION_TYPE, port, (char *) hostname.c_str()) {}

int FtpClient::login(const string& username, const string& password) {
    string user_command;    // QUIT Syntax: QUIT
    command_builder_.user_command(&user_command, username);
    client_socket_.writeToSocket((char *)user_command.c_str());
    client_socket_.readFromSocket(buffer_, BUFFER_SIZE);
    string response(buffer_);
    response_parser_.parse_ftp_response(response);
    int code = response_parser_.get_code();
    if (code == 331) {
        // username OK, waiting for password
        string password_command;
        command_builder_.password_command(&password_command, password);
        client_socket_.writeToSocket((char *)password_command.c_str());
        client_socket_.readFromSocket(buffer_, BUFFER_SIZE);
        response = string(buffer_);
        response_parser_.parse_ftp_response(response);
        code = response_parser_.get_code();
        if (code == 201) {
            // valid username and password.
                //voter
            return 1;
        } else if(code == 202){
            // valid username and password.
            return 2;
        } else {
            return 0;
        }
    } else {
        // Invalid response.
        return 0;
    }
}

void FtpClient::list_files(vector<FtpFile>* files, const string& directory) {
    string list_command;
    command_builder_.list_command(&list_command, directory);
    client_socket_.writeToSocket((char *) list_command.c_str());
    client_socket_.readFromSocket(buffer_, BUFFER_SIZE);
    string response(buffer_);
    FtpParser parser;
    parser.read_server_list(response);
    parser.get_files(files);
}

void * upload_aux(void *args){
    ClientSocket dataSocket('U', 7071);

    char file_name_buf[256];
    int client_fd;

    FileInfo *f = (FileInfo*) args;
    cout << "AAA " << f->source << " " << f->destination << endl;
    char args_local[1<<10];
    strcpy(args_local, ((f->source) + " 0").c_str()); //1 for download, 0 for upload
    sscanf(f->source.c_str(), "%d %s", &client_fd, file_name_buf);

    cerr << "Requesting file: " << file_name_buf << endl;
    dataSocket.writeToSocket(args_local);

    char dummy[1<<10];
    dataSocket.readFromSocket(dummy, 1<<10);
    // delete(f);
    cout << "Dummy Message Received" << endl;

    FILE *fin = fopen(f->destination.c_str(), "r");

    // MOUSTAFA PASTE BIN

    int bufSz=1<<20; //this MUST BE >= buffer size of the FTP server, so as not to cause buffer over flow, and drop data
    char packet[bufSz];
    memset(packet,0,bufSz);
    int n, total=0;

    if(!fin){
              cerr << "Error! couldn't open the file: " << file_name_buf << endl;
              // close(fd);
              return false;
      }

      sleep(1);
      while((n=fread(packet, 1, bufSz, fin))){
          dataSocket.writeToSocket(packet, n);
          sleep(2);
      }
      //TODO::REMOVE THIS
      //send dummy line
      dataSocket.writeToSocket((char*)"",0);

      cerr << "closing file " << file_name_buf << endl;
      if(fclose(fin)==EOF){
              cerr << "Error! couldn't close the file: " << file_name_buf << endl;
              return false;
      }

    return NULL;
}

bool FtpClient::remote_store(const string& remote_filename, const string& source_filename) {
    string store_command;
    command_builder_.upload_command(&store_command, remote_filename);
    client_socket_.writeToSocket((char *) store_command.c_str());

//========================================================
    char response[1<<8];
    client_socket_.readFromSocket(response, 1<<8);
    cout << "THIS IS THE CLIENT FD: " << response;
    int client_fd;
    sscanf(response, "%d", &client_fd);
    if(client_fd < 0){
            if(client_fd==-1) {
                    cerr << "You have to wait until you current file transfer is finished" << endl;
            } else if(client_fd==-2){
                    cerr << "Error! file not found" << endl;
            } else if(client_fd < -2){
                    cerr << "Unexpected error has occurred, please try again later!" << endl;
            }
            return false;
    }
    FileInfo *f = new FileInfo();
    f->source = string(string(response) + " " + remote_filename);
    f->destination= string(source_filename);
    pthread_t thrd;
    pthread_create(&thrd, NULL, upload_aux, (void *) f);

    return true;
}

void * download_aux(void *args){
    ClientSocket dataSocket('U', 7071);

    char file_name_buf[256];
    int client_fd;

    FileInfo *f = (FileInfo*) args;
    cout << "AAA " << f->source << " " << f->destination << endl;
    char args_local[1<<10];
    strcpy(args_local, ((f->source) + " 1").c_str()); //1 for download, 0 for upload
    sscanf(f->source.c_str(), "%d %s", &client_fd, file_name_buf);

    cerr << "Requesting file: " << file_name_buf << endl;
    cerr << args_local << endl;
    dataSocket.writeToSocket(args_local);

    cout << "GO TO SOCKET" << endl;
    string stdDest(file_name_buf);
    int x = stdDest.rfind("/");
    stdDest = f->destination + stdDest.substr(x,stdDest.length()-x);
    cout << "F-DESTT " <<f->destination << endl;
    cout << "Downloading too : " << stdDest << endl;
    delete(f);

    FILE *fout = fopen(stdDest.c_str(), "w");

    int bufSz=1<<20; //this MUST BE >= buffer size of the FTP server, so as not to cause buffer over flow, and drop data
    char packet[bufSz];
    memset(packet,0,bufSz);
    int n, total=0;

    while((n = dataSocket.readFromSocket(packet, bufSz))){
            total+=n;
            fwrite(packet, 1, n, fout);
            cout << "chunk" << endl;
    }

    fclose(fout);

    cerr << "total = " << 1.0*total/1000.0 << "Kbyte" << endl;
    cerr << "File successfully received, thank God :)" << endl;

    return NULL;
}


bool FtpClient::retrieve_file(const string& fileName,const string & destination) {
    char file_name_buf[1<<8];
    //strcpy(file_name_buf, fileName.c_str());
    strcpy(file_name_buf, fileName.c_str());
    cerr << "Requesting file: " << file_name_buf << endl;
    string download_command;
    //command_builder_.download_command(&download_command, fileName);
    command_builder_.download_command(&download_command, fileName);
    client_socket_.writeToSocket((char*) download_command.c_str());

//========================================================
    char response[1<<8];
    client_socket_.readFromSocket(response, 1<<8);
    cout << "THIS IS THE CLIENT FD: " << response;
    int client_fd;
    sscanf(response, "%d", &client_fd);

    if(client_fd < 0){
            if(client_fd==-1){
                    cerr << "You have to wait until you current file transfer is finished" << endl;
            } else if(client_fd==-2){
                    cerr << "Error! file not found" << endl;
            } else if(client_fd < -2){
                    cerr << "Unexpected error has occurred, please try again later!" << endl;
            }

            return false;
    }

    FileInfo *f = new FileInfo();
    f->source = string(string(response) + " " + fileName);
    f->destination= string(destination);
    pthread_t thrd;
    pthread_create(&thrd, NULL, download_aux, (void *) f);

    return true;
}

bool FtpClient::abort() {
    string abort_command;
    command_builder_.abort_command(&abort_command);
    client_socket_.writeToSocket((char*)abort_command.c_str());
    char response[1<<10];
    client_socket_.readFromSocket(response,1 << 10 );
    return true;
}

bool FtpClient::change_working_directory(const string& directory) {
    string cd_command;
    command_builder_.cwd_command(&cd_command, directory);
    client_socket_.writeToSocket((char *) cd_command.c_str());
    client_socket_.readFromSocket(buffer_, BUFFER_SIZE);
    string response(buffer_);
    response_parser_.parse_ftp_response(response);
    int code = response_parser_.get_code();
    if (code == 200) {
        return true;
    } else {
        return false;
    }
    return false;
}

void FtpClient::print_working_directory(string* directory) {
    string pwd_command;
    command_builder_.pwd_command(&pwd_command);
}

bool FtpClient::remove_directory(const string& directory) {
    string rm_command;
    command_builder_.rmd_command(&rm_command, directory);
    client_socket_.writeToSocket((char*)rm_command.c_str());
    client_socket_.readFromSocket(buffer_, BUFFER_SIZE);
    string response;
    response_parser_.parse_ftp_response(response);
    int code = response_parser_.get_code();
    if (code == 200) {
        return true;
    } else {
        return false;
    }
    return false;
}

bool FtpClient::make_directory(const string& pathname) {
    string mkdir_command;
    command_builder_.mkd_command(&mkdir_command, pathname);
    client_socket_.writeToSocket((char*) mkdir_command.c_str());
    client_socket_.readFromSocket(buffer_, BUFFER_SIZE);
    string response(buffer_);
    response_parser_.parse_ftp_response(response);
    int code = response_parser_.get_code();
    if (code == 200) {
        return true;
    } else {
        return false;
    }
}

bool FtpClient::disconnect() {
    string bye_command;
    command_builder_.bye_command(&bye_command);
    client_socket_.writeToSocket((char*) bye_command.c_str());
    client_socket_.readFromSocket(buffer_, BUFFER_SIZE);
    string response(buffer_);
    response_parser_.parse_ftp_response(response);
    int code = response_parser_.get_code();
    if(code == 200) {
        return true;
    } else {
        return false;
    }
}
