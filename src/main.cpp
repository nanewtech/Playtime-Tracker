#include <arc/time/Sleep.hpp>
#include <Geode/Geode.hpp>
#include <Geode/utils/async.hpp>

using namespace geode::prelude;


$on_mod (Loaded){
	Mod::get()->setSavedValue<bool>("is-paused", false);
}