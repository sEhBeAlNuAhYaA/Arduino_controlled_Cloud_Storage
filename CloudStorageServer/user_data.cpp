#include "user_data.h"

user_data::user_data(std::string file_name) {
	this->file_name = file_name;
	this->db.open(this->file_name);
}
bool user_data::Authorisation(std::string login, std::string password) {
	
	return false;
}