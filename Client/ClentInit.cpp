#include <iostream>
#include <boost/asio.hpp>
#include <fstream>
#include <Http_Builder.h>
#include "bcrypt.h"
#include "Client.h"
#include <thread>
#include <chrono>
#include <File_Sender.h>

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
            //authorization cycle
			while (true) {
				std::string login;
				std::string password;
				std::cout << "LOGIN: ";
				std::cin >> login;
				std::cout << "PASSWORD: ";
				std::cin >> password;

				this->client.write_http(this->http_builder.Authentification(login, password));
  				this->http_builder.clearBuilder();
				this->client.read_http();
                this->http_parser.setRequest(this->client.get_Request());
                this->http_parser.Parsing();

				client_or_server_color("SERVER");

				if (this->http_parser.getPars().keys_map["info"] == "200 OK") {
					std::cout << "YOU LOGGED IN" << std::endl;
					this->http_builder.clearBuilder();
					this->http_parser.clearRequest();
					break;
				}

                this->http_parser.clearRequest();

                std::cout << "WRONG PASSWORD" << std::endl;
			}

            //main cycle with all operations
            int input;
			while (true) {
				std::cout << "1. send file;\n2. take file;\n3. delete file;\n4. check storage size;\ninput: " << std::endl;
				std::cin >> input;
				switch (input) {
				case 1: {
                    std::string file_name;
                    std::cin >> file_name;
					File_Sender file_sender;
                    
					while (true) {
                        

						if (file_sender.return_action() == "start" || file_sender.return_action() == "full") {
							//init file (open it)
							file_sender.init_File_sender(file_name);
						}
						this->http_builder.Sending_A_File(file_name,
							file_sender.return_action(), file_sender.getFileSize(),
							file_sender.split_file());

						if (file_sender.return_action() == "end") {
							file_sender.close_file();
						}
						this->client.write_http(this->http_builder.get_HTTP());
    
                    }
									
                    
                    this->http_builder.clearBuilder();
					break;
				}
				case 2: {
                    std::ofstream out;
                    std::string file;
                    std::cin >> file;
                    out.open(file, std::ios::binary);
                    while (true) {
                        //set message and send it
                        
                        this->client.write_http(this->http_builder.Builder(TakingAFile, nullptr, file, ""));
                        this->http_builder.clearBuilder();

                        //read message and pars
                        this->client.read_http();
                        this->http_parser.setRequest(this->client.get_Request());
                        this->http_parser.Parsing();
                        this->client.clearRequest();
                        
                        //check for file parts and write those in new file
                        if (this->http_parser.getPars().keys_map["Part-File"] == "start" || this->http_parser.getPars().keys_map["Part-File"] == "body") {
							out.write(this->http_parser.getPars().binary_part, FILE_READ_BUFFER);
                            std::cout << ((double)out.tellp() / (stod(this->http_parser.getPars().keys_map["Content-Length"]))) * 100.0 << "%" << std::endl;
							std::cout << "\x1b[1A";
							this->http_parser.clearRequest();
							continue;
						}
						if (this->http_parser.getPars().keys_map["Part-File"] == "full" || this->http_parser.getPars().keys_map["Part-File"] == "end") {
							out.write(this->http_parser.getPars().binary_part, stoi(this->http_parser.getPars().keys_map["Content-Length"]) % FILE_READ_BUFFER);
							this->http_parser.clearRequest();
							this->client.write_http(this->http_builder.Builder_Answer(RequestAnswer, "200 OK"));
							this->http_builder.clearBuilder();
							out.close();
							break;
						}
					}
					break;
				}
				case 3: {
                    //this->client.write_http(this->http_builder.Builder(DeleteAFile, "", "image.png", ""));
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