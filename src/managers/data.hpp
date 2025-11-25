#pragma once
#define data_h

class data {
public : 

	static void startLevel(std::string levelID);

	static void pauseLevel(std::string levelID);

	static void resumeLevel(std::string levelID);

	static void exitLevel(std::string levelID);

	static int getLatestSession(std::string levelID);

	static int getSessionPlaytimeRaw(std::string levelID);

	static int getPlaytimeRaw(std::string levelID);

	static std::string formattedPlaytime(int playtime);

	static tm* getLastPlayedRaw(std::string levelID);

	static tm* getPlayedRawAtIndex(std::string levelID, int index);

	static int getSessionPlaytimeRawAtIndex(std::string levelID, int index);

	static std::string getPlayedFormatted(tm* localTimestamp);

	static int getSessionCount(std::string levelID);

	static void deleteLevelData(std::string levelID);

	static void deleteSessionAtIndex(std::string levelID, int index);
};