#ifndef HTTP_GET_REQUEST_BUILDER_H
#define HTTP_GET_REQUEST_BUILDER_H

#include <string>
#include <map>

class HttpGetRequestBuilder{
    private:
        //requested file path
        std::string file_path;
        //parameters
        std::map<std::string,std::string>* parameters;
    public:
        //constructor using file path
        HttpGetRequestBuilder(std::string file_path);

        //destructor
        ~HttpGetRequestBuilder();

        //add a parameter to the request
        void addParameter(std::string name,std::string value);

        //returns parameter value
        std::string getParameter(std::string name);

        //returns the request serialization
        std::string getSerialization();
};


#endif // HTTP_GET_REQUEST_BUILDER_H
