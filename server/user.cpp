#include "user.h"
#include <string>
using namespace std;

User::User(){
    User("", "", "");
}

User::User(string type,string username,string password){
    this->type = type;
    this->username = username;
    this->password = password;
}

void User::setType(string type){
    this->type = type;
}

void User::setUserName(string username){
    this->username = username;
}

void User::setPassword(string password){
    this->password = password;
}

string User::getType(){
    return this->type;
}

string User::getPassword(){
    return this->password;
}

string User::getUserName(){
    return this->username;
}

User::~User(){
}
