#include "./backup.hpp"

using namespace geode::prelude;

static std::filesystem::path getBackupDirPath() {
	return Mod::get()->getSaveDir() / "leveldata.backup";
}


bool Backup::fileExists() {
	return std::filesystem::exists(getBackupDirPath());
}

matjson::Value Backup::getFile() {
	if (Backup::fileExists()) {
		return file::readJson(getBackupDirPath()).unwrapOrDefault();
	}
	matjson::Value data;
	Backup::createBackup(data);
	return data;
}

void Backup::writeFile(matjson::Value const& data) {
	std::string output = data.dump(matjson::NO_INDENTATION);
	(void)file::writeString(getBackupDirPath(), output);
}

void Backup::createBackup(matjson::Value const& data) {
	log::info("Creating backup of leveldata.json");
	Backup::writeFile(data);
}

void Backup::loadBackup() {
	auto data = Backup::getFile();
	std::string output = data.dump(matjson::NO_INDENTATION);
	(void)file::writeString(Mod::get()->getSaveDir() / "leveldata.json", output);
}