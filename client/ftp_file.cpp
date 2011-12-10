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

void FtpFile::set_link(const string& link) {
    this->link = link;
}

void FtpFile::set_raw_listing(const string& raw_listing) {
    this->raw_listing = raw_listing;
}

void FtpFile::set_day(const string& day) {
    this->day = day;
}

void FtpFile::set_time(const string& time) {
    this->time = time;
}

void FtpFile::set_type(int type) {
    this->type = type;
}
