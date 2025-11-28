#pragma once

#include <ctime>
#include <matjson.hpp>
#include <string>

class Data {
public: 
	static bool fileExists();

	static matjson::Value getFile();

	static void startLevel(std::string const& levelID);

	static void pauseLevel(std::string const& levelID);

	static void resumeLevel(std::string const& levelID);

	static void exitLevel(std::string const& levelID);

	static int getLatestSession(std::string const& levelID);

	static int getSessionPlaytimeRaw(std::string const& levelID);

	static int getPlaytimeRaw(std::string const& levelID);

	static std::string formattedPlaytime(int playtime);

	static tm* getLastPlayedRaw(std::string const& levelID);

	static tm* getPlayedRawAtIndex(std::string const& levelID, int index);

	static int getSessionPlaytimeRawAtIndex(std::string const& levelID, int index);

	static std::string getPlayedFormatted(tm* localTimestamp);

	static int getSessionCount(std::string const& levelID);

	static void deleteLevelData(std::string const& levelID);

	static void deleteSessionAtIndex(std::string const& levelID, int index);

	static void fixSessionAtIndex(std::string const& levelID, int index);
};