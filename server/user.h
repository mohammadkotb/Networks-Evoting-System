#ifndef USER_H
#define USER_H

#include <string>

class User{
    private:
        std::string name;
        std::string username;
        std::string password;
    public:
        //constructor
        User(std::string name,std::string username,std::string password);
        //getters
        std::string getName();
        std::string getUserName();
        std::string getPassword();
        //setters
        void setName(std::string name);
        void setUserName(std::string username);
        void setPassword(std::string password);
        //destructor
        ~User();

};
#endif
