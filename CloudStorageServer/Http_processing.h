#pragma once
#include <iostream>
#include <queue>
#include <boost/asio.hpp>
#include <Http_Builder.h>
#include "bcrypt.h"
#include "user_data.h"
#include <fstream>
#include <File_Sender.h>

static std::mutex queue_lock;

class http_processing {
public:
    Http_Builder builder;
    Http_Parser parser;
private:
    File_Sender file_sender;
    user_data* userdata;
    std::ofstream fileout;
public:
    http_processing();
    void processing_client_requests(char* current_req);
};
