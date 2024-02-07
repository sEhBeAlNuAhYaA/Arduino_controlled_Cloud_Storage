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
void files_state::clear_files_state() {
	this->file_name.clear();
	this->file_name_copy_counter = 0;
	this->file_users.clear();
}


Space_Saver::Space_Saver() {
	this->new_file_name = "";

}
std::string Space_Saver::name_compare(std::string new_file_name, std::string user) {
	files_state file;
	int max_copy = -1;
	for (auto file : this->all_files) {
		if (file.file_name == new_file_name) {
			if (file.file_name_copy_counter > max_copy) max_copy = file.file_name_copy_counter;
		}
	}
	if (max_copy != -1) {
		new_file_name.insert(0, "(" + std::to_string(max_copy + 1) + ")");
		file.file_name_copy_counter = max_copy + 1;
	}
	file.file_name = new_file_name;
	file.file_users.push_back(user);
	this->all_files.push_back(file);
	return new_file_name;
}
bool Space_Saver::new_file_name_compare(std::string file_name, std::string user) {
	this->new_file_name = file_name;
	files_state new_file;
	bool compare = false;
	//new_file.add_a_new_file(this->new_file_name, 0, )
	std::string f = "Files\\" + user + "_" + this->all_files[this->all_files.size() - 1].file_name;
	std::ifstream fin;
	fin.open(f, std::ios::binary);
	//size of input file 
	if (fin.is_open()) {
		std::cout << 1;
	}
	fin.seekg(0,fin.end);
	size_t fin_size = fin.tellg();
	fin.seekg(0,fin.beg);
	//another file
	std::ifstream fanother;
	for (auto file : this->all_files) {
		if (this->new_file_name == file.file_name) {
			if (file.file_users.size() == 1) {
				fanother.open("Files\\" + user + "_" + file.file_name, std::ios::binary);
			}
			else {
				fanother.open("Files\\all_" + file.file_name, std::ios::binary);
			}

			fanother.seekg(0,fanother.end);
			size_t fanother_size = fanother.tellg();
			fanother.seekg(0,fin.beg);

			if (fin_size == fanother_size) {
				compare = std::equal(std::istreambuf_iterator<char>(fin.rdbuf()),
					std::istreambuf_iterator<char>(),
					std::istreambuf_iterator<char>(fanother.rdbuf()));
				if (compare) return compare;
			}
		}
		fanother.close();
	}

	fin.close();
	return compare;
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
	this->read_db();
}
Files_OPERATOR::~Files_OPERATOR() {
	this->files_data_base.close();
}

void Files_OPERATOR::read_db() {
	std::string line;
	files_state file;
	while (std::getline(this->files_data_base, line)) {
		int del = line.find(")");
		if (del != -1) {
			file.file_name_copy_counter = stoi(line.substr(1, del));
			line.erase(0, del + 1);
		}
		del = line.find(":");
		if (del != -1) {
			file.file_name = line.substr(0, del);
			line.erase(0,del+1);
			del = line.find(":");
			while (del != -1) {
				file.file_users.push_back(line.substr(0,del));
				line.erase(0,del+1);
				del = line.find(":");
			}
			file.file_users.push_back(line.substr(0, line.size()));
		}
		this->all_files.push_back(file);
		file.clear_files_state();
	}
	
}
