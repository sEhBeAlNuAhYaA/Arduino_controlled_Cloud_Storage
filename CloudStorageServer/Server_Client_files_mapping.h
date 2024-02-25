#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <string>

struct own_file_line {
	std::string user;
	std::unordered_map <std::string, std::string> files_map;
};

class Files_Mapping {
	std::vector<own_file_line> files_map_data;
public:
	Files_Mapping();
	void read_from_db();
	void add_a_line_for_user(std::string user_name, std::string server_name, std::string client_name);
	void create_user_file(std::string user_name);
	void write_full(std::string user_name);
	void delete_a_file(std::string user_name, std::string server_file);
	std::string server_client_alternative(std::string user_name, std::string server_file);
	std::string client_server_alternative(std::string user_name, std::string client_file);
	void reset_a_file(std::string user_name);
};
