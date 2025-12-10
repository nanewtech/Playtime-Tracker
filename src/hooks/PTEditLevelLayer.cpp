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
		std::string m_levelID;
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


		auto infoButtonMenu = this->getChildByID("info-button-menu");
		auto folderMenu = this->getChildByID("folder-menu");

		playtimeButton->setID("playtime-tracker-button");
		

		m_fields->m_levelID = fmt::format("Editor-{}", EditorIDs::getID(level));
		
		
		playtimeButton->setZOrder(1);

		if (Settings::getInfoLayerPosition() == "Bottom") {

			sprite->setScale(0.75f);

			float x = 34.5f;
			float y = infoButtonMenu->getChildByID("settings-button")->getPositionY();

			playtimeButton->setPosition({ x, y });

			infoButtonMenu->addChild(playtimeButton);
			infoButtonMenu->updateLayout();
		}
		else {
			sprite->setScale(0.8f);

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

		MenuPopup::create(m_fields->m_level)->show();
		/*
			FLAlertLayer::create(
				"Playtime Tracker",
				CCString::create("Total Playtime: " + data::formattedPlaytime(totalPlaytime) + " Last Session: " + data::formattedPlaytime(lastPlaytime))->getCString(),
				"Close")->show();
		*/
	}
};