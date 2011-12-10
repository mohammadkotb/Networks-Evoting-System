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

    // QUIT Syntax: QUIT

    // Updates the output parameter "command" with the properly formatted syntax
    // for the FTP QUIT command.
    void bye_command(string* command);

    // RETR Syntax: RETR remote-filename

    // Updates the output parameter "command" with the properly formatted syntax
    // for the FTP RETR command.
    void download_file(string* command, const string& remote_file);

    //Syntax: STOR remote-filename
    // Begins transmission of a file to the remote site. Must be preceded by
    // either a PORT command or a PASV command so the server knows where to accept data from.

    // Updates the output parameter "command" with the properly formatted syntax
    // for the FTP STOR command.
    void upload_file(string* command, const string& remote_file);


    //PORT Syntax: PORT a1,a2,a3,a4,p1,p2
    // Specifies the host and port to which the server should connect for the
    // next file transfer. This is interpreted as IP address a1.a2.a3.a4, port p1*256+p2.

    // Updates the output parameter "command" with the properly formatted syntax
    // for the FTP PORT command.
    void connect_command(string* command, const string& a1, const string& a2,
        const string& a3, const string& a4, const string& p1, const string& p2);
};

#endif // FTP_COMMAND_BUILDER_H
