#include "user.h"
#include <string>
using namespace std;

User::User(string name,string username,string password){
    this->name = name;
    this->username = username;
    this->password = password;
}

void User::setName(string name){
    this->name = name;
}

void User::setUserName(string username){
    this->username = username;
}

void User::setPassword(string password){
    this->password = password;
}

string User::getName(){
    return this->name;
}

string User::getPassword(){
    return this->password;
}

string User::getUserName(){
    return this->username;
}

User::~User(){
}
