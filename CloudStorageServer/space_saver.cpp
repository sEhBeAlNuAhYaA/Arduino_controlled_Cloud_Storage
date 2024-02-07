#include "space_saver.h"

static size_t REPOSITORY_MAX_SIZE = 10000000000;

files_state::files_state() {
	this->file_name = "";
	this->file_name_copy_counter = 0;
}
void files_state::add_a_new_file(std::string file_name, int count, std::vector <std::string>& users) {
	this->file_name = file_name;
	this->file_name_copy_counter = count;
	this->file_users = users;
}
Space_Saver::Space_Saver() {
	this->new_file_name = "";

}


std::string Space_Saver::new_file_name_compare(std::string file_name, std::string user) {
	this->new_file_name = file_name;
	files_state new_file;
	//new_file.add_a_new_file(this->new_file_name, 0, )
	for (auto file : this->all_files) {
		if (this->new_file_name == file.file_name) {

		}
	}
	return this->new_file_name;
}


std::vector <std::string> Files_Checker::update_all_list(std::string directory_name) {
	std::vector <std::string> Files_List;
	for (auto el : std::filesystem::directory_iterator(directory_name)) {
		std::filesystem::path filepath = el.path();
		Files_List.push_back((filepath.string()).erase(0, filepath.string().find_first_of("_") + 1));
	}
	return Files_List;
}
std::vector <std::string> Files_Checker::update_own_list(std::string directory_name, std::string user_name) {
	std::vector <std::string> Files_List;
	for (auto el : std::filesystem::directory_iterator(directory_name)) {
		std::filesystem::path filepath = el.path();
		int user_name_poz = filepath.string().find(user_name);
		if (user_name_poz == 6) {
			Files_List.push_back((filepath.string()).erase(0, filepath.string().find_first_of("_") + 1));
		}
	}
	return Files_List;
}


Files_OPERATOR::Files_OPERATOR() {
	this->files_data_base.open("files_db.txt");
}
