#include <ctime>
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include "../managers/data.hpp"
#include "../managers/settings.hpp"

using namespace geode::prelude;


class $modify(PTPlayLayer, PlayLayer) {
	struct Fields {
		int m_levelID = 1;
	};
	bool init(GJGameLevel * level, bool useReplay, bool dontCreateObjects) {
		if (!PlayLayer::init(level, useReplay, dontCreateObjects)) {
			return false;
		}
		time_t timestamp;
		m_fields->m_levelID = level->m_levelID.value();
		Mod::get()->setSavedValue<int>("current-level-id", level->m_levelID.value());
		
		log::debug("STARTED LEVEL: {}", fmt::to_string(m_fields->m_levelID));
		log::debug("STARTED LEVEL AT: {}", fmt::to_string(time(&timestamp)));
		
		data::startLevel(m_fields->m_levelID);

		return true;
	}

	void levelComplete() {
		data::pauseLevel(m_fields -> m_levelID);

		log::debug("PLAYLAYER levelComplete() CALLED!!!!");

		PlayLayer::levelComplete();
	}

	void onQuit() {
		data::exitLevel(m_fields->m_levelID);

		time_t timestamp;
		log::debug("QUIT AT: {}", fmt::to_string(time(&timestamp)));

		log::debug("PLAYLAYER onQuit() CALLED!!!!");

		PlayLayer::onQuit();
	}
};