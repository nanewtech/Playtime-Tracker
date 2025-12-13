#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>

#include "../managers/data.hpp"

using namespace geode::prelude;

class $modify(PTEndLevelLayer, EndLevelLayer) {
	struct Fields {
		std::string m_levelID = "-1";
	};

	void customSetup() {
		EndLevelLayer::customSetup();
		m_fields->m_levelID = Mod::get()->getSavedValue<std::string>("current-level-id");
	}

	void onReplay(CCObject * sender) {
		Data::resumeLevel(m_fields->m_levelID);
		Mod::get()->setSavedValue<bool>("is-paused", false);
		EndLevelLayer::onReplay(sender);
	}

	void onRestartCheckpoint(CCObject* sender) {
		Data::resumeLevel(m_fields->m_levelID);
		Mod::get()->setSavedValue<bool>("is-paused", false);
		EndLevelLayer::onRestartCheckpoint(sender);
	}

	void onEdit(CCObject* sender) {
		Data::exitLevel(m_fields->m_levelID);
		Mod::get()->setSavedValue<bool>("is-paused", false);

		EndLevelLayer::onEdit(sender);
	}
	
};