#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <chrono>
#include <thread>

using namespace boost::asio;

namespace Arduino_Connection {
	void send_Arduino_message(const std::string& message);
	std::string serialize_aduino_answer(const std::string& ID, std::string procent);
	std::string serialize_main_frame(std::string counter);
}

class Arduino_Connector {
	serial_port com;
	std::mutex mute;
	io_context& context;
public:
	Arduino_Connector(io_context& context);
	void sendArduino(const std::string message);
}; 