#include <ctime>
#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>

#include "../managers/data.hpp"
#include "../layers/pausePopup.hpp"
#include "../managers/settings.hpp"

using namespace geode::prelude;

class $modify(PTPauseLayer, PauseLayer) {
	struct Fields {
		std::string m_levelID = "-1";
	};
	void customSetup() {
		PauseLayer::customSetup();

		Mod::get()->setSavedValue<bool>("is-paused", true);

		m_fields->m_levelID = Mod::get()->getSavedValue<std::string>("current-level-id");

		time_t timestamp = time(nullptr);

		Mod::get()->setSavedValue<time_t>("pause-timestamp", timestamp);

		Data::pauseLevel(m_fields->m_levelID);
		auto buttonSprite = CCSprite::createWithSpriteFrameName("GJ_plainBtn_001.png");
		auto clockSprite = CCSprite::create("clock.png"_spr);
		clockSprite->setPosition(buttonSprite->getContentSize() / 2);
		buttonSprite->addChild(clockSprite);
		// auto sprite = CCSprite::create("playtimeButton.png"_spr);
		buttonSprite->setScale(0.8f);
		if (Settings::getPauseLayerPosition() == "Right") buttonSprite->setScale(0.65f);

		auto ptButton = CCMenuItemSpriteExtra::create(
			buttonSprite,
			this,
			menu_selector(PTPauseLayer::onPtButton)
		);

		if (!Settings::getEnablePauseButton()) return;

		auto leftMenu = this->getChildByID("left-button-menu");
		auto rightMenu = this->getChildByID("right-button-menu");
		ptButton->setID("playtime-tracker-button");

		if (Settings::getPauseLayerPosition() == "Right") {
			rightMenu->addChild(ptButton);
			rightMenu->updateLayout();
		}
		else {
			leftMenu->addChild(ptButton);
			leftMenu->updateLayout();
		}

	} /*
	void onQuit(CCObject * sender) {

		time_t timestamp;
		log::debug("QUIT AT: {}", fmt::to_string(time(&timestamp)));
		log::debug("PauseLayer onQuit() CALLED!!!!");
		data::exitLevel(m_fields->m_levelID);

		PauseLayer::onQuit(sender);
	}
	*/
	/* void onResume(CCObject* sender) {
		time_t timestamp;

		Data::resumeLevel(m_fields->m_levelID);

		Mod::get()->setSavedValue<bool>("is-paused", false);

		PauseLayer::onResume(sender);
	} */
	void onPtButton(CCObject* sender) {

		// auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);


		
		PausePopup::create(m_fields->m_levelID)->show();

		/*
		auto playtime = Data::getSessionPlaytimeRaw(m_fields->m_levelID);
			FLAlertLayer::create(
				"Playtime Tracker",
				CCString::create("Current Session: " + Data::formattedPlaytime(playtime))->getCString(),
				"close")->show();
		*/
	}
	/* void onPracticeMode(CCObject* sender) {

		Data::resumeLevel(m_fields->m_levelID);
		Mod::get()->setSavedValue<bool>("is-paused", false);
		PauseLayer::onPracticeMode(sender);
	} */

	void onEdit(CCObject* sender) {

		auto pauseTimestamp = Mod::get()->getSavedValue<time_t>("pause-timestamp");
		time_t currTimestamp = time(nullptr);

		if (std::difftime(currTimestamp, pauseTimestamp) >= Settings::getAFKThreshold() && Settings::getAFKEnable() && !Settings::getRemovePauses()) {
			Data::appendPauseTimestamp(m_fields->m_levelID, pauseTimestamp);
		} else 	Data::exitLevel(m_fields->m_levelID);

		if (Settings::getSessionType() == "Exit Game") {
			if (Data::isLevelPlayedSession(m_fields->m_levelID)) {
				Data::addSessionAttempts(m_fields->m_levelID, PlayLayer::get()->m_attempts);
			}
			else {
				Data::appendAttempts(m_fields->m_levelID, PlayLayer::get()->m_attempts);
			}
		}
		else {
			Data::appendAttempts(m_fields->m_levelID, PlayLayer::get()->m_attempts);
		}

		if (Settings::getSessionType() == "Exit Game") {
			if (!Data::isLevelPlayedSession(m_fields->m_levelID)) {
				Data::appendPlayedLevel(m_fields->m_levelID);
			}
		}

		Mod::get()->setSavedValue<bool>("is-paused", false);
		PauseLayer::onEdit(sender);
	}

	void onRestart(CCObject* sender) {
			if (Mod::get()->getSavedValue<bool>("is-paused")) {
				Data::resumeLevel(m_fields->m_levelID);
				auto pauseTimestamp = Mod::get()->getSavedValue<time_t>("pause-timestamp");
				time_t currTimestamp = time(nullptr);

				if (std::difftime(currTimestamp, pauseTimestamp) >= Settings::getAFKThreshold() && Settings::getAFKEnable() && !Settings::getRemovePauses()) {
					Data::appendPauseTimestamp(m_fields->m_levelID, pauseTimestamp);
					Data::resumeLevel(m_fields->m_levelID, true);
				}
			}
			Mod::get()->setSavedValue<bool>("is-paused", false);

			PauseLayer::onRestart(sender);
	}

	void onRestartFull(CCObject* sender) {
		if (Mod::get()->getSavedValue<bool>("is-paused")) {
			Data::resumeLevel(m_fields->m_levelID);

			auto pauseTimestamp = Mod::get()->getSavedValue<time_t>("pause-timestamp");
			time_t currTimestamp = time(nullptr);

			if (std::difftime(currTimestamp, pauseTimestamp) >= Settings::getAFKThreshold() && Settings::getAFKEnable() && !Settings::getRemovePauses()) {
				Data::appendPauseTimestamp(m_fields->m_levelID, pauseTimestamp);
				Data::resumeLevel(m_fields->m_levelID, true);
			}
		}
		Mod::get()->setSavedValue<bool>("is-paused", false);

		PauseLayer::onRestartFull(sender);
	}
};