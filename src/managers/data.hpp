#ifndef data_h
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
};
#endif // !data_h