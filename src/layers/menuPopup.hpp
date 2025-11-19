#ifndef menuPopup_h
#define menuPopup_h

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class MenuPopup : public geode::Popup<GJGameLevel* const&> {
protected:
	GJGameLevel* m_level;
	bool setup(GJGameLevel* const& level) override;
public:
	static MenuPopup* create(GJGameLevel* const& level);
};

#endif // !menuPopup_h
