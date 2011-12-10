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

    // Sets the file symbolic link.
    void set_link(const string& link);

    // Sets the file raw listing.
    void set_raw_listing(const string& raw_listing);

    // Sets the file creation day.
    void set_day(const string& day);

    // Sets the file creation time.
    void set_time(const string& time);

    // Sets the file type.
    void set_type(int type);

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

    // File's timestamp day.
    string day;

    // File's timestamp hours and minutes.
    string time;

    // File type: symbolic link, directory or file.
    int type;

};

#endif // FTP_FILE_H
