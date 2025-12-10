#pragma once

#include <Geode/Geode.hpp>
#include <string>

using namespace geode::prelude;

class MenuPopup : public geode::Popup<GJGameLevel*> {
protected:
	GJGameLevel* m_level;
	bool setup(GJGameLevel* level) override;
public:
	static MenuPopup* create(GJGameLevel* level);
	static CCMenu* sessionMenuElement(std::string const& levelID, int index);
};
