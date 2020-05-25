# BBCF-Improvement-Mod-GGPO
A netplay implementation utilizing kovidomi's BBCF Improvment mod for use with the Steam release of Blazblue Central Fiction.

## Table of contents
* [Compiling and usage](#compiling-and-usage)
* [Contributing](#contributing)
* [Authors](#authors)
* [License](#license)
* [External Licenses and Copyright Information](#external-licenses-and-copyright-information)

## Compiling and usage
BBCF Improvement Mod GGPO is coded using Visual Studio 2019. <br>
To compile, you should only need to load the sln file and compile as-is. No changes should be needed to the solution or source.<br>
Copy the compiled binary, settings.ini, and palettes.ini file from the bin/ folder into the game's root folder.

## Requirements:
- Visual Studio 2019
- Detours 1.5
- DirectX SDK
- Steamworks SDK (v1.42)

## Contributing
Contributions of all kinds are welcome!
We're cuurently seeking help with C++ development, x86 assembly, reverse engineering, and networking code, but non-technical skills are also appreciated. Some ways you can contribute include:

* Reading [CONTRIBUTE.md](https://github.com/GrimFlash/BBCF-Improvment-Mod-GGPO/blob/master/CONTRIBUTE.md) and submittting new pull requests
* Updating and organizing the [wiki](https://github.com/GrimFlash/BBCF-Improvment-Mod-GGPO/wiki)
* Testing early-access reases and reporting [issues](https://github.com/GrimFlash/BBCF-Improvment-Mod-GGPO/issues/new) on the issue tracker
* Spreading the word and recruiting new players and contributors!

## Why GGPO? Why BBCF? Why now?
While lockstep netcode was considered industry standard for many years
(including triple-A titles like _Street Fighter IV_), its failures have
been severe and widespread (_Ultimate Marvel vs. Capcom 3_ and _King of
Fighters XIII_).

In the last decade, many prominent fighting-game developers have documented
the negative player effects that lockstep netcode and its necessary input
latency create. Instead, developers and community members alike have
espoused the virtues of rollback-based netcode, including
[Mike "MikeZ" Zaimont of Lab Zero and Skullgirls](https://mikezsez.blogspot.com/2019/11/lets-talk-about-rollbacks.html),
[Michael Stallone of Netherrealm and Mortal Kombat](https://youtu.be/7jb0FOcImdg),
and [Adam "Keits" Heart of Iron Galaxy and Killer Instinct](https://twitter.com/thekeits/status/1143897723848003584?lang=en).

BBCF uses an engine that was built on lockstep netcode.
While in some cases its netcode can hold up relatively well, playing with
community members from across a country (or across an ocean!) is a
poor experience. Fortunately, thanks to GGPO's open-source release in October
2019, adapting a game to rollback-based netcode no longer requires modifying
the game _and_ writing an entire netcode implementation. This opens up the
possibility for fighting games new and old to be brought up to a new gold
standard- including this project.

## Authors

Authors

* @GrimFlash - founder

## License
This project is licensed under the MIT License -
see the [LICENSE](https://github.com/GrimFlash/BBCF-Improvment-Mod-GGPO/blob/master/LICENSE) file for details.

## External Licenses and Copyright Information

Blazblue, Blazblue Central Fiction and all related software
Copyright © Arc System Works.

Steam
Copyright © Valve Corporation.

Detours
Copyright © Microsoft Corporation.

GGPO (Good Game Peace Out)
Copyright © Groundstorm Studios, LLC.

Dear Imgui
Copyright © Omar Cornut
