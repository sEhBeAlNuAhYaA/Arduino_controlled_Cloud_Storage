#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <chrono>
#include <thread>
#include <queue>
#include <mutex>
#include <Http_Builder.h>
using boost::asio::ip::tcp;

std::queue <char*> request_queue;

struct Vector_Clients;
static int client_ID_counter = 0;

std::mutex g_lock;
//Http_Builder builder;
Http_Parser parser;


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
        socket_.async_write_some(boost::asio::mutable_buffer(this->http_request, strlen(this->http_request)),
            boost::bind(&NEW_connection::handle_write, shared_from_this(), 
                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)); 
    }

    void read_message() {
        //socket_.read_some(boost::asio::mutable_buffer(this->http_request,1024), error);
        socket_.async_read_some(boost::asio::mutable_buffer(this->http_request, 1024),
            boost::bind(&NEW_connection::handle_read, shared_from_this(),
               boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));         
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
        memset(this->http_request, '\0', 1024);
    }

    
    void handle_write(const boost::system::error_code& err, size_t transferred) {
        if (!err) {
            client_or_server_color("SERVER");
            std::cout << "MESSAGE WAS SEND ";
            client_or_server_color("CLIENT");
            color_client_id(this->client_ID);
            std::cout << std::endl;

        }
        else {
            std::cout << err.what() << std::endl;
        }
    }

    void handle_read(const boost::system::error_code& err, size_t transferred) {
                
        if (!err) {
            //copy into queue
            char* to_queue = new char[1024];
            memcpy_s(to_queue, 1024, this->http_request, 1024);
            request_queue.push(to_queue);
            memset(this->http_request, '\0', 1024);
  
            client_or_server_color("SERVER");
            std::cout << "MESSAGE WAS TAKED FROM ";
            client_or_server_color("CLIENT");
            color_client_id(this->client_ID);
            std::cout << std::endl << request_queue.back() << std::endl;
            
            //next_iterarion
            read_message();
            //analyze requests
            processing_client_requests();
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
    
    void processing_client_requests() {
        g_lock.lock();
        while(!request_queue.empty()) {
            parser.setRequest(request_queue.back());
            client_or_server_color("CLIENT");
            std::cout << req_back_converter(parser.Parsing().type) << std::endl;
            request_queue.pop();
        }
        g_lock.unlock();
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
        NEW_connection::pointer new_connection = NEW_connection::create(this->context_);
        
        acceptor_.async_accept(new_connection->get_socket(),
            boost::bind(&Cloud_Storage::handle_accept, this, 
                new_connection, boost::asio::placeholders::error));

    }

    void handle_accept(NEW_connection::pointer new_connection, const boost::system::error_code& error) {
        if (!error) {
            new_connection->setHttp_request("CONNECTED!");
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
