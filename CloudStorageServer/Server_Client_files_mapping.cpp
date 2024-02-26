#include "Server_Client_files_mapping.h"

Files_Mapping::Files_Mapping() {}

void Files_Mapping::read_from_db() {
	for (auto &user_data : this->files_map_data) {
		std::ifstream db(user_data.user + ".txt");
		std::string line;
		while (std::getline(db, line)) {
			int del = line.find(':');
			if (del != -1) {
				own_map mp;
				mp.first = line.substr(0, del);
				mp.second = line.substr(del + 1, line.size());
				user_data.files_map.push_back(mp);
			}
		}
		db.close();
	}
}

void Files_Mapping::set_right_copy_number() {
	for (auto& line : this->files_map_data) {
		int counter = -1;
		std::vector <std::string> prev;
		for (auto el : line.files_map) {
			std::string current_str = el.first;
			for (auto& ul : line.files_map) {
				if (std::find(prev.begin(), prev.end(),
					el.first.substr(el.first.find(")") + 1,
						el.first.size())) == prev.end()) {

					if (el.first.substr(el.first.find(")") + 1, el.first.size()) == ul.first.substr(ul.first.find(")") + 1, ul.first.size())) {
						counter++;
						if (counter > 0) {
							int pos = ul.second.find(")");
							if (pos != -1) {
								ul.second.erase(0, pos+1);
							}
							ul.second.insert(0, "(" + std::to_string(counter) + ")");
						}
					}
				}
			}
			prev.push_back(el.first.substr(el.first.find(")") + 1, el.first.size()));
		}
	}
}

void Files_Mapping::add_new_user(const std::string& new_user) {
	own_file_line new_line;
	new_line.user = new_user;
	this->create_user_file(new_user);
}

void Files_Mapping::load_users(std::vector <std::string> users) {
	for (int i = 0; i < users.size(); i++) {
		own_file_line new_line;
		new_line.user = users[i];
		this->files_map_data.push_back(new_line);
		this->create_user_file(users[i]);
	}
}

std::vector <std::string> Files_Mapping::read_users_logins() {
	std::ifstream file("db.txt");
	std::vector <std::string> users;
	std::string line;
	while (getline(file, line)) {
		line = line.substr(0, line.find(':'));
		users.push_back(line);
	}
	return users;
	
}

void Files_Mapping::add_a_line_for_user(const std::string& user_name, const std::string& server_name, const std::string& client_name) {
	if (this->files_map_data.empty()) {
		create_user_file(user_name);
		this->add_a_line_for_user(user_name, server_name, client_name);
		write_full(user_name);
	}
	bool is_here_user = true;

	for (auto &user_data : this->files_map_data) {
		if (user_data.user == user_name) {
			is_here_user = true;
			own_map mp;
			mp.first = server_name;
			mp.second = client_name;
			user_data.files_map.push_back(mp);
			this->set_right_copy_number();
			reset_a_file(user_name);
			write_full(user_name);
			break;
		}
 		else {
			is_here_user = false;
		}
	}
	if (!is_here_user) {
		create_user_file(user_name);
		this->add_a_line_for_user(user_name, server_name, client_name);
		reset_a_file(user_name);
		write_full(user_name);
	}


}
void Files_Mapping::reset_a_file(const std::string& user_name) {
	std::ofstream fout(user_name + ".txt");
	fout.clear();
	fout.close();
}


void Files_Mapping::create_user_file(const std::string& user_name) {
	if (std::filesystem::exists(user_name + ".txt")) {
		return;
	}
	std::ofstream file(user_name + ".txt");
	file.close();
}

void Files_Mapping::write_full(const std::string& user_name) {
	std::ofstream db(user_name + ".txt");
	for (auto user_data : this->files_map_data) {
		if (user_data.user == user_name) {
			bool is_first_line = true;
			for (auto el : user_data.files_map) {
				if (is_first_line) {
					db << el.first + ":" + el.second;
					is_first_line = false;
				}
				else {
					db << '\n' + el.first + ":" + el.second;
				}
			}
			break;
		}
	}
	db.close();
}

void Files_Mapping::delete_a_file(const std::string& user_name, const std::string& server_file) {
	for (auto user_data : this->files_map_data) {
		if (user_data.user == user_name) {
			for (auto el : user_data.files_map) {
				if (el.first == server_file) {
					//user_data.files_map.erase(server_file);
					break;
				}
			}
		}
	}
	write_full(user_name);
}

std::string Files_Mapping::server_client_alternative(const std::string& user_name, const std::string& server_file) {
	for (auto user_data : this->files_map_data) {
		if (user_data.user == user_name) {
			for (auto server_file_in_db : user_data.files_map) {
				if (server_file_in_db.first == server_file) { 
					return server_file_in_db.second;
				}
			}
		}
	}
	return server_file;
}

std::string Files_Mapping::client_server_alternative(const std::string& user_name, const std::string& client_file) {
	for (auto user_data : this->files_map_data) {
		if (user_data.user == user_name) {
			for (auto server_file_in_db : user_data.files_map) {
				if (server_file_in_db.second == client_file) {
					return server_file_in_db.first;
				}
			}
		}
	}
	return client_file; 
}

std::vector <std::string> Files_Mapping::get_files_list(std::string user_name) {
	std::vector <std::string> list;
	for (auto user_data : this->files_map_data) {
		if (user_data.user == user_name) {
			for (auto own_list : user_data.files_map) {
				list.push_back(own_list.second);
			}
		}
	}
	return list;
}

