#include "RAID_Manager.h"

std::string path_maker(parts_names part) {
	if (part == A1) return "A1";
	if (part == A2) return "A2";
	if (part == A3) return "A3";
	if (part == A4) return "A4";
	if (part == A5) return "A5";
	if (part == A6) return "A6";
	if (part == A7) return "A7";
	if (part == A8) return "A8";
}

single_part::single_part(){}

single_part::single_part(const std::string& files_db, const parts_names& part_name, std::string directory)
{
	this->parts_db = directory + "\\" + files_db;
	this->part_name = part_name;

	if (!std::filesystem::exists(directory)) {
		std::filesystem::create_directory(directory);
		std::fstream file(this->parts_db);
	}
}

single_disk::single_disk(const std::string disk_db, const std::string directory)
{
	this->disk_db = directory + "\\" + disk_db;
	if (!std::filesystem::exists(directory)) {
		std::filesystem::create_directory(directory);
		std::fstream file(this->disk_db);
	}
}

void single_part::create_a_part(std::string directory) {
	if (!std::filesystem::exists(directory)) {
		std::filesystem::create_directory(directory);
		std::fstream file(this->parts_db);
	}
}

single_disk::single_disk(){}

void single_disk::memoryse_a_disk(const single_disk& disk, std::string directory) {
	this->disk_db = disk.disk_db;
	this->parts = disk.parts;

	for (int i = 0; i < this->parts.size(); i++) {
		this->parts[i].create_a_part(directory + path_maker(parts[i].part_name));
	}
}

void single_disk::fill_disk(const std::vector <single_part> parts) {
	this->parts = parts;
}

RAID_Manager::RAID_Manager(const std::string& manager_db, const std::vector <parts_names> &parts_vector)
{
	this->manager_db = manager_db;
	if (!std::filesystem::exists("RAID")) {
		std::filesystem::create_directory("RAID");
	}
	std::vector <single_part> parts_odd;
	std::vector <single_part> parts_even;
	
	//disks
	single_disk disk1("disk1.txt", "RAID\\disk1");
	single_disk disk2("disk2.txt", "RAID\\disk2");
	single_disk disk3("disk3.txt", "RAID\\disk3");
	single_disk disk4("disk4.txt", "RAID\\disk4");

	for (auto el : parts_vector) {
		std::string path = path_maker(el);
		if (el % 2 == 0) {
			single_part part(path, el, "RAID\\disk1");
			parts_odd.push_back(part);
		}
		else {
			single_part part(path, el, "RAID\\disk3");
			parts_even.push_back(part);
		}
	}
	
	disk1.fill_disk(parts_odd);
	disk2.fill_disk(parts_even);
	disk3.memoryse_a_disk(disk1, "RAID\\disk2\\");
	disk4.memoryse_a_disk(disk2, "RAID\\disk4\\");
}

