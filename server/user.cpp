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
    this->voted = false;
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

void User::setVoted(bool vote){
	this->voted = vote;
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

bool User::getVoted(){
    return this->voted;
}

User::~User(){
}
