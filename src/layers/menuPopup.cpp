#include "menuPopup.hpp"
#include <Geode/Geode.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <arc/task/Yield.hpp>

#include <cvolton.level-id-api/include/EditorIDs.hpp>

#include "../managers/data.hpp"
#include "../managers/settings.hpp"

class OpenSettings : public CCLayer {
public:
    void open(CCObject*) {
        geode::openSettingsPopup(Mod::get(), true);
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

    m_levelID = levelID;

    std::string levelName = level->m_levelName;

    auto subtitleLabel = CCLabelBMFont::create(fmt::format("{} - Sessions: {}", levelName, Data::getSessionCount(levelID)).c_str(), "goldFont.fnt");
    subtitleLabel->setScale(0.5f);
    subtitleLabel->setPosition({150.f, 233.f});

    m_mainLayer->addChild(subtitleLabel);

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



    auto content = scrollLayer->m_contentLayer;
    content->setPosition({ 10.f,0.f });
    content->setLayout(contentLayout);

    initContentLayer(content);

    // I REALLY wanted to use the proper word for it cause it sounds funny
    auto throbber = LoadingCircle::create();
    throbber->setID("pt-throbber");
    throbber->setParentLayer(scrollLayer);
    //throbber->setPosition({ scrollLayer->getContentSize()/2 });
    throbber->show();
    throbber->setZOrder(2000);
    throbber->setPosition({ -145.f,-50.f }); // -148 -37

    auto loadingLabel = CCLabelBMFont::create("Loading data", "goldFont.fnt");
    loadingLabel->setScale(0.75f);
    loadingLabel->setPosition({ 140.f,60.f }); // 138 73
    loadingLabel->setID("pt-loading-label");
    scrollLayer->addChild(loadingLabel);

    m_throbber = throbber;

    buildSessionTask.spawn(
        "Playtime Tracker Session Task",
        createSessions(levelID),
            [this] {
                m_taskDone = true;
                this->scheduleUpdate();
            }
        );



    // extra buttons (delete and settings stuff)
    auto extrabuttons = m_mainLayer->getChildByType<CCMenu>(0);

    auto deleteSpr = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
    deleteSpr->setScale(0.75f);

    auto deleteButton = CCMenuItemExt::createSpriteExtraWithFrameName(
        "GJ_trashBtn_001.png",
        0.75f,
        [this] (auto) {
            geode::createQuickPopup(
                "Delete level data",
                "Are you sure you want to <cr>delete ALL data</c> on this level? (<cr>ALL your sessions will be deleted! This action is IRREVERSIBLE</c>)",
                "Don't delete", "Delete everything",
                [this](auto, bool btn2) {
                    if (btn2) {
                        Data::deleteLevelData(m_levelID);
                        if (!Settings::getDisableDeletionPopup()) FLAlertLayer::create(
                            "Delete level Data",
                            "Deleted all data. Reopen the popup for changes to take effect. <cy>You can disable this popup in the settings.</c>",
                            "OK"
                        )->show();
                        m_scrollLayer->m_contentLayer->removeAllChildrenWithCleanup(true);
                        initContentLayer(m_scrollLayer->m_contentLayer);

                        auto noSessionLabel = CCLabelBMFont::create("No sessions yet!", "bigFont.fnt");
                        noSessionLabel->setScale(0.35f);

                        m_scrollLayer->m_contentLayer->addChild(noSessionLabel);
                        m_scrollLayer->m_contentLayer->setContentHeight(180.f);
                        m_scrollLayer->scrollToTop();
                        m_scrollLayer->m_contentLayer->updateLayout();
                    }
                }
            );
        }
    );
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
    attemptsButton->setUserData(level);

    attemptsButton->setPosition({3.f, 0.f});

    extrabuttons->addChild(attemptsButton);

    m_mainLayer->addChild(scrollbar);
    m_mainLayer->addChild(scrollLayer);
    m_scrollLayer = scrollLayer;
    scrollLayer->m_contentLayer->setVisible(false);
	// m_mainLayer->addChildAtPosition(label, Anchor::Center);
	return true;
}

arc::Future<> MenuPopup::createSessions(std::string const levelID) {
    co_await arc::yield();
    auto sessionCount = Data::getSessionCount(levelID);

    for (int i = 0; i < sessionCount; i++) {
        int offset = Data::getAttemptSessionCount(levelID) - Data::getSessionCount(levelID);
            Session currSession = {
                fmt::format("Session {} - {}", i + 1, Data::getPlayedFormatted(Data::getPlayedRawAtIndex(levelID, i) )),
                Data::formattedPlaytime(Data::getSessionPlaytimeRawAtIndex(levelID, i)),
                fmt::format("{} Attempts", Data::getSessionAttemptsAtIndex(levelID, offset + i)),
                i
            };
        if (Data::getSessionPlaytimeRawAtIndex(levelID, i) == -1) currSession.playtime = "corrupted session, will disappear";
        if (Data::getSessionAttemptsAtIndex(levelID, offset + i) == 1) currSession.attempts = "1 Attempt";
        if (Data::getSessionAttemptsAtIndex(levelID, offset + i) < 1) currSession.attempts = "Untracked Attempts";

        sessions.push_back(currSession);
        co_await arc::yield();
        }

    co_return;
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

void MenuPopup::onClose(CCObject* sender) {
    buildSessionTask.cancel();

    Popup::onClose(sender);
}

void MenuPopup::update(float delta) {
    bool done = false;
    if (!buildSessionTask.isPending()) { // task is done
        // TODO: figure out if it should always be 10 or if it should be adjustable in settings (3 levels? direct number?)
        int i = 0;
        while (i < 10) {
            if (sessions.empty()) {
                done = true;
                break;
            }
            auto menu = sessionMenuElement();
            menu->setID(fmt::format("session-{}", menu->getTag()));
            this->m_scrollLayer->m_contentLayer->addChild(menu);
            i++;
        }
    }
    if (done) {
        auto sessionCount = Data::getSessionCount(m_levelID);
        m_scrollLayer->m_contentLayer->setContentSize({ 265.f, 180.f + 45.f * (sessionCount) });

        if (m_scrollLayer->m_contentLayer->getContentHeight() < 196.f) m_scrollLayer->m_contentLayer->setContentSize({ 265.f, 196.f});

        auto noSessionLabel = CCLabelBMFont::create("No sessions yet!", "bigFont.fnt");
        noSessionLabel->setScale(0.35f);

        if (sessionCount == 0) m_scrollLayer->m_contentLayer->addChild(noSessionLabel);

        m_scrollLayer->m_contentLayer->updateLayout();

        m_scrollLayer->scrollToTop();

        m_throbber->removeMeAndCleanup();
        m_scrollLayer->m_contentLayer->setVisible(true);
        m_scrollLayer->getChildByID("pt-loading-label")->removeMeAndCleanup();
        this->unscheduleUpdate();
    }
    Popup::update(delta);
}

void MenuPopup::initContentLayer(CCContentLayer* content) {
    auto totalTitle = CCLabelBMFont::create("Total Playtime:", "goldFont.fnt");
    auto totalValue = CCLabelBMFont::create(Data::formattedPlaytime(Data::getPlaytimeRaw(m_levelID)).c_str(), "bigFont.fnt");
    auto sessionTitle = CCLabelBMFont::create("Last Session:", "goldFont.fnt");
    auto sessionValue = CCLabelBMFont::create(Data::formattedPlaytime(Data::getLatestSession(m_levelID)).c_str(), "bigFont.fnt");

    totalTitle->setScale(0.75f);
    totalValue->limitLabelWidth(300.f, .35f, .1f);
    sessionTitle->setScale(0.75f);
    sessionValue->limitLabelWidth(300.f, .35f, .1f);

    content->addChild(totalTitle);
    content->addChild(totalValue);
    content->addChild(sessionTitle);
    content->addChild(sessionValue);

    std::string timeAttemptStat = Data::formattedPlaytime(Data::getPlaytimeRaw(m_levelID));
    if (m_level->m_attempts != 0) timeAttemptStat = Data::formattedPlaytime(Data::getPlaytimeRaw(m_levelID) / m_level->m_attempts);
    std::string timeSessionsStat = Data::formattedPlaytime(Data::getPlaytimeRaw(m_levelID));
    if (Data::getSessionCount(m_levelID) != 0) timeSessionsStat = Data::formattedPlaytime(Data::getPlaytimeRaw(m_levelID) / Data::getSessionCount(m_levelID));

    auto statsLabel = CCLabelBMFont::create("Level Stats", "goldFont.fnt");
    auto lastPlayedLabel = CCLabelBMFont::create("Last Played: Never", "bigFont.fnt");
    if (Data::getSessionCount(m_levelID) > 0) lastPlayedLabel = CCLabelBMFont::create(fmt::format("Last Played: {}", Data::getPlayedFormatted(Data::getLastPlayedRaw(m_levelID))).c_str(), "bigFont.fnt");
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
}

CCMenu* MenuPopup::sessionMenuElement() {

    if (sessions.empty()) return nullptr; // shouldn't ever happen but just in case
    auto currSession = sessions.back();
    auto index = currSession.index;


    auto menu = CCMenu::create();
    menu->setContentSize({ 265.f, 40.f });
    menu->setTag(index);
    auto sessionTitle = CCLabelBMFont::create(currSession.sessionTitle.c_str(), "bigFont.fnt");
    auto sessionPlaytime = CCLabelBMFont::create(currSession.playtime.c_str(), "bigFont.fnt");


    // value to offset index in case attempt sessions count is less than time session count (if you played a level update v1.1.0)

    auto sessionAttempts = CCLabelBMFont::create(currSession.attempts.c_str(), "bigFont.fnt");

    auto deleteSprite = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");

    deleteSprite->setScale(0.55f);

    auto deleteSessionButton = CCMenuItemExt::createSpriteExtraWithFrameName(
        "GJ_trashBtn_001.png",
        .55f,
        [this, index](auto) {
            geode::createQuickPopup(
                "Delete session",
                fmt::format("Are you sure you want to <cr>delete session {}</c> on this level? (<cr>This action is IRREVERSIBLE!</c>)", index + 1),
                "Don't delete", "Delete session",
                [this, index](auto, bool btn2) {
                    if (btn2) {
                        Data::deleteSessionAtIndex(m_levelID, index);
                        if (!Settings::getDisableDeletionPopup()) FLAlertLayer::create(
                            "Delete session Data",
                            fmt::format("Deleted session {}. Reopen the popup for changes to take effect. <cy>You can disable this popup in the settings.</c>", index + 1),
                            "OK"
                        )->show();
                        m_scrollLayer->m_contentLayer->removeChildByTag(index);
                        m_scrollLayer->m_contentLayer->updateLayout();
                    }
                }
            );
        }
    );

    deleteSessionButton->setTag(index);
    deleteSessionButton->setUserObject("level-id", CCString::create(m_levelID));

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
    if (Data::getSessionCount(m_levelID) > 1) menu->addChild(deleteSessionButton);

    menu->addChild(sessionTitle);
    menu->addChild(sessionPlaytime);
    menu->addChild(sessionAttempts);

    sessions.pop_back();

    return menu;
}