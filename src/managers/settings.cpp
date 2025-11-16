#include "./settings.hpp"

using namespace geode::prelude;


bool settings::getRemovePauses() {
	return Mod::get()->getSettingValue<bool>("remove-pauses");
}