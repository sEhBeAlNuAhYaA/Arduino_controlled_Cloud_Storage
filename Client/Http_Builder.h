#pragma once
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <fstream>
#include <string>


enum Http_Methods {
   GET,
   POST,   
   PUT,
   DEL
};

enum requests_types {
    ArduinoInfo,
    RegistrationCheck,
    Registration,
    Authorisation,
    SendingAFile,
    TakingAFile,
    DeleteAFile,
    ErrorFromServer,
    RequestsError,
    RequestAnswer
};

requests_types req_converter(std::string str_req) {
    if (str_req == "ArduinoInfo") return ArduinoInfo;
    if (str_req == "RegistrationCheck") return RegistrationCheck;
    if (str_req == "Registration") return Registration;
    if (str_req == "Authorisation") return Authorisation;
    if (str_req == "SendingAFile") return SendingAFile;
    if (str_req == "TakingAFile") return TakingAFile;
    if (str_req == "ErrorFromServer") return ErrorFromServer;
    if (str_req == "RequestAnswer") return RequestAnswer;
    return RequestsError;
}

std::string req_back_converter(requests_types type) {
    if (type == ArduinoInfo) return "ArduinoInfo";
    if (type == RegistrationCheck) return "RegistrationCheck";
    if (type == Registration) return "Registration";
    if (type == Authorisation) return "Authorisation";
    if (type == SendingAFile) return "SendingAFile";
    if (type == TakingAFile) return "TakingAFile";
    if (type == ErrorFromServer) return "ErrorFromServer";
    if (type == RequestAnswer) return "RequestAnswer";
    return "RequestsError";
}


void filling_an_array(char* http_request, std::string input_string, int begin) {
    for (int i = begin; i < input_string.size() + begin; i++) {
        http_request[i] = input_string[i - begin];
    }
}

class Http_Builder {
    char* http_builded;
public:

    Http_Builder() {
        this->http_builded = new char[1024];
    }
    
    void Builder(requests_types type_of_request, std::string type_of_file) {
        if (type_of_request == RequestsError) {
            //return this->http_builded;
        }

        if (type_of_request == SendingAFile) {

        }

        if (type_of_request == TakingAFile || type_of_request == DeleteAFile) {

        }
    }

    void Builder(requests_types type_of_request) {
        if (type_of_request == RequestsError) {
            //return this->http_builded;
        }
        
        if (type_of_request == ArduinoInfo || type_of_request == RegistrationCheck) {
            filling_an_array(this->http_builded, "HEADER / ", 0);
            filling_an_array(this->http_builded, req_back_converter(type_of_request), 9);
            filling_an_array(this->http_builded, " HTTP / 1.1\n", 10 + req_back_converter(type_of_request).size());
        }
        if (type_of_request == Registration || type_of_request == Authorisation 
                                            || type_of_request == RequestAnswer 
                                            || type_of_request == ErrorFromServer) {
            if (type_of_request == Registration || type_of_request == Authorisation) {
                filling_an_array(this->http_builded, "PUT / ", 0);
                filling_an_array(this->http_builded, req_back_converter(type_of_request), 6);
                filling_an_array(this->http_builded, " HTTP / 1.1\n", 7 + req_back_converter(type_of_request).size());
                std::string login;
                std::string password;
                std::cout << "Enter your login: ";
                std::cin >> login;
                std::cout << std::endl << "Enter your password: ";
                std::cin >> password;
                filling_an_array(this->http_builded, "\n{\nlogin=", req_back_converter(type_of_request).size() + 20);
                filling_an_array(this->http_builded, login, req_back_converter(type_of_request).size() + 29);
                filling_an_array(this->http_builded, "\npassword=", req_back_converter(type_of_request).size() + 29 + login.size());
                filling_an_array(this->http_builded, password, req_back_converter(type_of_request).size() + 40 + login.size());
                filling_an_array(this->http_builded, "\n}", req_back_converter(type_of_request).size() + 40 + login.size() + password.size());
            }
        }
    }

    char* get_HTTP() {
        return this->http_builded;
    }

    
};


class Http_Parser {
    char* http_parsed;
public:

    Http_Parser(char* http_parsed) {
        this->http_parsed = http_parsed;
    }


};
