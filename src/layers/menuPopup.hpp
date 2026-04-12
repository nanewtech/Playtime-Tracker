#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/async.hpp>
#include <string>

using namespace geode::prelude;

class MenuPopup : public geode::Popup {
protected:
	GJGameLevel* m_level;
	std::string m_levelID;
	ScrollLayer* m_scrollLayer;
	LoadingCircle* m_throbber;
	bool m_taskDone = false;
	async::TaskHolder<> buildSessionTask;

	typedef struct {
		std::string sessionTitle;
		std::string playtime;
		std::string attempts;
		int index;
	} Session;

	std::vector<Session> sessions;

	bool init(GJGameLevel* level);
	arc::Future<> createSessions(std::string const levelID);
	void onClose(CCObject* sender) override;
	void update(float delta) override;
public:
	static MenuPopup* create(GJGameLevel* level);
	CCMenu* sessionMenuElement();
};
