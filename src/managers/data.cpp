#include "./data.hpp"
#include "./settings.hpp"
#include "./backup.hpp"


using namespace geode::prelude;

std::unordered_set<std::string> playedIDs;

std::filesystem::path getDataDirPath() {
    return Mod::get()->getSaveDir() / "leveldata.json";
}

bool Data::fileExists() {
    return std::filesystem::exists(getDataDirPath());
}

void writeFile(matjson::Value const& data) {
    std::string output = data.dump(matjson::NO_INDENTATION); //lowkey just no indentation cause it makes editing the file harder = less "cheating" :skull: @mizuki :eyes: :eyes:
    (void)file::writeString(getDataDirPath(), output);
}

static void initializeFile() {
    matjson::Value data;

    writeFile(data);
}

matjson::Value Data::getFile() {
    if (Data::fileExists()) {
        if (auto data = file::readJson(getDataDirPath())) {
            return data.unwrap();
        }
        else {
            log::info("Leveldata.json broken, Loading backup!");
            Backup::loadBackup();
        }
    }
    if (Backup::fileExists()) {
        log::info("Leveldata.json doesn't exist, Loading backup!");
        Backup::loadBackup();
        return Backup::getFile();
    }
        log::info("backup doesn't exist, Creating empty leveldata.json!");
        initializeFile();
        matjson::Value data;
        return data;
}

static bool sessionsInitialized(std::string const& levelID) {
    auto data = Data::getFile();
    return !data[levelID].isNull();
}

void Data::startLevel(std::string const& levelID) {
    auto data = getFile();
    time_t timestamp;

    auto& sessions = data[levelID]["sessions"];
    

    if (Settings::getStopOnCompletion() && Mod::get()->getSavedValue<int>("current-level-best") == 100) return;

    if (!(sessions.isArray())) sessions = matjson::Value::array(); 

    sessions.push(matjson::Value::array());

    auto& latestSession = sessions[sessions.size() - 1];

    latestSession.push(matjson::Value::array());

    latestSession[latestSession.size() - 1].push(time(&timestamp));

    writeFile(data);
}

void Data::pauseLevel(std::string const& levelID) {

    if (Settings::getStopOnCompletion() && Mod::get()->getSavedValue<int>("current-level-best") == 100) return; 
        
    if (!Settings::getRemovePauses()) return;
        
    auto data = getFile();
    time_t timestamp;

    auto& sessions = data[levelID]["sessions"];
    auto& latestSession = sessions[sessions.size() - 1];


    latestSession[latestSession.size() - 1].push(time(&timestamp));

    writeFile(data);
    Backup::createBackup(Data::getFile());
}

void Data::resumeLevel(std::string const& levelID, bool removePauseOverride) {
    if (Settings::getStopOnCompletion() && Mod::get()->getSavedValue<int>("current-level-best") == 100) return;

    if (!Settings::getRemovePauses() && !removePauseOverride) return;
    
    auto data = getFile();
    time_t timestamp;

    auto& sessions = data[levelID]["sessions"];
    auto& latestSession = sessions[sessions.size() - 1];

    if (sessions.size() <= 0) {
        Data::startLevel(levelID);
        return;
    }

    latestSession.push(matjson::Value::array());
    latestSession[latestSession.size() - 1].push(time(&timestamp));

    writeFile(data);
}

void Data::exitLevel(std::string const& levelID) {
    if (Settings::getStopOnCompletion() && Mod::get()->getSavedValue<int>("current-level-best") == 100) return;

    if (!(Settings::getRemovePauses()) || (Mod::get()->getSavedValue<bool>("is-paused") && !(Settings::getRemovePauses())) || (!(Mod::get()->getSavedValue<bool>("is-paused")) && Settings::getRemovePauses())) {
        auto data = getFile();
        time_t timestamp;

        auto& sessions = data[levelID]["sessions"];
        auto& latestSession = sessions[sessions.size() - 1];

        latestSession[latestSession.size() - 1].push(time(&timestamp));

        writeFile(data);
        Backup::createBackup(Data::getFile());
        }
    }

int Data::getLatestSession(std::string const& levelID) {
    auto data = getFile();

    int playtime = 0;

    auto& sessions = data[levelID]["sessions"];
    auto& latestSession = sessions[sessions.size() - 1];

    time_t timestamp;

    int index = 0;
    for (auto& currPair : latestSession) {
        if (currPair.size() >= 2) {
            if (!currPair[0].isNumber() || !currPair[1].isNumber()) {
                Backup::loadBackup();
                return playtime;
            }
            playtime += currPair[1].asInt().unwrap() - currPair[0].asInt().unwrap();
        }
        else Data::fixSessionAtIndex(levelID, sessions.size() - 1);
    }
    return playtime;

}

int Data::getSessionPlaytimeRaw(std::string const& levelID) {
    auto data = getFile();

    int playtime = 0;

    auto& sessions = data[levelID]["sessions"];
    auto& latestSession = sessions[sessions.size() - 1];

    time_t timestamp;
    if (Settings::getRemovePauses()) return getLatestSession(levelID);

    auto& latestTime = latestSession[latestSession.size() - 1][0];
    return latestTime.isNumber() ? time(&timestamp) - latestTime.asInt().unwrap() : playtime;
}

// do this inside level
int Data::getPlaytimeRaw(std::string const& levelID) {
    auto data = getFile();

    int playtime = 0;

    auto& sessions = data[levelID]["sessions"];
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

tm* Data::getLastPlayedRaw(std::string const& levelID) {
    auto data = getFile();
    time_t timestamp = time(0);

    auto& sessions = data[levelID]["sessions"];
    auto& latestSession = sessions[sessions.size() - 1];
    auto& latestPair = latestSession[latestSession.size() - 1];

    if (latestPair[0].isNumber()) {
        int sessionStart = latestPair[0].asInt().unwrap();
        timestamp = static_cast<time_t>(sessionStart);
    }
    return localtime(&timestamp);
}

tm* Data::getPlayedRawAtIndex(std::string const& levelID, int index) {
    auto data = getFile();
    time_t timestamp = time(0);
    auto& sessionValue = data[levelID]["sessions"][index][0][0];
    if (sessionValue.isNumber()) {
        int sessionStart = data[levelID]["sessions"][index][0][0].asInt().unwrap();
        timestamp = static_cast<time_t>(sessionStart);

        return localtime(&timestamp);
    }
    else {
        Data::deleteSessionAtIndex(levelID, index);
        return localtime(&timestamp);
    }
}

int Data::getSessionPlaytimeRawAtIndex(std::string const& levelID, int index) {
    auto data = getFile();
    int playtime = 0;
    for (auto& currPair : data[levelID]["sessions"][index]) {
        if (currPair.size() >= 2) {
            if (!currPair[0].isNumber() || !currPair[1].isNumber()) {
                Backup::loadBackup();
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

std::string Data::getPlayedFormatted(tm* localTimestamp) {
    
    auto timeformat = Settings::getCustomTimeFormat();
    if (!(Settings::getUseCustomTimeFormat())) {
        if (Settings::getTimeFormat() == "ISO") timeformat = "%F %T";
        if (Settings::getTimeFormat() == "USA") timeformat = "%m/%d/%Y %r";
        if (Settings::getTimeFormat() == "EU") timeformat = "%d.%m.%Y %T";
    }
        
    char formatted[64];

    strftime(formatted, sizeof(formatted), timeformat.c_str(), localTimestamp);
    return formatted;
}

int Data::getSessionCount(std::string const& levelID) {
    auto data = getFile();
    return data[levelID]["sessions"].size();
}

void Data::deleteLevelData(std::string const& levelID) {
    auto data = getFile();
    data[levelID]["sessions"] = matjson::Value::array();
    writeFile(data);
}

void Data::deleteSessionAtIndex(std::string const& levelID, int index) {
    auto data = getFile();
    auto& sessions = data[levelID]["sessions"];

    if (sessions.size() == 1) {
        deleteLevelData(levelID);
    } else {
        sessions[index] = NULL;

        auto newSessions = matjson::Value::array();

        for (auto& currItem : sessions) {
            if (!currItem[0].isNull()) newSessions.push(currItem);
        }

        sessions = newSessions;

        writeFile(data);
    }
}

void Data::fixSessionAtIndex(std::string const& levelID, int index) {
    auto data = getFile();

    auto newSession = matjson::Value::array();
    auto& session = data[levelID]["sessions"][index];
    for (auto& currPair : session) {
        if (currPair.size() >= 2) newSession.push(currPair);
    }
    session = newSession;

    if (newSession.size() == 0) Data::deleteSessionAtIndex(levelID, index);

    writeFile(data);
}

int Data::getTotalPlaytime(std::string const& levelID) {
    auto data = getFile();
    auto& sessions = data[levelID]["sessions"];
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

    auto data = getFile();

    auto& sessions = data[levelID]["sessions"];
    auto& latestSession = sessions[sessions.size() - 1];

    latestSession[latestSession.size() - 1].push(timestamp);

    writeFile(data);
}