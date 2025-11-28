#include "./data.hpp"
#include "./settings.hpp"
#include "./backup.hpp"

#include <fstream>


using namespace geode::prelude;

std::string getDataDir() {
    return Mod::get()->getSaveDir().string() + "/leveldata.json";
}

std::filesystem::path getDataDirPath() {
    return std::filesystem::path(getDataDir());
}

bool Data::fileExists() {
    return std::filesystem::exists(getDataDirPath());
}

void writeFile(matjson::Value data) {
    std::string output = data.dump(matjson::NO_INDENTATION); //lowkey just no indentation cause it makes editing the file harder = less "cheating" :skull: @mizuki :eyes: :eyes:
    std::ofstream o(getDataDirPath());
    o << output;
    o.close();
}

static void initializeFile() {
    matjson::Value data;

    writeFile(data);
}

matjson::Value Data::getFile() {
    if (Data::fileExists()) {
        try {
            std::ifstream i(getDataDir());
            auto data = matjson::parse(i).unwrap();
            i.close();
            return data;
        }
        catch (const geode::UnwrapException&) {
            log::info("LEVELDATA BROKEN, LOADING BACKUP");
            Backup::loadBackup();
        }
    }
    if (Backup::fileExists()) {
        log::info("FILE DOESNT EXIST, LOADING BACKUP");
        Backup::loadBackup();
        return Backup::getFile();
    }
        log::info("BACKUP DOESNT EXIST, CREATING EMPTY FILE");
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

    log::debug("START LEVEL CALLED");

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

void Data::resumeLevel(std::string const& levelID) {
    if (Settings::getStopOnCompletion() && Mod::get()->getSavedValue<int>("current-level-best") == 100) return;
    
    if (!Settings::getRemovePauses()) return;
    
    auto data = getFile();
    time_t timestamp;

    auto& sessions = data[levelID]["sessions"];
    auto& latestSession = sessions[sessions.size() - 1];

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
    try {

        for (auto& currPair : latestSession) {
            if (currPair.size() >= 2) playtime += currPair[1].asInt().unwrap() - currPair[0].asInt().unwrap();
        }
        return playtime;
    }
    catch (const geode::UnwrapException&) {
        Backup::loadBackup();
        return playtime;
    }
}

int Data::getSessionPlaytimeRaw(std::string const& levelID) {
    auto data = getFile();

    int playtime = 0;

    auto& sessions = data[levelID]["sessions"];
    auto& latestSession = sessions[sessions.size() - 1];

    time_t timestamp;
    try {
        if (Settings::getRemovePauses()) return getLatestSession(levelID);

        return time(&timestamp) - latestSession[latestSession.size() - 1][0].asInt().unwrap();
    } catch (const geode::UnwrapException&) {
        return playtime;
    }
}

// do this inside level
int Data::getPlaytimeRaw(std::string const& levelID) {
    auto data = getFile();

    int playtime = 0;

    auto& sessions = data[levelID]["sessions"];
    auto& latestSession = sessions[sessions.size() - 1];
    auto& latestPair = latestSession[latestSession.size() - 1];

    if (!(sessionsInitialized(levelID))) return playtime;
    try {
        if (Settings::getRemovePauses()) {
            for (auto& session : sessions) {
                for (auto& currPair : session) {
                    playtime += currPair[1].asInt().unwrap() - currPair[0].asInt().unwrap();
                }
            }
            return playtime;
        }
        time_t timestamp;
        if (latestPair[0].isExactlyUInt() && latestPair.size() == 1) {
            return time(&timestamp) - latestPair[0].asInt().unwrap();
        }
    }
    catch (const geode::UnwrapException&) {
        return playtime;
    }
}

std::string Data::formattedPlaytime(int playtime) {
    int days = 0;
    int hours = 0;
    int minutes = 0;
    std::string formatted = "";
    std::list<std::string> suffix = { "d ", "h ", "m ", "s" };

    if (!Settings::getShortText())  suffix = { " Days, ", " Hours, ", " Minutes, ", " Seconds" };

    // @nanew optimize plssss :3
    // there HAS to be a better way to do this 
    if (Settings::getHighestConvert() == "Days") {
        days = playtime / 86400;
        if (days > 0) {
            playtime -= days * 86400;
            if (days == 1 && !Settings::getShortText()) formatted += std::to_string(days) + " Day, ";
            if (days > 1 || Settings::getShortText()) formatted += std::to_string(days) + *suffix.begin();
        }
    }
    if (Settings::getHighestConvert() == "Days" || Settings::getHighestConvert() == "Hours") {
        hours = playtime / 3600;
        if (hours > 0) {
            playtime -= hours * 3600;
            if (hours == 1 && !Settings::getShortText()) formatted += std::to_string(hours) + " Hour, ";
            if (hours > 1 || Settings::getShortText()) formatted += std::to_string(hours) + *next(suffix.begin(), 1);
        }
    }
    if (!(Settings::getHighestConvert() == "Seconds")) {
        minutes = playtime / 60;
        if (minutes > 0) {
            playtime -= minutes * 60;
            if (minutes == 1 && !Settings::getShortText()) formatted += std::to_string(minutes) + " Minute, "; // grammar......
            if (minutes > 1 || Settings::getShortText()) formatted += std::to_string(minutes) + *next(suffix.begin(), 2);
        }
    }
    // grammar........ :hugefrownyface:
    if (playtime == 1 && !Settings::getShortText()) { 
        formatted += "1 Second";
        return formatted;
    }
        formatted += std::to_string(playtime) + *next(suffix.begin(), 3);

        return formatted;
}

tm* Data::getLastPlayedRaw(std::string const& levelID) {
    auto data = getFile();
    time_t timestamp = time(0);

    auto& sessions = data[levelID]["sessions"];
    auto& latestSession = sessions[sessions.size() - 1];
    auto& latestPair = latestSession[latestSession.size() - 1];

    if (latestPair[0].isExactlyUInt()) {
        int sessionStart = latestPair[0].asInt().unwrap();
        timestamp = static_cast<time_t>(sessionStart);
    }
    return localtime(&timestamp);
}

tm* Data::getPlayedRawAtIndex(std::string const& levelID, int index) {
    auto data = getFile();
    time_t timestamp = time(0);
        
    int sessionStart = data[levelID]["sessions"][index][0][0].asInt().unwrap();
    timestamp = static_cast<time_t>(sessionStart);

    return localtime(&timestamp);
}

int Data::getSessionPlaytimeRawAtIndex(std::string const& levelID, int index) {
    auto data = getFile();
    int playtime = 0;
   try {
        for (auto& currPair : data[levelID]["sessions"][index]) {
            if (currPair.size() >= 2) {
                playtime += currPair[1].asInt().unwrap() - currPair[0].asInt().unwrap();
            }
            else {
                Data::fixSessionAtIndex(levelID, index);
            }
        }
        return playtime;
    }
    catch (const geode::UnwrapException&) {
        Backup::loadBackup();
        return -1;
    }
}

std::string Data::getPlayedFormatted(tm* localTimestamp) {
    
    auto timeformat = CCString::create(Settings::getCustomTimeFormat())->getCString();
    if (!(Settings::getUseCustomTimeFormat())) {
        if (Settings::getTimeFormat() == "ISO") timeformat = "%F %T";
        if (Settings::getTimeFormat() == "USA") timeformat = "%m/%d/%Y %r";
        if (Settings::getTimeFormat() == "EU") timeformat = "%d.%m.%Y %T";
    }
        
    char formatted[64];

    strftime(formatted, sizeof(formatted), timeformat, localTimestamp);
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

    if (data[levelID]["sessions"].size() == 1) {
        deleteLevelData(levelID);
    } else {
        data[levelID]["sessions"][index] = NULL;

        auto sessions = matjson::Value::array();

        for (auto& currItem : data[levelID]["sessions"]) {
            if (!currItem[0].isNull()) sessions.push(currItem);
        }

        data[levelID]["sessions"] = sessions;

        writeFile(data);
    }
}

void Data::fixSessionAtIndex(std::string const& levelID, int index) {
    auto data = getFile();

        auto session = matjson::Value::array();

        for (auto& currPair : data[levelID]["sessions"][index]) {
            if (currPair.size() >= 2) session.push(currPair);
        }

        data[levelID]["sessions"][index] = session;

        writeFile(data);
}