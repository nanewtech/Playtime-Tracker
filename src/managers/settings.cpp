#include "./settings.hpp"

using namespace geode::prelude;


bool Settings::getRemovePauses() {
	return Mod::get()->getSettingValue<bool>("remove-pauses");
}

bool Settings::getShortText() {
	return Mod::get()->getSettingValue<bool>("short-text");
}

std::string Settings::getHighestConvert() {
	return Mod::get()->getSettingValue<std::string>("highest-convert");
}

std::string Settings::getInfoLayerPosition() {
	return Mod::get()->getSettingValue<std::string>("infoLayer-position");
}

bool Settings::getStopOnCompletion() {
	return Mod::get()->getSettingValue<bool>("stop-completion");
}

std::string Settings::getTimeFormat() {
	return Mod::get()->getSettingValue<std::string>("time-format");
}

bool Settings::getUseCustomTimeFormat() {
	return Mod::get()->getSettingValue<bool>("use-custom-time-format");
}

std::string Settings::getCustomTimeFormat() {
	return Mod::get()->getSettingValue<std::string>("custom-time-format");
}

bool Settings::getDisableDeletionPopup() {
	return Mod::get()->getSettingValue<bool>("disable-deletion-popup");
}