#ifndef FTP_COMMAND_PARSER_H
#define FTP_COMMAND_PARSER_H

#include <string>

using std::string;

// Encapsulates the functionality of parsing the FTP commands sent from the
// client to the server.
class FtpCommandParser {
public:
    // Parses the given command and updates the local attributes accordingly.
    void parse_command(const string& command);

    // Getters for private fields.
    void get_command_head(string* head);

    void get_command_body(string* body);

private:
    // Represents the head of the command.
    // Ex: LIST, PWD, CWD,.. etc
    string head;

    // Represents the body of the command
    // Ex: remote_filespec
    string body;
};

#endif // FTP_COMMAND_PARSER_H
