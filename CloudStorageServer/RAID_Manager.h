#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>


enum parts_names {
	A1,
	A2,
	A3,
	A4,
	A5,
	A6,
	A7,
	A8
};

enum disk_type {
	mirror,
	main
};

class single_part {
	std::string parts_db;
	parts_names part_name;
public:
	single_part(const std::string& files_db, parts_names part_name);
};

class single_disk {
	std::string disk_db;
	std::vector <single_part> parts;
public:
	single_disk(const std::string& disk_db, std::vector <single_part>& parts);
};

class RAID_Manager {
	std::string manager_db;
	std::vector <single_disk> disks;
public:
	RAID_Manager(const std::string& manager_db, std::vector <single_disk>& disks);
	static std::vector <single_disk>& create_system(std::vector <parts_names> parts);

};
