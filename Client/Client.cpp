#include "Client.h"

//constructor
Client::Client(boost::asio::io_context& context)
	: context_(context), resolver_(context), socket_(context), endpoints_(resolver_.resolve("127.0.0.1", "80")) {

	boost::asio::connect(socket_, endpoints_);
	this->current_http_request = new char[1024];
	memset(this->current_http_request, '\0', 1024);
}
//sync write
void Client::write_http(char* http_req) {
	socket_.write_some(boost::asio::mutable_buffer(http_req, strlen(http_req)));
	if (!error) {
		client_or_server_color("CLIENT");
		std::cout << "REQUEST WAS SEND" << std::endl;

	}
	else {
		std::cout << error.what() << std::endl;
	}
}
//sync read
void Client::read_http() {
	size_t len = socket_.read_some(boost::asio::mutable_buffer(this->current_http_request, 1024));
	if (error) {
		std::cout << error.what() << std::endl;
	}

	memset(reinterpret_cast<void*>(this->current_http_request + len), '\0', 1024 - len);
	client_or_server_color("SERVER");
	//std::cout << this->current_http_request << " (size: " << len << ")" << std::endl;

}

void Client::clearRequest() {
	memset(this->current_http_request, '\0', 1024);
}

char* Client::get_Request() {
	return this->current_http_request;
}