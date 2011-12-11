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

bool FtpClient::retrieve_file(ostream* output_stream, const string& remote) {
    return false;
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
