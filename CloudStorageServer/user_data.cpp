#include "user_data.h"

user_data::user_data(std::string file_name) {
	this->file_name = file_name;
}

bool user_data::Authorisation(std::string login, std::string password) {
	this->db.open(this->file_name);
	std::string login_in_db;
	std::string password_in_db;
	std::string line;
	while (std::getline(this->db, line)) {
		int login_end_position = line.find(':');
		login_in_db = line.substr(0, login_end_position);
		password_in_db = line.substr(login_end_position + 1);
		if (login == login_in_db && bcrypt::validatePassword(password, password_in_db)) {
			this->user_name = login_in_db;
			return true;
		}
	}
	this->db.close();
	return false;
}

bool user_data::Registration(std::string login, std::string password) {
	this->db.open(this->file_name);
	std::string login_in_db;
	std::string line;
	bool size_of_db = true;
	if (!this->db.is_open()) {
		size_of_db = 0;
	}
	while (std::getline(this->db, line)) {
		int login_end_position = line.find(':');
		login_in_db = line.substr(0, login_end_position);
		if (login == login_in_db) {
			return false;
		}
	}
	this->db.close();
	std::ofstream out;
	out.open(this->file_name,std::ios::app);
	if (!size_of_db){
		out << login + ":" + bcrypt::generateHash(password);
	}
	else {
		out << "\n" + login + ":" + bcrypt::generateHash(password);
	}
	this->user_name = login;
	out.close();
	return true;
}

std::string user_data::get_user_name() {
	return this->user_name;
}