#pragma once
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <fstream>
#include <string>


using boost::asio::ip::tcp;


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
    ErrorFromServer,
    RequestsError
};

requests_types req_converter(std::string str_req) {
    if (str_req == "ArduinoInfo") return ArduinoInfo;
    if (str_req == "RegistrationCheck") return RegistrationCheck;
    if (str_req == "Registration") return Registration;
    if (str_req == "Authorisation") return Authorisation;
    if (str_req == "SendingAFile") return SendingAFile;
    if (str_req == "TakingAFile") return TakingAFile;
    if (str_req == "ErrorFromServer") return ErrorFromServer;
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
    return "RequestsError";
}

void insert_in_array(std::string& http_request, std::string insert_string, int begin) {
    http_request += insert_string;
}

class Http_Builder {
    std::string http_request;
public:
    Http_Builder() : http_request{} {
    }
    std::string getBuildRequest() {
        return this->http_request;
    }
    int Fill_Http(int start, std::string name, std::string filetype, std::string file_size, Http_Methods method) {
        insert_in_array(this->http_request, name, start);
        insert_in_array(this->http_request, " HTTP/1.1\n", start + name.size());
        if (method == PUT || method == POST) {
            insert_in_array(this->http_request, "Content-Type: ", start + 11 + name.size());
            if (method == POST) {
                insert_in_array(this->http_request, filetype, start + 26 + name.size());
                insert_in_array(this->http_request, "Content-Length: ", start + filetype.size() + 26 + name.size());
                insert_in_array(this->http_request, file_size, start + filetype.size() + 43 + name.size());
                return start + filetype.size() + 43 + name.size() + file_size.size();
            }
            return  start + 26 + name.size();
        }
        return start + 11 + name.size();
    }

    void Building(std::string name, Http_Methods method, std::string filetype, std::string file_size, requests_types req_type){
        switch (method) {
        case GET: {
            insert_in_array(this->http_request, "GET /", 0);
            Fill_Http(5, name, filetype, file_size, GET);
            break;
        }
        case POST: {
            insert_in_array(this->http_request, "POST /", 0);
            Fill_Http(6, name, filetype, file_size, POST);        
            break;
        }
        case PUT: {
            insert_in_array(this->http_request, "PUT / ", 0);
            Fill_Http(5, name, filetype, file_size, PUT);

            break;
        }
        case DEL: {
            insert_in_array(this->http_request, "DEL /", 0);
            Fill_Http(5, name, filetype, file_size, DEL);
            break;
        }
        }
        for (auto el : this->http_request) {
                std::cout << el;
        }
    }
    void clear_build() {
        this->http_request = "";
    }
};

class Http_Parser {
    std::string http_request;
public:
    Http_Parser() : http_request{} {
    }
    void Parsing() {
        std::string request = "";
        bool itis = false;
        for (int i = 0; i < 30; i++) {
            if (http_request[i] == '/') {
                itis = true;
            }
            if (itis) {
                request += http_request[i+1];
                if (http_request[i + 2] == 'H') {
                    break;
                }
            }
        }
        std::cout << "[SERVER] request: " << request << std::endl;
    }

     void setSend_request(std::string http_request) {
        this->http_request = http_request;
     }
};
