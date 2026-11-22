#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/async.hpp>
#include <string>

using namespace geode::prelude;

class MenuPopup : public geode::Popup {
protected:
	GJGameLevel*m_level{nullptr};
	std::string m_levelID{};
	ScrollLayer* m_scrollLayer{nullptr};
	LoadingCircle* m_throbber{nullptr};
	bool m_taskDone {false};
	async::TaskHolder<> buildSessionTask{};

	struct Session{
		std::string sessionTitle{};
		std::string playtime{};
		std::string attempts{};
		int index{};
	};

	std::vector<Session> sessions;

	bool init(GJGameLevel* level);
	arc::Future<> createSessions(std::string levelID);
	void onClose(CCObject* sender) override;
	void update(float delta) override;
	void initContentLayer(CCContentLayer* content);
public:
	static MenuPopup* create(GJGameLevel* level);
	CCMenu* sessionMenuElement();
};
