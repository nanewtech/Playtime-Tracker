# v1.2.2
- fixed a typo where junk data would be created, preventing real data to ever be written
# v1.2.1
- made it so level data is now in a separate file for each level instead of one big file to prevent lag when saving / loading
- added level cache to reduce unnecessary read operations when opening the menu popup
- made loading the data asynchronous to prevent the game freezing while loading the menu popup
- made the pause menu popup update when not using 'Remove pauses from playtime'
- made it so the menu popup updates when deleting sessions
- fixed an issue where attempts wouldn't get deleted properly when deleting sessions
- fixed an issue where 'remove pauses when AFK' would not remove pauses when exiting or resuming in certain ways
# v1.1.0
- Ported to 2.2081 
- added attempt count per session
- added display for attempt count per session in the menu popup
- indented session attempts and playtime in popup for better visibility
- added button to overwrite a levels attempts with the mods counted attempts
- fixed minor bugs and typos
# v1.0.7
- fixed minor issue where geode would crash occasionally with large amounts of mods enabled
- adjusted listing of credits / special thanks
# v1.0.6
- added option to move the button in the pause menu
- added option to disable the button in the pause menu
- made the buttons compatible with texture packs
- updated about page
- added homepage
- fixed typos
# v1.0.5
- added customizable afk timeout when not removing pauses from playtime
- fixed an issue where pressing the replay button in platformer mode wouldn't count playtime
- fixed tracking when choosing options on level complete screen
- fixed an issue where the session wouldn't continue if you had 'session end type' as 'exit game' and 'remove pauses from playtime' off
# v1.0.4
- added an option to have the session end at game exit or level exit
- made "remove pauses from playtime" on by default
- fixed minor issue with the session continuing twice after resuming
- changed about page to include feature suggestion
# v1.0.3
- Codebase improvements and index requirements (#1)
# v1.0.2
- fixed a bug where gd would crash on mobile when clicking on the button in the pause menu
# v1.0.1
- removed debug messages in the geode console
- added default value for custom time format
- added color highlighting for settings info
- reordered time format settings
- fixed minor typos
# v1.0.0
- Initial Release!