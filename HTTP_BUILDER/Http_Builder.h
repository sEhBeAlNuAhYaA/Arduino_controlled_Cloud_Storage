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



class Http_Builder {
    char* http_builded;
    int current_length;
public:

    Http_Builder() {
        this->http_builded = new char[1024];
        this->current_length = 0;
    }
    
    void filling_an_array(std::string input_string) {
        for (int i = this->current_length; i < input_string.size() + this->current_length; i++) {
            http_builded[i] = input_string[i - this->current_length];
        }
        this->current_length+=input_string.size();
    }

    char* Builder(requests_types type_of_request, std::string type_of_file) {
        if (type_of_request == RequestsError) {
            return this->http_builded;
        }

        if (type_of_request == SendingAFile) {

        }

        if (type_of_request == TakingAFile || type_of_request == DeleteAFile) {

        }
    }
    
    char* Builder_Answer(requests_types type_of_request,std::string message) {
        if (type_of_request == RequestAnswer) {
            filling_an_array("HEADER / ");
            filling_an_array(req_back_converter(type_of_request));
            filling_an_array(" HTTP / 1.1\n");
            filling_an_array("\n{\n");
            filling_an_array(message);
            filling_an_array("\n}");
            return this->http_builded;
        }
        return this->http_builded;
    }

    char* Builder(requests_types type_of_request) {
        if (type_of_request == RequestsError) {
            return this->http_builded;
        }
        if (type_of_request == ArduinoInfo || type_of_request == RegistrationCheck) {
            filling_an_array("HEADER / ");
            filling_an_array(req_back_converter(type_of_request));
            filling_an_array(" HTTP / 1.1\n");
            return this->http_builded;
        }
        if (type_of_request == Registration || type_of_request == Authorisation 
                                            || type_of_request == RequestAnswer 
                                            || type_of_request == ErrorFromServer) {
            if (type_of_request == Registration || type_of_request == Authorisation) {
                filling_an_array("PUT / ");
                filling_an_array(req_back_converter(type_of_request));
                filling_an_array(" HTTP / 1.1\n");
                std::string login;
                std::string password;
                std::cout << "Enter your login: ";
                std::cin >> login;
                std::cout << "Enter your password: ";
                std::cin >> password;
                filling_an_array("\n{\nlogin=");
                filling_an_array(login);
                filling_an_array("\npassword=");
                filling_an_array(password);
                filling_an_array("\n}");
                return this->http_builded;
            }
        }
    }

    char* get_HTTP() {
        return this->http_builded;
    }

    int getSize() {
        return this->current_length;
    }

    
};


class Http_Parser {
    char* http_parsed;
public:

    Http_Parser(char* http_parsed) {
        this->http_parsed = http_parsed;
    }


};
