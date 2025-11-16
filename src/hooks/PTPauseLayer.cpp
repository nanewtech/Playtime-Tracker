#include <ctime>
#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>

#include "../managers/data.hpp"

using namespace geode::prelude;

class $modify(PTPauseLayer, PauseLayer) {

	void customSetup() {
		PauseLayer::customSetup();

		time_t timestamp;
		log::debug("PAUSED AT: {}", fmt::to_string(time(&timestamp)));

		data::pauseLevel(Mod::get()->getSavedValue<int>("current-level-id"));

		auto ptButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
			this,
			menu_selector(PTPauseLayer::onPtButton)
		);

		auto menu = this->getChildByID("left-button-menu");
		ptButton->setID("playtime-tracker-button");
		ptButton->setScale(0.7f);
		menu->addChild(ptButton);

		menu->updateLayout();

	}
	void onQuit(CCObject * sender) {

		time_t timestamp;
		log::debug("QUIT AT: {}", fmt::to_string(time(&timestamp)));

		data::exitLevel(Mod::get()->getSavedValue<int>("current-level-id"));

		PauseLayer::onQuit(sender);
	}

	void onResume(CCObject * sender) {
		time_t timestamp;
		log::debug("RESUMED AT: {}", fmt::to_string(time(&timestamp)));

		data::resumeLevel(Mod::get()->getSavedValue<int>("current-level-id"));


		PauseLayer::onResume(sender);
	}
	void onPtButton(CCObject * sender) {

		auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);

		btn->setScale(0.7f);

		FLAlertLayer::create(
			"Playtime Tracker",
			"test text",
			"close")->show();
	}
};