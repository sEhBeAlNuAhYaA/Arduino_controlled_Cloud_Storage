#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <string>
#include <fstream>
#include <Http_Builder.h>
#include <boost/beast/core/detail/base64.hpp>
#include "bcrypt.h"
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
        memset(this->current_http_request, '\0', 1024);
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
        
        memset(reinterpret_cast<void*>(this->current_http_request + len), '\0', 1024 - len);
        client_or_server_color("SERVER");
        //std::cout << this->current_http_request << " (size: " << len << ")" << std::endl;
        
    }

    void clearRequest() {
        memset(this->current_http_request, '\0', 1024);
    }
    
    char* get_Request() {
        return this->current_http_request;
    }
};
class ClientInit {
    requests_types previous_action;
    Http_Builder http_builder;
    Http_Parser http_parser;
    Client client;
    
public:
    ClientInit(boost::asio::io_context& context)
        :client(context), previous_action(Authorisation),http_builder(), http_parser(){
        start_ping_pong();
        
    }

	void cycle() {
		try {
			int input;
			std::string login;
			std::string password;
			std::cin >> login;
			std::cin >> password;
			this->client.write_http(this->http_builder.Authentification(login, password));
			this->http_builder.clearBuilder();
			this->client.read_http();
			this->http_parser.setRequest(this->client.get_Request());
			this->http_parser.Parsing();
			while (true) {
				std::cout << this->http_parser.getPars().keys_map["info"] << std::endl;
				if (this->http_parser.getPars().keys_map["info"] == "200 OK") {
					this->http_builder.clearBuilder();
					this->http_parser.clearRequest();
					break;
				}
				this->http_parser.clearRequest();
				std::string login;
				std::string password;
				std::cin >> login;
				std::cin >> password;
				this->client.write_http(this->http_builder.Authentification(login, password));
				this->http_builder.clearBuilder();
				this->client.read_http();
				this->http_parser.setRequest(this->client.get_Request());
				this->http_parser.Parsing();
			}

			while (true) {
                

				std::cout << "1. send file;\n2. take file;\n3. delete file;\n4. check storage size;\ninput: " << std::endl;
				std::cin >> input;
				switch (input) {
				case 1: {
					this->client.write_http(this->http_builder.Builder(SendingAFile, "png", "image.png"));
					this->http_builder.clearBuilder();
					break;
				}
				case 2: {
                    this->client.write_http(this->http_builder.Builder(TakingAFile, "", "image.png"));
                    this->http_builder.clearBuilder();
                    break;
				}
				case 3: {
                    this->client.write_http(this->http_builder.Builder(DeleteAFile, "", "image.png"));
                    this->http_builder.clearBuilder();
					break;
				}
				case 4: {
                    this->client.write_http(this->http_builder.Builder(ArduinoInfo));
                    this->http_builder.clearBuilder();
					break;
				}
				}

                this->client.read_http();
                this->http_parser.setRequest(this->client.get_Request());
                this->client.clearRequest();
                this->http_parser.Parsing();
                std::cout << this->http_parser.getPars().keys_map["info"] << std::endl;
                this->http_parser.clearRequest();
                std::cout << "\x1b[2J\x1b[H";

			}
        }
        catch (std::system_error& error) {
            std::cout << error.what() << std::endl;
            return;
        }

    }

    void start_ping_pong() {
        client.read_http();
        this->http_parser.setRequest(client.get_Request());
        std::cout << this->http_parser.Parsing().keys_map["info"] << std::endl;
        this->http_parser.clearRequest();
        this->cycle();
        std::cout << "pingpong end" << std::endl;
    }
};


int main(int argc, char* argv[]){

    try
    {
        //init context and create client class object
        boost::asio::io_context context;
        ClientInit interface(context);
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