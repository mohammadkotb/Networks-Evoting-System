#include <iostream>

using namespace std;

#include "ftp_client.h"
#include "ftp_list_parser.h"

const char CONNECTION_TYPE = 'T';

FtpClient::FtpClient(const string& hostname, int port)
    : client_socket_(CONNECTION_TYPE, port, (char *) hostname.c_str()) {}

bool FtpClient::login(const string& username, const string& password) {
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
        if (code == 200) {
            // valid username and password.
            return true;
        } else {
            return false;
        }
    } else {
        // Invalid response.
        return false;
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

bool FtpClient::remote_store(const string& filename) {
    string store_command;
    command_builder_.upload_command(&store_command, filename);
    return false;
}

void * download_aux(void *args){
                ClientSocket dataSocket('T', 7071);

                char file_name_buf[256];
                int client_fd;

                string *ar = (string *) args;
                char args_local[1<<10];
                strcpy(args_local, ((*ar) + " 1").c_str()); //1 for download, 0 for upload
                sscanf(ar->c_str(), "%d %s", &client_fd, file_name_buf);
                delete(ar);

                cerr << "Requesting file: " << file_name_buf << endl;
                dataSocket.writeToSocket(args_local);


                FILE *fout = fopen(file_name_buf, "w");

                int bufSz=1<<20; //this MUST BE >= buffer size of the FTP server, so as not to cause buffer over flow, and drop data
                char packet[bufSz];
                memset(packet,0,bufSz);
                int n, total=0;

                while((n = dataSocket.readFromSocket(packet, bufSz))){
                        total+=n;
                        fwrite(packet, 1, n, fout);
                }

                fclose(fout);

                cerr << "total = " << 1.0*total/1000.0 << "Kbyte" << endl;
                cerr << "File successfully received, thank God :)" << endl;

                return NULL;
}

bool FtpClient::retrieve_file(const string& fileName) {
    char file_name_buf[1<<8];
    strcpy(file_name_buf, fileName.c_str());
    cerr << "Requesting file: " << file_name_buf << endl;
    client_socket_.writeToSocket(file_name_buf);

//========================================================
    char response[1<<8];
    client_socket_.readFromSocket(response, 1<<8);
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

    string *arg = new string(string(response)+ " " + fileName);
    pthread_t thrd;
    pthread_create(&thrd, NULL, download_aux, (void *) arg);

    return true;
}

bool FtpClient::abort() {
    return false;
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
