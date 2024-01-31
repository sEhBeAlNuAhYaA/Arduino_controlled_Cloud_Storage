#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <queue>
#include <Http_Builder.h>
#include "Http_processing.h"

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
	char* http_request;
	boost::system::error_code error;
	http_processing http_process;
	Http_Builder server_builder;
	Http_Parser server_parser;
	std::string user_name;
    client_state cl_state;
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
        : socket_(context){

        client_ID_counter++;
        client_ID = client_ID_counter;
        this->http_request = new char[BUFFER];
        memset(this->http_request, '\0', BUFFER);
        this->cl_state = none;
    }

    
    void handle_write(const boost::system::error_code& err, size_t transferred) {
        if (!err) {
            //client_or_server_color("SERVER");
            //std::cout << "MESSAGE WAS SENT ";
			//client_or_server_color("CLIENT");
			//color_client_id(this->client_ID);
            //std::cout << std::endl;
			this->clearRequest();
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
            std::cout << this->http_request << std::endl;
            this->server_parser.setRequest(this->http_request);
			this->server_parser.Parsing();
            
			switch (this->server_parser.getPars().type) {
			case Authorisation: {
				this->http_process.processing_client_requests(this->server_parser.getPars(), this->user_name);
				this->server_parser.clearRequest();
				this->setHttp_request(this->http_process.builder.get_HTTP());
				this->http_process.builder.clearBuilder();
				break;
			}
            case Registration: {
                this->http_process.processing_client_requests(this->server_parser.getPars(), this->user_name);
				this->server_parser.clearRequest();
				this->setHttp_request(this->http_process.builder.get_HTTP());
				this->http_process.builder.clearBuilder();
                break;
            }
			case RequestAnswer: {

				break;
			}
			case RequestsError: {

				break;
			}
			case TakingAFile: {
				this->cl_state = on_write;
				Files_Operator(this->server_parser.getPars().type, this->http_process, this->server_parser.getPars());
				break;
			}
			case SendingAFile: {
				this->cl_state = on_read;
				Files_Operator(this->server_parser.getPars().type, this->http_process, this->server_parser.getPars());
				break;
			}
			case DeleteAFile: {

				break;
			}
            case ArduinoInfo: {

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
				return;
			}
			else {
				std::cout << err.what() << std::endl;
			}

		}
    
                
    }

	void Files_Operator(requests_types current_request, http_processing& http_process, parsed_request parsed_req) {

		if (current_request == SendingAFile) {
            /////////////////////////////////////////////////////////////////////
			http_process.processing_client_requests(parsed_req, this->user_name);
            /////////////////////////////////////////////////////////////////////
			if (parsed_req.keys_map["Part-File"] == "end" ||
				parsed_req.keys_map["Part-File"] == "full") {
				this->clearRequest();
                // set ANSWER
				server_builder.Builder_Answer("200 OK");
				this->setHttp_request(server_builder.get_HTTP());
				server_builder.clearBuilder();
                //set state
				this->cl_state = none;
                //clear server parser
				this->server_parser.clearRequest();
				return;
			}
            //clear client current request
			this->clearRequest();
      
			this->start_read();

			this->server_parser.clearRequest();
		}
		if (current_request == TakingAFile) {
			while (true) {
                /////////////////////////////////////////////////////////////////////
				http_process.processing_client_requests(parsed_req, this->user_name);
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
            this->server_parser.clearRequest();
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
            new_connection->start_read();
            client_or_server_color("SERVER");
            std::cout << "CLIENT(ID:" << new_connection->getID() << ") joined the server" << std::endl;
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
