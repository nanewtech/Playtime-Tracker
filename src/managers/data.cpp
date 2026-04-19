#include "data.hpp"
#include "settings.hpp"
#include "backup.hpp"


using namespace geode::prelude;

std::unordered_set<std::string> playedIDs;

typedef struct {
    matjson::Value data;
    bool changed;
}cachedLevel;

std::unordered_map<std::string, cachedLevel> levelCache;

std::filesystem::path getDataDirPath(std::string const& levelID) {
    return Mod::get()->getSaveDir() / "data" / (levelID + ".json");
}

std::filesystem::path getLegacyDataDirPath() {
    return Mod::get()->getSaveDir() / "leveldata.json";
}

bool Data::fileExists(std::string const& levelID) {
    return std::filesystem::exists(getDataDirPath(levelID));
}

bool Data::legacyFileExists() {
    return std::filesystem::exists(getLegacyDataDirPath());
}

void putCachedLevel(std::string const& levelID, matjson::Value const& data) {
    levelCache[levelID] = cachedLevel(data, true);
}

void writeFile(matjson::Value const& data, std::string const& levelID) {
    putCachedLevel(levelID, data);
    Data::flushCache();

    //auto const& output = data.dump(matjson::NO_INDENTATION);
    //(void)file::writeString(getDataDirPath(levelID), output);
}

void Data::flushCache() {
    for (auto& [levelID, level] : levelCache) {
        if (!level.changed) continue;

        auto const& output = level.data.dump(matjson::NO_INDENTATION);
        (void)file::writeStringSafe(getDataDirPath(levelID), output);
        level.changed = false;
    }
}

static void initializeFile(std::string const& levelID) {
    matjson::Value data;
    data["version"] = 2;
    data["linked"] = matjson::Value::array();
    data["sessions"] = matjson::Value::array();



    writeFile(data, levelID);
}

matjson::Value Data::getFile(std::string const& levelID) {
    if (levelCache.contains(levelID)) {
        auto data = levelCache[levelID].data;
        return data;
    }

    if (Data::fileExists(levelID)) {
        if (auto data = file::readJson(getDataDirPath(levelID))) {
            putCachedLevel(levelID, data.unwrap());
            return data.unwrap();
        }
    }
    if (Backup::fileExists(levelID)) {
        log::info("{}.json doesn't exist, Loading backup!", levelID);
        Backup::loadBackup(levelID);
        auto data = Backup::getFile(levelID);
        putCachedLevel(levelID, data);
        return data;
    }

    if (Data::legacyFileExists()) {
        if (auto data = file::readJson(getLegacyDataDirPath())) {
            const auto& legacyData = data.unwrap();
            if (!legacyData[levelID]["sessions"].isNull()){
                log::warn("backup doesn't exist, loading legacy file temporarily (loading might be extremely slow, restart the game!)");
                putCachedLevel(levelID, data.unwrap());
                return legacyData[levelID]; // return data if level is in old data
            }
        }
    }

    if (Backup::legacyFileExists()) {
        if (auto data = file::readJson(Backup::getLegacyBackupDirPath())) {
            const auto& legacyData = data.unwrap();
            if (!legacyData[levelID]["sessions"].isNull()){
                log::warn("legacy file doesnt exist, loading legacy file backup temporarily (loading might be extremely slow, restart the game!)");
                putCachedLevel(levelID, data.unwrap());
                return legacyData[levelID]; // return data if level is in old backup
            }
        }
    }
        log::info("no data found, Creating empty {}.json!", levelID);
        matjson::Value data;
        data["version"] = 2;
        data["linked"] = matjson::Value::array();
        data["sessions"] = matjson::Value::array();
        putCachedLevel(levelID, data);
        initializeFile(levelID);
        return data;
}

static bool sessionsInitialized(std::string const& levelID) {
    auto data = Data::getFile(levelID);
    return !data["sessions"].isNull();
}

void Data::startLevel(std::string const& levelID) {
    auto data = getFile(levelID);
    time_t timestamp;

    auto& sessions = data["sessions"];
    

    if (Settings::getStopOnCompletion() && Mod::get()->getSavedValue<int>("current-level-best") == 100) return;

    if (!(sessions.isArray())) sessions = matjson::Value::array(); 

    sessions.push(matjson::Value::array());

    auto& latestSession = sessions[sessions.size() - 1];

    latestSession.push(matjson::Value::array());

    latestSession[latestSession.size() - 1].push(time(&timestamp));

    putCachedLevel(levelID, data);
}

void Data::pauseLevel(std::string const& levelID) {

    if (Settings::getStopOnCompletion() && Mod::get()->getSavedValue<int>("current-level-best") == 100) return; 
        
    if (!Settings::getRemovePauses()) return;
        
    auto data = getFile(levelID);
    time_t timestamp;

    auto& sessions = data["sessions"];
    auto& latestSession = sessions[sessions.size() - 1];


    latestSession[latestSession.size() - 1].push(time(&timestamp));

    writeFile(data, levelID);
    Backup::createBackup(Data::getFile(levelID), levelID);
}

void Data::resumeLevel(std::string const& levelID, bool removePauseOverride) {
    if (Settings::getStopOnCompletion() && Mod::get()->getSavedValue<int>("current-level-best") == 100) return;

    if (!Settings::getRemovePauses() && !removePauseOverride) return;
    
    auto data = getFile(levelID);
    time_t timestamp;

    auto& sessions = data["sessions"];
    auto& latestSession = sessions[sessions.size() - 1];

    if (sessions.size() <= 0) {
        Data::startLevel(levelID);
        return;
    }

    latestSession.push(matjson::Value::array());
    latestSession[latestSession.size() - 1].push(time(&timestamp));

    putCachedLevel(levelID, data);
}

void Data::exitLevel(std::string const& levelID) {
    if (Settings::getStopOnCompletion() && Mod::get()->getSavedValue<int>("current-level-best") == 100) return;

    if (!(Settings::getRemovePauses()) || (Mod::get()->getSavedValue<bool>("is-paused") && !(Settings::getRemovePauses())) || (!(Mod::get()->getSavedValue<bool>("is-paused")) && Settings::getRemovePauses())) {
        auto data = getFile(levelID);
        time_t timestamp;

        auto& sessions = data["sessions"];
        auto& latestSession = sessions[sessions.size() - 1];

        latestSession[latestSession.size() - 1].push(time(&timestamp));

        writeFile(data, levelID);
        Backup::createBackup(Data::getFile(levelID), levelID);
        }
    }

int Data::getLatestSession(std::string const& levelID) {
    auto data = getFile(levelID);

    int playtime = 0;

    auto& sessions = data["sessions"];
    auto& latestSession = sessions[sessions.size() - 1];

    for (auto& currPair : latestSession) {
        if (currPair.size() >= 2) {
            if (!currPair[0].isNumber() || !currPair[1].isNumber()) {
                Backup::loadBackup(levelID);
                return playtime;
            }
            playtime += currPair[1].asInt().unwrap() - currPair[0].asInt().unwrap();
        }
        else Data::fixSessionAtIndex(levelID, sessions.size() - 1);
    }
    return playtime;

}

int Data::getSessionPlaytimeRaw(std::string const& levelID) {
    auto data = getFile(levelID);

    int playtime = 0;

    auto& sessions = data["sessions"];
    auto& latestSession = sessions[sessions.size() - 1];

    time_t timestamp;
    if (Settings::getRemovePauses()) return getLatestSession(levelID);

    auto& latestTime = latestSession[latestSession.size() - 1][0];
    return latestTime.isNumber() ? time(&timestamp) - latestTime.asInt().unwrap() : playtime;
}

// do this inside level
int Data::getPlaytimeRaw(std::string const& levelID) {
    auto data = getFile(levelID);

    int playtime = 0;

    auto& sessions = data["sessions"];
    auto& latestSession = sessions[sessions.size() - 1];
    auto& latestPair = latestSession[latestSession.size() - 1];

    if (!(sessionsInitialized(levelID))) return playtime;
    if (Settings::getRemovePauses()) {
        for (auto& session : sessions) {
            for (auto& currPair : session) {
                if (!currPair[0].isNumber() || !currPair[1].isNumber()) return playtime;
                playtime += currPair[1].asInt().unwrap() - currPair[0].asInt().unwrap();
            }
        }
        return playtime;
    }
    time_t timestamp;
    if (latestPair[0].isNumber() && latestPair.size() == 1) {
        return (time(&timestamp) - latestPair[0].asInt().unwrap());
    }
    for (auto& session : sessions) {
        for (auto& currPair : session) {
            if (!currPair[0].isNumber() || !currPair[1].isNumber()) return playtime;
            playtime += currPair[1].asInt().unwrap() - currPair[0].asInt().unwrap();
        }
    }
    return playtime;
}

std::string Data::formattedPlaytime(int playtime) {
    int days = 0;
    int hours = 0;
    int minutes = 0;
    fmt::memory_buffer formatted;
    std::array<std::string_view, 4> suffix = { "d ", "h ", "m ", "s" };

    if (!Settings::getShortText())  suffix = { " Days, ", " Hours, ", " Minutes, ", " Seconds" };

    // @nanew optimize plssss :3
    // there HAS to be a better way to do this 
    if (Settings::getHighestConvert() == "Days") {
        days = playtime / 86400;
        if (days > 0) {
            playtime -= days * 86400;
            if (days == 1 && !Settings::getShortText()) fmt::format_to(std::back_inserter(formatted), "{} Day, ", days);
            if (days > 1 || Settings::getShortText()) fmt::format_to(std::back_inserter(formatted), "{}{}", days, suffix[0]);
        }
    }
    if (Settings::getHighestConvert() == "Days" || Settings::getHighestConvert() == "Hours") {
        hours = playtime / 3600;
        if (hours > 0) {
            playtime -= hours * 3600;
            if (hours == 1 && !Settings::getShortText()) fmt::format_to(std::back_inserter(formatted), "{} Hour, ", hours);
            if (hours > 1 || Settings::getShortText()) fmt::format_to(std::back_inserter(formatted), "{}{}", hours, suffix[1]);
        }
    }
    if (!(Settings::getHighestConvert() == "Seconds")) {
        minutes = playtime / 60;
        if (minutes > 0) {
            playtime -= minutes * 60;
            if (minutes == 1 && !Settings::getShortText()) fmt::format_to(std::back_inserter(formatted), "{} Minute, ", minutes); // grammar......
            if (minutes > 1 || Settings::getShortText()) fmt::format_to(std::back_inserter(formatted), "{}{}", minutes, suffix[2]);
        }
    }
    // grammar........ :hugefrownyface:
    if (playtime == 1 && !Settings::getShortText()) { 
        fmt::format_to(std::back_inserter(formatted), "1 Second");
        return fmt::to_string(formatted);
    }
        fmt::format_to(std::back_inserter(formatted), "{}{}", playtime, suffix[3]);

        return fmt::to_string(formatted);
}

tm Data::getLastPlayedRaw(std::string const& levelID) {
    auto data = getFile(levelID);
    time_t timestamp = time(nullptr);

    auto& sessions = data["sessions"];
    auto& latestSession = sessions[sessions.size() - 1];
    auto& latestPair = latestSession[latestSession.size() - 1];

    if (latestPair[0].isNumber()) {
        int sessionStart = latestPair[0].asInt().unwrap();
        timestamp = static_cast<time_t>(sessionStart);
    }
    return geode::localtime(timestamp);
}

tm Data::getPlayedRawAtIndex(std::string const& levelID, int index) {
    auto data = getFile(levelID);
    time_t timestamp = time(nullptr);
    auto& sessionValue = data["sessions"][index][0][0];
    if (sessionValue.isNumber()) {
        int sessionStart = data["sessions"][index][0][0].asInt().unwrap();
        timestamp = static_cast<time_t>(sessionStart);

        return geode::localtime(timestamp);
    }
    else {
        Data::deleteSessionAtIndex(levelID, index);
        return geode::localtime(timestamp);
    }
}

int Data::getSessionPlaytimeRawAtIndex(std::string const& levelID, int index) {
    auto data = getFile(levelID);
    int playtime = 0;
    for (auto& currPair : data["sessions"][index]) {
        if (currPair.size() >= 2) {
            if (!currPair[0].isNumber() || !currPair[1].isNumber()) {
                Backup::loadBackup(levelID);
                return -1;
            }
            playtime += currPair[1].asInt().unwrap() - currPair[0].asInt().unwrap();
        }
        else {
            Data::fixSessionAtIndex(levelID, index);
        }
    }
    return playtime;
}

std::string Data::getPlayedFormatted(tm const& localTimestamp) {
    auto timeformat = Settings::getCustomTimeFormat();
    if (!(Settings::getUseCustomTimeFormat())) {
        if (Settings::getTimeFormat() == "ISO") timeformat = "%F %T";
        if (Settings::getTimeFormat() == "USA") timeformat = "%m/%d/%Y %r";
        if (Settings::getTimeFormat() == "EU") timeformat = "%d.%m.%Y %T";
    }
        
    char formatted[64];

    strftime(formatted, sizeof(formatted), timeformat.c_str(), &localTimestamp);
    return formatted;
}

int Data::getSessionCount(std::string const& levelID) {
    auto data = getFile(levelID);
    return data["sessions"].size();
}

void Data::deleteLevelData(std::string const& levelID) {
    auto data = getFile(levelID);
    data["sessions"] = matjson::Value::array();
    data["linked"] = matjson::Value::array();
    data["attempts"] = matjson::Value::array();
    writeFile(data, levelID);
}

void Data::deleteSessionAtIndex(std::string const& levelID, int const index) {
    auto data = getFile(levelID);
    auto& sessions = data["sessions"];
    auto& attempts = data["attempts"];

    int offset = Data::getAttemptSessionCount(levelID) - Data::getSessionCount(levelID);

    if (sessions.size() == 1) {
        deleteLevelData(levelID);
    } else {
        sessions[index] = NULL;
        attempts[index + offset] = NULL;

        auto newSessions = matjson::Value::array();

        for (auto& currItem : sessions) {
            if (!currItem[0].isNull()) newSessions.push(currItem);
        }

        auto newAttempts = matjson::Value::array();

        for (auto& currItem : attempts) {
            if (currItem.asInt().unwrap() != 0) newAttempts.push(currItem);
        }

        sessions = newSessions;

        attempts = newAttempts;

        writeFile(data, levelID);
    }
}

void Data::fixSessionAtIndex(std::string const& levelID, int const index) {
    auto data = getFile(levelID);

    auto newSession = matjson::Value::array();
    auto& session = data["sessions"][index];
    for (auto& currPair : session) {
        if (currPair.size() >= 2) newSession.push(currPair);
    }
    session = newSession;

    if (newSession.size() == 0) Data::deleteSessionAtIndex(levelID, index);

    writeFile(data, levelID);
}

int Data::getTotalPlaytime(std::string const& levelID) {
    auto data = getFile(levelID);
    auto& sessions = data["sessions"];
    time_t timestamp;

    int playtime = 0;

    for (auto& currSession : sessions) {
        for (auto& currPair : currSession) {
            if (currPair.size() >= 2) {
                if (!currPair[0].isNumber() || !currPair[1].isNumber()) return playtime;
                playtime += currPair[1].asInt().unwrap() - currPair[0].asInt().unwrap();
            }
            if (currPair.size() == 1) {
                if (!currPair[0].isNumber()) return playtime;
                playtime += time(&timestamp) - currPair[0].asInt().unwrap();
            }
        }
    }
    return playtime;
}

bool Data::isLevelPlayedSession(std::string const& levelID) {
    return playedIDs.find(levelID) != playedIDs.end();
}

void Data::appendPlayedLevel(std::string const& levelID) {
    playedIDs.insert(levelID);
}

void Data::appendPauseTimestamp(std::string const& levelID, time_t timestamp) {
    if (Settings::getStopOnCompletion() && Mod::get()->getSavedValue<int>("current-level-best") == 100) return;

    auto data = getFile(levelID);

    auto& sessions = data["sessions"];
    auto& latestSession = sessions[sessions.size() - 1];

    latestSession[latestSession.size() - 1].push(timestamp);

    writeFile(data, levelID);
}

void Data::initAttemptsList(std::string const& levelID) {
    auto data = getFile(levelID);

    auto& attempts = data["attempts"];

    if (attempts.isArray()) return;

    attempts = matjson::Value::array();

    writeFile(data, levelID);
}

void Data::appendAttempts(std::string const& levelID, int newAttempts) {
    auto data = getFile(levelID);
    auto& attempts = data["attempts"];


    attempts.push(newAttempts);

    writeFile(data, levelID);
}

void Data::addSessionAttempts(std::string const& levelID, int newAttempts) {
    auto data = getFile(levelID);
    auto& attempts = data["attempts"];
    auto& latestAttempts = attempts[attempts.size() - 1];

    if (!latestAttempts.isExactlyUInt()) return;


    int count = latestAttempts.asInt().unwrap();

    latestAttempts = count + newAttempts;
    writeFile(data, levelID);

    // return;

    // if (Settings::getSessionType() == "Exit Game" && Data::isLevelPlayedSession(levelID)) { }
}

int Data::getLevelAttempts(std::string const& levelID) {
    auto data = getFile(levelID);
    auto& attempts = data["attempts"];

    int sum = 0;

    for (auto& currAttempt : attempts) {
        if (currAttempt.isNumber()) sum += currAttempt.asInt().unwrap();
    }

    return sum;
}

int Data::getSessionAttemptsAtIndex(std::string const& levelID, int index) {
    auto data = getFile(levelID);
    auto& attempts = data["attempts"];
    if (index < 0) return -1;

    if (index < attempts.size()) return attempts[index].asInt().unwrap();

    return -1;
}

int Data::getAttemptSessionCount(std::string const& levelID) {
    auto data = getFile(levelID);
    auto& attempts = data["attempts"];

    return attempts.size();
}