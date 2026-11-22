#include <ctime>
#include <Geode/Geode.hpp>
#include <Geode/modify/EditLevelLayer.hpp>

#include <cvolton.level-id-api/EditorIDs.hpp>

#include "../managers/data.hpp"
#include "../managers/settings.hpp"
#include "../layers/menuPopup.hpp"

using namespace geode::prelude;

class $modify(PTEditLevelLayer, EditLevelLayer) {
	struct Fields {
		std::string m_levelID;
	};
	
	bool init(GJGameLevel* level) {
		if (!(EditLevelLayer::init(level))) {
			return false;
		}
		
		auto buttonSprite = CCSprite::createWithSpriteFrameName("GJ_plainBtn_001.png");
		auto clockSprite = CCSprite::create("clock.png"_spr);
		clockSprite->setPosition(buttonSprite->getContentSize() / 2);
		buttonSprite->addChild(clockSprite);

		float spriteScale = Settings::getInfoLayerPosition() == "Left" ? .6f : .8f;

			buttonSprite->setScale(spriteScale);

		// auto sprite = CircleButtonSprite::createWithSprite("clock.png"_spr);
		// auto sprite = CCSprite::create("playtimeButton.png"_spr);
		auto playtimeButton = CCMenuItemSpriteExtra::create(
			buttonSprite,
			this,
			menu_selector(PTEditLevelLayer::onPlaytimeButton)
		);


		auto leftButtonMenu = this->getChildByID("level-actions-menu");
		auto folderMenu = this->getChildByID("folder-menu");

		playtimeButton->setID("playtime-tracker-button");
		

		m_fields->m_levelID = fmt::format("Editor-{}", EditorIDs::getID(level));
		
		
		playtimeButton->setZOrder(1);

		if (Settings::getInfoLayerPosition() == "Left") {
			leftButtonMenu->addChild(playtimeButton);
			leftButtonMenu->updateLayout();
		}
		else {
			folderMenu->addChild(playtimeButton);
			folderMenu->updateLayout();
		}

		return true;
	}

	void onPlaytimeButton(CCObject* sender) {
		std::string str = m_fields->m_levelID;
		auto totalPlaytime = Data::getPlaytimeRaw(str);
		auto lastPlaytime = Data::getLatestSession(str);

		// call menupopup here as well

		MenuPopup::create(this->m_level)->show();
		/*
			FLAlertLayer::create(
				"Playtime Tracker",
				CCString::create("Total Playtime: " + data::formattedPlaytime(totalPlaytime) + " Last Session: " + data::formattedPlaytime(lastPlaytime))->getCString(),
				"Close")->show();
		*/
	}
};