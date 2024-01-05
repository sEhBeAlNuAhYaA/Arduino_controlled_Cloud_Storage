#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <chrono>
#include <thread>
#include <vector>
#include <Http_Builder.h>
using boost::asio::ip::tcp;

std::vector <std::string> queue;

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

public:
    typedef std::shared_ptr<NEW_connection> pointer;
    
    static pointer create(boost::asio::io_context& context) {
        return pointer(new NEW_connection(context));
    }
    //get socket
    tcp::socket& get_socket() {
        return socket_;
    }

    void send_async_message() {
        boost::system::error_code error;
        socket_.write_some(boost::asio::mutable_buffer(this->http_request,1024));
        if (!error) {
            std::cout << "MESSAGE WAS SEND" << std::endl;
            //clear this->http_request
            memset(this->http_request, 0, sizeof(this->http_request));
        }
        else {
            std::cout << error.message() << std::endl;
        }

       // socket_.async_write_some(boost::asio::mutable_buffer(this->http_request, 1024),
         //   boost::bind(&NEW_connection::handle_write, shared_from_this(),
        //    boost::asio::placeholders::error,
         //   boost::asio::placeholders::bytes_transferred));
    }

    void read_async_message() {
        socket_.async_read_some(boost::asio::mutable_buffer(this->http_request, 1024),boost::bind(&NEW_connection::handle_read,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
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

    
    void handle_write(const boost::system::error_code& err, size_t transferred) {
        if (!err) {
            std::cout << "MESSAGE WAS SEND" << std::endl;
        }
        else {
            std::cout << err.what() << std::endl;
        }
    }

    void handle_read(const boost::system::error_code& err, size_t transferred) {
        if (!err) {
            queue.push_back(this->http_request);
            memset(this->http_request, 0, sizeof(this->http_request));
            read_async_message();
            std::this_thread::sleep_for(std::chrono::seconds(5));
            std::cout << "MESSAGE WAS TAKED" << std::endl;
            //std::this_thread::sleep_for(std::chrono::seconds(5));
            for (int i = 0; i < queue.size();i++) {
                auto el = queue[i];
                std::cout << std::endl << "current request: "<< i << std::endl << el << std::endl;
            }

           
            //clear this->http_request
            //memset(this->http_request, 0, sizeof(this->http_request));
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
            new_connection->send_async_message();
            std::cout << "[SERVER] " << "CLIENT(ID:" << new_connection->getID() << ") joined the server" << std::endl;
            new_connection->read_async_message();
        }
        start_accept();
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
