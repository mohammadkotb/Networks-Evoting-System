#ifndef HTTP_GET_REQUEST_PARSER_H
#define HTTP_GET_REQUEST_PARSER_H

#include <string>
#include <map>
using namespace std;

class HttpGetRequestParser{
    private:
        //raw data
        string data;
        //requested file path
        string file_path;
        //parameters
        map<string,string>* parameters;
    public:
        //constructor using raw data
        HttpGetRequestParser(string & data);

        //destructor
        ~HttpGetRequestParser();

        //returns parameter value
        string getParameter(string name);

        //return all parameters
        map<string,string> * getParameters();

};

#endif
