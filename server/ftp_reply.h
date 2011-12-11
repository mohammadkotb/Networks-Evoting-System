#ifndef FTP_REPLY_H
#define FTP_REPLY_H

#define COMMAND_OK "200 Command okay"

#define SYNTAX_ERROR "500 Syntax error, command unrecognized"

#define NOT_IMPLEMENTED "502 Command not implemented"

const char LIST[] = "LIST";
const char PWD[] = "PWD";
const char CWD[] = "CWD";
const char MKD[] = "MKD";
const char RMD[] = "RMD";
const char BYE[] = "QUIT";
const char RETR[] = "RETR";
const char STOR[] = "STOR";
const char PORT[] = "PORT";

#endif // FTP_REPLY_H
