#include "server_manager.h"
#include <fstream>
#include <iomanip>
#include <iostream>

using std::ifstream;
using std::iostream;

ServerManager::ServerManager(){
    pthread_mutex_init(&users_map_mutex, NULL);
}

ServerManager::~ServerManager(){
    pthread_mutex_destroy(&users_map_mutex);
}

void ServerManager::prepare_response_with_code(string* prepared_response, const string& file_path, ResponseCode code) {
    ifstream input_file(file_path.c_str());
    string prepared_string = string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    ResponseCode response_code(code);
    HttpResponseBuilder response_builder(response_code);
    response_builder.setBody(prepared_string);
    *prepared_response = response_builder.getSerialization();
}

void ServerManager::prepare_response_from_file(string* prepared_response, const string& file_path) {
    ResponseCode code = ResponseCode(OK);
    prepare_response_with_code(prepared_response, file_path, code);
}

void ServerManager::handle_request(string* response, const string& request_data) {
    HttpGetRequestParser get_request_parser(request_data);
    string required_file_name = get_request_parser.getRequiredFileName();
    if (required_file_name == "/login.html") {
        prepare_response_from_file(response, string("../htdocs/login.html"));
    } else if(required_file_name == "/login.php") {
        if (valid_user(request_data)) {
           if (users_map_[get_request_parser.getParameter("username")].getType() == "\"voter\""){
                prepare_response_from_file(response, string("../htdocs/voter_home.html"));
            } else {
                prepare_response_from_file(response, string("../htdocs/candidate_home.html"));
            }
        } else {
            prepare_response_from_file(response, string("../htdocs/invalid_data.html"));
        }
    } else if (required_file_name == "/signup.html") {
        prepare_response_from_file(response, string("../htdocs/signup.html"));
    } else if (required_file_name == "/signup.php") {
        VALIDATION_CODE validation_code = can_add_user(request_data);
        if (validation_code == VALID) {
            prepare_response_from_file(response, "../htdocs/login.html");
        } else if (validation_code == USED_USERNAME) {
            prepare_response_from_file(response, "../htdocs/invalid_username.html");
        } else if (validation_code == MISSING_FIELD) {
            prepare_response_from_file(response, "../htdocs/missing_field.html");
        } else {
            prepare_response_from_file(response, "../htdocs/wrong_type.html");
        }
    } else {
        ResponseCode code(NOT_FOUND);
        prepare_response_with_code(response, "../htdocs/404.html", code);
    }
}

bool ServerManager::valid_user(const string& request_data) {
    HttpGetRequestParser get_request_parser(request_data);
    if(users_map_.count(get_request_parser.getParameter("username")) == 0)
        return false;
    User system_user = users_map_[get_request_parser.getParameter("username")];
    if(get_request_parser.getParameter("password") != system_user.getPassword())
        return false;
    return true;
}

VALIDATION_CODE ServerManager::can_add_user(const string& request_data) {
    HttpGetRequestParser get_request_parser(request_data);
    pthread_mutex_lock(&users_map_mutex);
    // Extract paramters from GET request
    string username = get_request_parser.getParameter("username");
    string password = get_request_parser.getParameter("password");
    string type = get_request_parser.getParameter("type");
    // check for empty parameters.
    if (username == "\"\"" || password == "\"\"" || type == "\"\"") {
        pthread_mutex_unlock(&users_map_mutex);
        return MISSING_FIELD;
    }
    // type should be either a voter or a candidate.
    if (type != "\"voter\"" && type != "\"candidate\"") {
        pthread_mutex_unlock(&users_map_mutex);
        return WRONG_TYPE;
    }
    if(users_map_.count(username) == 0) {
        User new_user(type, username, password);
        users_map_[username] = new_user;
        pthread_mutex_unlock(&users_map_mutex);
        return VALID;
    } else {
	pthread_mutex_unlock(&users_map_mutex);
        return USED_USERNAME;
    }
}
