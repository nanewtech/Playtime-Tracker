#include "./menuPopup.hpp"
#include <Geode/Geode.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>

#include <cvolton.level-id-api/include/EditorIDs.hpp>

#include "../managers/data.hpp"
#include "../managers/settings.hpp"

using namespace geode::prelude;

// idk why but this just works anythign else doesnt idunoooooooo
class OpenSettings : public CCLayer {
public:
    void open(CCObject*) {
        geode::openSettingsPopup(Mod::get(), true);
    }
};

bool MenuPopup::setup(GJGameLevel* const& level) {
	this->setTitle("Playtime Tracker");
    this->setID("Playtime-Tracker-Popup");

    std::string levelID = std::to_string(EditorIDs::getID(level));
    if (level->m_levelType == GJLevelType::Editor) levelID = "Editor-" + levelID;

    std::string levelName = level->m_levelName.c_str();

    auto subtitleLabel = CCLabelBMFont::create(CCString::create(levelName + " - Sessions: " + std::to_string(data::getSessionCount(levelID)))->getCString(), "goldFont.fnt");
    subtitleLabel->setScale(0.5f);
    subtitleLabel->setPosition({150.f, 233.f});

    m_mainLayer->addChild(subtitleLabel);

    int totalPlaytime = data::getPlaytimeRaw(levelID);
    int sessionPlaytime = data::getLatestSession(levelID);

	auto totalTitle = CCLabelBMFont::create("Total Playtime:", "goldFont.fnt");
    auto totalValue = CCLabelBMFont::create(CCString::create(data::formattedPlaytime(totalPlaytime))->getCString(), "bigFont.fnt");
    auto sessionTitle = CCLabelBMFont::create("Last Session:", "goldFont.fnt");
    auto sessionValue = CCLabelBMFont::create(CCString::create(data::formattedPlaytime(sessionPlaytime))->getCString(), "bigFont.fnt");

    totalTitle->setScale(0.75f);
    totalValue->setScale(0.35f);
    sessionTitle->setScale(0.75f);
    sessionValue->setScale(0.35f);

    // the scroll thing
    auto scrollLayer = geode::ScrollLayer::create({ 10.f,10.f,265.f,195.f});
    scrollLayer->setAnchorPoint({ 0.0f, 0.0f });
    scrollLayer->setID("scroll-layer");
    // scrollbar!?!? lets hope it works without anything else
    auto scrollbar = geode::Scrollbar::create(scrollLayer);
    scrollbar->setAnchorPoint({ 0.f, 0.f });
    scrollbar->setPosition({ 278.f, 12.f });

    auto scrollBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
    scrollBg->setAnchorPoint({ 0, 0 });
    scrollBg->setColor({ 0, 0, 0 });
    scrollBg->setOpacity(128);
    scrollBg->setContentSize({ 280.f, 200.f });
    scrollBg->setID("content-bg");
    m_mainLayer->addChildAtPosition(scrollBg, geode::Anchor::BottomLeft, { 10.f, 10.f });


    auto contentLayout = geode::AxisLayout::create(geode::Axis::Column);
    contentLayout->setAxisAlignment(geode::AxisAlignment::End);
    contentLayout->setAutoScale(false);
    contentLayout->setCrossAxisLineAlignment(geode::AxisAlignment::Start);
    contentLayout->setAxisReverse(true);
    
    

    auto content = scrollLayer->getChildByID("content-layer");
    content->setPosition({ 10.f,0.f });
    content->setLayout(contentLayout);
    

    // total and session
    content->addChild(totalTitle);
    content->addChild(totalValue);
    content->addChild(sessionTitle);
    content->addChild(sessionValue);


    // data
    std::string timeAttemptStat = data::formattedPlaytime(data::getPlaytimeRaw(levelID) / 1);
    if (level->m_attempts != 0) timeAttemptStat = data::formattedPlaytime(data::getPlaytimeRaw(levelID) / level->m_attempts);
    std::string timeSessionsStat = data::formattedPlaytime(data::getPlaytimeRaw(levelID) / 1);
    if (data::getSessionCount(levelID) != 0) timeSessionsStat = data::formattedPlaytime(data::getPlaytimeRaw(levelID) / data::getSessionCount(levelID));

    auto statsLabel = CCLabelBMFont::create("Level Stats", "goldFont.fnt");
    auto lastPlayedLabel = CCLabelBMFont::create(CCString::create("Last Played: Never")->getCString(), "bigFont.fnt");
    if (data::getSessionCount(levelID) > 0) lastPlayedLabel = CCLabelBMFont::create(CCString::create("Last Played: " + data::getPlayedFormatted(data::getLastPlayedRaw(levelID)))->getCString(), "bigFont.fnt");
    auto timeAttemptLabel = CCLabelBMFont::create(CCString::create("Time/Attempt: " + timeAttemptStat)->getCString(), "bigFont.fnt");
    auto timeSessionsLabel = CCLabelBMFont::create(CCString::create("Time/Session: " + timeSessionsStat)->getCString(), "bigFont.fnt");

    statsLabel->setScale(0.75f);
    lastPlayedLabel->setScale(0.35f);
    timeAttemptLabel->setScale(0.3f);
    timeSessionsLabel->setScale(0.3f);

    content->addChild(statsLabel);
    content->addChild(lastPlayedLabel);
    content->addChild(timeAttemptLabel);
    content->addChild(timeSessionsLabel);


    //sessions
    auto sessionLabel = CCLabelBMFont::create("Sessions", "goldFont.fnt");
    sessionLabel->setScale(0.75f);
    content->addChild(sessionLabel);
    for (int i = data::getSessionCount(levelID) - 1; i >= 0; i--) {
        auto menu = SessionMenuElement(levelID, i);
        menu->setID(fmt::format("session-{}", i + 1));
        content->addChild(menu);
    }


    
    
    content->setContentSize({ 265.f, 250.f + 25.f * data::getSessionCount(levelID) });

    content->updateLayout();

    scrollLayer->scrollToTop();


    // extra buttons (delete and settings stuff)
    auto extrabuttons = m_mainLayer->getChildByType<CCMenu>(0);
    
    auto settingsSpr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    settingsSpr->setScale(0.75f);

    auto settingsButton = CCMenuItemSpriteExtra::create(
        settingsSpr,
        this,
        menu_selector(OpenSettings::open)
    );

    settingsButton->setPosition({300.f, 272.f});

    extrabuttons->addChild(settingsButton);



    m_mainLayer->addChild(scrollbar);
    m_mainLayer->addChild(scrollLayer);
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

CCMenu* MenuPopup::SessionMenuElement(std::string levelID, int index) {
    auto menu = CCMenu::create();
    menu->setContentSize({ 265.f, 25.f });

    auto sessionTitle = CCLabelBMFont::create(CCString::create("Session " + std::to_string(index + 1) + " - " + data::getPlayedFormatted(data::getPlayedRawAtIndex(levelID, index)))->getCString(), "chatFont.fnt");
    auto sessionPlaytime = CCLabelBMFont::create(CCString::create(data::formattedPlaytime(data::getSessionPlaytimeRawAtIndex(levelID, index)))->getCString(), "chatFont.fnt");

    sessionTitle->setPosition({ 0.f,25.f });
    sessionTitle->setScale(0.725f);
    sessionTitle->setAnchorPoint({ 0.f,1.f });

    sessionPlaytime->setScale(0.725f);
    sessionPlaytime->setAnchorPoint({ 0.f,0.f });

    menu->addChild(sessionTitle);
    menu->addChild(sessionPlaytime);

    return menu;
}