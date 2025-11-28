#pragma once

#include <matjson.hpp>

class Backup {
protected:
	static void writeFile(matjson::Value data);

public:
	static matjson::Value getFile();
	static bool fileExists();
	static void createBackup(matjson::Value data);
	static void loadBackup();
};