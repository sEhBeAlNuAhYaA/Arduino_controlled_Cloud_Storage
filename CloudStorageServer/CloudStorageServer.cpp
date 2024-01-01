#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

class NEW_connection 
    : public std::enable_shared_from_this<NEW_connection>
{
    //private socket and file
    tcp::socket socket_;
    boost::array<char,1024> http_request;

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

        boost::asio::async_write(this->socket_, boost::asio::buffer("penis"),
            boost::bind(&NEW_connection::handle_write, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }

    void read_async_message() {

        socket_.async_read_some(boost::asio::buffer(this->http_request),boost::bind(&NEW_connection::handle_read,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

        /*boost::asio::async_read(this->socket_, boost::asio::buffer(this->http_request), 
            boost::bind(&NEW_connection::handle_read, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));*/
     }

private:
    NEW_connection(boost::asio::io_context& context)
        : socket_(context){
    }

    void handle_write(const boost::system::error_code& err, size_t transferred) {
        //send_async_message();
    }

    void handle_read(const boost::system::error_code& err, size_t transferred) {
        
        std::cout << std::string(this->http_request.begin(), this->http_request.end()) << std::endl;
        for (auto& el : this->http_request) {
            el = '\0';
        }
        read_async_message();
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
            new_connection->send_async_message();

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

    context.run();

    return 0;
}
