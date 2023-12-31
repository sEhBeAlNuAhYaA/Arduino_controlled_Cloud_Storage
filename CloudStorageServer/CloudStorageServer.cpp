#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost::asio::ip::tcp;

std::string make_daytime_string()
{
    return "YOU ARE CONNECTED!";
}

int main()
{ 
    std::cout << "SERVER IS WORKING!" << std::endl; 
    try
    {
        boost::asio::io_context io_context; 

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 13)); 
        boost::array<char, 5> mes;
        
        tcp::socket socket(io_context); 
        acceptor.accept(socket); 
        for (;;)
        {
             std::string message = make_daytime_string(); 

             boost::system::error_code ignored_error; 

             boost::asio::write(socket, boost::asio::buffer(message), ignored_error); 

             socket.read_some(boost::asio::buffer(mes));
             std::cout.write(mes.data(),3);
             std::cout << std::endl;
        }
    }
    catch (std::exception& e) 
    {
         std::cerr << e.what() << std::endl; 
    }

     return 0;
}
