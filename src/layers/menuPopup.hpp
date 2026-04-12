#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/async.hpp>
#include <string>

using namespace geode::prelude;

class MenuPopup : public geode::Popup {
protected:
	GJGameLevel* m_level;
	ScrollLayer* m_scrollLayer;
	async::TaskHolder<> buildSessionTask;

	typedef struct {
		std::string datePlayed;
		std::string playtime;
		std::string attempts;
		int index;
	} session;

	std::vector<session> sessions;

	bool init(GJGameLevel* level);
	arc::Future<> createSessions(std::string const levelID);
	void onClose(CCObject* sender) override;
	void update(float delta) override;
public:
	static MenuPopup* create(GJGameLevel* level);
	static CCMenu* sessionMenuElement(std::string const& levelID, int index);
};
