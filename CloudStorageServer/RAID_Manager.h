#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>


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
	main_
};

struct single_part {
	std::string parts_db;
	parts_names part_name;

	single_part();
	single_part(const std::string& files_db, const parts_names& part_name, std::string directory);
	void create_a_part(const std::string directory);
};

struct single_disk {
	std::string disk_db;
	std::vector <single_part> parts;

	single_disk();
	single_disk(const std::string disk_db, const std::string directory);
	void fill_disk(const std::vector <single_part> parts);
	void memoryse_a_disk(const single_disk& disk,std::string directory);
};

struct RAID_Manager {
	std::string manager_db;
	std::vector <single_disk> disks;
	std::vector <parts_names> parts_vector;

	RAID_Manager(const std::string& manager_db, const std::vector <parts_names> &parts_vector);
};
