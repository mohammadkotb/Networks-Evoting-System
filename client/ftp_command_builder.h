#ifndef FTP_COMMAND_BUILDER_H
#define FTP_COMMAND_BUILDER_H

#include <string>

using std::string;

// Encapsulates the data and actions required for sending FTP commands to the
// FTP server.
class FtpCommandBuilder {
public:

    // LIST syntax: LIST [remote-filespec]
    // If remote-filespec refers to a file, sends information about that file. If
    // remote-filespec refers to a directory, sends information about each file in
    // that directory. remote-filespec defaults to the current directory.
    // This command must be preceded by a PORT or PASV command.

    // Updates the output parameter "command" with the properly foramtted syntax
    // for the FTP LIST command.
    void list_command(string* command, const string& remote_filespec);

    // PWD Syntax: PWD
    // Returns the name of the current directory on the remote host.

    // Updates the output parameter "command" with the properly formatted syntax
    // for the FTP PWD command.
    void pwd_command(string* command);

    //CWD Syntax: CWD remote-directory
    // Makes the given directory be the current directory on the remote host.

    // Updates the output parameter "command" with the properly formatted syntax
    // for the FTP CWD command.
    void cwd_command(string* command, const string& remote_directory);

    // MKD Syntax: MKD remote-directory

    // Updates the output parameter "command" with the properly formatted syntax
    // for the FTP MKD command.
    void mkd_command(string* command, const string& remote_directory);

    // RMD Syntax: RMD remote-directory

    // Updates the output parameter "command" with the properly formatted syntax
    // for the FTP RMD command.
    void rmd_command(string* command, const string& remote_directory);

    // connect (user name)
    // upload file
    // download file
    // bye
};

#endif // FTP_COMMAND_BUILDER_H
