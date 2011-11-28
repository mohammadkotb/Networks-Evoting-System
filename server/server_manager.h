#ifndef SERVER_MANAGER_H
#define SERVER_MANAGER_H

#include <string>
#include <map>
#include "http_get_request_parser.h"
#include "http_response_builder.h"
#include "user.h"

using std::string;
using std::map;

// Enumerated types that indicates the result of the can_add_user function.
enum VALIDATION_CODE{
    VALID = 0,
    USED_USERNAME = 1,
    MISSING_FIELD = 2,
    WRONG_TYPE = 3
};

class ServerManager {

public:

    // Handles the client request. The function accepts the request_data as input and
    // sets the output parameter "response" with the serialized server response.
    void handle_request(string* response, const string& request_data);

   // constructor
   ServerManager();

   // destructor
   virtual ~ServerManager();

private:
    // Contains the data for all the system users. Maps each username to a user object.
    map<string, User> users_map_;

    // mutex for users_map_
    pthread_mutex_t users_map_mutex;

    // Similar to prepare_response_from_file. However, the response code is set to the input parameter: "code".
    void prepare_response_with_code(string *prepared_response, const string& file_path, ResponseCode code);

    // Reads a file from "file_path" and serializes its content as a string. The string will be used as the body
    // for the prepared_response. The response code is set by default to OK.
    void prepare_response_from_file(string *prepared_response, const string& file_path);

    // Checks whether or not the user can be added to the system. udates the users_map_ accordingly.
    VALIDATION_CODE can_add_user(const string& request_data);

    // Checks whether the user data is valid or not.
    bool valid_user(const string& request_data);

};

#endif // SERVER_MANAGER_H
