#include "space_saver.h"

static size_t REPOSITORY_MAX_SIZE = 10000000000;

struct files_state {
	std::string file_name;
	int file_name_copy_counter;
	std::vector <std::string> file_users;

	files_state() {
		this->file_name = "";
		this->file_name_copy_counter = 0;
	}

	void add_a_new_file(std::string file_name, int count, std::vector <std::string>& users) {
		this->file_name = file_name;
		this->file_name_copy_counter = count;
		this->file_users = users;
	}
};


class Space_Saver {
	std::vector <files_state> current_files;
	std::string new_file_name;
public:
	Space_Saver() {
		this->new_file_name = "";
	}


	std::string new_file_name_compare(std::string file_name, std::string user) {
		this->new_file_name = file_name;
		files_state new_file;
		//new_file.add_a_new_file(this->new_file_name, 0, )
		for (auto file : this->current_files) {
			if (this->new_file_name == file.file_name) {

			}
		}
	}


};