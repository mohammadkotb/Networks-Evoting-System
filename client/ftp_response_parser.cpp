#include <sstream>

#include "ftp_response_parser.h"

using std::string;
using std::stringstream;

FtpResponseParser::FtpResponseParser() {}

void FtpResponseParser::parse_ftp_response(const string& response) {
    message = response;
    stringstream string_stream(response);
    if(string_stream.good()) {
        // Extract code.
        string_stream >> code;
    } else {
        // Invalid response, set code to -1.
        code = -1;
    }
}

int FtpResponseParser::get_code() {
    return this->code;
}

void FtpResponseParser::get_message(string* message) {
    *message = this->message;
}
