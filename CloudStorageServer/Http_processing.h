#pragma once
#include <iostream>
#include <queue>
#include <boost/asio.hpp>
#include <Http_Builder.h>

std::queue <char*> request_queue;
std::mutex queue_lock;

using namespace std;

class user_data {
    fstream db;
    std::string file_name;
public:
    user_data(std::string file_name) {
        this->file_name = file_name;
        this->db.open(this->file_name);
    }
    bool Authorisation(std::string login, std::string password) {
        std::string login_in_db;
        std::string password_in_db;
        std::string line;
        //while(getline())
    }
};

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
};
