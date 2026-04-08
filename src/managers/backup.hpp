#pragma once

#include <matjson.hpp>

class Backup {
protected:
	static void writeFile(matjson::Value const& data, std::string const& levelID);

public:
	static matjson::Value getFile(std::string const& levelID);
	static bool fileExists(std::string const& levelID);
	static void createBackup(matjson::Value const& data, std::string const& levelID);
	static void loadBackup(std::string const& levelID);
};