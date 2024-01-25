#include "Http_processing.h"

http_processing::http_processing() {
	this->userdata = new user_data("db.txt");
}

void http_processing::processing_client_requests(char* current_req, std::string& user_name) {
	//lock a thread
	queue_lock.lock();
	//setting request and parsing
	parser.setRequest(current_req);
	std::cout << current_req << std::endl;
	//std::cout << current_req << std::endl;
	//client_or_server_color("CLIENT");
	parser.Parsing();
	//clear this request in queue

	switch (this->parser.getPars().type) {
	case Authorisation: {
		if (this->userdata->Authorisation(this->parser.getPars().keys_map["login"], this->parser.getPars().keys_map["password"])) {
			this->builder.Builder_Answer(RequestAnswer, "200 OK");
			std::cout << "Client logged in" << std::endl;
			user_name = this->userdata->get_user_name();
			std::filesystem::create_directory(user_name);
		}
		else {
			this->builder.Builder_Answer(RequestAnswer, "401");
		}
		break;
	}
	case TakingAFile: {
		//bilding a request with file data
		if (this->file_sender.return_action() == "start") {
			//init file (open it)
			this->file_sender.init_File_sender(user_name + "\\" + this->parser.getPars().keys_map["Content-Name"]);
			this->builder.Sending_A_File(this->parser.getPars().keys_map["Content-Name"],
				this->file_sender.split_file(),
				this->file_sender.getFileSize(),
				"start");
			break;
		}
		if (this->file_sender.return_action() == "full") {
			this->file_sender.init_File_sender(user_name + "\\" + this->parser.getPars().keys_map["Content-Name"]);
			this->builder.Sending_A_File(this->parser.getPars().keys_map["Content-Name"],
				this->file_sender.split_file(),
				this->file_sender.getFileSize(),
				"full");
			this->file_sender.close_file();
			break;
		}
		this->file_sender.set_right_state(); //--->set the current state of part of file

		this->builder.Sending_A_File(this->parser.getPars().keys_map["Content-Name"],
			this->file_sender.split_file(),
			this->file_sender.getFileSize(),
			this->file_sender.return_action());

		if (this->file_sender.return_action() == "end") {
			this->file_sender.close_file();
		}
		break;
	}
	case DeleteAFile: {
		//remove a file from repository
		this->builder.Builder_Answer(RequestAnswer, "200 OK");
		break;
	}
	case SendingAFile: {
		if (this->parser.getPars().keys_map["Part-File"] == "start") {
			this->fileout.open(user_name + "\\" + this->parser.getPars().keys_map["Content-Name"], std::ios::binary);
			this->fileout.write(this->parser.getPars().binary_part, 9000);
		}
		if (this->parser.getPars().keys_map["Part-File"] == "body") {
			this->fileout.write(this->parser.getPars().binary_part, 9000);
		}
		if (this->parser.getPars().keys_map["Part-File"] == "end" ||
			this->parser.getPars().keys_map["Part-File"] == "full") {
			this->fileout.write(this->parser.getPars().binary_part, stoi(this->parser.getPars().keys_map["Content-Length"]) % 9000);
			this->fileout.close();
		}
		std::cout << current_req << std::endl;
		break;
	}
	case ArduinoInfo: {
		//free space calculation
		this->builder.Builder_Answer(RequestAnswer, "200 OK");
		//arduino connection
		break;
	}
	case RequestAnswer: {
		this->builder.Builder_Answer(RequestAnswer, "200 OK");
		break;
	}
	}
	this->parser.clearRequest();

	//open a thread
	queue_lock.unlock();

}
