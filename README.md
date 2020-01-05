# BBCF-Improvement-Mod
Additional features and tweaks for BlazBlue Centralfiction

The goal is to extend the game with community-created content, provide additional graphical options, and improve performance for players with computer specs below the minimal system requirements.

Join the [BB Improvement Mod Discord Server](https://discord.gg/j2mCX9s) to discuss about the development and the game itself!

## Info
Due to lack of time and motivation to continue working on BBCF Improvement Mod, I am releasing the source code in hopes of someone improving upon it, and continuing its development.

Right now the quality of the codebase is in a really poor state that needs a lot of rework both structurally and logically, most of which has been done and improved in BBTAG Improvement Mod.

Along with some terrible implementation decisions (like palette data stored in std strings), there is a lot of boilerplate and messy code that needs to be cleaned up.

## Legal
```
BBCF Improvement Mod is NOT associated with Arc System Works or any of its partners / affiliates.
BBCF Improvement Mod is NOT intended for malicious use.
BBCF Improvement Mod is NOT intended to give players unfair advantages in online matches.
BBCF Improvement Mod is NOT intended to unlock unreleased / unpurchased contents of the game.
BBCF Improvement Mod should only be used on the official version that you legally purchased and own.

Use BBCF Improvement Mod at your own risk. 
I, KoviDomi, am not responsible for what happens while using BBCF Improvement Mod. You take full reponsibility for any outcome that happens to you while using this application.
```

## Compiling and usage
BBCF Improvement Mod is coded using Visual Studio 2017 (toolset v141). <br>
To compile, you should only need to load the sln file and compile as-is. No changes should be needed to the solution or source.<br>
Copy the compiled binary, settings.ini, and palettes.ini file from the bin/ folder into the game's root folder.

## Requirements:
- Visual Studio 2017 (toolset v141) (Windows SDK 10.0.17763.0)
- Detours 1.5
- DirectX SDK
- Steamworks SDK (v1.42)


## Support
If you feel like throwing some dollars my way feel free to, it enables me spending more time working on the mod.

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=C8EDR78DJ8PU2)

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=C8EDR78DJ8PU2)

## Thanks to the people who have helped the mod along the way:
* Neptune
* Rouzel
* Dormin
* NeoStrayCat
* Everybody in the BlazBlue PC community that has given support or feedback of any kind!

## Extra thanks to:
Atom0s for their DirectX9.0 Hooking article<br>
Durante for their dsfix source
