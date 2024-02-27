#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <filesystem>
#include "Server_Client_files_mapping.h"

class files_state {
public:
	std::string file_name;
	int file_name_copy_counter;
	std::vector <std::string> file_users;

	files_state();
	void add_a_new_file(std::string file_name, int count, std::vector <std::string>& users);
	void clear_files_state();
};


class Files_OPERATOR {
protected:
	std::fstream files_data_base;
	std::vector <files_state> all_files;
public:
	Files_OPERATOR();
	~Files_OPERATOR();

	void read_db();
	void set_a_line(files_state new_file);
	void reset_a_file_info();
};

class Space_Saver : protected Files_OPERATOR {
	std::string new_file_name;
public:
	Space_Saver();
	std::string name_compare(std::string new_file_name, const std::string& user, Files_Mapping& files_mapping);
	bool new_file_name_compare(const std::string& file_name, const std::string& user);
	void add_file_to_db(const std::string& file_name, const std::string& user,Files_Mapping& files_mapping);
	void rem_file_from_db(const std::string& file_name, const std::string& user);
	bool check_a_user(const std::string& user);
	bool check_a_file(const std::string& file_name, const std::string& user);
	std::vector <std::string> update_own_list(const std::string user_name);
	std::vector <std::string> update_all_list();
	int space_counter(const std::string& user_name);
};
