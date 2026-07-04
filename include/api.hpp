#pragma once

#include <Geode/loader/Dispatch.hpp>

#undef MY_MOD_ID
#define MY_MOD_ID "nanew.playtime-tracker"
namespace PlaytimeAPI {
    /**
     * Returns a formated levelID for the API
     * @param levelID The ID to be formated
     * @param isEditor If the specified ID is for an editor level
     * @return A formated level ID string
     * @note This function will simply return the levelID as a
     * string if the level is not an editor level
     */
    inline std::string formatLevelID(int levelID, bool isEditor) GEODE_EVENT_EXPORT_NORES(&formatLevelID, (levelID, isEditor));

    /**
     * Returns the levels total playtime
     * @param levelID The levelID of the level you want the playtime of.
     * Use the API's formatLevelID to have the correct formating
     * @return The playtime in seconds or 0 if the level wasn't found
     */
    inline int getPlaytime(std::string const& levelID) GEODE_EVENT_EXPORT_NORES(&getPlaytime, (levelID));

    /**
     * Returns the attempts count of the specified level that playtime tracker counted
     * @param levelID The levelID of the level you want the attempts of.
     * Use the API's fortmatLevelID to have the correct formating
     * @return The total amount of attempts across all sessions or 0 if the level wasn't found
     */
    inline int getAttempts(std::string const& levelID) GEODE_EVENT_EXPORT_NORES(&getAttempts, (levelID));

    /**
     * Returns the amount of sessions on the level
     * @param levelID The levelID of the level you want the playtime of.
     * Use the API's formatLevelID to have the correct formating
     * @return The amount of sessions or 0 if the level wasn't found
     */
    inline int getSessionCount(std::string const& levelID) GEODE_EVENT_EXPORT_NORES(&getSessionCount, (levelID));

    /**
     * Returns the Playtime of the specified session
     * @param levelID The levelID of the level you want the playtime of.
     * Use the API's formatLevelID to have the correct formating
     * @param index Which session to get the playtime of, 0 being the oldest session
     * @return The sessions playtime in seconds or 0 if the level/session wasn't found
     */
    inline int getSessionPlaytime(std::string const& levelID, const int index) GEODE_EVENT_EXPORT_NORES(&getSessionPlaytime, (levelID, index));

    /**
     * Returns the attempt count for the specified session
     * @param levelID The levelID of the level you want the playtime of.
     * Use the API's formatLevelID to have the correct formating
     * @param index Which session to get the attempts of, 0 being the oldest session with tracked attempts
     * @return The session attempts or -1 if there are no tracked attempts for that session
     */
    inline int getSessionAttempts(std::string const& levelID, const int index) GEODE_EVENT_EXPORT_NORES(&getSessionAttempts, (levelID, index));

    /**
     * Returns the raw file data of the specified level
     * @param levelID The levelID of the level you want the playtime of.
     * Use the API's formatLevelID to have the correct formating
     * @return The JSON data of the specified level or an empty
     * @note Read the code in the data manager on GitHub or look at the JSON files to see
     * the format / structure of the data
     */
    inline matjson::Value getRawLevelData(std::string const& levelID) GEODE_EVENT_EXPORT_NORES(&getRawLevelData, (levelID));
}