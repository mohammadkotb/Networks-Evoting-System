#ifndef COMMAND_SUPPORT_H
#define COMMAND_SUPPORT_H

#include <string>

using std::string;

// Encapsulates the functionality of executing different FTP commands
class CommandSupporter {
public:

    // Executes the LIST command.
    string ls(string directory);

    // Executes the MKD command.
    bool mkdir(string path);

    // Executes the RMD command.
    bool rm(string path);

    // Executes the CWD command.
    bool cd(string current_path, string arg);

};

#endif // COMMAND_SUPPORT_H
