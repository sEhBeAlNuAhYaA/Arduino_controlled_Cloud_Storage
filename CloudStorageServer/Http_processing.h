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
#include "space_saver.h"
#include "Server_Client_files_mapping.h"

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
    void complicated_requests_processing(parsed_request parsed_req, std::string& user_name, Space_Saver& space_saver, Files_Mapping& files_mapping);
};
