#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
    try
    {
        
        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints =
        resolver.resolve("127.0.0.1", "80");

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        boost::array<char, 128> buf;

        socket.read_some(boost::asio::buffer(buf));

        std::cout.write(buf.data(), 5);

        boost::system::error_code error;
            
        for (;;)
        {
            std::string buff;
            getline(std::cin, buff);
            socket.write_some(boost::asio::buffer(buff));
        }
    }
    catch (std::exception& e)
    { 
        std::cerr << e.what() << std::endl;
    }

  return 0;
}