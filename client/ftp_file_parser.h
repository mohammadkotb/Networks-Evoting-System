#ifndef FTP_FILE_ENTRY_PARSER_H
#define FTP_FILE_ENTRY_PARSER_H

#include <string>
#include "ftp_file.h"

using std::string;

// Pares a single raw FTP server file listing and converts the raw listing
// into usable FtpFile instance.
class FtpFileParser {
public:
    // Pares a line in the FTP server files listing to form a usable
    // FtpFile instance.
    void parse_ftp_entry(FtpFile* ftp_file, const string& list_entry);
};

#endif // FTP_FILE_ENTRY_PARSER_H
