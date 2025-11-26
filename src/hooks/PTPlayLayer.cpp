#include <ctime>
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <cvolton.level-id-api/include/EditorIDs.hpp>

#include "../managers/data.hpp"

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
		log::debug("UNPAUSED!!!!!");

		Mod::get()->setSavedValue<int>("current-level-best", level->m_normalPercent.value());

		m_fields->m_levelID = std::to_string(EditorIDs::getID(level));
		
		if (level->m_levelType == GJLevelType::Editor) m_fields->m_levelID = "Editor-" + std::to_string(EditorIDs::getID(level));
		log::debug("EDITOR ID: {}", EditorIDs::getID(level));
		log::debug("LEVEL ID FIELD: {}", m_fields->m_levelID);

		Mod::get()->setSavedValue<std::string>("current-level-id", m_fields->m_levelID);

		log::debug("LEVEL ID SAVED VALUE: {}", Mod::get()->getSavedValue<std::string>("current-level-id"));


		log::debug("STARTED LEVEL: {}", fmt::to_string(Mod::get()->getSavedValue<std::string>("current-level-id")));
		log::debug("STARTED LEVEL AT: {}", fmt::to_string(time(&timestamp)));
		
		Data::startLevel(m_fields->m_levelID);

		return true;
	}

	void levelComplete() {
		Mod::get()->setSavedValue<bool>("is-paused", true);
		
		Data::pauseLevel(m_fields -> m_levelID);

		log::debug("PLAYLAYER levelComplete() CALLED!!!!");

		PlayLayer::levelComplete();
	}

	void onQuit() {
		Data::exitLevel(m_fields->m_levelID);

		time_t timestamp;
		log::debug("QUIT AT: {}", fmt::to_string(time(&timestamp)));

		log::debug("PLAYLAYER onQuit() CALLED!!!!");
		Mod::get()->setSavedValue<bool>("is-paused", false);
		PlayLayer::onQuit();
	}

	void resetLevel() {
		if (Mod::get()->getSavedValue<bool>("is-paused")) Data::resumeLevel(m_fields->m_levelID);
		Mod::get()->setSavedValue<bool>("is-paused", false);
		log::debug("UNPAUSED!!!!!");

		PlayLayer::resetLevel();
	}
};