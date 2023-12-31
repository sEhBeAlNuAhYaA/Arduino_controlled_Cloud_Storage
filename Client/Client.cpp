﻿#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <string>
#include "Http_Builder.h"

using boost::asio::ip::tcp;




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
    Client(boost::asio::io_context& context)
        : context_(context), resolver_(context), socket_(context), endpoints_(resolver_.resolve("127.0.0.1", "80")) {
        boost::asio::connect(socket_, endpoints_);
        this->current_http_request = new char[1024];
    }
    //sync write
    void write_http(char* http_request) {
        socket_.write_some(boost::asio::mutable_buffer(http_request, 1024));
        if (!error) {
            std::cout << "[CLIENT] REQUEST WAS SEND" << std::endl;
        }
        else {
            std::cout << error.what() << std::endl;
        }
    }
    //sync read
    void read_http() {
        size_t len = socket_.read_some(boost::asio::mutable_buffer(this->current_http_request, 1024));
        if (error) {
            std::cout << error.what() << std::endl;
        }
 
        std::cout << std::string(this->current_http_request) << std::endl;
        std::cout << "Size of cathed message: " << len << std::endl;
    }

    void setRequest(char* request) {
        this->current_http_request = request;
    }

    void clearRequest() {
        memset(this->current_http_request, 0, 1024);
    }
};


int main(int argc, char* argv[])
{
    try
    {
        //init context and create client class object
        boost::asio::io_context context;
        Client client(context);

        client.read_http();
        
               
        //client.registration_check(http_build, http_pars);

 
                
        for (;;)
        {
            Http_Builder build;
            int g;
            std::cin >> g;
            if (g == 1) {
                //client.setRequest(request);        
                client.write_http(build.Builder(ArduinoInfo));
                client.clearRequest();
            }
            if (g == 2) {
                client.write_http(build.Builder(RegistrationCheck));
                client.clearRequest();
            }


        }
    }
    catch (std::exception& e)
    { 
        std::cerr << e.what() << std::endl;
    }

  return 0;
} 