#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
    try
    {
        
        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints =
        resolver.resolve("127.0.0.1", "daytime");

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        boost::array<char, 128> buf;

        boost::system::error_code error;

        size_t len = socket.read_some(boost::asio::buffer(buf), error);

        std::cout.write(buf.data(), len);

        for (;;)
        {
            std::string buff;
            std::cin >> buff;
            socket.send(boost::asio::buffer(buff));
        }
    }
    catch (std::exception& e)
    { 
        std::cerr << e.what() << std::endl;
    }

  return 0;
}