#include "./menuPopup.hpp"
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

        auto obj = static_cast<CCNode*>(sender)->getUserObject();
        std::string levelID = static_cast<CCString*>(obj)->getCString();
        geode::createQuickPopup(
            "Delete level data",
            "Are you SURE you want to <cr>delete ALL data</c> on this level? (<cr>ALL your sessions will be deleted! This action is IRREVERSIBLE</c>)",
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

        auto obj = static_cast<CCNode*>(sender)->getUserObject();
        std::string levelID = static_cast<CCString*>(obj)->getCString();
        geode::createQuickPopup(
            "Delete session",
            CCString::createWithFormat("Are you SURE you want to <cr>delete session %i</c> on this level? (<cr>This action is IRREVERSIBLE!</c>)", index + 1)->getCString(),
            "Don't delete", "Delete session",
            [levelID, index](auto, bool btn2) {
                if (btn2) {
                    Data::deleteSessionAtIndex(levelID, index);
                    if (!Settings::getDisableDeletionPopup()) FLAlertLayer::create(
                        "Delete session Data",
                        CCString::createWithFormat("Deleted session %i. Reopen the popup for changes to take effect. <cy>You can disable this popup in the settings.</c>", index + 1)->getCString(),
                        "OK"
                    )->show();
                }
            }
        );
    }
};


bool MenuPopup::setup(GJGameLevel* const& level) {
	this->setTitle("Playtime Tracker");
    this->setID("Playtime-Tracker-Popup"_spr);

    std::string levelID = std::to_string(EditorIDs::getID(level));
    if (level->m_levelType == GJLevelType::Editor) levelID = "Editor-" + levelID;

    std::string levelName = level->m_levelName;

    auto subtitleLabel = CCLabelBMFont::create(CCString::create(levelName + " - Sessions: " + std::to_string(Data::getSessionCount(levelID)))->getCString(), "goldFont.fnt");
    subtitleLabel->setScale(0.5f);
    subtitleLabel->setPosition({150.f, 233.f});

    m_mainLayer->addChild(subtitleLabel);


	auto totalTitle = CCLabelBMFont::create("Total Playtime:", "goldFont.fnt");
    auto totalValue = CCLabelBMFont::create(CCString::create(Data::formattedPlaytime(Data::getPlaytimeRaw(levelID)))->getCString(), "bigFont.fnt");
    auto sessionTitle = CCLabelBMFont::create("Last Session:", "goldFont.fnt");
    auto sessionValue = CCLabelBMFont::create(CCString::create(Data::formattedPlaytime(Data::getLatestSession(levelID)))->getCString(), "bigFont.fnt");

    totalTitle->setScale(0.75f);
    totalValue->setScale(0.35f);
    sessionTitle->setScale(0.75f);
    sessionValue->setScale(0.35f);

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
    auto lastPlayedLabel = CCLabelBMFont::create(CCString::create("Last Played: Never")->getCString(), "bigFont.fnt");
    if (Data::getSessionCount(levelID) > 0) lastPlayedLabel = CCLabelBMFont::create(CCString::create("Last Played: " + Data::getPlayedFormatted(Data::getLastPlayedRaw(levelID)))->getCString(), "bigFont.fnt");
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
    for (int i = Data::getSessionCount(levelID) - 1; i >= 0; i--) {
        auto menu = sessionMenuElement(levelID, i);
        menu->setID(fmt::format("session-{}", i + 1));
        content->addChild(menu);
    }


    
    
    content->setContentSize({ 265.f, 180.f + 30.f * (Data::getSessionCount(levelID)) });

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
    deleteButton->setUserObject(CCString::create(levelID));
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

CCMenu* MenuPopup::sessionMenuElement(std::string const& levelID, int index) {
    auto menu = CCMenu::create();
    menu->setContentSize({ 265.f, 25.f });
    menu->setTag(index);
    auto sessionTitle = CCLabelBMFont::create(CCString::create("Session " + std::to_string(index + 1) + " - " + Data::getPlayedFormatted(Data::getPlayedRawAtIndex(levelID, index)))->getCString(), "bigFont.fnt");
    auto sessionPlaytime = CCLabelBMFont::create("corrupted session, will disappear", "bigFont.fnt");
    if (Data::getSessionPlaytimeRawAtIndex(levelID, index) != -1) sessionPlaytime = CCLabelBMFont::create(CCString::create(Data::formattedPlaytime(Data::getSessionPlaytimeRawAtIndex(levelID, index)))->getCString(), "bigFont.fnt");

    auto deleteSprite = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
    deleteSprite->setScale(0.55f);

    auto deleteSessionButton = CCMenuItemSpriteExtra::create(
        deleteSprite,
        menu,
        menu_selector(DeleteButton::onDeleteSessionButton)
    );

    deleteSessionButton->setTag(index);
    deleteSessionButton->setUserObject(CCString::create(levelID));
    deleteSessionButton->setPosition({ 240.f, 12.5f });

    sessionTitle->setPosition({ 0.f,25.f });
    sessionTitle->setScale(0.35f);
    sessionTitle->setAnchorPoint({ 0.f,1.f });

    sessionPlaytime->setScale(0.35f);
    sessionPlaytime->setAnchorPoint({ 0.f,0.f });

    if (Data::getSessionCount(levelID) > 1) menu->addChild(deleteSessionButton);
    menu->addChild(sessionTitle);
    menu->addChild(sessionPlaytime);

    return menu;
}