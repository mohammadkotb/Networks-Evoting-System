#include "http_response_builder.h"

HttpResponseBuilder::HttpResponseBuilder(enum ResponseCode code){
    this->responseCode = code;
}

void HttpResponseBuilder::setBody(string body){
    this->body = body;
}

int HttpResponseBuilder::getResponseCode(){
    return this->responseCode;
}

string HttpResponseBuilder::getSerialization(){
    string result = "HTTP/1.0 ";
    stringstream ss;
    ss << responseCode;
    result += ss.str();
    if (responseCode == 200)
        result += " Document follows";
    else if(responseCode == 404)
        result += " Not Found";
    result += "\r\n";
    result += body;
    return result;
}

HttpResponseBuilder::~HttpResponseBuilder(){
}
