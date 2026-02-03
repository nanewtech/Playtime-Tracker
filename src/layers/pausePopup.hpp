#pragma once

#include <Geode/Geode.hpp>
#include <string>

class PausePopup : public geode::Popup {
protected:
	bool init(std::string const& levelID);
public:
	static PausePopup* create(std::string const& levelID);
};
