#include "http_response_parser.h"

using namespace std;

HttpResponseParser::HttpResponseParser(string & data){
    size_t new_line_index = data.find("\r\n");
    //if there is no new line then throw an exception
    if (new_line_index == string::npos)
        throw -1;
    this->statusLine = data.substr(0,new_line_index);

    //if invalid header found or using an unsupported http version
    //then throw an exception
    if (this->statusLine.substr(0,8) != "HTTP/1.0")
        throw -1;

    string code = this->statusLine.substr(9,3);
    stringstream ss(code);
    ss >> this->statusCode;
    //if invalid or not supported status code found then throw an exception
    if (!(this->statusCode == 200 || this->statusCode == 404))
        throw -1;
    //TODO: try to avoid copying the data into body
    this->body = data.substr(new_line_index,data.length()-1);
}

string & HttpResponseParser::getResponseBody(){
    return this->body;
}

int HttpResponseParser::getStatusCode(){
    return this->statusCode;
}

string HttpResponseParser::getStatusLine(){
    return this->statusLine;
}

//destructor
HttpResponseParser::~HttpResponseParser(){}
