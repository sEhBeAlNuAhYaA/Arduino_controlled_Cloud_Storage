#pragma once
#include <iostream>
#include <queue>
#include <boost/asio.hpp>
#include <Http_Builder.h>
#include "bcrypt.h"
#include "user_data.h"
#include <fstream>
#include <File_Sender.h>
#include <filesystem>
static size_t REPOSITORY_MAX_SIZE = 10000000000;
static std::mutex queue_lock;

class http_processing {
public:
    Http_Builder builder;
    parsed_request parsed_req;
    File_Sender file_sender;
    
private:
    user_data* userdata;
    std::ofstream fileout;
public:
    http_processing();
    void processing_client_requests(parsed_request parsed_req, std::string& user_name);
};
