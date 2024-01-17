﻿#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <queue>
#include <Http_Builder.h>
#include "Http_processing.h"

using boost::asio::ip::tcp;

static int client_ID_counter = 0;

Http_Builder server_builder;
Http_Parser server_parser;

class NEW_connection 
    : public std::enable_shared_from_this<NEW_connection>
{
    tcp::socket socket_;
    int client_ID;
    char *http_request;
    boost::system::error_code error;
    http_processing http_process;

public:
    typedef std::shared_ptr<NEW_connection> pointer;
    
    static pointer create(boost::asio::io_context& context) {
        return pointer(new NEW_connection(context));
    }
    //get socket
    tcp::socket& get_socket() {
        return socket_;
    }

    void send_message(){
        socket_.async_write_some(boost::asio::mutable_buffer(this->http_request, strlen(this->http_request)),
            boost::bind(&NEW_connection::handle_write, shared_from_this(), 
                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)); 
    }

    void read_message() {
        socket_.async_read_some(boost::asio::mutable_buffer(this->http_request, 1024),
            boost::bind(&NEW_connection::handle_read, shared_from_this(),
               boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));         
    }

    int getID() {
        return this->client_ID;
    }

    void setHttp_request(std::string input_string) {
        memcpy_s(this->http_request, input_string.size(), input_string.c_str(), input_string.size());
    }
    
private:
    void clearRequest() {
        memset(this->http_request, '\0', 1024);
    }

    NEW_connection(boost::asio::io_context& context)
        : socket_(context){

        client_ID_counter++;
        client_ID = client_ID_counter;
        this->http_request = new char[1024];
        memset(this->http_request, '\0', 1024);
    }

    
    void handle_write(const boost::system::error_code& err, size_t transferred) {
        if (!err) {
            client_or_server_color("SERVER");
            std::cout << "MESSAGE WAS SENT ";
            client_or_server_color("CLIENT");
            color_client_id(this->client_ID);
            std::cout << std::endl;
            this->clearRequest();
        }
        else {
            std::cout << err.what() << std::endl;
        }
    }

    void handle_read(const boost::system::error_code& err, size_t transferred) {
                
        if (!err) {
            //copy into queue
            char* to_queue = new char[1024];
            memcpy_s(to_queue, strlen(this->http_request), this->http_request, strlen(this->http_request));
            request_queue.push(to_queue);
            //clear a http_request
            this->clearRequest();
  
            
            client_or_server_color("SERVER");
            std::cout << "MESSAGE WAS RECEIVED FROM ";
            client_or_server_color("CLIENT");
            color_client_id(this->client_ID);
            std::cout << std::endl << request_queue.back() << std::endl;
            
            //next_iterarion
            read_message();
            //analyze requests
            this->http_process.processing_client_requests();
            this->setHttp_request(this->http_process.builder.get_HTTP());
            this->send_message();
            
        }
        else {
            if (err.value() == 10054) {
                client_or_server_color("SERVER");
                std::cout << "CLIENT(ID:" << this->client_ID << ") disconnected" << std::endl;
                return;
            }
            else {
                std::cout << err.what() << std::endl;
            }
            
        }
                
    }
};


class Cloud_Storage {
    //private context and acceptor
    boost::asio::io_context& context_;
    tcp::acceptor acceptor_;
public:
    Cloud_Storage(boost::asio::io_context& context)
        :context_(context), 
        acceptor_(context, tcp::endpoint(tcp::v4(),80)) 
    {
        start_accept();
    }
private:
    void start_accept() {
        try {
            NEW_connection::pointer new_connection = NEW_connection::create(this->context_);
            acceptor_.async_accept(new_connection->get_socket(),
                boost::bind(&Cloud_Storage::handle_accept, this, 
                    new_connection, boost::asio::placeholders::error));
        }
        catch (boost::system::error_code& error) {
            std::cout << error.what() << std::endl;
        }
        
    }

    void handle_accept(NEW_connection::pointer new_connection, const boost::system::error_code& error) {
        if (!error) {
            new_connection->setHttp_request(server_builder.Builder_Answer(RequestAnswer, "YOU ARE CONNECTED!"));
            new_connection->send_message();
            client_or_server_color("SERVER");
            std::cout << "CLIENT(ID:" << new_connection->getID() << ") joined the server" << std::endl;
            new_connection->read_message();
            start_accept();
        }
        else {
            std::cout << error.what() << std::endl;
            return;
        }
    }
};


int main()
{ 
    
    boost::asio::io_context context;
    Cloud_Storage storage(context);

    client_or_server_color("SERVER");
    std::cout << "START" << std::endl;

    context.run();
    
    return 0;
}
