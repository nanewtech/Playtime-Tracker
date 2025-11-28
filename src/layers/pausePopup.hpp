#pragma once

#include <Geode/Geode.hpp>
#include <string>

class PausePopup : public geode::Popup<std::string const&> {
protected:
	bool setup(std::string const& levelID) override;
public:
	static PausePopup* create(std::string const& levelID);
};
