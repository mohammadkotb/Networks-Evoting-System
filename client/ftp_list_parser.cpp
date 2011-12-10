#include <sstream>
#include <iostream>
#include "ftp_list_parser.h"
#include "ftp_file_parser.h"

using std::stringstream;

void FtpParser::get_files(vector<FtpFile>* files) {
    stringstream string_stream(input_stream);
    FtpFileParser ftp_file_parser;
    FtpFile ftp_file;
    while(string_stream.good()) {
        string line;
        getline(string_stream, line);
        ftp_file_parser.parse_ftp_entry(&ftp_file, line);
        files->push_back(ftp_file);
    }
}

void FtpParser::read_server_list(const string& input_stream) {
    this->input_stream = input_stream;
}
