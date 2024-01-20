#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

class File_Sender {
	std::string file_name;
	std::ifstream file;
	char* binary_part_of_file;
	int size_of_file;
	std::vector <int> parts;
public:
	File_Sender(std::string file_name) {
		this->binary_part_of_file = new char[1024];
		this->file_name = file_name;
		this->file.open(this->file_name, std::ios::in | std::ios::binary);
		this->size_of_file = this->file.tellg();
		std::ostringstream oss;
		int len = this->file.readsome(this->binary_part_of_file, 1024);
		while ((len = this->file.readsome(this->binary_part_of_file, 1024)) > 0) {
			oss.write(this->binary_part_of_file, this->size_of_file);
		};
		std::string output = oss.str();
		std::cout << output << std::endl;
	}
};