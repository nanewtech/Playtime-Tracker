# Playtime-Tracker
![Mod Version](https://api.geode-sdk.org/v1/mods/nanew.playtime-tracker/status_badge?stat=version)
![Downloads](https://api.geode-sdk.org/v1/mods/nanew.playtime-tracker/status_badge?stat=downloads)
![GD Version](https://api.geode-sdk.org/v1/mods/nanew.playtime-tracker/status_badge?stat=gd_version)
![Geode Version](https://api.geode-sdk.org/v1/mods/nanew.playtime-tracker/status_badge?stat=geode_version)

A mod to track your playtime on levels. 

Install through the geode mod index ingame :)

### API Usage
Add `nanew.playtime-tracker` to your mod dependencies to use the API
```JSON
{
  "dependencies": {
    "nanew.playtime-tracker": ">=1.2.4"
  }
}
```
And using it works like this:
```cpp
#include <nanew.playtime-tracker/include/api.hpp>

std::string levelID = PlaytimeAPI::formatLevelID(12345, true);
int playtime = PlaytimeAPI::getPlaytime(levelID);
```
check [the header file](include/api.hpp) for all functions
## planned features
see [TODO](TODO.md)

## Credits / Special Thanks
**MostDefinitelyAlbert** - For the initial idea behind the mod + testing

**Jimaras17** - For the button / logo art + testing

**hiimjasmine00** - For helping with the code

**st** - For helping me playtest the mod and being awesome in general

## building

see the [geode website](https://docs.geode-sdk.org/getting-started/cpp-stuff/) for a guide on how to build the mod yourself.
