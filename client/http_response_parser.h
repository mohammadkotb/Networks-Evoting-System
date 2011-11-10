#ifndef HTTP_RESPONSE_PARSER_H
#define HTTP_RESPONSE_PARSER_H

#include <string>
#include <iostream>
#include <sstream>

/*
 * constructs an http response from the row data
 */
class HttpResponseParser{
    private:
        std::string statusLine;
        std::string body;
        int statusCode;
    public:
        //constructor using row ascii data
        HttpResponseParser(std::string & data);

        //destructor
        ~HttpResponseParser();

        //returns the response body
        std::string & getResponseBody();

        //returns the status line
        std::string getStatusLine();

        //returns the status code
        int getStatusCode();
};

#endif // HTTP_RESPONSE_PARSER_H
