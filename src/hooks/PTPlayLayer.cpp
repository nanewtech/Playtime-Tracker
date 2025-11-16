#include <ctime>
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include "../managers/data.hpp"

using namespace geode::prelude;


class $modify(PTPlayLayer, PlayLayer) {

	bool init(GJGameLevel * level, bool useReplay, bool dontCreateObjects) {
		if (!PlayLayer::init(level, useReplay, dontCreateObjects)) {
			return false;
		}
		time_t timestamp;
		int levelID = level->m_levelID.value();

		log::debug("STARTED LEVEL AT: {}", fmt::to_string(time(&timestamp)));
		
		Mod::get()->setSavedValue<int>("current-level-id", levelID);

		data::startLevel(levelID);

		return true;
	}


};