#pragma once
#define pausePopup_h

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class PausePopup : public geode::Popup<GJGameLevel* const&> {
protected:
	bool setup(GJGameLevel* const& level) override;
public:
	static PausePopup* create(GJGameLevel* const& level);
};
