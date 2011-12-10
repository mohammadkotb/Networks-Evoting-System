#include <sstream>
#include "ftp_file_parser.h"

using std::stringstream;

void FtpFileParser::parse_ftp_entry(FtpFile* ftp_file,
    const string& list_entry) {
    //drwxr-xr-x 8 amr amr 4096 2011-12-10 02:11 .git
    //permissions, number of hard links, owner, group, size, date, and filename
    stringstream string_input(list_entry);
    string permissions, owner, group, date, filename;
    long long int size;
    int num_hard_links;
    string_input >> permissions;
    string_input >> num_hard_links;
    string_input >> owner;
    string_input >> group;
    string_input >> size;
    // TODO: create clander class for the date
    string_input >> date;
    string_input >> date;
    string_input >> filename;
    ftp_file->set_file_group(group);
    ftp_file->set_file_user(owner);
    ftp_file->set_name(filename);
    ftp_file->set_num_hard_links(num_hard_links);
    ftp_file->set_size(size);
}
