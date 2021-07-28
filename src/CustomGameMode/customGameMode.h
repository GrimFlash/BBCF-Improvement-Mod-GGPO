#pragma once

#include <string>
#include <vector>

extern float steroid_hp_multiplier;
extern float steroid_overdriveChargeMultiplier;
extern int steroid_maximumHeat;
extern float steroid_heatModifyMultiplier;
extern float steroid_heatLowHPChargeMultiplier;
extern float vampire_healing_percent;
extern float vampire_health_loss_percent_per_sec;
extern float exVampire_healing_percent;
extern float exVampire_health_loss_percent_per_sec;

enum CustomGameMode
{
	CustomGameMode_None,
	CustomGameMode_Steroid,
	CustomGameMode_Vampire,
	CustomGameMode_ExVampire,
	CustomGameMode_OnePunch,
	CustomGameMode_TwoPunch,
	CustomGameMode_FivePunch,
	CustomGameMode_TugOfWar,
	CustomGameMode_InfiniteHeat
};

struct GameMode_t
{
	std::string name;
	int id;
	std::string desc;
	void (*init_func)();
	std::vector<std::string> hook_labels;
};

std::vector<GameMode_t> CreateGameModesVector();
