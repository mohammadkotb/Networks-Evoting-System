#include "http_get_request_parser.h"

using namespace std;

//creates a new Http get request from the given string
HttpGetRequestParser::HttpGetRequestParser(const string & data){
    this->parameters = new map<string,string>();
    //unsupported header
    if (data.substr(0,3) != "GET")
        throw -1;
    size_t parameter_start_index = data.find("?");
    //no parameters
    if (parameter_start_index == string::npos){
        this->file_path = data.substr(4,data.find(" ",5)-4);
    }else{
        this->file_path = data.substr(4,parameter_start_index-4);
        //parse parameters
        size_t secondIndex = parameter_start_index;
        while (secondIndex != string::npos){
            size_t firstIndex = secondIndex + 1;
            secondIndex = data.find("&",firstIndex);
            string parameter;
            if (secondIndex == string::npos)
                parameter = data.substr(firstIndex,data.find(" ",firstIndex)-firstIndex);
            else
                parameter = data.substr(firstIndex,secondIndex-firstIndex);
            size_t eq_index = parameter.find("=");
            //if it is a valid parameter (on the format name = value)
            if (eq_index != string::npos){
                string name = parameter.substr(0,eq_index);
                string value = parameter.substr(eq_index+1,parameter.length()-eq_index+1);
                parameters->insert(pair<string,string>(name,value));
            }

        }
    }
}

string HttpGetRequestParser::getRequiredFileName(){
    return this->file_path;
}

string HttpGetRequestParser::getParameter(string name){
    if (parameters->find(name) == parameters->end())
        return "";
    return parameters->find(name)->second;
}

map<string,string>* HttpGetRequestParser::getParameters(){
    return parameters;
}

//destructor
HttpGetRequestParser::~HttpGetRequestParser(){
    delete parameters;
}
