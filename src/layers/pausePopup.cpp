#include "./pausePopup.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>

#include "../managers/data.hpp"

using namespace geode::prelude;

bool PausePopup::setup(std::string const& levelID) {
    this->setID("Playtime-Tracker-Popup"_spr);
    this->setTitle("Playtime Tracker");

    auto bg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
    bg->setColor({ 0, 0, 0 });
    bg->setOpacity(128);
    bg->setContentSize({ 220.f, 110.f });
    bg->setID("content-bg");
    bg->setAnchorPoint({ 0.f, 0.f });
    bg->setPosition({ 10.f,10.f });
    m_mainLayer->addChild(bg);

    auto playtimeTitle = CCLabelBMFont::create("Current session:", "goldFont.fnt");
    playtimeTitle->setScale(0.75f);
    auto totalTitle = CCLabelBMFont::create("Total Playtime:", "goldFont.fnt");
    totalTitle->setScale(0.75f);


    auto totalPlaytime = Data::getPlaytimeRaw(levelID);
    auto totalLabel = CCLabelBMFont::create(Data::formattedPlaytime(Data::getTotalPlaytime(levelID)).c_str(), "bigFont.fnt");
    auto playtime = Data::getSessionPlaytimeRaw(levelID);
    auto playtimeLabel = CCLabelBMFont::create(Data::formattedPlaytime(playtime).c_str() , "bigFont.fnt");
    playtimeLabel->setScale(0.375f);
    totalLabel->setScale(0.375f);



    auto contentLayout = geode::AxisLayout::create(geode::Axis::Column);
    contentLayout->setAutoScale(false);
    contentLayout->setAxisAlignment(geode::AxisAlignment::End);
    contentLayout->setAxisReverse(true);
    contentLayout->setCrossAxisLineAlignment(geode::AxisAlignment::Start);


    auto content = geode::GenericContentLayer::create(215.f, 110.f);

    
    content->setLayout(contentLayout);
    content->setAnchorPoint({ 0.f,0.f });
    content->setPosition({ 15.f,10.f });


    content->addChild(totalTitle);
    content->addChild(totalLabel);
    content->addChild(playtimeTitle);
    content->addChild(playtimeLabel);

    content->updateLayout();
    m_mainLayer->addChild(content);
    return true;
}


PausePopup* PausePopup::create(std::string const& levelID) {
    auto ret = new PausePopup();
    if (ret->initAnchored(240.f, 160.f, levelID)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}