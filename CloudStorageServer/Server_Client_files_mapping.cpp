#include "Server_Client_files_mapping.h"

Files_Mapping::Files_Mapping() {}

 
void Files_Mapping::read_from_db() {
	for (auto user_data : this->files_map_data) {
		std::ifstream db(user_data.user + ".txt");
		std::string line;
		while (std::getline(db, line)) {
			int del = line.find(':');
			if (del != -1) {
				user_data.files_map[line.substr(1, del)] = line.substr(del, line.size());
			}
		}
		db.close();
	}
}

void Files_Mapping::add_a_line_for_user(std::string user_name, std::string server_name, std::string client_name) {
	if (this->files_map_data.empty()) {
		create_user_file(user_name);
		this->add_a_line_for_user(user_name, server_name, client_name);
		write_full(user_name);
	}

	for (auto &user_data : this->files_map_data) {
		if (user_data.user == user_name) {
			user_data.files_map[server_name] = client_name;
			reset_a_file(user_name);
			write_full(user_name);
			break;
		}
		else {
			create_user_file(user_name);
			this->add_a_line_for_user(user_name, server_name, client_name);
			reset_a_file(user_name);
			write_full(user_name);
		}
	}

}
void Files_Mapping::reset_a_file(std::string user_name) {
	std::ofstream fout(user_name + ".txt");
	fout.clear();
	fout.close();
}


void Files_Mapping::create_user_file(std::string user_name) {
	own_file_line line;
	line.user = user_name;
	this->files_map_data.push_back(line);
	std::ofstream file(user_name + ".txt");
	file.close();
}

void Files_Mapping::write_full(std::string user_name) {
	std::ofstream db(user_name + ".txt");
	for (auto user_data : this->files_map_data) {
		if (user_data.user == user_name) {
			bool is_first_line = true;
			for (auto el : user_data.files_map) {
				if (is_first_line) {
					db << el.first + ":" + el.second;
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

void Files_Mapping::delete_a_file(std::string user_name, std::string server_file) {
	for (auto user_data : this->files_map_data) {
		if (user_data.user == user_name) {
			for (auto el : user_data.files_map) {
				if (el.first == server_file) {
					user_data.files_map.erase(server_file);
					break;
				}
			}
		}
	}
	write_full(user_name);
}

std::string Files_Mapping::server_client_alternative(std::string user_name, std::string server_file) {
	for (auto user_data : this->files_map_data) {
		if (user_data.user == user_name) {
			for (auto server_file_in_db : user_data.files_map) {
				if (server_file_in_db.first == server_file) { 
					return user_data.files_map[server_file];
				}
			}
		}
	}
	return server_file;
}

std::string Files_Mapping::client_server_alternative(std::string user_name, std::string client_file) {
	for (auto user_data : this->files_map_data) {
		if (user_data.user == user_name) {
			for (auto server_file_in_db : user_data.files_map) {
				if (server_file_in_db.second == client_file) {
					return user_data.files_map[client_file]; 
				}
			}
		}
	}
	return client_file; 
}


