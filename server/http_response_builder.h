#ifndef HTTP_GET_RESPONSE_BUILDER_H
#define HTTP_GET_RESPONSE_BUILDER_H

#include <string>
#include <sstream>
using namespace std;

enum ResponseCode{
    OK = 200,
    NOT_FOUND = 404,
};

class HttpResponseBuilder{
    private:
        int responseCode;
        string body;
        string serialization;
    public:
        //constructor using responce code
        HttpResponseBuilder(enum ResponseCode code);

        //returns the repsponse status code
        int getResponseCode();

        //set the body of the response
        void setBody(string body);

        //returns the serialization of the response to be sent
        string getSerialization();

        //destructor
        ~HttpResponseBuilder();
};

#endif
