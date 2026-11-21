#include "backup.hpp"

#include "asp/fs/fs.hpp"

using namespace geode::prelude;

static asp::fs::Path getBackupDirPath(std::string const& levelID) {
	return Mod::get()->getSaveDir() / "data" / (levelID + ".backup");
}

asp::fs::Path Backup::getLegacyBackupDirPath() {
	return Mod::get()->getSaveDir() / "leveldata.backup";
}

bool Backup::legacyFileExists() {
	return asp::fs::exists(getLegacyBackupDirPath());
}

bool Backup::fileExists(std::string const& levelID) {
	return asp::fs::exists(getBackupDirPath(levelID));
}

matjson::Value Backup::getFile(std::string const& levelID) {
	if (Backup::fileExists(levelID)) {
		return file::readJson(getBackupDirPath(levelID)).unwrapOrDefault();
	}
	matjson::Value data;
	Backup::createBackup(data, levelID);
	return data;
}

void Backup::writeFile(matjson::Value const& data, std::string const& levelID) {
	auto const& output = data.dump(matjson::NO_INDENTATION);
	(void)file::writeStringSafe(getBackupDirPath(levelID), output);
}

void Backup::createBackup(matjson::Value const& data, std::string const& levelID) {
	log::info("Creating backup of {}.json", levelID);
	Backup::writeFile(data, levelID);
}

void Backup::loadBackup(std::string const& levelID) {
	auto data = Backup::getFile(levelID);
	auto const& output = data.dump(matjson::NO_INDENTATION);
	(void)file::writeStringSafe(Mod::get()->getSaveDir() / "data" / (levelID + ".json"), output);
}