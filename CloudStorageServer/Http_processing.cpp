#include "Http_processing.h"

http_processing::http_processing() {
	this->userdata = new user_data("db.txt");
}

void http_processing::processing_client_requests(std::queue <char*>& request_queue) {
	while (!request_queue.empty()) {
		//lock a thread
		queue_lock.lock();
		//setting request and parsing
		parser.setRequest(request_queue.back());
		//client_or_server_color("CLIENT");
		parser.Parsing();
		//clear this request in queue
		request_queue.pop();

		switch (this->parser.getPars().type) {
		case Authorisation: {
			if (this->userdata->Authorisation(this->parser.getPars().keys_map["login"], this->parser.getPars().keys_map["password"])) {
				this->builder.Builder_Answer(RequestAnswer, "200 OK");
				std::cout << "Client logged in" << std::endl;
			}
			else {
				this->builder.Builder_Answer(RequestAnswer, "401");
			}
			break;
		}
		case TakingAFile: {
			//bilding a request with file data
			if (this->file_sender.return_action() == "start" || this->file_sender.return_action() == "full") {
				//init file (open it)
				this->file_sender.init_File_sender(this->parser.getPars().keys_map["Content-Name"]);
			}
			this->builder.Sending_A_File(this->parser.getPars().keys_map["Content-Name"],
				this->file_sender.return_action(), this->file_sender.getFileSize(),
				this->file_sender.split_file());

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
			this->builder.Builder_Answer(RequestAnswer, "200 OK");
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
}
