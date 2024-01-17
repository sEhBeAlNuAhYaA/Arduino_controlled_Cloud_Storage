#ifndef HTTP_PROCESSING_H
#define	HTTP_PROCESSING_H
#include <iostream>
#include <queue>
#include <boost/asio.hpp>
#include <Http_Builder.h>

static std::queue <char*> request_queue;
static std::mutex queue_lock;

using namespace std;


class http_processing {
	Http_Builder builder;
	Http_Parser parser;
public:
	void processing_client_requests();
};

#endif 
