#include "ftp_command_builder.h"

const char LIST[] = "LIST";
const char PWD[] = "PWD";
const char CWD[] = "CWD";
const char MKD[] = "MKD";
const char RMD[] = "RMD";

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
