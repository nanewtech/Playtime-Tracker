#include "menuPopup.hpp"
#include <Geode/Geode.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>

#include <cvolton.level-id-api/include/EditorIDs.hpp>

#include "../managers/data.hpp"
#include "../managers/settings.hpp"

class OpenSettings : public CCLayer {
public:
    void open(CCObject*) {
        geode::openSettingsPopup(Mod::get(), true);
    }
};


class DeleteButton : public CCLayer {
public:
    void onDeleteAllButton(CCObject* sender) {

        auto obj = static_cast<CCNode*>(sender)->getUserObject("level-id");
        std::string levelID = static_cast<CCString*>(obj)->getCString();
        geode::createQuickPopup(
            "Delete level data",
            "Are you sure you want to <cr>delete ALL data</c> on this level? (<cr>ALL your sessions will be deleted! This action is IRREVERSIBLE</c>)",
            "Don't delete", "Delete everything",
            [levelID](auto, bool btn2) {
                if (btn2) {
                    Data::deleteLevelData(levelID);
                    if (!Settings::getDisableDeletionPopup()) FLAlertLayer::create(
                        "Delete level Data",
                        "Deleted all data. Reopen the popup for changes to take effect. <cy>You can disable this popup in the settings.</c>",
                        "OK"
                    )->show();
                }
            }
        );
        
    }

    void onDeleteSessionButton(CCObject* sender) {
        int index = static_cast<CCNode*>(sender)->getTag();

        auto obj = static_cast<CCNode*>(sender)->getUserObject("level-id");
        std::string levelID = static_cast<CCString*>(obj)->getCString();
        geode::createQuickPopup(
            "Delete session",
            fmt::format("Are you sure you want to <cr>delete session {}</c> on this level? (<cr>This action is IRREVERSIBLE!</c>)", index + 1),
            "Don't delete", "Delete session",
            [levelID, index](auto, bool btn2) {
                if (btn2) {
                    Data::deleteSessionAtIndex(levelID, index);
                    if (!Settings::getDisableDeletionPopup()) FLAlertLayer::create(
                        "Delete session Data",
                        fmt::format("Deleted session {}. Reopen the popup for changes to take effect. <cy>You can disable this popup in the settings.</c>", index + 1),
                        "OK"
                    )->show();
                }
            }
        );
    }
};

class OpenAttempts : public CCLayer {
public:
    void open(CCObject* sender) {

        auto obj = static_cast<CCNode*>(sender)->getUserObject("level-id");
        std::string levelID = static_cast<CCString*>(obj)->getCString();

        auto levelObj = static_cast<CCNode*>(sender)->getUserData();
        auto level = static_cast<GJGameLevel*>(levelObj);

        int gameAttempts = level->m_attempts.value();
        int modAttempts = Data::getLevelAttempts(levelID);

        geode::createQuickPopup(
            "Overwrite attempt count",
            fmt::format("Are you sure you want to overwrite the game's \n<cy>{} tracked attempts</c> with the mods <cy>{} tracked attempts?</c> (<cr>This action is IRREVERSIBLE!</c>)", gameAttempts, modAttempts),
            "Don't overwrite", "Overwrite attempts",
            [level, modAttempts](auto, bool btn2) {
                if (btn2) {
                    level->setAttempts(modAttempts);
                }
            }
            );
    }
};

bool MenuPopup::init(GJGameLevel* level) {

    if (!Popup::init(300.f, 275.f)) return false;

	this->setTitle("Playtime Tracker");
    this->setID("Playtime-Tracker-Popup"_spr);

    std::string levelID = fmt::to_string(EditorIDs::getID(level));
    if (level->m_levelType == GJLevelType::Editor) levelID = fmt::format("Editor-{}", levelID);

    std::string levelName = level->m_levelName;

    auto subtitleLabel = CCLabelBMFont::create(fmt::format("{} - Sessions: {}", levelName, Data::getSessionCount(levelID)).c_str(), "goldFont.fnt");
    subtitleLabel->setScale(0.5f);
    subtitleLabel->setPosition({150.f, 233.f});

    m_mainLayer->addChild(subtitleLabel);


	auto totalTitle = CCLabelBMFont::create("Total Playtime:", "goldFont.fnt");
    auto totalValue = CCLabelBMFont::create(Data::formattedPlaytime(Data::getPlaytimeRaw(levelID)).c_str(), "bigFont.fnt");
    auto sessionTitle = CCLabelBMFont::create("Last Session:", "goldFont.fnt");
    auto sessionValue = CCLabelBMFont::create(Data::formattedPlaytime(Data::getLatestSession(levelID)).c_str(), "bigFont.fnt");

    totalTitle->setScale(0.75f);
    totalValue->limitLabelWidth(300.f, .35f, .1f);
    sessionTitle->setScale(0.75f);
    sessionValue->limitLabelWidth(300.f, .35f, .1f);

    // the scroll thing
    auto scrollLayer = geode::ScrollLayer::create({ 10.f,10.f,265.f,196.f});
    scrollLayer->setAnchorPoint({ 0.0f, 0.0f });
    scrollLayer->setID("scroll-layer");
    // scrollbar!?!? lets hope it works without anything else
    auto scrollbar = geode::Scrollbar::create(scrollLayer);
    scrollbar->setAnchorPoint({ 0.f, 0.f });
    scrollbar->setPosition({ 278.f, 12.f });

    auto scrollBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
    scrollBg->setAnchorPoint({ 0.f, 0.f });
    scrollBg->setColor({ 0, 0, 0 });
    scrollBg->setOpacity(128);
    scrollBg->setContentSize({ 280.f, 200.f });
    scrollBg->setID("content-bg");
    scrollBg->setPosition({ 10.f, 10.f });
    m_mainLayer->addChild(scrollBg);


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
    std::string timeAttemptStat = Data::formattedPlaytime(Data::getPlaytimeRaw(levelID));
    if (level->m_attempts != 0) timeAttemptStat = Data::formattedPlaytime(Data::getPlaytimeRaw(levelID) / level->m_attempts);
    std::string timeSessionsStat = Data::formattedPlaytime(Data::getPlaytimeRaw(levelID));
    if (Data::getSessionCount(levelID) != 0) timeSessionsStat = Data::formattedPlaytime(Data::getPlaytimeRaw(levelID) / Data::getSessionCount(levelID));

    auto statsLabel = CCLabelBMFont::create("Level Stats", "goldFont.fnt");
    auto lastPlayedLabel = CCLabelBMFont::create("Last Played: Never", "bigFont.fnt");
    if (Data::getSessionCount(levelID) > 0) lastPlayedLabel = CCLabelBMFont::create(fmt::format("Last Played: {}", Data::getPlayedFormatted(Data::getLastPlayedRaw(levelID))).c_str(), "bigFont.fnt");
    auto timeAttemptLabel = CCLabelBMFont::create(fmt::format("Time/Attempt: {}", timeAttemptStat).c_str(), "bigFont.fnt");
    auto timeSessionsLabel = CCLabelBMFont::create(fmt::format("Time/Session: {}", timeSessionsStat).c_str(), "bigFont.fnt");

    statsLabel->setScale(0.75f);

    lastPlayedLabel->limitLabelWidth(650.f, .35f, .1f);
    timeAttemptLabel->limitLabelWidth(650.f, .3f, .1f);
    timeSessionsLabel->limitLabelWidth(650.f, .3f, .1f);

    content->addChild(statsLabel);
    content->addChild(lastPlayedLabel);
    content->addChild(timeAttemptLabel);
    content->addChild(timeSessionsLabel);


    //sessions
    auto sessionLabel = CCLabelBMFont::create("Sessions", "goldFont.fnt");
    sessionLabel->setScale(0.75f);
    content->addChild(sessionLabel);
    for (int i = Data::getSessionCount(levelID) - 1; i >= 0; i--) {
        auto menu = sessionMenuElement(levelID, i);
        menu->setID(fmt::format("session-{}", i + 1));
        content->addChild(menu);
    }


    
    
    content->setContentSize({ 265.f, 180.f + 45.f * (Data::getSessionCount(levelID)) });

    if (content->getContentHeight() < 196.f) content->setContentSize({ 265.f, 196.f});

    auto noSessionLabel = CCLabelBMFont::create("No sessions yet!", "bigFont.fnt");
    noSessionLabel->setScale(0.35f);

    if (Data::getSessionCount(levelID) == 0) content->addChild(noSessionLabel);

    content->updateLayout();

    scrollLayer->scrollToTop();


    // extra buttons (delete and settings stuff)
    auto extrabuttons = m_mainLayer->getChildByType<CCMenu>(0);
    
    auto deleteSpr = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
    deleteSpr->setScale(0.75f);

    auto deleteButton = CCMenuItemSpriteExtra::create(
        deleteSpr,
        this,
        menu_selector(DeleteButton::onDeleteAllButton)
    );
    deleteButton->setUserObject("level-id", CCString::create(levelID));
    deleteButton->setPosition({300.f, 0.f});

    extrabuttons->addChild(deleteButton);

    auto settingsSpr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    settingsSpr->setScale(0.75f);

    auto settingsButton = CCMenuItemSpriteExtra::create(
        settingsSpr,
        this,
        menu_selector(OpenSettings::open)
    );

    settingsButton->setPosition({300.f, 272.f});

    extrabuttons->addChild(settingsButton);


    auto attemptsSpr = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");

    attemptsSpr->setScale(0.67f);

    auto attemptsButton = CCMenuItemSpriteExtra::create(
        attemptsSpr,
        this,
        menu_selector(OpenAttempts::open)
    );


    attemptsButton->setUserObject("level-id", CCString::create(levelID));
    (void) attemptsButton->setUserData(level);

    attemptsButton->setPosition({3.f, 0.f});

    extrabuttons->addChild(attemptsButton);





    m_mainLayer->addChild(scrollbar);
    m_mainLayer->addChild(scrollLayer);
	// m_mainLayer->addChildAtPosition(label, Anchor::Center);
	return true;
}

MenuPopup* MenuPopup::create(GJGameLevel* level) {
    auto ret = new MenuPopup();
    ret->m_level = level;
    if (ret->init(level)) { // 300.f, 275.f
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

CCMenu* MenuPopup::sessionMenuElement(std::string const& levelID, int index) {
    auto menu = CCMenu::create();
    menu->setContentSize({ 265.f, 40.f });
    menu->setTag(index);
    auto sessionTitle = CCLabelBMFont::create(fmt::format("Session {} - {}", index + 1, Data::getPlayedFormatted(Data::getPlayedRawAtIndex(levelID, index) )).c_str(), "bigFont.fnt");
    auto sessionPlaytime = CCLabelBMFont::create("corrupted session, will disappear", "bigFont.fnt");
    if (Data::getSessionPlaytimeRawAtIndex(levelID, index) != -1) sessionPlaytime = CCLabelBMFont::create(Data::formattedPlaytime(Data::getSessionPlaytimeRawAtIndex(levelID, index)).c_str(), "bigFont.fnt");


    // value to offset index in case attempt sessions count is less than time session count (if you played a level update v1.1.0)
    int offset = Data::getAttemptSessionCount(levelID) - Data::getSessionCount(levelID);

    auto sessionAttempts = CCLabelBMFont::create(fmt::format("{} Attempts", Data::getSessionAttemptsAtIndex(levelID, offset + index)).c_str(), "bigFont.fnt");

    if (Data::getSessionAttemptsAtIndex(levelID, offset + index) == 1) sessionAttempts->setCString("1 Attempt");

    if (Data::getSessionAttemptsAtIndex(levelID, offset + index) < 1) sessionAttempts->setCString("Untracked Attempts");

    auto deleteSprite = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");

    deleteSprite->setScale(0.55f);

    auto deleteSessionButton = CCMenuItemSpriteExtra::create(
        deleteSprite,
        menu,
        menu_selector(DeleteButton::onDeleteSessionButton)
    );

    deleteSessionButton->setTag(index);
    deleteSessionButton->setUserObject("level-id", CCString::create(levelID));

    sessionTitle->setPosition({ 0.f,40.f });
    sessionTitle->limitLabelWidth(600.f, .35f, .1f);
    sessionTitle->setAnchorPoint({ 0.f,1.f });

    sessionPlaytime->setPosition({ 10.f,15.f });
    sessionPlaytime->limitLabelWidth(600.f, .35f, .1f);
    sessionPlaytime->setAnchorPoint({ 0.f,0.f });
    sessionPlaytime->setColor({200, 200, 200});

    sessionAttempts->setPosition({ 10.f,0.f });
    sessionAttempts->limitLabelWidth(600.f, .35f, .1f);
    sessionAttempts->setAnchorPoint({0.f, 0.f});
    sessionAttempts->setColor({200, 200, 200});

    deleteSessionButton->setPosition({ 240.f, 20.f });
    if (Data::getSessionCount(levelID) > 1) menu->addChild(deleteSessionButton);

    menu->addChild(sessionTitle);
    menu->addChild(sessionPlaytime);
    menu->addChild(sessionAttempts);

    return menu;
}