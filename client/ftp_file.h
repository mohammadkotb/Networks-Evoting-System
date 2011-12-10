#ifndef FTP_FILE_H
#define FTP_FILE_H

#include <string>

using std::string;

// Represents information about fles stored in an FTP server.
class FtpFile {
public:
    // Creates an empty FtpFile.
    FtpFile();

    // Sets the file name.
    void set_name(const string& name);

    // Sets the file size.
    void set_size(long long int size);

    // Sets the file user.
    void set_file_user(const string& user);

    // Sets the file group.
    void set_file_group(const string& group);

    // Sets the number of hard links.
    void set_num_hard_links(int num_hard_links);

private:

    // Name of the file.
    string name;

    // Size of the file in bytes.
    long long int size;

    // Name of the user owning the file.
    string user;

    // Name of the group owning the file.
    string group;

    // Number of hard links associated with this file.
    int num_hard_links;

    // If the file is a symbolic link, this string contains the link value.
    string link;

    // original FTP server raw listing from which the FtpFile was created.
    string raw_listing;

    // File's timestamp
    long long int timestamp;

    // File type: symbolic link, directory or file.
    int type;

};

#endif // FTP_FILE_H
