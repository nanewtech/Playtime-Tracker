#include <ctime>
#include <Geode/Geode.hpp>
#include <Geode/modify/EditLevelLayer.hpp>

#include <cvolton.level-id-api/include/EditorIDs.hpp>

#include "../managers/data.hpp"
#include "../managers/settings.hpp"
#include "../layers/menuPopup.hpp"

using namespace geode::prelude;

class $modify(PTEditLevelLayer, EditLevelLayer) {
	struct Fields {
		GJGameLevel* m_level;
	};
	
	bool init(GJGameLevel* level) {
		if (!(EditLevelLayer::init(level))) {
			return false;
		}

		m_fields->m_level = level;

		auto sprite = CCSprite::create("playtimeButton.png"_spr);
		auto playtimeButton = CCMenuItemSpriteExtra::create(
			sprite,
			this,
			menu_selector(PTEditLevelLayer::onPlaytimeButton)
		);

		sprite->setScale(0.75f);

		auto infoButtonMenu = this->getChildByID("info-button-menu");

		playtimeButton->setID("playtime-tracker-button");
		

		playtimeButton->setUserObject(CCString::create("Editor-" + std::to_string(EditorIDs::getID(level))));
		
		
		playtimeButton->setZOrder(1);

		float x = 34.5f;
		float y = infoButtonMenu->getChildByID("settings-button")->getPositionY();

		playtimeButton->setPosition({ x, y });

		infoButtonMenu->addChild(playtimeButton);
		infoButtonMenu->updateLayout();

		return true;
	}

	void onPlaytimeButton(CCObject* sender) {
		auto obj = static_cast<CCNode*>(sender)->getUserObject();

		std::string str = static_cast<CCString*>(obj)->getCString();
		auto totalPlaytime = data::getPlaytimeRaw(str);
		auto lastPlaytime = data::getLatestSession(str);

		// call menupopup here as well

		MenuPopup::create(m_fields->m_level)->show();
		/*
			FLAlertLayer::create(
				"Playtime Tracker",
				CCString::create("Total Playtime: " + data::formattedPlaytime(totalPlaytime) + " Last Session: " + data::formattedPlaytime(lastPlaytime))->getCString(),
				"Close")->show();
		*/
	}
};