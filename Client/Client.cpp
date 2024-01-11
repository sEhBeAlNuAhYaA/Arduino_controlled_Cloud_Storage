#include <iostream>
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
            client_or_server_color("CLIENT");
            std::cout << "REQUEST WAS SEND" << std::endl;

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
        client_or_server_color("SERVER");
        std::cout << this->current_http_request << " (size: " << len << ")" << std::endl;
    }

    void setRequest(char* request) {
        this->current_http_request = request;
    }

    void clearRequest() {
        memset(this->current_http_request, 0, 1024);
    }
};

class InterFace {
    requests_types current_action;

public:
    
    
    

};


int main(int argc, char* argv[])
{
    try
    {
        //init context and create client class object
        boost::asio::io_context context;
        Client client(context);
        client.read_http();

        Http_Builder build;
        //client.setRequest(build.Builder(ArduinoInfo));
        client.write_http(build.Builder(ArduinoInfo));
        build.clearBuilder();
        client.write_http(build.Builder(TakingAFile,"png","popa.png"));
        for (;;)
        {
            
        }
    }
    catch (std::exception& e)
    { 
        std::cerr << e.what() << std::endl;
    }

  return 0;
} 