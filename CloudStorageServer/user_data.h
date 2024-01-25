#pragma once
#include <iostream>
#include <fstream>
#include "bcrypt.h"

class user_data {
    std::fstream db;
    std::string file_name;
    std::string user_name;
public:
    user_data(std::string file_name);
    bool Authorisation(std::string login, std::string password);
    std::string get_user_name();
};
