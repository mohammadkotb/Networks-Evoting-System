#ifndef FTP_REPLY_H
#define FTP_REPLY_H

#define USER_NAME "username"

#define PASSWORD "password"

#define COMMAND_OK "200 Command okay"

#define USER_NAME_OK "331 User name ok , need password"

#define NOT_AUTHENTICATED "332 not authenticated"

#define INVALID_AUTHENTICATION "430 invalid username or password"

#define SYNTAX_ERROR "500 Syntax error, command unrecognized"

#define NOT_IMPLEMENTED "502 Command not implemented"

#define LOGIN_HTML "../htdocs/login.html"

#define SIGNUP_HTML "../htdocs/signup.html"

#define INVALID_USERNAME_HTML "../htdocs/invalid_username.html"

#define MISSING_FIELD_HTML "../htdocs/missing_field.html"

#define WRONG_TYPE_HTML "../htdocs/wrong_type.html"

#define NOTFOUND_HTML "../htdocs/404.html"

const char LIST[] = "LIST";
const char PWD[] = "PWD";
const char CWD[] = "CWD";
const char MKD[] = "MKD";
const char RMD[] = "RMD";
const char BYE[] = "QUIT";
const char RETR[] = "RETR";
const char STOR[] = "STOR";
const char PORT[] = "PORT";
const char USER[] = "USER";
const char PASS[] = "PASS";

#endif // FTP_REPLY_H
