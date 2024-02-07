#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <filesystem>


class files_state {
public:
	std::string file_name;
	int file_name_copy_counter;
	std::vector <std::string> file_users;

	files_state();
	void add_a_new_file(std::string file_name, int count, std::vector <std::string>& users);
};


namespace Files_Checker {
	std::vector <std::string> update_own_list(std::string directory_name, std::string user_name);
	std::vector <std::string> update_all_list(std::string directory_name);
};


class Files_OPERATOR {
protected:
	std::fstream files_data_base;
	std::vector <files_state> all_files;
public:
	Files_OPERATOR();
};

class Space_Saver : protected Files_OPERATOR {
	std::string new_file_name;
public:
	Space_Saver();
	std::string new_file_name_compare(std::string file_name, std::string user);
};
