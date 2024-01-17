#pragma once
#include <iostream>
#include <fstream>

class user_data {
    std::fstream db;
    std::string file_name;
public:
    user_data(std::string file_name);
    bool Authorisation(std::string login, std::string password);
};
