#include <ctime>
#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

#include "../managers/data.hpp"

using namespace geode::prelude;


class $modify(PTLevelInfoLayer, LevelInfoLayer) {

	

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
			menu_selector(PTLevelInfoLayer::onPlaytimeButton)
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

	void onPlaytimeButton(CCObject * sender) {

		auto id = sender->getTag();
		//auto obj = static_cast<CCNode*>(sender)->getUserObject();

		//auto str = static_cast<CCString*>(obj)->getCString();

		auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);

		btn->setScale(0.8f);

		data::initializeLevel(id);


		FLAlertLayer::create(
			"Playtime Tracker",
			CCString::createWithFormat("%d", id)->getCString(),
			"Close")->show();
	}
};
