#include "./backup.hpp"
#include <matjson.hpp>
#include <fstream>

using namespace geode::prelude;

auto backupDir = Mod::get()->getSaveDir().string() + "/leveldata.backup";
std::filesystem::path backupDirPath = std::filesystem::path(backupDir);

bool Backup::fileExists() {
	if (std::filesystem::exists(backupDirPath)) return true;
	return false;
}

matjson::Value Backup::getFile() {
	if (Backup::fileExists()) {
		std::ifstream i(backupDir);
		auto data = matjson::parse(i).unwrap();
		i.close();
		return data;
	}
	matjson::Value data;
	Backup::createBackup(data);
	return data;
}

void Backup::writeFile(matjson::Value data) {
	std::string output = data.dump(matjson::NO_INDENTATION);
	std::ofstream o(backupDir);
	o << output;
	o.close();
}

void Backup::createBackup(matjson::Value data) {
	log::debug("Creating backup of levelData");
	Backup::writeFile(data);
}

void Backup::loadBackup() {
	auto data = Backup::getFile();
	std::string output = data.dump(matjson::NO_INDENTATION);
	std::ofstream o(std::filesystem::path(Mod::get()->getSaveDir().string() + "/leveldata.json"));
	o << output;
	o.close();
}