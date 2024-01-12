#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <string>
#include <fstream>
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
    void write_http(char* http_req) {
        socket_.write_some(boost::asio::mutable_buffer(http_req, strlen(http_req)));
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

    void clearRequest() {
        memset(this->current_http_request, '\0', 1024);
    }

    char* get_Request() {
        return this->current_http_request;
    }
};

class InterFace {
    requests_types previous_action;
    Http_Builder http_builder;
    Http_Parser http_parser;
    Client client;
public:
    InterFace(boost::asio::io_context& context)
        :client(context), previous_action(Registration),http_builder(), http_parser(){
        start_ping_pong();
    }

    void start_ping_pong() {
        client.write_http(this->http_builder.Builder(this->previous_action));
        client.read_http();

        http_parser.setRequest(client.get_Request());
        http_parser.Parsing().type;

        switch (this->previous_action) {
        case Registration: {
            
            break;
        }
        case Authorisation: {

            break;
        }
        case RequestAnswer: {

            break;
        }
        case TakingAFile: {

            break;
        }
        case DeleteAFile: {

            break;
        }
        case SendingAFile: {

            break;
        }
        case ArduinoInfo: {

            break;
        }
        }
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