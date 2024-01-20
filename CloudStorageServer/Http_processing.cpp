#include "Http_processing.h"

http_processing::http_processing() {
	this->file_sender = new File_Sender("image.jpg");
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
			this->builder.Builder_Answer(RequestAnswer, "200 OK");
			break;
		}
		case DeleteAFile: {
			//remove a file from repository
			this->builder.Builder_Answer(RequestAnswer, "200 OK");
			break;
		}
		case SendingAFile: {
			//saving file in repository
			this->builder.Builder_Answer(RequestAnswer, "200 OK");
			break;
		}
		case ArduinoInfo: {
			//free space calculation
			this->builder.Builder_Answer(RequestAnswer, "200 OK");
			//arduino connection
			break;
		}
		}
		this->parser.clearRequest();

		//open a thread
		queue_lock.unlock();
	}
}
