#include "./backup.hpp"
#include <fstream>

using namespace geode::prelude;

static std::string getBackupDir() {
	return Mod::get()->getSaveDir().string() + "/leveldata.backup";
}

static std::filesystem::path getBackupDirPath() {
	return std::filesystem::path(getBackupDir());
}


bool Backup::fileExists() {
	return std::filesystem::exists(getBackupDirPath());
}

matjson::Value Backup::getFile() {
	if (Backup::fileExists()) {
		std::ifstream i(getBackupDir());
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
	std::ofstream o(getBackupDir());
	o << output;
	o.close();
}

void Backup::createBackup(matjson::Value data) {
	log::info("CREATING BACKUP OF LEVELDATA!");
	Backup::writeFile(data);
}

void Backup::loadBackup() {
	auto data = Backup::getFile();
	std::string output = data.dump(matjson::NO_INDENTATION);
	std::ofstream o(std::filesystem::path(Mod::get()->getSaveDir().string() + "/leveldata.json"));
	o << output;
	o.close();
}