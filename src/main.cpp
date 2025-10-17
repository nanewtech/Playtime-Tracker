#include <Geode/Geode.hpp>


using namespace geode::prelude;


#include <Geode/modify/LevelInfoLayer.hpp>
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

		menu->updateLayout();

		return true;
	}

	void onPlaytimeButton(CCObject*) {
		FLAlertLayer::create("Geode", "Hello from my custom mod!", "OK")->show();
	}
};