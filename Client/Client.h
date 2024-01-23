#pragma once
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <string>
#include <fstream>
#include <Http_Builder.h>
#include <boost/beast/core/detail/base64.hpp>
#include "bcrypt.h"

using boost::asio::ip::tcp;

extern int BUFFER;
extern int FILE_READ_BUFFER;

class Client {
    //needed params for make connection
    boost::asio::io_context& context_;
    tcp::resolver resolver_;
    tcp::socket socket_;
    tcp::resolver::results_type endpoints_;
    char* current_http_request;
    boost::system::error_code error;
public:
    //constructor
    Client(boost::asio::io_context& context);    //sync write
    void write_http(char* http_req);    //sync read
    void read_http();
    void clearRequest();
    char* get_Request();
};