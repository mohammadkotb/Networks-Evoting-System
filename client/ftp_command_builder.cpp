#include "ftp_command_builder.h"

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
const char ABRT[] = "ABRT";

void FtpCommandBuilder::abort_command (string * command){
    *command = ABRT;
}
void FtpCommandBuilder::list_command(string* command,
    const string& remote_filespec) {
    *command = LIST;
    *command += " ";
    *command += remote_filespec;
}

void FtpCommandBuilder::pwd_command(string* command) {
    *command = PWD;
}

void FtpCommandBuilder::cwd_command(string* command,
    const string& remote_directory) {
    *command += CWD;
    *command += " ";
    *command += remote_directory;
}

void FtpCommandBuilder::mkd_command(string* command,
    const string& remote_directory) {
    *command += MKD;
    *command += " ";
    *command += remote_directory;
}

void FtpCommandBuilder::rmd_command(string* command,
    const string& remote_directory) {
    *command += RMD;
    *command += " ";
    *command += remote_directory;
}

void FtpCommandBuilder::bye_command(string* command) {
    *command += BYE;
}

void FtpCommandBuilder::download_command(string* command,
    const string& remote_directory) {
    *command += RETR;
    *command += " ";
    *command += remote_directory;
}

void FtpCommandBuilder::upload_command(string* command,
    const string& remote_directory) {
    *command += STOR;
    *command += " ";
    *command += remote_directory;
}

//PORT Syntax: PORT a1,a2,a3,a4,p1,p2
// Specifies the host and port to which the server should connect for the
// next file transfer. This is interpreted as IP address a1.a2.a3.a4, port p1*256+p2.

// Updates the output parameter "command" with the properly formatted syntax
// for the FTP PORT command.
void FtpCommandBuilder::connect_command(string* command, const string& a1, const string& a2,
    const string& a3, const string& a4, const string& p1, const string& p2) {
    *command += PORT;
    *command += " ";
    *command += a1;
    *command += ",";
    *command += a2;
    *command += ",";
    *command += a3;
    *command += ",";
    *command += a4;
    *command += ",";
    *command += p1;
    *command += ",";
    *command += p2;
}

void FtpCommandBuilder::password_command(string* command,
    const string& password) {
    *command += PASS;
    *command += " ";
    *command += password;
}

void FtpCommandBuilder::user_command(string* command,
    const string& username) {
    *command += USER;
    *command += " ";
    *command += username;
}

