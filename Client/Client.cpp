#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;

class Client {
    //needed params for make connection
    boost::asio::io_context& context_;
    tcp::resolver resolver_;
    tcp::socket socket_;
    tcp::resolver::results_type endpoints_;
    boost::array <char, 1024> current_http_request;

public:
    //constructor
    Client(boost::asio::io_context& context)
        : context_(context), resolver_(context), socket_(context), endpoints_(resolver_.resolve("127.0.0.1", "80")), current_http_request{} {
        boost::asio::connect(socket_, endpoints_);
    }
    //sync write
    void write_http(boost::array<char, 1024> http_request) {
        socket_.write_some(boost::asio::buffer(http_request));
        std::cout << "REQUEST WAS SENDED" << std::endl;
    }
    //sync read
    void read_http() {
        socket_.read_some(boost::asio::buffer(current_http_request));
        std::cout << "[SERVER] " << std::string(current_http_request.begin(), current_http_request.end())<< std::endl;
    }
};


int main(int argc, char* argv[])
{
    try
    {
        //init context and create client class object
        boost::asio::io_context context;
        Client client(context);
                    
        for (;;)
        {
            client.read_http();
        }
    }
    catch (std::exception& e)
    { 
        std::cerr << e.what() << std::endl;
    }

  return 0;
}