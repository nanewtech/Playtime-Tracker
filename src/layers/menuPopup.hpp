#pragma once

#include <Geode/Geode.hpp>
#include <string>

using namespace geode::prelude;

class MenuPopup : public geode::Popup<GJGameLevel* const&> {
protected:
	GJGameLevel* m_level;
	bool setup(GJGameLevel* const& level) override;
public:
	static MenuPopup* create(GJGameLevel* const& level);
	static CCMenu* sessionMenuElement(std::string const& levelID, int index);
};
