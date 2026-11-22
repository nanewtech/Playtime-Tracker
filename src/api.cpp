#define GEODE_DEFINE_EVENT_EXPORTS
#include "../include/api.hpp"
#include "managers/data.hpp"

geode::Result<std::string> PlaytimeAPI::formatLevelID(int levelID, bool isEditor) {
    return geode::Ok(isEditor ? fmt::format("Editor-{}", levelID) : fmt::to_string(levelID));
}

geode::Result<int> PlaytimeAPI::getPlaytime(std::string const &levelID) {
    return geode::Ok(Data::getTotalPlaytime(levelID));
}

geode::Result<std::string> PlaytimeAPI::formatPlaytime(int const playtime) {
    return geode::Ok(Data::formattedPlaytime(playtime));
}

geode::Result<int> PlaytimeAPI::getAttempts(std::string const &levelID) {
    return geode::Ok(Data::getLevelAttempts(levelID));
}

geode::Result<int> PlaytimeAPI::getSessionCount(std::string const &levelID) {
    return geode::Ok(Data::getSessionCount(levelID));
}

geode::Result<int> PlaytimeAPI::getSessionPlaytime(std::string const &levelID, const int index) {
    return geode::Ok(Data::getSessionPlaytimeRawAtIndex(levelID, index));
}

geode::Result<int> PlaytimeAPI::getSessionAttemptsCount(std::string const &levelID) {
    return geode::Ok(Data::getSessionCount(levelID));
}

geode::Result<int> PlaytimeAPI::getSessionAttempts(std::string const &levelID, const int index) {
    int offset = Data::getAttemptSessionCount(levelID) - Data::getSessionCount(levelID);
    return geode::Ok(Data::getSessionAttemptsAtIndex(levelID, index + offset));
}

geode::Result<matjson::Value> PlaytimeAPI::getRawLevelData(std::string const &levelID) {
    return geode::Ok(Data::getFile(levelID));
}
