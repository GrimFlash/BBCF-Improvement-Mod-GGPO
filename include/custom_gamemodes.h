#pragma once
#include <vector>

struct GameMode_t;
extern int activatedGameMode;
extern int opponent_activatedGameMode;
extern int P1_activatedGameMode;
extern int P2_activatedGameMode;
extern std::vector<GameMode_t> GameModes;

enum customGameModes
{
	customGameMode_none,
	customGameMode_steroid,
	customGameMode_vampire,
	customGameMode_ex_vampire,
	customGameMode_onepunch,
	customGameMode_twopunch,
	customGameMode_fivepunch,
	customGameMode_tugofwar,
	customGameMode_infiniteheat,
	customGameMode_overdrive,
};

struct GameMode_t
{
	char* name;
	int id;
	char* desc;
	void (*init_func)();
	std::vector<char*> hook_labels;
};

void CustomGamemodeHooks();
void SetCustomGamemode(int mode);
void InitCustomGamemode();
void EndCustomGamemode();