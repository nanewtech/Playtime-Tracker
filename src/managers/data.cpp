#include "./data.hpp"
#include "../managers/settings.hpp"

#include <fstream>
#include <ctime>
#include <matjson.hpp>


using namespace geode::prelude;

auto dataDir = Mod::get()->getSaveDir().string() + "/leveldata.json";
std::filesystem::path dataDirPath = std::filesystem::u8path(dataDir); // check if necessary, deprecated but saw online its something with windows path that could break?!?!


bool fileExists() {
    if (std::filesystem::exists(dataDirPath)) {
        return true;
    }
    return false;
}

void writeFile(matjson::Value data) {
    std::string output = data.dump(); //REMEMBER TO READD NO INDENT lowkey just no indentation cause it makes editing the file harder = less cheating @mizuki :eyes: :eyes:
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

void data::startLevel(std::string levelID) {
    auto data = getFile();
    time_t timestamp;
    
    
    if (!(data[levelID]["sessions"].isArray())) { 
        data[levelID]["sessions"] = matjson::Value::array(); 
    }

    // I'm so sorry about what youre about to read..... :3
    // I'm too stupid to make it simpler.,,..,. forgive me for i have sinned..... 

    data[levelID]["sessions"].push(matjson::Value::array());

    data[levelID]["sessions"][data[levelID]["sessions"].size() - 1].push(matjson::Value::array());
    
    data[levelID]["sessions"][data[levelID]["sessions"].size() - 1][data[levelID]["sessions"][data[levelID]["sessions"].size() - 1].size() - 1].push(time(&timestamp));



    writeFile(data);
}

void data::pauseLevel(std::string levelID) {
    if (settings::getRemovePauses()) {

        auto data = getFile();
        time_t timestamp;

        data[levelID]["sessions"][data[levelID]["sessions"].size() - 1][data[levelID]["sessions"][data[levelID]["sessions"].size() - 1].size() - 1].push(time(&timestamp));

        writeFile(data);
    }
}

void data::resumeLevel(std::string levelID) {
    if (settings::getRemovePauses()) {
        auto data = getFile();
        time_t timestamp;


        data[levelID]["sessions"][data[levelID]["sessions"].size() - 1].push(matjson::Value::array());
        data[levelID]["sessions"][data[levelID]["sessions"].size() - 1][data[levelID]["sessions"][data[levelID]["sessions"].size() - 1].size() - 1].push(time(&timestamp));

        writeFile(data);
    }
}

void data::exitLevel(std::string levelID) {
    
    if (!settings::getRemovePauses()) {
        auto data = getFile();
        time_t timestamp;

        data[levelID]["sessions"][data[levelID]["sessions"].size() - 1][data[levelID]["sessions"][data[levelID]["sessions"].size() - 1].size() - 1].push(time(&timestamp));

        writeFile(data);
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
            formatted += std::to_string(days) + *suffix.begin();
        }
    }
    if (settings::getHighestConvert() == "Days" || settings::getHighestConvert() == "Hours") {
        hours = playtime / 3600;
        if (hours > 0) {
            playtime -= hours * 3600;
            formatted += std::to_string(hours) + *next(suffix.begin(), 1);
        }
    }
    if (!(settings::getHighestConvert() == "Seconds")) {
        minutes = playtime / 60;
        if (minutes > 0) {
            playtime -= minutes * 60;
            formatted += std::to_string(minutes) + *next(suffix.begin(), 2);
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