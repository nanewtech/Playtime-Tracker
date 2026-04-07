#include <Geode/Geode.hpp>
#include <Geode/utils/async.hpp>
#include <arc/task/Yield.hpp>

#include "managers/data.hpp"

using namespace geode::prelude;


$on_mod (Loaded){
	Mod::get()->setSavedValue<bool>("is-paused", false);

	// convert from v1.1 to v1.2
	//if (!Mod::get()->setSavedValue<bool>("updated-save", true)) {
		auto success = arc::spawn([] -> arc::Future<bool> {
			bool success{true};
			auto newPath = Mod::get()->getSaveDir() / "data";

			// fetch data
			if (!file::createDirectory(newPath).isOk()) co_return false;
			auto oldSave = Mod::get()->getSaveDir() / "leveldata.json";
			if (!std::filesystem::exists(oldSave)) {
				oldSave = Mod::get()->getSaveDir() / "leveldata.backup";
			}

			if (!std::filesystem::exists(oldSave)) co_return false;

			auto dataRaw = file::readJson(oldSave);
			if (!dataRaw.isOk()) co_return false;

			auto data = dataRaw.unwrap();

			co_await arc::yield();

			log::info("Starting to convert v1.1 save to v1.2");
			auto levelCount = data.size();
			int levelIndex{0};

			log::info("Levels found: {}", levelCount);
			// convert files
			for (auto& level : data) {
				co_await arc::yield();
				levelIndex++;
				std::string levelID;

				if (level.getKey()->empty()) { // if level somehow doesn't have a levelID associated with it
					success = false;
					log::warn("[{}/{}] Skipping due to broken levelID!", levelIndex, levelCount);
					continue;
				}

				levelID = level.getKey().value();

				level["version"] = 2;
				level["linked"] = matjson::Value::array();

				std::string output = level.dump(matjson::NO_INDENTATION);
				if (!file::writeString(newPath/ (levelID + ".json"), output)) success = false;
				log::info("[{}/{}] Converted level {}", levelIndex, levelCount, levelID);
			}

			log::info("Finished Converting");

			co_return success;
		});
	//}
}