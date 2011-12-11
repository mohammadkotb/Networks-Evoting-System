#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>

#include "command_support.h"

using namespace std;

string CommandSupporter::ls(string directory) {
  // given a directory return a string \n delimited containing all
  // sub files and directories, and return "" if there is any error
  // occurred

  // directory parameter must be passed ended with '/'
  if (directory[directory.size()-1] != '/') {
    directory += "/";
  }

  DIR *dir;
  struct dirent *ent;
  dir = opendir(directory.c_str());

  if (dir != NULL) {
    stringstream ss; // create stringstream for getting data for the directory
    bool first = true;
    while ((ent = readdir(dir)) != NULL) {
      char *file = ent->d_name;

      char file_path[200];
      memset(file_path, 0, sizeof(file_path));
      strcat(file_path, directory.c_str());
      strcat(file_path, file);

      struct stat fileInfo;
      if (stat(file_path, &fileInfo) != 0) {
        return "";
      }

      string type = "";
      if ((fileInfo.st_mode & S_IFMT) == S_IFDIR) {
        type = "dir";
      } else {
        type = "file";
      }

      if (!first) {
          ss << endl;
      }
      first = false;
      ss << "0 0 0 0 ";

      // pushing size in case of file and 0 otherwise
      if (type == "file") {
        ss << fileInfo.st_size << " ";
      } else {
        ss << "0 ";
      }

      // pushing date time
      char time_buffer[100];
      struct tm  *timeinfo;
      timeinfo = localtime(&fileInfo.st_mtime);
      memset(time_buffer, 0, sizeof(time_buffer));
      strftime(time_buffer, 100,
                "%Y-%m-%d %H:%M", timeinfo);
      string mod_time(time_buffer);

      ss << mod_time << " ";

      // pushing file name
      string file_name(file);
      ss << file_name;

      // pushing file type
      if (type == "dir") ss << "/ ";
      else ss << " ";
      ss << type;
    }
    return ss.str();
  } else {
    return "";
  }
}

bool CommandSupporter::mkdir(string path) {
  // make a new directory given the path and support multiple new direcotries
  // for example mkdir a/b/c where a, b, and c are not available
    cout << path << endl;
  return system(("mkdir -p " + path).c_str()) == 0? true : false;
}

bool CommandSupporter::rm(string path) {
  // given a path, check if it is a directory or a file
  // and remove it and return true if removal is done successfully
  // false otherwise
  struct stat fileInfo;
  if (stat(path.c_str(), &fileInfo) != 0) {
    return false;
  }
  string cmd = "rm ";
  if ((fileInfo.st_mode & S_IFMT) == S_IFDIR) {
    cmd += "-r ";
  }
  cmd += path;
  return system(cmd.c_str()) == 0 ? true : false;
}

bool CommandSupporter::cd(string current_path, string arg) {
  // given a current directory and the argument written after cd
  // return true if this argument is a valid directory
  // false otherwise
  if (current_path[current_path.size()-1] != '/') {
    current_path += "/";
  }
  string path = current_path + arg;
  DIR *dir;
  struct dirent *ent;
  dir = opendir(path.c_str());
  if (dir != NULL) {
    return true; 
  } else {
    return false;
  }
}

