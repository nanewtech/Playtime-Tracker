#define GEODE_DEFINE_EVENT_EXPORTS
#include "../include/api.hpp"
#include "managers/data.hpp"

std::string PlaytimeAPI::formatLevelID(int levelID, bool isEditor) {
    return isEditor ? fmt::format("Editor-{}", levelID) : fmt::to_string(levelID);
}

int PlaytimeAPI::getPlaytime(std::string const& levelID) {
    return Data::getTotalPlaytime(levelID);
}

std::string PlaytimeAPI::formatPlaytime(int const playtime) {
    return Data::formattedPlaytime(playtime);
}

int PlaytimeAPI::getAttempts(std::string const &levelID) {
    return Data::getLevelAttempts(levelID);
}

int PlaytimeAPI::getSessionCount(std::string const &levelID) {
    return Data::getSessionCount(levelID);
}

int PlaytimeAPI::getSessionPlaytime(std::string const& levelID, const int index) {
    return Data::getSessionPlaytimeRawAtIndex(levelID, index);
}

int PlaytimeAPI::getSessionAttemptsCount(std::string const &levelID) {
    return Data::getSessionCount(levelID);
}

int PlaytimeAPI::getSessionAttempts(std::string const &levelID, const int index) {
    int offset = Data::getAttemptSessionCount(levelID) - Data::getSessionCount(levelID);
    return Data::getSessionAttemptsAtIndex(levelID, index + offset);
}

matjson::Value PlaytimeAPI::getRawLevelData(std::string const &levelID) {
    return Data::getFile(levelID);
}
