#pragma once

#include <Geode/Geode.hpp>
#include <string>

using namespace geode::prelude;

class MenuPopup : public geode::Popup {
protected:
	GJGameLevel* m_level;
	bool init(GJGameLevel* level);
public:
	static MenuPopup* create(GJGameLevel* level);
	static CCMenu* sessionMenuElement(std::string const& levelID, int index);
};
