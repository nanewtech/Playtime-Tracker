#include <ctime>
#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(ptLevelInfoLayer, LevelInfoLayer) {



	bool init(GJGameLevel * level, bool challenge) {
		if (!LevelInfoLayer::init(level, challenge)) {
			return false;
		}
		auto savedir = Mod::get()->getSaveDir();
		auto savedirstr = savedir.string();


		log::debug("Save Directory: {}", fmt::to_string(savedirstr));



		auto playtimeButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
			this,
			menu_selector(ptLevelInfoLayer::onPlaytimeButton)
		);

		auto menu = this->getChildByID("other-menu");
		menu->addChild(playtimeButton);

		playtimeButton->setID("playtime-tracker-button");
		playtimeButton->setPosition({ 38.f, 76.5f });
		playtimeButton->setScale(0.8f);
		//playtimeButton->setUserObject(CCString::create(level->m_levelID.value()));
		playtimeButton->setTag(level->m_levelID.value());

		menu->updateLayout();

		return true;
	}

	void onPlaytimeButton(CCObject* sender) {

		auto id = sender->getTag();
		//auto obj = static_cast<CCNode*>(sender)->getUserObject();

		//auto str = static_cast<CCString*>(obj)->getCString();

		auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);

		btn->setScale(0.8f);

		FLAlertLayer::create(
			"Playtime Tracker",
			CCString::createWithFormat("%d", id)->getCString(),
			"Close")->show();
	}
};

class $modify(ptPauseLayer, PauseLayer) {
	
	void customSetup(){
		PauseLayer::customSetup();

		auto ptButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
			this,
			menu_selector(ptPauseLayer::onPtButton)
		);

		auto menu = this->getChildByID("left-button-menu");
		ptButton->setID("playtime-tracker-button");
		ptButton->setScale(0.7f);
		menu->addChild(ptButton);

		menu->updateLayout();

	}

	void onPtButton(CCObject* sender) {
		
		auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);

		btn->setScale(0.7f);

		FLAlertLayer::create(
			"Playtime Tracker",
			"test text",
			"close")->show();
	}
};
class $modify(ptPlayLayer, PlayLayer) {

	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		if (!PlayLayer::init(level, useReplay, dontCreateObjects)) {
			return false;
		}
		time_t timestamp;
		log::debug("STARTED LEVEL AT: {}", fmt::to_string(time(&timestamp)));

		return true;
	}


};