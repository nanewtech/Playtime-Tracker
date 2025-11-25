#include "./data.hpp"
#include "../managers/settings.hpp"

#include <fstream>
#include <ctime>
#include <matjson.hpp>


using namespace geode::prelude;

auto dataDir = Mod::get()->getSaveDir().string() + "/leveldata.json";
std::filesystem::path dataDirPath = std::filesystem::u8path(dataDir); 


bool fileExists() {
    if (std::filesystem::exists(dataDirPath)) {
        return true;
    }
    return false;
}

void writeFile(matjson::Value data) {
    std::string output = data.dump(matjson::NO_INDENTATION); //REMEMBER TO READD NO INDENT lowkey just no indentation cause it makes editing the file harder = less cheating @mizuki :eyes: :eyes:
    std::ofstream o(dataDirPath);
    o << output;
    o.close();
}

void initializeFile() {
    matjson::Value data;

    writeFile(data);

    log::debug("Created data file at: {}", fmt::to_string(dataDirPath));
}

matjson::Value getFile() {
    if (fileExists()) {
        std::ifstream i(dataDir);
        auto data = matjson::parse(i).unwrap();
        i.close();
        return data;
    }
    log::debug("FILE DOESNT EXIST, CREATING FILE");
    initializeFile();
    return getFile(); // could end in infinite loop, TODO: prevent possible infinite loop
}

bool sessionsInitialized(std::string levelID) {
    auto data = getFile();

    if (data[levelID].isNull()) return false;
    return true;
}

void data::startLevel(std::string levelID) {
    auto data = getFile();
    time_t timestamp;
    
    
    if (!(data[levelID]["sessions"].isArray())) { 
        data[levelID]["sessions"] = matjson::Value::array(); 
    }

    // I'm so sorry about what youre about to read..... :3
    // I'm too stupid to make it simpler.,,..,. forgive me for i have sinned..... 
    if (!(settings::getStopOnCompletion() && Mod::get()->getSavedValue<int>("current-level-best") == 100)) { 
        data[levelID]["sessions"].push(matjson::Value::array());

        data[levelID]["sessions"][data[levelID]["sessions"].size() - 1].push(matjson::Value::array());

        data[levelID]["sessions"][data[levelID]["sessions"].size() - 1][data[levelID]["sessions"][data[levelID]["sessions"].size() - 1].size() - 1].push(time(&timestamp));



        writeFile(data);
    }
}

void data::pauseLevel(std::string levelID) {
    
    if (!(settings::getStopOnCompletion() && Mod::get()->getSavedValue<int>("current-level-best") == 100)) {
        if (settings::getRemovePauses()) {

            auto data = getFile();
            time_t timestamp;

            data[levelID]["sessions"][data[levelID]["sessions"].size() - 1][data[levelID]["sessions"][data[levelID]["sessions"].size() - 1].size() - 1].push(time(&timestamp));

            writeFile(data);
        }
    }
}

void data::resumeLevel(std::string levelID) {
    if (!(settings::getStopOnCompletion() && Mod::get()->getSavedValue<int>("current-level-best") == 100)) {
        if (settings::getRemovePauses()) {
            auto data = getFile();
            time_t timestamp;


            data[levelID]["sessions"][data[levelID]["sessions"].size() - 1].push(matjson::Value::array());
            data[levelID]["sessions"][data[levelID]["sessions"].size() - 1][data[levelID]["sessions"][data[levelID]["sessions"].size() - 1].size() - 1].push(time(&timestamp));

            writeFile(data);
        }
    }
}

void data::exitLevel(std::string levelID) {
    log::debug("EXIT LEVEL CALLED!");
    if (!(settings::getStopOnCompletion() && Mod::get()->getSavedValue<int>("current-level-best") == 100)) {
        log::debug("GETS PAST COMPLETION CHECK");
        if (!(settings::getRemovePauses()) || (Mod::get()->getSavedValue<bool>("is-paused") && !(settings::getRemovePauses())) || (!(Mod::get()->getSavedValue<bool>("is-paused")) && settings::getRemovePauses())) {
            auto data = getFile();
            time_t timestamp;

            data[levelID]["sessions"][data[levelID]["sessions"].size() - 1][data[levelID]["sessions"][data[levelID]["sessions"].size() - 1].size() - 1].push(time(&timestamp));

            writeFile(data);
        }
    }
}

int data::getLatestSession(std::string levelID) {
    auto data = getFile();

    int playtime = 0;

    time_t timestamp;

    for (auto& currPair : data[levelID]["sessions"][data[levelID]["sessions"].size() - 1]) {

        playtime += currPair[1].asInt().unwrap() - currPair[0].asInt().unwrap();
    }
    return playtime;
}

int data::getSessionPlaytimeRaw(std::string levelID) {
    auto data = getFile();

    int playtime = 0;

    time_t timestamp;

    if (settings::getRemovePauses()) return getLatestSession(levelID);

    return time(&timestamp) - data[levelID]["sessions"][data[levelID]["sessions"].size() - 1][data[levelID]["sessions"][data[levelID]["sessions"].size() - 1].size() - 1][0].asInt().unwrap();
}

// do this inside level
int data::getPlaytimeRaw(std::string levelID) {
    auto data = getFile();

    int playtime = 0;
    
    if (!(sessionsInitialized(levelID))) return playtime;

    if (settings::getRemovePauses()) {
        for (auto& value : data[levelID]["sessions"]) {
            // value is matjson::Value
            for (auto& currPair : value) {
                
                playtime += currPair[1].asInt().unwrap() - currPair[0].asInt().unwrap();
            }
        }

        return playtime;
    }
    time_t timestamp;
    if (data[levelID]["sessions"][data[levelID]["sessions"].size() - 1][data[levelID]["sessions"][data[levelID]["sessions"].size() - 1].size() - 1][0].isExactlyUInt() && data[levelID]["sessions"][data[levelID]["sessions"].size() - 1][data[levelID]["sessions"][data[levelID]["sessions"].size() - 1].size() - 1].size() == 1) {
        log::debug("SESSION PT VAL: {}", data[levelID]["sessions"][data[levelID]["sessions"].size() - 1][data[levelID]["sessions"][data[levelID]["sessions"].size() - 1].size() - 1][0].asInt().unwrap());
        return time(&timestamp) - data[levelID]["sessions"][data[levelID]["sessions"].size() - 1][data[levelID]["sessions"][data[levelID]["sessions"].size() - 1].size() - 1][0].asInt().unwrap();
    }

    for (auto& value : data[levelID]["sessions"]) {
        // value is matjson::Value
        for (auto& currPair : value) {
            playtime += currPair[1].asInt().unwrap() - currPair[0].asInt().unwrap();
        }
    }

    return playtime;
}

std::string data::formattedPlaytime(int playtime) {
    int days = 0;
    int hours = 0;
    int minutes = 0;
    std::string formatted = "";
    std::list<std::string> suffix = { "d ", "h ", "m ", "s" };

    if (!settings::getShortText())  suffix = { " Days, ", " Hours, ", " Minutes, ", " Seconds" };

    // @nanew optimize plssss :3
    // there HAS to be a better way to do this 
    if (settings::getHighestConvert() == "Days") {
        days = playtime / 86400;
        if (days > 0) {
            playtime -= days * 86400;
            if (days == 1 && !settings::getShortText()) formatted += std::to_string(minutes) + "Day, ";
            if (days > 1 || settings::getShortText()) formatted += std::to_string(days) + *suffix.begin();
        }
    }
    if (settings::getHighestConvert() == "Days" || settings::getHighestConvert() == "Hours") {
        hours = playtime / 3600;
        if (hours > 0) {
            playtime -= hours * 3600;
            if (hours == 1 && !settings::getShortText()) formatted += std::to_string(minutes) + "Hour, ";
            if (hours > 1 || settings::getShortText()) formatted += std::to_string(hours) + *next(suffix.begin(), 1);
        }
    }
    if (!(settings::getHighestConvert() == "Seconds")) {
        minutes = playtime / 60;
        if (minutes > 0) {
            playtime -= minutes * 60;
            if (minutes == 1 && !settings::getShortText()) formatted += std::to_string(minutes) + "Minute, "; // grammar......
            if (minutes > 1 || settings::getShortText()) formatted += std::to_string(minutes) + *next(suffix.begin(), 2);
        }
    }
    // grammar........ :hugefrownyface:
    if (playtime == 1 && !settings::getShortText()) { 
        formatted += "1 Second";
        return formatted;
    }
        formatted += std::to_string(playtime) + *next(suffix.begin(), 3);

        return formatted;
}

tm* data::getLastPlayedRaw(std::string levelID) {
    auto data = getFile();
    time_t timestamp = time(0);
    if (data[levelID]["sessions"][data[levelID]["sessions"].size() - 1][data[levelID]["sessions"][data[levelID]["sessions"].size() - 1].size() - 1][0].isExactlyUInt()) {
        int sessionStart = data[levelID]["sessions"][data[levelID]["sessions"].size() - 1][data[levelID]["sessions"][data[levelID]["sessions"].size() - 1].size() - 1][0].asInt().unwrap();
        timestamp = static_cast<time_t>(sessionStart);

    }
    return localtime(&timestamp);
}

tm* data::getPlayedRawAtIndex(std::string levelID, int index) {
    auto data = getFile();
    time_t timestamp = time(0);
        int sessionStart = data[levelID]["sessions"][index][0][0].asInt().unwrap();
        timestamp = static_cast<time_t>(sessionStart);

    return localtime(&timestamp);
}

int data::getSessionPlaytimeRawAtIndex(std::string levelID, int index) {
    auto data = getFile();
    int playtime = 0;

    for (auto& currPair : data[levelID]["sessions"][index]) {

        playtime += currPair[1].asInt().unwrap() - currPair[0].asInt().unwrap();
    }
    return playtime;
}

std::string data::getPlayedFormatted(tm* localTimestamp) {
    
    auto timeformat = CCString::create(settings::getCustomTimeFormat())->getCString();
    if (!(settings::getUseCustomTimeFormat())) {
        if (settings::getTimeFormat() == "ISO") timeformat = "%F %T";
        if (settings::getTimeFormat() == "USA") timeformat = "%m/%d/%Y %r";
        if (settings::getTimeFormat() == "EU") timeformat = "%d.%m.%Y %T";
    }
        
    char formatted[sizeof(timeformat)*10];

    strftime(formatted, sizeof(formatted), timeformat, localTimestamp);
    return formatted;
}

int data::getSessionCount(std::string levelID) {
    auto data = getFile();
    return data[levelID]["sessions"].size();
}

void data::deleteLevelData(std::string levelID) {
    auto data = getFile();
    data[levelID]["sessions"] = matjson::Value::array();
    writeFile(data);
}

void data::deleteSessionAtIndex(std::string levelID, int index) {
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