#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <chrono>
#include <thread>
#include <queue>
#include <Http_Builder.h>
using boost::asio::ip::tcp;

std::queue <std::string> request_queue;

struct Vector_Clients;
static int client_ID_counter = 0;

//Http_Builder builder;
//Http_Parser parser;

class NEW_connection 
    : public std::enable_shared_from_this<NEW_connection>
{
    //private socket and file
    tcp::socket socket_;
    int client_ID;
    char *http_request;
    boost::system::error_code error;

public:
    typedef std::shared_ptr<NEW_connection> pointer;
    
    static pointer create(boost::asio::io_context& context) {
        return pointer(new NEW_connection(context));
    }
    //get socket
    tcp::socket& get_socket() {
        return socket_;
    }

    void send_message() {
        socket_.write_some(boost::asio::mutable_buffer(this->http_request,1024));
        if (!error) {
            std::cout << "[SERVER] message was send" << std::endl;
            memset(this->http_request, 0, sizeof(this->http_request));
        }
        else {
            std::cout << error.message() << std::endl;
        }

    }

    void process_client_message() {

    }

    void read_message() {
      
        try {
            socket_.read_some(boost::asio::mutable_buffer(this->http_request,1024), error);
            
            if (!error) {
            std::cout << "[SERVER] message was taked" << std::endl;
            request_queue.push(std::string(this->http_request));
            std::cout << request_queue.front() << std::endl;
            request_queue.pop();
            }
            else {
                std::cout << error.message() << std::endl;
                return;
            }
            read_message();
        }
        catch(const boost::system::error_code& err){
            std::cout << err.message() << std::endl;
        }
    }

    int getID() {
        return this->client_ID;
    }

    void setHttp_request(std::string input_string) {
        for (int i = 0; i < input_string.size(); i++) {
            this->http_request[i] = input_string[i];
        }
    }

private:
    NEW_connection(boost::asio::io_context& context)
        : socket_(context) {
        client_ID_counter++;
        client_ID = client_ID_counter;
        this->http_request = new char[1024];
    }

    /*
    void handle_write(const boost::system::error_code& err, size_t transferred) {
        if (!err) {
            std::cout << "MESSAGE WAS SEND" << std::endl;
        }
        else {
            std::cout << err.what() << std::endl;
        }
    }

    void handle_read(const boost::system::error_code& err, size_t transferred, std::string http_request) {
        if (!err) {
            read_message();
            std::cout << "MESSAGE WAS TAKED" << std::endl;
        }
        else {
            if (err.value() == 10054) {
                std::cout << "CLIENT(ID:" << this->client_ID << ") disconnected" << std::endl;
            }
            else {
                std::cout << err.what() << std::endl;
            }
            
        }
        
    }
    */
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
        NEW_connection::pointer new_connection = NEW_connection::create(this->context_);
        
        acceptor_.async_accept(new_connection->get_socket(),
            boost::bind(&Cloud_Storage::handle_accept, this, 
                new_connection, boost::asio::placeholders::error));

    }

    void handle_accept(NEW_connection::pointer new_connection, const boost::system::error_code& error) {
        if (!error) {
            new_connection->setHttp_request("CONNECTED!");
            new_connection->send_message();
            std::cout << "[SERVER] " << "CLIENT(ID:" << new_connection->getID() << ") joined the server" << std::endl;
            new_connection->read_message();
            start_accept();
        }
        else {
            std::cout << error.what() << std::endl;
            return;
        }
    }
};




std::string make_daytime_string()
{
    return "YOU ARE CONNECTED!";
}




int main()
{ 
    
    boost::asio::io_context context;
    Cloud_Storage storage(context);

    std::cout << "[SERVER] START" << std::endl;

    context.run();
    
    return 0;
}
