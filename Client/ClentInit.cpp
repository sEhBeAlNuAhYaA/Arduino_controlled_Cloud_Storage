#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <string>
#include <fstream>
#include <Http_Builder.h>
#include <boost/beast/core/detail/base64.hpp>
#include "bcrypt.h"
#include "Client.h"

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