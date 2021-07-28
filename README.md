# BBCF-Improvement-Mod-GGPO
A netplay implementation utilizing kovidomi's BBCF Improvement mod for use with the Steam release of Blazblue Central Fiction.

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

## Contributing
Contributions of all kinds are welcome!
We're cuurently seeking help with C++ development, x86 assembly, reverse engineering, and networking code, but non-technical skills are also appreciated. Some ways you can contribute include:

* Reading [CONTRIBUTE.md](https://github.com/GrimFlash/BBCF-Improvment-Mod-GGPO/blob/master/CONTRIBUTE.md) and submittting new pull requests
* Updating and organizing the [wiki](https://github.com/GrimFlash/BBCF-Improvment-Mod-GGPO/wiki)
* Testing early-access reases and reporting [issues](https://github.com/GrimFlash/BBCF-Improvment-Mod-GGPO/issues/new) on the issue tracker
* Spreading the word and recruiting new players and contributors!
* Joining the [Discord](https://discord.gg/MnTR3Qp) and chatting with us

## Why GGPO? Why BBCF? Why now?
While lockstep netcode was considered industry standard for many years
(including titles such as Street Fighter IV), its flaws have become more omnipresent throughout the years.

In the last decade, numerous fighting-game developers have documented
the negative player effects that lockstep netcode and its necessary input
latency create. Instead, developers and community members alike have
espoused the virtues of rollback-based netcode, including
[Michael Stallone of Netherrealm and Mortal Kombat](https://youtu.be/7jb0FOcImdg),
and [Adam "Keits" Heart of Iron Galaxy and Killer Instinct](https://www.youtube.com/watch?v=1RI5scXYhK0).

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
* DaInfamoose
* JoeS

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
