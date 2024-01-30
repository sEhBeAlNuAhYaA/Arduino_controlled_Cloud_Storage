#include "Http_processing.h"

http_processing::http_processing() {
	this->userdata = new user_data("db.txt");
}


void http_processing::processing_client_requests(parsed_request parsed_req, std::string& user_name) {
	//setting request
	this->parsed_req = parsed_req;
	//clear this request in queue

	switch (this->parsed_req.type) {
	case Authorisation: {
		if (this->userdata->Authorisation(this->parsed_req.keys_map["login"], this->parsed_req.keys_map["password"])) {
			this->builder.Builder_Answer(RequestAnswer, "200 OK");
			std::cout << "Client logged in" << std::endl;
			user_name = this->userdata->get_user_name();
		
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
			this->file_sender.init_File_sender(this->parsed_req.keys_map["Content-Name"]);
			if (this->file_sender.return_action() == "start") {
				this->builder.Sending_A_File(this->parsed_req.keys_map["Content-Name"],
					this->file_sender.split_file(),
					this->file_sender.getFileSize(),
					"start");
			}
			if (this->file_sender.return_action() == "full") {
				this->builder.Sending_A_File(this->parsed_req.keys_map["Content-Name"],
					this->file_sender.split_file(),
					this->file_sender.getFileSize(),
					"full");

				this->file_sender.close_file();
			}
			break;
		}
		this->file_sender.set_right_state(); //--->set the current state of part of file

		this->builder.Sending_A_File(this->parsed_req.keys_map["Content-Name"],
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
		if (this->parsed_req.keys_map["Part-File"] == "start") {
			this->fileout.open(this->parsed_req.keys_map["Content-Name"], std::ios::binary);
			this->fileout.write(this->parsed_req.binary_part, 9000);
		}
		if (this->parsed_req.keys_map["Part-File"] == "body") {
			this->fileout.write(this->parsed_req.binary_part, 9000);
		}
		if (this->parsed_req.keys_map["Part-File"] == "end" ||
			this->parsed_req.keys_map["Part-File"] == "full") {

			if (this->parsed_req.keys_map["Part-File"] == "full") {
				this->fileout.write(this->parsed_req.binary_part, stoi(this->parsed_req.keys_map["Content-Length"]));
			}
			else if (stoi(this->parsed_req.keys_map["Content-Length"]) % 9000 == 0) {
				this->fileout.write(this->parsed_req.binary_part, 9000);
			}
			else {
				this->fileout.write(this->parsed_req.binary_part, stoi(this->parsed_req.keys_map["Content-Length"]) % 9000);
			
			}
			this->fileout.close();
		}
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
	this->parsed_req.clear_struct();

}