#ifndef USER_H
#define USER_H

#include <string>

class User{
    private:
        std::string type;
        std::string username;
        std::string password;
	bool voted;

    public:
        //constructor
        User(std::string type,std::string username,std::string password);
        //default costructor
        User();
        //getters
        std::string getType();
        std::string getUserName();
        std::string getPassword();
	bool getVoted();
        //setters
        void setType(std::string type);
        void setUserName(std::string username);
        void setPassword(std::string password);
	void setVoted(bool vote);
        //destructor
        ~User();

};
#endif
