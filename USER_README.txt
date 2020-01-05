BBCF Improvement Mod (v2.07) by KoviDomi -- README
==========================

What this mod provides
==========================
Unlocks the game's region restricted multiplayer.
Adds extra game modes.
Create custom palettes and effects.
Manage and load custom palettes without file modifications.
See each other's custom palettes in online matches.
Options to improve the performance of the game even further.
More flexibility to change the graphics options.
Change avatars and accessories in online rooms/lobbies without going back into menu.
Freely adjustable ingame currency value.

Where to place your custom palette files (.hpl)
==========================
- The palette folders are created in the path "..\BlazBlue Centralfiction\BBCF_IM\Palettes\" upon the first launch of the mod.
- Place your .hpl files into the character's corresponding palette folder (at "..\BlazBlue Centralfiction\BBCF_IM\Palettes\") to have the mod automatically load them upon start, and making them selectable ingame via the mod's menu.
- Effect files must end with the naming convention of "_effectXX.hpl". Where XX is the index of the effect file. 
For example, if you have a custom palette file named "Nyx_Izanami.hpl", then in the same palette folder a file named "Nyx_Izanami_effect01.hpl" will be loaded as that palette's first effect, and a file named "Nyx_Izanami_effect06.hpl" will be loaded as sixth, etc.
- A file created with name ending "_effectbloom.hpl" will turn on the bloom effect for that custom palette in the game. (Once activated, the bloom effect will keep the color it was first activated on, and can't be changed unless a new round is started)

Requirements
==============
The game must be up-to-date

How to install
==========================
1. Download and extract the latest BBCF_IM.zip
2. Put dinput8.dll and settings.ini into the game's folder (where BBCF.exe is located)
(Should be at "..\Steam\SteamApps\common\BlazBlue Centralfiction")
3. Adjust the values in settings.ini as desired

Uninstall
==========================
Delete or rename dinput8.dll. The mod makes no permanent changes to anything.

Known issues
==========================
Platinum keeps swapping between her default and the selected custom palette. To prevent this select the palette before the match starts, and do not change it during match.

Troubleshooting
==========================
First make sure that your issue doesn't persist if you remove/rename dinput8.dll
If the issue still persists, then it has nothing to do with the mod.

1. The mod's UI is very small, unreadable:
This happens if you run the game in windowed or full-window mode with a resolution that's higher than what your monitor supports.
Try changing your display setting to fullscreen, or lowering the resolution ingame.

2. Blackscreen problem:
Make sure that RenderingWidth and RenderingHeight values in the .ini file are correctly set to the resolution of your game. 
If their values are higher than your monitor's native resolution, then you will get blackscreen.

3. Game crashes when launched:
Make sure that the settings.ini file is present in the game's folder.
Check if the problem still occurs after you remove, or rename dinput8.dll
Turn off tools such as MSI Afterburner, Plays.tv, or other recording softwares and overlays that manipulate D3D.
Make sure you are running both Steam and the game with administrator privileges, and remove any steam_appid.txt file if present in the game's directory.
Restore the default settings in the settings.ini file.
Reboot your system.

Keep in mind that the mod may partially, or completely stop working whenever a new official patch of the game is released. (Though unlikely)

Changelog
==========================

??-??-???? -- version 2.07
===============================
- Added Highlight and Placeholder modes in the palette editor, to make it easier to find the corresponding colors
- Added donate button
- Players now send custom palettes and other datas to all spectators
- When spectating, BBCFIM will show both player1 and player2 whether they use BBCFIM
- Custom game modes are applied on spectators as well
- Disabled stage selection slider in game modes other than online/training/versus
- Fixed mistaking spectators as the opponent player
- Fixed Steroid mode starting Overdrive mode instead
- Fixed the vampire gamemodes' health draining not working when the timer is set to infinite in versus mode
- Fixed Jubei's stage missing from BBCFIM's stage selection slider

31-03-2018 -- version 2.06
===============================
- Added new custom game modes:
	Five Punch Mode:
		* Taking damage five times results in death, unless blocked with barrier
		* Each round 50% of the Burst Gauge is regenerated
		* Each round lasts 60 seconds
		* This game mode is always 5 rounds
	Tug of War Mode:
		* Start with 50% HP
		* All damages hurt/heal 10% HP, unless blocked with barrier
		* Each round 50% of the Burst Gauge is regenerated
		* Each round lasts 60 seconds
		* This game mode is always 5 rounds
	Infinite Heat Mode:
		* Infinite Heat Gauge
- Added MenuSize option
- Added buttons for picking custom palettes randomly
- Added Stage select slider, now you can select any stages online
- Added "Show transparency values" checkbox on the palette editor
- Added "Save with bloom effect" tickbox to the "Character file" page in the palette editor 
- Added logging to file
- Added Crash Dump generation on unhandled exceptions to track down crash causing bugs
- Custom palettes can now be placed in subfolders
- Custom game modes now can be applied on replays
- Transparency/Alpha values are now hidden by default on the palette editor
- Fixed receiving BBCFIM ID packets outside of the character selection screen
- Fixed some custom game mode modifications not resetting back to normal when gone back to the charselect screen without quitting to the main menu in offline modes
- Fixed spectators being able to switch the players' palettes
- Fixed crash when overwriting an existing palette file with upper-case/under-case differences in the palette editor

11-03-2018 -- version 2.05
===============================
- Fixed some players being unable the set their custom palettes in online matches

10-03-2018 -- version 2.04
===============================
- Added custom game modes that are playable in training, versus, and online: 
	Steroid Mode:
		* Max HP: +150%
		* Max Heat Gauge: +50%
		* Heat gain: +20%
		* Automatic Heat gain on low HP: +100%
		* Burst Gauge gain: +100%
	Vampire Mode:
		* 60% of damage dealt is healed back
		* Players lose 1% max HP per second
	Extreme Vampire Mode:
		* 200% of damage dealt is healed back
		* Healing beyond full HP increases max HP
		* Losing 1.5% max HP per second
	One Punch Mode: 
		* Any damage results in instakill, unless blocked with barrier
		* Each round 50% of the Burst Gauge is regenerated
		* Each round lasts 15 seconds
		* This game mode is always 5 rounds
	Two Punch Mode:
		* Taking damage twice results in death, unless blocked with barrier
		* Each round 50% of the Burst Gauge is regenerated
		* Each round lasts 15 seconds
		* This game mode is always 5 rounds
- Added notifications
- Added NotificationPopups option
- BBCFIM now lets you know if the opponent is using BBCFIM as well (detecting versions only v2.04 and above)
- Added overwrite confirmation when a palette with the same name already exists
- Saving a custom palette in the palette editor will now automatically load it into the palettes menu. No longer needed to manually reload them
- Saving custom palettes now support capital letters as well
- Saving custom palette won't clear the filename's textinput box anymore
- Loading custom effects now have better error-logging
- Fixed player 1 and player 2 palettes being switched up after "quick-changing" characters while playing as player 2 in the tutorial mode
- Fixed characters using palettes from previous replays in the Replay Theater mode
- Fixed players occasionally not sending the custom palettes to the opponent in online matches
- Fixed rare crash in lobby
- Fixed crash whenever losing in the Grim of Abyss mode

20-02-2018 -- version 2.03
===============================
- Fixed crash that occured whenever a custom palette was selected then switching to an another character while the BBCFIM menu was closed
- Fixed crash when attempting to select Platinum or Valkenhayn in the palette editor.
- Fixed bloom effect not being reactivated on the 22nd palettes upon rematches in offline modes
- Fixed default palettes changing to different ones upon rematches in offline modes

18-02-2018 -- version 2.02
===============================
- Added ingame palette editor
- Added support for bloom effect on custom palettes (see "Where to place your custom palettes" section for how-to)
- Added NMC's winning palette of Azure Abyss' february palette contest
- Fixed bloom effect not being visible on the 22nd palettes
- Fixed crash in arcade and versus modes when playing as player 2
- Fixed palette swapping bugs in arcade and versus modes when playing as player 2
- Fixed crash in versus mode when locally playing with two players

08-02-2018 -- version 2.01
===============================
- Fixed crashing in tutorial mode
- Fixed a palette bug that occurred whenever characters were "quick-changed" in training mode

07-02-2018 -- version 2.00
===============================
- New overlay GUI framework
- Palette files (.hpl) can be loaded without any file modification
- Can freely switch palettes anytime during matches
- Can freely switch avatars and accessories on the fly
- Can freely edit the value of the ingame currency
- Palettes are now exchanged at the start of the matches (instead at the "wheel of fate" part)
- Added ToggleButton option
- Added CpuUsageFix option to reduce the game's CPU usage
- Added CheckUpdates option to check if newer version of BBCFIM is available
- Removed CustomPalettes option as it will always be turned on
- This version's custom palette sharing is incompatible with BBCFIM version 1.23

28-11-2017 -- version 1.23
===============================
- Added CustomPalettes option (HIGHLY EXPERIMENTAL! Makes it possible to see each other's custom color palettes)

19-06-2017 -- version 1.22
===============================
- Rewritten certain parts of the code to eliminate any possible input lag, and fix some controller incompatibilities

12-06-2017 -- version 1.21
===============================
- Fixed blackscreen issue that occurred when RenderingWidth and RenderingHeight were set above 1920x1152

08-06-2017 -- version 1.20
===============================
- Added Regionlock options
- Added Viewport options
- Fixed the misbehaving texture filter
- Fixed background sign being misaligned on the stage Duel Field

07-06-2017 -- version 1.11
===============================
- Adjusted D-card and Chat text placements some more

06-06-2017 -- version 1.10
===============================
- Fixed D-card and Chat text placements

28-05-2017 -- version 1.00
===============================
- Initial release


Special thanks to
==========================
corpse warblade
Dormin
Euchre
NeoStrayCat
Rouzel
SegGel2009
Everybody in the BlazBlue PC community that has given support or feedback of any kind!