#include "./data.hpp"
#include "../managers/settings.hpp"

#include <fstream>
#include <ctime>
#include <matjson.hpp>


using namespace geode::prelude;

auto dataDir = Mod::get()->getSaveDir().string() + "/testing.json";
std::filesystem::path dataDirPath = std::filesystem::u8path(dataDir);


bool fileExists() {
    if (std::filesystem::exists(dataDirPath)) {
        return true;
    }
    return false;
}

void writeFile(matjson::Value data) {
    std::string output = data.dump();
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
    return getFile();
}

void data::initializeLevel(int levelID) {
    
    auto data = getFile();
        
    data[std::to_string(levelID)].set("sessions", "placeholder");

    writeFile(data);
}

void data::startLevel(int levelID) {
    auto data = getFile();
    time_t timestamp;
    
    
    if (!(data[std::to_string(levelID)]["sessions"].isArray())) { 
        data[std::to_string(levelID)]["sessions"] = matjson::Value::array(); 
    }

    data[std::to_string(levelID)]["sessions"].push(matjson::Value::array());

    data[std::to_string(levelID)]["sessions"][data[std::to_string(levelID)]["sessions"].size() - 1].push(matjson::Value::array());
    
    data[std::to_string(levelID)]["sessions"][data[std::to_string(levelID)]["sessions"].size() - 1][data[std::to_string(levelID)]["sessions"][data[std::to_string(levelID)]["sessions"].size() - 1].size() - 1].push(time(&timestamp)); 



    writeFile(data);
}

void data::pauseLevel(int levelID) {
    if (settings::getRemovePauses()) {

        auto data = getFile();
        time_t timestamp;

        data[std::to_string(levelID)]["sessions"][data[std::to_string(levelID)]["sessions"].size() - 1][data[std::to_string(levelID)]["sessions"][data[std::to_string(levelID)]["sessions"].size() - 1].size() - 1].push(time(&timestamp));

        writeFile(data);
    }
}

void data::resumeLevel(int levelID) {
    if (settings::getRemovePauses()) {
        auto data = getFile();
        time_t timestamp;


        data[std::to_string(levelID)]["sessions"][data[std::to_string(levelID)]["sessions"].size() - 1].push(matjson::Value::array());
        data[std::to_string(levelID)]["sessions"][data[std::to_string(levelID)]["sessions"].size() - 1][data[std::to_string(levelID)]["sessions"][data[std::to_string(levelID)]["sessions"].size() - 1].size() - 1].push(time(&timestamp));

        writeFile(data);
    }
}

void data::exitLevel(int levelID) {
    
    if (!settings::getRemovePauses()) {
        auto data = getFile();
        time_t timestamp;

        data[std::to_string(levelID)]["sessions"][data[std::to_string(levelID)]["sessions"].size() - 1][data[std::to_string(levelID)]["sessions"][data[std::to_string(levelID)]["sessions"].size() - 1].size() - 1].push(time(&timestamp));

        writeFile(data);
    }
}