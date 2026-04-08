#pragma once

#include <ctime>
#include <matjson.hpp>
#include <string>

class Data {
public: 
	static bool fileExists(std::string const& levelID);

	static matjson::Value getFile(std::string const& levelID);

	static void startLevel(std::string const& levelID);

	static void pauseLevel(std::string const& levelID);

	static void resumeLevel(std::string const& levelID, bool removePauseOverride = false);

	static void exitLevel(std::string const& levelID);

	static int getLatestSession(std::string const& levelID);

	static int getSessionPlaytimeRaw(std::string const& levelID);

	static int getPlaytimeRaw(std::string const& levelID);

	static std::string formattedPlaytime(int playtime);

	static tm getLastPlayedRaw(std::string const& levelID);

	static tm getPlayedRawAtIndex(std::string const& levelID, int index);

	static int getSessionPlaytimeRawAtIndex(std::string const& levelID, int index);

	static std::string getPlayedFormatted(tm const& localTimestamp);

	static int getSessionCount(std::string const& levelID);

	static void deleteLevelData(std::string const& levelID);

	static void deleteSessionAtIndex(std::string const& levelID, int index);

	static void fixSessionAtIndex(std::string const& levelID, int index);

	static int getTotalPlaytime(std::string const& levelID);

	static bool isLevelPlayedSession(std::string const& levelID);

	static void appendPlayedLevel(std::string const& levelID);

	static void appendPauseTimestamp(std::string const& levelID, time_t timestamp);

	static void initAttemptsList(std::string const& levelID);

	static void appendAttempts(std::string const& levelID, int newAttempts);

	static void addSessionAttempts(std::string const& levelID, int newAttempts);

	static int getLevelAttempts(std::string const& levelID);

	static int getSessionAttemptsAtIndex(std::string const& levelID, int index);

	static int getAttemptSessionCount(std::string const& levelID);
};