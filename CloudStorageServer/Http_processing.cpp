#include "Http_processing.h"

using namespace std;

void http_processing::processing_client_requests() {
	while (!request_queue.empty()) {
		//lock a thread
		queue_lock.lock();
		//setting request and parsing
		parser.setRequest(request_queue.back());
		client_or_server_color("CLIENT");
		std::cout << req_back_converter(parser.Parsing().type) << std::endl;
		//clear this request in queue
		request_queue.pop();

		switch (this->parser.getPars().type) {
		case Authorisation: {
			//the same

			break;
		}
		case TakingAFile: {
			//bilding a request with file data
			break;
		}
		case DeleteAFile: {
			//delete a file
			break;
		}
		case SendingAFile: {
			//saving file in repository
			break;
		}
		case ArduinoInfo: {
			//free space calculation
			break;
		}
		}

		//open a thread
		queue_lock.unlock();
	}
}
