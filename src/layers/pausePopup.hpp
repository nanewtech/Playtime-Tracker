#pragma once

#include <Geode/Geode.hpp>
#include <string>

using namespace geode::prelude;

class PausePopup : public geode::Popup {
protected:
	std::string m_levelID;
	float deltaAccumulator = 0.f;

	CCLabelBMFont* m_totalLabel;
	CCLabelBMFont* m_playtimeLabel;
	GenericContentLayer* m_content;

	bool init(std::string const& levelID);
public:
	static PausePopup* create(std::string const& levelID);
	void update(float delta) override;
};
