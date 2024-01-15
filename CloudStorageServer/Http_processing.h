#pragma once
#include <iostream>
#include <queue>
#include <boost/asio.hpp>
#include <Http_Builder.h>

std::queue <char*> request_queue;
std::mutex queue_lock;

using namespace std;

class http_processing {
    Http_Builder builder;
    Http_Parser parser;
public:

    void processing_client_requests() {
        while(!request_queue.empty()) {
            //lock a thread
            queue_lock.lock();
            //setting request and parsing
            parser.setRequest(request_queue.back());
            client_or_server_color("CLIENT");
            std::cout << req_back_converter(parser.Parsing().type) << std::endl;
            //clear this request in queue
            request_queue.pop();
            //open a thread
            queue_lock.unlock();

            switch (this->parser.getPars().type) {
                case Registration: {
                    
                    break;
                }
                case Authorisation: {

                    break;
                }
                case TakingAFile: {

                    break;
                }
                case DeleteAFile: {

                    break;
                }
                case SendingAFile: {

                    break;
                }
                case ArduinoInfo: {

                    break;
                }
            }
        }
    }


};
