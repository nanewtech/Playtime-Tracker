#include "./menuPopup.hpp"
#include <Geode/Geode.hpp>
#include <Geode/ui/ScrollLayer.hpp>

#include <cvolton.level-id-api/include/EditorIDs.hpp>

#include "../managers/data.hpp"
#include "../managers/settings.hpp"

using namespace geode::prelude;

bool MenuPopup::setup(GJGameLevel* const& level) {
	this->setTitle("Playtime Tracker");
    this->setID("Playtime-Tracker-Popup");

    

    std::string levelID = std::to_string(EditorIDs::getID(level));
    if (level->m_levelType == GJLevelType::Editor) levelID = "Editor-" + levelID;

    int totalPlaytime = data::getPlaytimeRaw(levelID);
    int sessionPlaytime = data::getLatestSession(levelID);

	auto totalTitle = CCLabelBMFont::create("Total Playtime:", "goldFont.fnt");
    auto totalValue = CCLabelBMFont::create(CCString::create(data::formattedPlaytime(totalPlaytime))->getCString(), "bigFont.fnt");
    auto sessionTitle = CCLabelBMFont::create("Last Session Playtime:", "goldFont.fnt");
    auto sessionValue = CCLabelBMFont::create(CCString::create(data::formattedPlaytime(sessionPlaytime))->getCString(), "bigFont.fnt");

    auto colLayout = ColumnLayout::create();
    colLayout->setAxisReverse(true);
    colLayout->setCrossAxisLineAlignment(geode::AxisAlignment::Start);

    auto contentLayer = geode::GenericContentLayer::create(150.f, 200.f);
    if (!(settings::getShortText)) contentLayer->setScale(.725f);

    // TODO: remove magic values, improve and finish layout

    contentLayer->setID("full-Layer");
    contentLayer->setContentSize({ 387.237f, 100.f});
    contentLayer->setScale(.625f);
    contentLayer->setPosition({ 150.f, 200.f });
    contentLayer->setLayout(colLayout);
    contentLayer->addChild(totalTitle);
    contentLayer->addChild(totalValue);
    contentLayer->addChild(sessionTitle);
    contentLayer->addChild(sessionValue);

    contentLayer->updateLayout();

    m_mainLayer->addChild(contentLayer);
	// m_mainLayer->addChildAtPosition(label, Anchor::Center);
	return true;
}

MenuPopup* MenuPopup::create(GJGameLevel* const& level) {
    auto ret = new MenuPopup();
    ret->m_level = level;
    if (ret->initAnchored(300.f, 275.f, level)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}
