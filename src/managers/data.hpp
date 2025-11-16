#ifndef data_h
#define data_h

class data {
public : 

	static void initializeLevel(int levelID);

	static void startLevel(int levelID);

	static void pauseLevel(int levelID);

	static void resumeLevel(int levelID);

	static void exitLevel(int levelID);
};
#endif // !data_h