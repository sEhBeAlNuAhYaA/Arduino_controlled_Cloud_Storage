#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <string>
#include <filesystem>

struct own_map {
	std::string first;
	std::string second;
};

struct own_file_line {
	std::string user;
	std::vector <own_map> files_map;
};

class Files_Mapping {
	std::vector<own_file_line> files_map_data;
public:
	Files_Mapping();
	void read_from_db();
	void load_users(std::vector <std::string> users);
	std::vector <std::string> read_users_logins();
	void add_new_user(const std::string& new_user);
	void add_a_line_for_user(const std::string& user_name, const std::string& server_name, const std::string& client_name);
	void create_user_file(const std::string& user_name);
	void set_right_copy_number();
	void write_full(const std::string& user_name);
	void delete_a_file(const std::string& user_name, const std::string& server_file);
	std::string server_client_alternative(const std::string& user_name, const std::string& server_file);
	std::string client_server_alternative(const std::string& user_name, const std::string& client_file);
	void reset_a_file(const std::string& user_name);
	std::vector <std::string> get_files_list(std::string user_name);
};
