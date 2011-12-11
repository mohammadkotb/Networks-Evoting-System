#include <sstream>
#include "ftp_command_parser.h"

using std::stringstream;

void FtpCommandParser::parse_command(const string& command) {
    stringstream string_stream(command);
    string_stream >> head;
    if (string_stream.good()) {
        string_stream >> body;
    } else {
        body = "";
    }
}

void FtpCommandParser::get_command_head(string* head) {
    *head = this->head;
}

void FtpCommandParser::get_command_body(string* body) {
    *body = this->body;
}
