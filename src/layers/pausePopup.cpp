#include "./pausePopup.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>


bool PausePopup::setup(GJGameLevel* const& level) {

    std::string levelID = std::to_string(EditorIDs::getID(level));
    if (level->m_levelType == GJLevelType::Editor) levelID = "Editor-" + levelID;




    return true;
}


PausePopup* PausePopup::create(GJGameLevel* const& level) {
    auto ret = new PausePopup();
    if (ret->initAnchored(240.f, 160.f, level)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}