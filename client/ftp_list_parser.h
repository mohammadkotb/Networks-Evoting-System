#ifndef FTP_LIST_PARSER_H
#define FTP_LIST_PARSER_H

#include <vector>
#include "ftp_file.h"

using std::vector;

// This class handles the process of parsing a list of file entries from the
// server. The class pares the results returned from the LIST FTP command.
class FtpParser {
public:
    // Returns a vector of FtpFile objects containing the whole list of files
    // retruned from the FTP server.
    void get_files(vector<FtpFile>* files);

    // Handles the initial reading and parsing of the list of files returned
    // from the server.
    void read_server_list(const string& input_stream);
private:
    string input_stream;
};

#endif // FTP_LIST_PARSER_H
