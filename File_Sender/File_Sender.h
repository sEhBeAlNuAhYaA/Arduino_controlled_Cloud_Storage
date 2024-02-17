#pragma once
#include <iostream>
#include <fstream>

static int READ_FILE_BUFFER = 9000;

class File_Sender {
	std::string file_name;
	char* binary_part_of_file;
	size_t size_of_file;
public:
	std::ifstream file;
	enum action {
		start,
		body,
		full,
		end
	};
	action state;
public:
	std::string return_action() {
		switch (this->state) {
		case start: {
			return "start";
			break;
		}

		case body: {
			return "body";
			break;
		}

		case end: {
			return "end";
			break;
		}
		case full: {
			return "full";
			break;
		}
		}
	}
	File_Sender() {
		this->file_name = "";
		this->state = this->start;
	}
	int getFileSize() {
		return this->size_of_file;
	}
	void init_File_sender(const std::string file_name) {
		//set buffer
		this->binary_part_of_file = new char[READ_FILE_BUFFER];
		//open file as binary
		this->file_name = file_name;
		this->file.open(this->file_name, std::ios::binary);
		//set size
		this->file.seekg(0, this->file.end);
		this->size_of_file = this->file.tellg();
		this->file.seekg(0, this->file.beg);
		//set state for current part of file 
		if (this->size_of_file <= READ_FILE_BUFFER) {
			this->state = this->full;
		}
		else {
			this->state = this->start;
		}
	}

	void set_right_state() {
		if (this->size_of_file - this->file.tellg() <= READ_FILE_BUFFER) {
			this->state = this->end;
		}
		else {
			this->state = this->body;
		}
	}

	char* split_file() {
		this->clear_binary_file();

		if (this->size_of_file <= READ_FILE_BUFFER) {
			//if file is full
			this->state = this->full;
			this->file.read(this->binary_part_of_file, size_of_file);
			return this->binary_part_of_file;
			
		}
		if (this->size_of_file - this->file.tellg() <= READ_FILE_BUFFER) {
			//if its end of file 
			this->state = this->end;
			this->file.read(this->binary_part_of_file, this->size_of_file - this->file.tellg());
			return this->binary_part_of_file;
		}
		else {
			//if its READ_FILE_BUFFER bytes files part
			this->state = this->body;
			this->file.read(this->binary_part_of_file, READ_FILE_BUFFER);	
			return this->binary_part_of_file;
		}
	}

	void clear_binary_file() {
		memset(this->binary_part_of_file, '\0', READ_FILE_BUFFER);
	}

	void close_file() {
		this->file.close();
		this->clear_binary_file();
		this->file_name = "";
		this->state = this->start;
		this->size_of_file = 0;
	}
};