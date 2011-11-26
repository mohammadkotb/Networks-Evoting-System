#ifndef SERVER_MANAGER_H
#define SERVER_MANAGER_H

#include <string>
#include <map>
#include "http_get_request_parser.h"
#include "http_response_builder.h"
#include "user.h"

using std::string;
using std::map;

class ServerManager {

public:

    // handle the client request. The function accepts the request_data as input and
    // sets the output parameter "response" with the serialized server response.
    void handle_request(string* response, const string& request_data);


private:
    // Contains the data for all the system users. Maps each username to a user object.
    map<string, User> users_map_;

    // Similar to prepare_response_from_file. However, the response code is set to the input parameter: "code".
    void prepare_response_with_code(string *prepared_response, const string& file_path, ResponseCode code);

    // Reads a file from "file_path" and serializes its content as a string. The string will be used as the body
    // for the prepared_response. The response code is set by default to OK.
    void prepare_response_from_file(string *prepared_response, const string& file_path);

    // Adds a new user to the system. Updates the users_map_ accordingly. Returns true when
    // the user is added successfully. False otherwise.
    bool can_add_user(const string& request_data);

    // Checks whether the user data is valid or not.
    bool valid_user(const string& request_data);

};

#endif // SERVER_MANAGER_H
