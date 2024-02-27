#include <iostream>
#include <boost/asio.hpp>
#include <fstream>
#include <Http_Builder.h>
#include "Client.h"
#include <thread>
#include <vector>
#include <chrono>
#include <File_Sender.h>
#include <functional>


#include "ftxui/component/captured_mouse.hpp"    
#include "ftxui/component/component.hpp"         
#include "ftxui/component/component_options.hpp" 
#include "ftxui/component/screen_interactive.hpp"

using namespace ftxui;

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
				

				auto screen = ScreenInteractive::TerminalOutput();

				std::vector<std::string> entries = {
					"1. Authentification",
					"2. Registration",
				};
				int selected = 0;

				MenuOption option;
				option.on_enter = screen.ExitLoopClosure();
				auto menu = Menu(&entries, &selected, option);

				screen.Loop(menu);

				std::string login;
				std::string password;
				std::cout << "  Login: ";
				std::cin >> login;
				std::cout << "  Password: ";
				std::cin >> password;
				if (selected == 0) {
					this->client.write_http(this->http_builder.Authentification(Authorisation, login, password));
				}
				if (selected == 1) {
					this->client.write_http(this->http_builder.Authentification(Registration, login, password));
				}
				this->http_builder.clearBuilder();
				this->client.clearRequest();
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
				else if (this->http_parser.getPars().keys_map["info"] == "401") {
					std::cout << "WRONG PARAMETRS" << std::endl;
				}
				else if (this->http_parser.getPars().keys_map["info"] == "380") {
					std::cout << "THIS LOGIN IN USE" << std::endl;
				}
				else {
					std::cout << "WRONG PASSWORD" << std::endl;
				}

				this->http_builder.clearBuilder();
				this->http_parser.clearRequest();

			}
			system("cls");

			while (true) {

				auto screen1 = ScreenInteractive::TerminalOutput();

				std::vector<std::string> entries1 = {
					"1. send file",
					"2. take file",
					"3. delete file",
					"4. check storage size",
					"5. Check files"
				};
				int selected1 = 0;

				MenuOption option1;
				option1.on_enter = screen1.ExitLoopClosure();
				auto menu1 = Menu(&entries1, &selected1, option1);

				screen1.Loop(menu1);

				switch (selected1) {
				case 0: {
                    std::string file_name;
					std::cin >> file_name;
					File_Sender file_sender;

					while (true) {

						if (file_sender.return_action() == "start") {
							//init file (open it)
							if (!file_sender.init_File_sender(file_name)) {
								std::cout << "  No such file or directory" << std::endl;
								this->client.clearRequest();
								break;
							}

							if (file_sender.return_action() == "start") {
							this->client.write_http(this->http_builder.Sending_A_File(file_name,
									file_sender.split_file(),
									file_sender.getFileSize(),
									"start"));
							this->http_builder.clearBuilder();
							continue;
							}
							if (file_sender.return_action() == "full") {
								this->client.write_http(this->http_builder.Sending_A_File(file_name,
									file_sender.split_file(),
									file_sender.getFileSize(),
									"full"));

								file_sender.close_file();
								this->http_builder.clearBuilder();
							}
							break;
						}						
						file_sender.set_right_state();

						this->client.write_http(this->http_builder.Sending_A_File(file_name,
							file_sender.split_file(),
							file_sender.getFileSize(),
							file_sender.return_action()));
						this->http_builder.clearBuilder();
						this->client.clearRequest();

						if (file_sender.return_action() == "end") {
							file_sender.close_file();
							break;
						}
					}
					this->client.clearRequest();
					break;
				}
				case 1: {
					std::ofstream out;
					std::string file;
					std::cin >> file;
					out.open(file, std::ios::binary);
                    this->client.write_http(this->http_builder.Take_or_Del(TakingAFile, file));
                    this->http_builder.clearBuilder();
					

                    while (true) {
                        //read message and pars
                        this->client.read_http();
                        this->http_parser.setRequest(this->client.get_Request());
                        this->http_parser.Parsing();
						this->client.clearRequest();
						if (this->http_parser.getPars().keys_map["info"] == "No such file or directory") {
							std::cout << "  No such file or directory" << std::endl;
							this->client.clearRequest();
							break;
						}

						std::string part = this->http_parser.getPars().keys_map["Part-File"];
						double size = stod(this->http_parser.getPars().keys_map["Content-Length"]);

					

						//check for file parts and write those in new file
						if (part == "start" || part == "body") {
							out.write(this->http_parser.getPars().binary_part, FILE_READ_BUFFER);
							std::cout << "(%)" << ((double)out.tellp() / size * 100.0) << std::endl;
							std::cout << "\x1b[1A";
							continue;
						}
						if (part == "full" || part == "end") {
							if (part == "full") {
								out.write(this->http_parser.getPars().binary_part, (int)size);
							}
							else if ((int)size % FILE_READ_BUFFER == 0) {
								out.write(this->http_parser.getPars().binary_part, FILE_READ_BUFFER);
							}
							else {
								out.write(this->http_parser.getPars().binary_part, (int)size % FILE_READ_BUFFER);
							}
							std::cout << "(%)" << ((double)out.tellp() / size * 100.0) << std::endl;
							std::cout << "\x1b[1A";
							out.close();
							break;
						}
					}
					this->client.clearRequest();
					break;
				}
				case 2: {
					std::string file_name;
					std::cin >> file_name;
					this->client.write_http(this->http_builder.Take_or_Del(DeleteAFile, file_name));
					this->http_builder.clearBuilder();
					break;
				} 
				case 3: {
                    this->client.write_http(this->http_builder.Arduino());
                    this->http_builder.clearBuilder();
					break;
				}
				case 4: {
					this->client.write_http(this->http_builder.get_Files_List());
					this->http_builder.clearBuilder();

					this->client.read_http();
					this->http_parser.setRequest(this->client.get_Request());
					this->http_parser.Parsing();
					std::system("cls");
					std::cout << this->http_parser.getPars().keys_map["info"] << std::endl;
					continue;
				}
				}
				
				client.read_http();
				this->http_parser.setRequest(this->client.get_Request());
				this->http_parser.Parsing();
				if (this->http_parser.getPars().keys_map["info"] == "200 OK") {
					std::cout << "-->!<--" << std::endl;
				}
				this->client.clearRequest();
				std::system("cls");
				continue;

			}
		}
        catch (std::system_error& error) {
            std::cout << error.what() << std::endl;
            return;
        }

    }

    void start_ping_pong() {                
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
    }
    catch (std::exception& e)
    { 
        std::cerr << e.what() << std::endl;
    }

    return 0;
} 