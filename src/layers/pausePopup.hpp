#pragma once

#include <Geode/Geode.hpp>
#include <string>

using namespace geode::prelude;

class PausePopup : public geode::Popup {
protected:
	std::string m_levelID{};
	float deltaAccumulator{0.f};

	Label* m_totalLabel{nullptr};
	Label* m_playtimeLabel{nullptr};
	GenericContentLayer* m_content{nullptr};

	bool init(std::string const& levelID);
public:
	static PausePopup* create(std::string const& levelID);
	void update(float delta) override;
};
