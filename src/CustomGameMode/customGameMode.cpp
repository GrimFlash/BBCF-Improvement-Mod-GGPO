#include "customGameMode.h"

#include "Core/interfaces.h"
#include "Core/logger.h"
#include "Hooks/HookManager.h"
#include "Hooks/hooks_customGameModes.h"
#include "Overlay/Logger/ImGuiLogger.h"

float steroid_hp_multiplier = 2.5f;
float steroid_overdriveChargeMultiplier = 2.0f;
int steroid_maximumHeat = 15000;
float steroid_heatModifyMultiplier = 1.2f;
float steroid_heatLowHPChargeMultiplier = 2.0f;
float vampire_healing_percent = 0.60f; //60%
float vampire_health_loss_percent_per_sec = 0.01f; // 1%
float exVampire_healing_percent = 2.0f; //200%
float exVampire_health_loss_percent_per_sec = 0.015f; // 1.5%

void InitNormalMode()
{
	// Empty function on purpose
}

void InitSteroidMode()
{
	LOG(2, "InitSteroidMode\n");
	g_imGuiLogger->Log("[system] Starting Steroid Mode\n");

	g_interfaces.player1.GetData()->maxHP *= steroid_hp_multiplier;
	g_interfaces.player2.GetData()->maxHP *= steroid_hp_multiplier;

	g_interfaces.player1.GetData()->currentHP *= steroid_hp_multiplier;
	g_interfaces.player2.GetData()->currentHP *= steroid_hp_multiplier;

	// Change heat limits from 10000 to 15000
	int result = HookManager::OverWriteBytes(
		(char*)steroid_HeatModifyJmpBackAddr,
		(char*)steroid_HeatModifyJmpBackAddr + 0x28,
		"\x10\x27\x00\x00",
		"xxxx",
		"\x98\x3A\x00\x00");

	if (result)
	{
		LOG(2, "Modified steroid_HeatModify, overwritten bytes: %d\n", result);
	}
}

void InitVampireMode()
{
	LOG(2, "InitVampireMode\n");
	g_imGuiLogger->Log("[system] Starting Vampire Mode\n");
}

void InitExVampireMode()
{
	LOG(2, "InitExVampireMode\n");
	g_imGuiLogger->Log("[system] Starting exVampire Mode\n");
}

void InitOnePunchMode()
{
	LOG(2, "InitOnePunchMode\n");
	g_imGuiLogger->Log("[system] Starting One Punch Mode\n");

	CharData& player1 = *g_interfaces.player1.GetData();
	CharData& player2 = *g_interfaces.player2.GetData();

	*g_gameVals.pMatchRounds = 5;
	*g_gameVals.pMatchTimer = 60 * 15;

	// New round, recharge 50% of the burst
	player1.overdriveMeter += 50000;
	if (player1.overdriveMeter > 100000)
		player1.overdriveMeter = 100000;

	player2.overdriveMeter += 50000;
	if (player2.overdriveMeter > 100000)
		player2.overdriveMeter = 100000;
}

void InitTwoPunchMode()
{
	LOG(2, "InitTwoPunchMode\n");
	g_imGuiLogger->Log("[system] Starting Two Punch Mode\n");

	CharData& player1 = *g_interfaces.player1.GetData();
	CharData& player2 = *g_interfaces.player2.GetData();

	*g_gameVals.pMatchRounds = 5;
	*g_gameVals.pMatchTimer = 60 * 15;

	// New round, recharge 50% of the burst
	player1.overdriveMeter += 50000;
	if (player1.overdriveMeter > 100000)
		player1.overdriveMeter = 100000;

	player2.overdriveMeter += 50000;
	if (player2.overdriveMeter > 100000)
		player2.overdriveMeter = 100000;
}

void InitFivePunchMode()
{
	LOG(2, "InitFivePunchMode\n");
	g_imGuiLogger->Log("[system] Starting Five Punch Mode\n");

	CharData& player1 = *g_interfaces.player1.GetData();
	CharData& player2 = *g_interfaces.player2.GetData();

	*g_gameVals.pMatchRounds = 5;
	*g_gameVals.pMatchTimer = 60 * 60;

	// New round, recharge 50% of the burst
	player1.overdriveMeter += 50000;
	if (player1.overdriveMeter > 100000)
		player1.overdriveMeter = 100000;

	player2.overdriveMeter += 50000;
	if (player2.overdriveMeter > 100000)
		player2.overdriveMeter = 100000;
}

void InitTugOfWar()
{
	LOG(2, "InitTugOfWarMode\n");
	g_imGuiLogger->Log("[system] Starting Tug of War Mode\n");

	CharData& player1 = *g_interfaces.player1.GetData();
	CharData& player2 = *g_interfaces.player2.GetData();

	*g_gameVals.pMatchRounds = 5;
	*g_gameVals.pMatchTimer = 60 * 60;

	// New round, recharge 50% of the burst
	player1.overdriveMeter += 50000;
	if (player1.overdriveMeter > 100000)
		player1.overdriveMeter = 100000;

	player2.overdriveMeter += 50000;
	if (player2.overdriveMeter > 100000)
		player2.overdriveMeter = 100000;
}

void InitInfiniteHeatMode()
{
	LOG(2, "InitInfiniteHeatMode\n");
	g_imGuiLogger->Log("[system] Starting Infinite Heat Mode\n");

	g_interfaces.player1.GetData()->heatMeter = 10000;
	g_interfaces.player2.GetData()->heatMeter = 10000;
}

std::vector<GameMode_t> CreateGameModesVector()
{
	LOG(2, "CreateGameModesVector\n");

	std::vector<GameMode_t> gameModes =
	{
		{
			"Normal",
			CustomGameMode_None,
			"",
			InitNormalMode
		},
		{
			"Steroid",
			CustomGameMode_Steroid,
			"- Max HP: +150%\n- Max Heat Gauge: +50%\n- Heat gain: +20%\n"\
			"- Automatic Heat gain on low HP: +100%\n- Burst Gauge gain: +100%",
			InitSteroidMode,
			{ "steroid_OverdriveCharge", "steroid_HeatModify", "steroid_HealthModify" }
		},
		{
			"Vampire",
			CustomGameMode_Vampire,
			"- 60% of damage dealt is healed back\n- Losing 1% max HP per second",
			InitVampireMode,
			{"vampire_HealthModify", "vampire_HealthDrain"}
		},
		{
			"Extreme Vampire",
			CustomGameMode_ExVampire,
			"- 200% of damage dealt is healed back\n- Healing beyond full HP increases max HP\n" \
			"- Losing 1.5% max HP per second",
			InitExVampireMode,
			{"exVampire_HealthModify", "exVampire_HealthDrain"}
		},
		{
			"One Punch",
			CustomGameMode_OnePunch,
			"- Any damage results in instakill, unless blocked with barrier\n" \
			"- Each round 50% of the Burst Gauge is regenerated\n- Each round lasts 15 seconds\n- 5 rounds",
			InitOnePunchMode,
			{"onepunch_HealthModify", "training_healthModifyFix"}
		},
		{
			"Two Punch",
			CustomGameMode_TwoPunch,
			"- Taking damage twice results in death, unless blocked with barrier\n" \
			"- Each round 50% of the Burst Gauge is regenerated\n- Each round lasts 15 seconds\n- 5 rounds",
			InitTwoPunchMode,
			{"twopunch_HealthModify", "training_healthModifyFix"}
		},
		{
			"Five Punch",
			CustomGameMode_FivePunch,
			"- Taking damage five times results in death, unless blocked with barrier\n" \
			"- Each round 50% of the Burst Gauge is regenerated\n- Each round lasts 60 seconds\n- 5 rounds",
			InitFivePunchMode,
			{"fivepunch_HealthModify", "training_healthModifyFix"}
		},
		{
			"Tug of War",
			CustomGameMode_TugOfWar,
			"- All damages hurt/heal 10% HP, unless blocked with barrier\n" \
			"- Each round 50% of the Burst Gauge is regenerated\n- Each round lasts 60 seconds\n- 5 rounds",
			InitTugOfWar,
			{"tugofwar_HealthModify", "training_healthModifyFix"}
		},
		{
			"Infinite Heat",
			CustomGameMode_InfiniteHeat,
			"- Heat Gauge is always full",
			InitInfiniteHeatMode,
			{"infiniteheat_HeatModify"}
		},
	};

	return gameModes;
}