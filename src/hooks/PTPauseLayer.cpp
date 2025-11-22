#include <ctime>
#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>

#include "../managers/data.hpp"
#include "../managers/settings.hpp"

using namespace geode::prelude;

class $modify(PTPauseLayer, PauseLayer) {
	struct Fields {
		std::string m_levelID = "-1";
	};
	void customSetup() {
		PauseLayer::customSetup();

		log::debug("IS PAUSED!!!!");
		Mod::get()->setSavedValue<bool>("is-paused", true);

		m_fields->m_levelID = Mod::get()->getSavedValue<std::string>("current-level-id");

		time_t timestamp;
		log::debug("PAUSED AT: {}", fmt::to_string(time(&timestamp)));

		data::pauseLevel(m_fields->m_levelID);
		auto sprite = CCSprite::create("playtimeButton.png"_spr);
		sprite->setScale(0.8f);

		auto ptButton = CCMenuItemSpriteExtra::create(
			sprite,
			this,
			menu_selector(PTPauseLayer::onPtButton)
		);

		auto menu = this->getChildByID("left-button-menu");
		ptButton->setID("playtime-tracker-button");
		menu->addChild(ptButton);

		menu->updateLayout();

	} /*
	void onQuit(CCObject * sender) {

		time_t timestamp;
		log::debug("QUIT AT: {}", fmt::to_string(time(&timestamp)));
		log::debug("PauseLayer onQuit() CALLED!!!!");
		data::exitLevel(m_fields->m_levelID);

		PauseLayer::onQuit(sender);
	}
	*/
	void onResume(CCObject* sender) {
		time_t timestamp;
		log::debug("RESUMED AT: {}", fmt::to_string(time(&timestamp)));
		log::debug("PauseLayer onResume() CALLED!!!!");
		data::resumeLevel(m_fields->m_levelID);

		log::debug("IS NOT PAUSED!!!!");
		Mod::get()->setSavedValue<bool>("is-paused", false);

		PauseLayer::onResume(sender);
	}
	void onPtButton(CCObject* sender) {

		// auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);

		// TODO: Make popup for pause menu, call it here

		auto playtime = data::getSessionPlaytimeRaw(m_fields->m_levelID);
			FLAlertLayer::create(
				"Playtime Tracker",
				CCString::create("Current Session: " + data::formattedPlaytime(playtime))->getCString(),
				"close")->show();
	}
	void onPracticeMode(CCObject* sender) {

		data::resumeLevel(m_fields->m_levelID);
		Mod::get()->setSavedValue<bool>("is-paused", false);
		PauseLayer::onPracticeMode(sender);
	}

	void onEdit(CCObject* sender) {
		Mod::get()->setSavedValue<bool>("is-paused", false);
		data::exitLevel(m_fields->m_levelID);
		PauseLayer::onEdit(sender);
	}
};