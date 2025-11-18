#include "./settings.hpp"

using namespace geode::prelude;


bool settings::getRemovePauses() {
	return Mod::get()->getSettingValue<bool>("remove-pauses");
}

bool settings::getShortText() {
	return Mod::get()->getSettingValue<bool>("short-text");
}

std::string settings::getHighestConvert() {
	return Mod::get()->getSettingValue<std::string>("highest-convert");
}

std::string settings::getInfoLayerPosition() {
	return Mod::get()->getSettingValue<std::string>("infoLayer-position");
}