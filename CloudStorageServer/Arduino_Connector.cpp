#include "Arduino_Connector.h"


void Arduino_Connection::send_Arduino_message(const std::string& message) {
	io_context context;
	serial_port com(context);
	boost::system::error_code com_error;

	com.open("COM7", com_error);
	if (com_error) {
		if (com_error == std::errc::permission_denied) {
			std::cout << "COM-port is busy!" << std::endl;
		}
		else if (com_error == std::errc::no_such_file_or_directory) {
			std::cout << "Not connected(Arduino)";
		}
		else {
			std::cout << com_error.what();
		}
	}
	std::this_thread::sleep_for(std::chrono::seconds(1));
	com.set_option(serial_port_base::baud_rate(57600));
	if (message.size() > 1) {
		com.write_some(const_buffer(message.c_str(),message.size()));
	}
	com.close();
}

std::string Arduino_Connection::serialize_aduino_answer(const std::string& ID, std::string procent) {

	std::string finish_procent;

	if (procent.size() == 1) {
		finish_procent = "0" + procent;
	}
	else if (procent.size() == 3) {
		finish_procent = "99";
	}
	else {
		finish_procent = procent;
	}

	finish_procent.insert(0, ID);
	return finish_procent;

}

std::string Arduino_Connection::serialize_main_frame(std::string counter) {
	std::string answer;
	if (counter.size() > 2) {
		counter = "MH"; // - more then one hundret users
	}
	else if (counter.size() == 1){
		counter.insert(0, "0");
	}
	return counter;
}

Arduino_Connector::Arduino_Connector(io_context& context): context(context), com(context) {
	this->com.open("COM7");
	com.set_option(serial_port_base::baud_rate(57600));
	boost::system::error_code com_error;

	if (com_error) {
		if (com_error == std::errc::permission_denied) {
			std::cout << "COM-port is busy!" << std::endl;
		}
		else if (com_error == std::errc::no_such_file_or_directory) {
			std::cout << "Not connected(Arduino)";
		}
		else {
			std::cout << com_error.what();
		}
	}
}
void Arduino_Connector::sendArduino(const std::string message) { 
	std::lock_guard guard(this->mute);
	if (message.size() == 2) {
		com.write_some(const_buffer(message.c_str(), message.size()));
	}
	else if (message.size() == 3) {
		com.write_some(const_buffer(message.c_str(), message.size()));
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}

}

