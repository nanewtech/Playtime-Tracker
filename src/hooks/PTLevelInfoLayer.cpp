#include <ctime>
#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

#include <cvolton.level-id-api/include/EditorIDs.hpp>

#include "../managers/data.hpp"
#include "../managers/settings.hpp"
#include "../layers/menuPopup.hpp"

using namespace geode::prelude;


class $modify(PTLevelInfoLayer, LevelInfoLayer) {
	struct Fields {
		GJGameLevel* m_level;
	};
	

	bool init(GJGameLevel* level, bool challenge) {
		
		
		if (!LevelInfoLayer::init(level, challenge)) {
			return false;
		}

		m_fields->m_level = level;

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
		playtimeButton->setUserObject(CCString::create(std::to_string(EditorIDs::getID(level))));
		playtimeButton->setZOrder(1);


		if (settings::getInfoLayerPosition() == "Bottom") {
			float x = otherMenu->getChildByID("favorite-button")->getPositionX();
			
			float y = 76.5f; // fucking find a way to get rid of magic number.... :3
			

			
			
			// playtimeButton->setScale(0.8f);



			// if (otherMenu->getChildByID("dt-skull-button") == nullptr) y = this->getChildByID("settings-menu")->getChildByID("settings-button")->getPositionY();
			// ^ i wanted to move depending on if death tracker has its button there but it can never find it sooooo blehhhh
			// fix if i figure out what the fuck goes wrongggg


			playtimeButton->setPosition({ x, y });

			otherMenu->addChild(playtimeButton);
			otherMenu->updateLayout();
		}
		else {
			leftSideMenu->addChild(playtimeButton);

			// playtimeButton->setScale(0.8f);
			// playtimeButton->setUserObject(CCString::create(level->m_levelID.value()));

			leftSideMenu->updateLayout();
		}

		

		return true;
	}

	void onPlaytimeButton(CCObject* sender) {

		auto obj = static_cast<CCNode*>(sender)->getUserObject();

		auto totalPlaytime = data::getPlaytimeRaw(static_cast<CCString*>(obj)->getCString());
		auto lastPlaytime = data::getLatestSession(static_cast<CCString*>(obj)->getCString());
		
		// log::debug("Playtime Raw: {}", fmt::to_string(playtime));
		// log::debug("Playtime Formatted: {}", fmt::to_string(data::formattedPlaytime(playtime, false)));

		// auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);

		// btn->setScale(0.8f);
		

		MenuPopup::create(m_fields->m_level)->show();

		/*
			FLAlertLayer::create(
				"Playtime Tracker",
				CCString::create("Total Playtime: " + data::formattedPlaytime(totalPlaytime) + " Last Session: " + data::formattedPlaytime(lastPlaytime))->getCString(),
				"Close")->show();
		*/
	}
};
