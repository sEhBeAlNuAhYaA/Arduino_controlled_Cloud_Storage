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
	std::string name_compare(std::string new_file_name, std::string user);
	bool new_file_name_compare(std::string file_name, std::string user);
	void add_file_to_db(std::string file_name, std::string user);
	void rem_file_from_db(std::string file_name,std::string user);

	std::vector <std::string> update_own_list(std::string user_name);
	std::vector <std::string> update_all_list();
};
