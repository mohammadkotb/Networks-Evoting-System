#ifndef FTP_RESPONSE_PARSER_H
#define FTP_RESPONSE_PARSER_H

#include <string>

using std::string;

// Encapsulates the functionality of parsing the FTP server response.
class FtpResponseParser
{
public:
    FtpResponseParser();

    // Parses the given response and updates the instance attributes
    // accordingly.
    void parse_ftp_response(const string& response);

    // Returns the response code.
    int get_code();

    // Returns the response message.
    void get_message(string* message);

private:
    int code;
    string message;
};

#endif // FTP_RESPONSE_PARSER_H
