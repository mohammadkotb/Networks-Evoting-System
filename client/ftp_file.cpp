#include "ftp_file.h"

FtpFile::FtpFile() {}

void FtpFile::set_name(const string& name) {
    this->name = name;
}

void FtpFile::set_size(long long int size) {
    this->size = size;
}

void FtpFile::set_file_user(const string& user) {
    this->user = user;
}

void FtpFile::set_file_group(const string& group) {
    this->group = group;
}

void FtpFile::set_num_hard_links(int num_hard_links) {
    this->num_hard_links = num_hard_links;
}
