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

void FtpFile::set_type(FileType type) {
    this->type = type;
}

void FtpFile::set_permissions(const string& permissions) {
    this->permissions = permissions;
}

void FtpFile::get_name(string* name) {
    *name = this->name;
}

long long int FtpFile::get_size() {
    return size;
}

void FtpFile::get_file_user(string* user) {
    *user = this->user;
}

void FtpFile::get_file_group(string* group) {
    *group = this->group;
}

int FtpFile::get_num_hard_links() {
    return this->num_hard_links;
}

void FtpFile::get_link(string* link) {
    *link = this->link;
}

void FtpFile::get_raw_listing(string* raw_listing) {
    *raw_listing = this->raw_listing;
}

void FtpFile::get_day(string* day) {
    *day = this->day;
}

void FtpFile::get_time(string* time) {
    *time = this->time;
}

FileType FtpFile::get_type() {
    return this->type;
}

void FtpFile::get_permissions(string* permissions) {
    *permissions = this->permissions;
}
