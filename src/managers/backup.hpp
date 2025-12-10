#pragma once

#include <matjson.hpp>

class Backup {
protected:
	static void writeFile(matjson::Value const& data);

public:
	static matjson::Value getFile();
	static bool fileExists();
	static void createBackup(matjson::Value const& data);
	static void loadBackup();
};