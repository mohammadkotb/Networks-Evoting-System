#ifndef HTTP_GET_REQUEST_PARSER_H
#define HTTP_GET_REQUEST_PARSER_H

#include <string>
#include <map>

class HttpGetRequestParser{
    private:
        //raw data
        std::string data;
        //requested file path
        std::string file_path;
        //parameters
        std::map<std::string,std::string>* parameters;
    public:
        //constructor using raw data
        HttpGetRequestParser(std::string & data);

        //destructor
        ~HttpGetRequestParser();

        //returns the required file
        std::string getRequiredFileName();

        //returns parameter value
        std::string getParameter(std::string name);

        //return all parameters
        std::map<std::string,std::string> * getParameters();

};

#endif
