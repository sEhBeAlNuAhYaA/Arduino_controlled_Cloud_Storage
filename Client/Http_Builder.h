#pragma once
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <fstream>
#include <string>

using boost::asio::ip::tcp;

void insert_in_array(boost::array <char,16000>& http_request, std::string insert_string, int begin) {
    for (int i = begin; i <= insert_string.size() + begin; i++) {
        http_request[i] = insert_string[i - begin];
    }
}


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
    ErrorFromServer
};


class Http_Builder {
    boost::array <char, 16000> http_request;
public:
    Http_Builder() : http_request{} {
    }

    void Fill_Http(int start, std::string name, std::string filetype, std::string file_size, Http_Methods method) {
        insert_in_array(this->http_request, name, start);
        insert_in_array(this->http_request, " HTTP/1.1\n", start + name.size());
        if (method == PUT || method == POST) {
            insert_in_array(this->http_request, "Content-Type: ", start + 11 + name.size());
            if (method == POST) {
                insert_in_array(this->http_request, filetype, start + 26 + name.size());
                insert_in_array(this->http_request, "Content-Length: ", start + filetype.size() + 26 + name.size());
                insert_in_array(this->http_request, file_size, start + filetype.size() + 43 + name.size());
            }
            
        }
    }

    boost::array <char, 16000> Building(std::string name, Http_Methods method, std::string filetype, std::string file_size){
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
        return this->http_request;
    }
};

class Http_Parser {
    boost::array <char,16000> http_request;
public:
    Http_Parser() : http_request{} {
    }
    
    void Parsing(boost::array <char, 16000> sended_request, requests_types type) {
        
        
    }
};
