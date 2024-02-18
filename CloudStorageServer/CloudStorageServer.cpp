#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <Http_Builder.h>
#include <string>
#include "Http_processing.h"
#include "Arduino_Connector.h"

int BUFFER = 10000;

using boost::asio::ip::tcp;

static int client_ID_counter = 0;

enum client_state {
    on_read,
    on_write,
    none
};

class NEW_connection
	: public std::enable_shared_from_this<NEW_connection>
{
	tcp::socket socket_;
	int client_ID;
	std::string user_name;
	char* http_request;
	client_state cl_state;

	boost::system::error_code error;

	http_processing http_process;
	Http_Builder server_builder;
	Http_Parser server_parser;

    Space_Saver space_saver;
public:
	typedef std::shared_ptr<NEW_connection> pointer;

    static pointer create(boost::asio::io_context& context) {
        return pointer(new NEW_connection(context));
    }
    //get socket
    tcp::socket& get_socket() {
        return socket_;
    }

    void start_write(){
        socket_.async_write_some(boost::asio::mutable_buffer(this->http_request, BUFFER),
            boost::bind(&NEW_connection::handle_write, shared_from_this(), 
                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)); 
    }

    void start_read() {
        socket_.async_read_some(boost::asio::mutable_buffer(this->http_request, BUFFER),
            boost::bind(&NEW_connection::handle_read, shared_from_this(),
               boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));         
    }

    int getID() {
        return this->client_ID;
    }

    void setHttp_request(char* input_message) {
        memcpy_s(this->http_request, BUFFER, input_message, BUFFER);
    }
    
private:
    void clearRequest() {
        memset(this->http_request, '\0', BUFFER);
    }

    NEW_connection(boost::asio::io_context& context)
        : socket_(context) {

        client_ID_counter++;
        client_ID = client_ID_counter;
        this->http_request = new char[BUFFER];
        memset(this->http_request, '\0', BUFFER);
        this->cl_state = none;
       
    }

    
    void handle_write(const boost::system::error_code& err, size_t transferred) {
        if (!err) {
            if (this->cl_state == none) {
                this->start_read();
            }
        }
        else {
            std::cout << err.what() << std::endl;
        }
    }

    void handle_read(const boost::system::error_code& err, size_t transferred) {
                
        if (!err) {  
            //std::cout << this->http_request << std::endl;
            this->server_parser.setRequest(this->http_request);
			this->server_parser.Parsing();
            
			switch (this->server_parser.getPars().type) {
			case Authorisation: {
				this->http_process.complicated_requests_processing(this->server_parser.getPars(), this->user_name, this->space_saver);
				this->setHttp_request(this->http_process.builder.get_HTTP());
				this->http_process.builder.clearBuilder();
				break;
			}
			case Registration: {
				this->http_process.complicated_requests_processing(this->server_parser.getPars(), this->user_name, this->space_saver);
				this->setHttp_request(this->http_process.builder.get_HTTP());
				this->http_process.builder.clearBuilder();
				break;
			}
			case TakingAFile: {
				if (!this->space_saver.check_a_file(this->server_parser.getPars().keys_map["Content-Name"], this->user_name)) {
					this->server_builder.Builder_Answer("NO SUCH FILE OR DIRECTORY");
					break;
				}
				else {
					this->cl_state = on_write;
					Files_Operator(this->server_parser.getPars().type, this->http_process, this->server_parser.getPars());
					break;
				}
			}
			case SendingAFile: {
				this->cl_state = on_read;
				Files_Operator(this->server_parser.getPars().type, this->http_process, this->server_parser.getPars());
				break;
			}
			case DeleteAFile: {
				if (this->space_saver.check_a_file(this->server_parser.getPars().keys_map["Content-Name"], this->user_name)){
					this->space_saver.rem_file_from_db(this->server_parser.getPars().keys_map["Content-Name"], this->user_name);
					this->server_builder.Builder_Answer("200 OK");
                }
                else {
                    this->server_builder.Builder_Answer("NO SUCH FILE OR DIRECTORY");
                }
				this->setHttp_request(this->server_builder.get_HTTP());
				this->server_builder.clearBuilder();
				break;
			}
			case GetFilesList: {
				this->server_builder.Files_List(space_saver.update_own_list(this->user_name));
				this->setHttp_request(this->server_builder.get_HTTP());
				this->server_builder.clearBuilder();
				break;
			}
			case ArduinoInfo: {
				Arduino_Connection::send_Arduino_message(Arduino_Connection::serialize_aduino_answer(std::to_string(this->client_ID),
                    std::to_string(this->space_saver.space_counter(this->user_name))));
				this->server_builder.Builder_Answer("200 OK");
				this->setHttp_request(this->server_builder.get_HTTP());
				this->server_builder.clearBuilder();
				break;
			}
			}

			if (this->cl_state == none) {
				this->start_write();
			}

		}
		else {
			if (err.value() == 10054) {
				client_or_server_color("SERVER");
                std::cout << "CLIENT(ID:" << this->client_ID << ") disconnected" << std::endl;
			}
			else {
				std::cout << err.what() << std::endl;
			}

		}
    
                
    }

	void Files_Operator(requests_types current_request, http_processing& http_process, parsed_request parsed_req) {

		if (current_request == SendingAFile) {
			/////////////////////////////////////////////////////////////////////
			http_process.complicated_requests_processing(parsed_req, this->user_name, this->space_saver);
			/////////////////////////////////////////////////////////////////////
			if (parsed_req.keys_map["Part-File"] == "end" ||
				parsed_req.keys_map["Part-File"] == "full") {
				// set ANSWER
				server_builder.Builder_Answer("200 OK");
				this->setHttp_request(server_builder.get_HTTP());
				server_builder.clearBuilder();
				//set state
				this->cl_state = none;
				//clear server parser
                this->space_saver.add_file_to_db(this->server_parser.getPars().keys_map["Content-Name"], this->user_name);
				
				//binary part of file comparing
				return;
			}
		
			this->start_read();
		}
		if (current_request == TakingAFile) {
			while (true) {
                /////////////////////////////////////////////////////////////////////
				http_process.complicated_requests_processing(parsed_req, this->user_name, this->space_saver);
                /////////////////////////////////////////////////////////////////////
				this->setHttp_request(http_process.builder.get_HTTP());
				this->start_write();

				if (http_process.file_sender.getFileSize() == 0) {
					http_process.builder.clearBuilder();
					client_or_server_color("SERVER");
					std::cout << "File sent" << std::endl;

					server_builder.Builder_Answer("200 OK");
					this->setHttp_request(server_builder.get_HTTP());
					server_builder.clearBuilder();

					this->cl_state = none;
					break;
				}
				http_process.builder.clearBuilder();
			}
		}
	}
};



class single_client_data {
    NEW_connection::pointer client;
    int ID;
public:
    single_client_data(NEW_connection::pointer client, int ID) {
        this->client = client;
        this->ID = ID;
    }
    int get_ID() {
        return ID;
    }
};

class Client_Vector {
	std::vector <single_client_data> client_vector;
public:
	Client_Vector() {
	}
	void add_new_client(single_client_data client) {
		this->client_vector.push_back(client);
	}
	void delete_a_client(int ID) {
		int counter = 0;
		for (auto client_in_vector : this->client_vector) {
			if (ID == client_in_vector.get_ID()) {
				this->client_vector.erase(this->client_vector.begin() + counter);
				break;
			}
			counter++;
		}
    }
    int getClientsCounter() {
        return this->client_vector.size();
    }
};



class Cloud_Storage {
    //private context and acceptor
    boost::asio::io_context& context_;
    tcp::acceptor acceptor_;
    Client_Vector client_vector;
public:
    Cloud_Storage(boost::asio::io_context& context)
        :context_(context), 
        acceptor_(context, tcp::endpoint(tcp::v4(),80)) 
    {
        start_accept();
        std::filesystem::create_directories("Files");
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
            new_connection->start_read();
            single_client_data new_client(new_connection, new_connection->getID());
            this->client_vector.add_new_client(new_client);
			Arduino_Connection::send_Arduino_message(Arduino_Connection::serialize_main_frame(std::to_string(this->client_vector.getClientsCounter())));
            std::cout << this->client_vector.getClientsCounter() << std::endl;
            client_or_server_color("SERVER");
            std::cout << "CLIENT(ID:" << new_connection->getID() << ") joined the server" << std::endl;
            start_accept();
        }
        else {
            this->client_vector.delete_a_client(new_connection->getID());
            std::cout << this->client_vector.getClientsCounter() << std::endl;
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
