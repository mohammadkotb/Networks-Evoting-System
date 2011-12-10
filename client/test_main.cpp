//#include <iostream>
//#include "ftp_list_parser.h"
//using namespace std;

//int main(int argc, char *argv[])
//{
//    FtpFile ftp_file;
//    string raw_data = "drwxr-xr-x 8 amr amr 4096 2011-12-10 02:11 .git\n";
//    raw_data += "drwxr-xr-x 2 amr amr 4096 2011-12-10 11:46 client\n";
//    raw_data += "-rw-r--r-- 1 amr amr  130 2011-11-26 17:49 README";
//    FtpParser ftp_parser;
//    ftp_parser.read_server_list(raw_data);
//    vector<FtpFile> files;
//    ftp_parser.get_files(&files);
//    for (int i = 0 ; i < files.size(); i++) {
//        ftp_file = files[i];
//        string permissions, owner, group, day, time, filename;
//        long long int size;
//        int num_hard_links;
//        ftp_file.get_permissions(&permissions);
//        ftp_file.get_file_group(&group);
//        ftp_file.get_file_user(&owner);
//        ftp_file.get_name(&filename);
//        size = ftp_file.get_size();
//        num_hard_links = ftp_file.get_num_hard_links();
//        ftp_file.get_day(&day);
//        ftp_file.get_time(&time);
//        cout << "permissions: " << permissions << endl;
//        cout << "owner: " << owner << endl;
//        cout << "group: " << group << endl;
//        cout << "day: " << day << endl;
//        cout << "time: " << time << endl;
//        cout << "name: " << filename << endl;
//        cout << "size: " << size << endl;
//        cout << "num_links: " << num_hard_links << endl;
//        cout << "---------------------------" << endl;
//    }
//    return 0;
//}

