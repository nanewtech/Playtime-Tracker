#include <ctime>
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <cvolton.level-id-api/include/EditorIDs.hpp>

#include "../managers/data.hpp"
#include "../managers/backup.hpp"
#include "../managers/settings.hpp"

using namespace geode::prelude;


class $modify(PTPlayLayer, PlayLayer) {
	struct Fields {
	std::string m_levelID;
	};
	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		if (!PlayLayer::init(level, useReplay, dontCreateObjects)) {
			return false;
		}
		time_t timestamp;

		Mod::get()->setSavedValue<bool>("is-paused", false);

		Mod::get()->setSavedValue<int>("current-level-best", level->m_normalPercent.value());

		m_fields->m_levelID = fmt::to_string(EditorIDs::getID(level));

		if (level->m_levelType == GJLevelType::Editor) m_fields->m_levelID = fmt::format("Editor-{}", EditorIDs::getID(level));

		Mod::get()->setSavedValue<std::string>("current-level-id", m_fields->m_levelID);

		if (Settings::getSessionType() == "Exit Game") {
			if (Data::isLevelPlayedSession(m_fields->m_levelID)) {
				Data::resumeLevel(m_fields->m_levelID, true);
				return true;
			}
			else {
				Data::appendPlayedLevel(m_fields->m_levelID);
			}
		}

		Data::startLevel(m_fields->m_levelID);
		return true;
	}

	void resume() {
		Data::resumeLevel(m_fields->m_levelID);

		auto pauseTimestamp = Mod::get()->getSavedValue<time_t>("pause-timestamp");
		time_t currTimestamp = time(nullptr);

		if (std::difftime(currTimestamp, pauseTimestamp) >= Settings::getAFKThreshold() && Settings::getAFKEnable() && !Settings::getRemovePauses()) {
			Data::appendPauseTimestamp(m_fields->m_levelID, pauseTimestamp);
			Data::resumeLevel(m_fields->m_levelID, true);
		}

		Mod::get()->setSavedValue<bool>("is-paused", false);
		PlayLayer::resume();
	}

	void levelComplete() {
		Mod::get()->setSavedValue<bool>("is-paused", true);
		
		Data::pauseLevel(m_fields -> m_levelID);

		PlayLayer::levelComplete();
	}

	void onQuit() {
		Data::exitLevel(m_fields->m_levelID);

		time_t timestamp;

		Mod::get()->setSavedValue<bool>("is-paused", false);
		PlayLayer::onQuit();
	}
};