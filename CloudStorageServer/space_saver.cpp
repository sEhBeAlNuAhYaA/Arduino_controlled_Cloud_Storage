#include "space_saver.h"

static size_t REPOSITORY_MAX_SIZE = 100000000;

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
std::string Space_Saver::name_compare(std::string new_file_name, const std::string& user) {
	this->all_files.clear();
	this->read_db();
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
bool Space_Saver::new_file_name_compare(const std::string& file_name, const std::string& user) {
	this->new_file_name = file_name;
	files_state new_file;
	bool compare = false;
	//new_file.add_a_new_file(this->new_file_name, 0, )
	std::string f = "Files\\"  + this->all_files[this->all_files.size() - 1].file_name;
	std::ifstream fin;
	fin.open(f, std::ios::binary);
	fin.seekg(0,fin.end);
	size_t fin_size = fin.tellg();
	fin.seekg(0,fin.beg);
	//another file
	std::ifstream fanother;
	for (int i = 0; i < this->all_files.size() - 1; i++) {
		auto file = this->all_files[i];
		if (this->new_file_name == file.file_name) {
			fanother.open("Files\\" + file.file_name, std::ios::binary);
		
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
void Space_Saver::add_file_to_db(const std::string& file_name, const std::string& user) {
	if (this->new_file_name_compare(file_name, user)) {
		std::filesystem::remove("Files\\" + this->all_files[this->all_files.size() - 1].file_name);
		this->all_files.pop_back();
		for (auto& file : this->all_files) {
			if (file.file_name == file_name) {
				if (std::find(file.file_users.begin(), file.file_users.end(), user) == file.file_users.end()) {
					file.file_users.push_back(user);
				}
				reset_a_file_info();
				
			}
		}
	}
	else {
		set_a_line(this->all_files[this->all_files.size() - 1]);
	}
}
void Space_Saver::rem_file_from_db(const std::string& file_name, const std::string& user) {

	for (auto& file : this->all_files) {
		if (file_name == file.file_name) {
			if (file.file_users.size() > 1) {
				file.file_users.erase(std::find(file.file_users.begin(), file.file_users.end(), user));
				break;
			}

			if (file.file_users.size() == 1 && file.file_users[0] == user) {
				//std::filesystem::remove("Files\\" + file_name);
				file.file_users.clear();
				file.file_users.push_back("none");
				break;
			}
			else {
				std::cout << "none root for user " << user << std::endl;
				break;
			}
		}
	}
	this->reset_a_file_info();
}
bool Space_Saver::check_a_user(const std::string& user) {
	for (auto file : this->all_files) {
		for (auto one_user : file.file_users) {
			if (one_user == user) {
				return true;
			}
		}
	}
	return false;
}

bool Space_Saver::check_a_file(const std::string& file_name, const std::string& user_name) {
	for (auto file : this->all_files) {
		if (file.file_name == file_name) {
			for (auto one_user : file.file_users) {
				if (one_user == user_name) {
					return true;
				}
			}
		}
	}
	return false;
}
std::vector <std::string> Space_Saver::update_all_list() {
	std::vector <std::string> Files_List;
	for (auto el : this->all_files) {
		Files_List.push_back(el.file_name);
	}
	return Files_List;
}
std::vector <std::string> Space_Saver::update_own_list(std::string user_name) {
	std::vector <std::string> Files_List;
	for (auto file : this->all_files) {
		for (auto user : file.file_users) {
			if (user == user_name) {
				Files_List.push_back(file.file_name);
			}
		}
	}
	return Files_List;
}
int Space_Saver::space_counter(const std::string& user_name) {

	size_t files_size = 0;
	for (auto file : this->all_files) {
		for (auto user : file.file_users) {
			if (user == user_name) {
				std::ifstream fin("Files\\" + file.file_name);
				fin.seekg(0, fin.end);
				files_size += fin.tellg();
				fin.close();
			}
		}
	}
	float procent = float(files_size) / (REPOSITORY_MAX_SIZE) * 100.0;
	files_size = procent;
	return files_size;

}


Files_OPERATOR::Files_OPERATOR() {
	this->read_db();
}
Files_OPERATOR::~Files_OPERATOR() {
}

void Files_OPERATOR::read_db() {
	this->files_data_base.open("files_db.txt");
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
	this->files_data_base.close();
}

void Files_OPERATOR::set_a_line(files_state new_file) {
	std::ofstream fout("files_db.txt", std::ios::app);
	if (!fout.is_open()) {
		fout << new_file.file_name + ":" + new_file.file_users[0];
	}
	else {
		fout << "\n" + new_file.file_name + ":" + new_file.file_users[0];
	}
	fout.close();
}

void Files_OPERATOR::reset_a_file_info() {
	std::ofstream fout("files_db.txt");
	fout.clear();
	for (int i = 0; i < this->all_files.size(); i++) {
		auto file = this->all_files[i];
		fout << file.file_name;
		for (auto user : file.file_users) {
			fout << ":" + user;
		}
		if (i != this->all_files.size() - 1) fout << "\n";
	}
	fout.close();
}


