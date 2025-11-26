#pragma once
#define settings_h

class Settings {
public:
	static bool getRemovePauses();

	static bool getShortText();

	static std::string getHighestConvert();

	static std::string getInfoLayerPosition();

	static bool getStopOnCompletion();

	static std::string getTimeFormat();

	static bool getUseCustomTimeFormat();

	static std::string getCustomTimeFormat();

	static bool getDisableDeletionPopup();
};