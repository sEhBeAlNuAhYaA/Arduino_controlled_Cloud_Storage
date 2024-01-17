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
        while (getline(this->db, line)) {
            int login_end_position = line.find(':');
            login_in_db = line.substr(0, login_end_position);
            password_in_db = line.substr(login_end_position + 1);
            if (login == login_in_db && password == password_in_db) {
                return true;
            }
        }
        return false;
    }
};

class http_processing {
public:
    Http_Builder builder;
    Http_Parser parser;
private:
    user_data *userdata;
public:
    http_processing() {
        this->userdata = new user_data("db.txt");
    }
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
                    if (this->userdata->Authorisation(this->parser.getPars().keys_map["login"],this->parser.getPars().keys_map["password"])) {
                        this->builder.Builder_Answer(RequestAnswer, "200 OK");
                    }
                    else {
                        this->builder.Builder_Answer(RequestAnswer, "401");
                    }
                        
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
