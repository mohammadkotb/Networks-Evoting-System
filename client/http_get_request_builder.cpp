#include "http_get_request_builder.h"

using namespace std;

//creates a new Http get request with the required file name
//and an empty parameter list
HttpGetRequestBuilder::HttpGetRequestBuilder(string file_path){
    this->file_path = file_path;
    this->parameters = new map<string,string>();
}

void HttpGetRequestBuilder::addParameter(string name,string value){
    this->parameters->insert(pair<string,string>(name,value));
}

string HttpGetRequestBuilder::getParameter(string name){
    if (parameters->find(name) == parameters->end())
        return "";
    return parameters->find(name)->second;
}

string HttpGetRequestBuilder::getSerialization(){
    //file path
    string result = "GET " + this->file_path;

    //parameters
    int size = parameters->size();
    if (size > 0) result += "?";
    map<string,string>::iterator it;
    for (it = parameters->begin(); it != parameters->end(); ++it){
        if (size == 1)
            result+= it->first + "=" + it->second;
        else
            result+= it->first + "=" + it->second + "&";
        size--;
    }

    //HTTP version
    result += " HTTP/1.0\r\n\r\n";
    return result;
}

//destructor
HttpGetRequestBuilder::~HttpGetRequestBuilder(){
    delete parameters;
}

