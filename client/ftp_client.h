#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

#include <iostream>
#include <string>
#include <vector>

#include "client_socket.h"
#include "ftp_file.h"
#include "ftp_command_builder.h"
#include "ftp_response_parser.h"

const int BUFFER_SIZE = 1 << 20;

using std::string;
using std::vector;
using std::ostream;

class FtpClient
{
public:
    // Constructor.
    FtpClient(const string& hostname, int port);

    // Login to FTP server using username and password.
    int login(const string& username, const string& password);

    // Sends a LIST command to the sever and retrieves the list of files in the
    // given directory.
    void list_files(vector<FtpFile>* files, const string& directory);

    // Initiate a server to server file transfer.
    bool remote_store(const string& remote_filename, const string& source_filename);

    // Retrieves a named file from the server and writes it to the given OutputStream.
    bool retrieve_file(const string& remote, const string& destination);

    // Abort a transfer in progress.
    bool abort();

    // Changes the current working directory.
    bool change_working_directory(const string& directory);

    // Prints the working directory.
    void print_working_directory(string* directory);

    // Remove the given directory.
    bool remove_directory(const string& directory);

    // Make a new directory
    bool make_directory(const string& pathname);

    // Closes the connection to the FTP server.
    bool disconnect();

private:
    ClientSocket client_socket_;
    FtpCommandBuilder command_builder_;
    FtpResponseParser response_parser_;
    char buffer_[BUFFER_SIZE];
};

#endif // FTP_CLIENT_H
