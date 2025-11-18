#ifndef data_h
#define data_h

class data {
public : 

	static void startLevel(int levelID);

	static void pauseLevel(int levelID);

	static void resumeLevel(int levelID);

	static void exitLevel(int levelID);

	static int getSessionPlaytimeRaw(int levelID);

	static int getPlaytimeRaw(int levelID);

	static std::string formattedPlaytime(int playtime, bool longString);
};
#endif // !data_h