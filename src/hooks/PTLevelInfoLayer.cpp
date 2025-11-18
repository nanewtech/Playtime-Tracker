#include <ctime>
#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

#include "../managers/data.hpp"
#include "../managers/settings.hpp"

using namespace geode::prelude;


class $modify(PTLevelInfoLayer, LevelInfoLayer) {

	

	bool init(GJGameLevel * level, bool challenge) {
		if (!LevelInfoLayer::init(level, challenge)) {
			return false;
		}
		auto savedir = Mod::get()->getSaveDir();
		auto savedirstr = savedir.string();


		auto sprite = CCSprite::create("playtimeButton.png"_spr);
		if(settings::getInfoLayerPosition() == "Bottom") sprite->setScale(0.8f);

		auto playtimeButton = CCMenuItemSpriteExtra::create(
			sprite,
			this,
			menu_selector(PTLevelInfoLayer::onPlaytimeButton)
		);

		auto otherMenu = this->getChildByID("other-menu");
		auto leftSideMenu = this->getChildByID("left-side-menu");

		playtimeButton->setID("playtime-tracker-button");
		// playtimeButton->setTag(level->m_levelID.value());
		playtimeButton->setUserObject(CCString::create(std::to_string(level->m_levelID.value())));
		playtimeButton->setZOrder(1);


		if (settings::getInfoLayerPosition() == "Bottom") {
			float x = otherMenu->getChildByID("favorite-button")->getPositionX();
			
			float y = 76.5f; // fucking find a way to get rid of magic number.... :3
			

			otherMenu->addChild(playtimeButton);
			
			
			// playtimeButton->setScale(0.8f);



			// if (otherMenu->getChildByID("dt-skull-button") == nullptr) y = this->getChildByID("settings-menu")->getChildByID("settings-button")->getPositionY();
			// ^ i wanted to move depending on if death tracker has its button there but it can never find it sooooo blehhhh


			playtimeButton->setPosition({ x, y });
			otherMenu->updateLayout();
		}
		else {
			leftSideMenu->addChild(playtimeButton);

			playtimeButton->setID("playtime-tracker-button");
			// playtimeButton->setScale(0.8f);
			// playtimeButton->setUserObject(CCString::create(level->m_levelID.value()));
			playtimeButton->setUserObject(CCString::create(std::to_string(level->m_levelID.value())));

			leftSideMenu->updateLayout();
		}

		

		return true;
	}

	void onPlaytimeButton(CCObject * sender) {

		auto obj = static_cast<CCNode*>(sender)->getUserObject();

		auto playtime = data::getPlaytimeRaw(std::stoi(static_cast<CCString*>(obj)->getCString()));
		// log::debug("Playtime Raw: {}", fmt::to_string(playtime));
		// log::debug("Playtime Formatted: {}", fmt::to_string(data::formattedPlaytime(playtime, false)));

		// auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);

		// btn->setScale(0.8f);

		FLAlertLayer::create(
			"Playtime Tracker",
			CCString::create(data::formattedPlaytime(playtime, false))->getCString(),
			"Close")->show();
	}
};
