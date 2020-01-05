#include "../include/custom_gamemodes.h"
#include "../include/containers.h"
#include "../include/hook_manager.h"
#include "../include/gamestates_defines.h"
#include "../include/ImGui/ImGuiSystem.h"
#include <imgui.h>

std::vector<GameMode_t> GameModes = std::vector<GameMode_t>();
int activatedGameMode = customGameMode_none;
int opponent_activatedGameMode = customGameMode_none;
int P1_activatedGameMode = customGameMode_none;
int P2_activatedGameMode = customGameMode_none;

float steroid_hp_multiplier = 2.5;
float steroid_overdriveChargeMultiplier = 2.0;
int steroid_maximumHeat = 15000;
float steroid_heatModifyMultiplier = 1.2;
float steroid_heatLowHPChargeMultiplier = 2.0;
float vampire_healing_percent = 0.60; //60%
float vampire_health_loss_percent_per_sec = 0.01; // 1%
float exVampire_healing_percent = 2; //200%
float exVampire_health_loss_percent_per_sec = 0.015; // 1.5%

void ActivateCustomGameMode();
void ResetAllHooks();
void InitSteroidMode();
void steroid_HealthModify();
void steroid_HeatModify();
void steroid_OverdriveCharge();
void InitVampireMode();
void vampire_HealthDrain();
void vampire_HealthModify();
void InitExVampireMode();
void exVampire_HealthDrain();
void exVampire_HealthModify();
void InitOnePunchMode();
void onepunch_HealthModify();
void InitTwoPunchMode();
void twopunch_HealthModify();
void InitFivePunchMode();
void fivepunch_HealthModify();
void InitTugOfWar();
void tugofwar_HealthModify();
void InitInfiniteHeatMode();
void infiniteheat_HeatModify();
void InitOverdriveMode();
void overdrive_FreezeOverdriveTimeleft();
void overdrive_KeepTimerGoing();

DWORD steroid_OverdriveChargeJmpBackAddr = 0;
DWORD steroid_HeatModifyJmpBackAddr = 0;
DWORD steroid_HealthModifyJmpBackAddr = 0;
DWORD vampire_HealthModifyJmpBackAddr = 0;
DWORD vampire_HealthDrainJmpBackAddr = 0;
DWORD exVampire_HealthModifyJmpBackAddr = 0;
DWORD exVampire_HealthDrainJmpBackAddr = 0;
DWORD onepunch_HealthModifyJmpBackAddr = 0;
DWORD twopunch_HealthModifyJmpBackAddr = 0;
DWORD fivepunch_HealthModifyJmpBackAddr = 0;
DWORD tugofwar_HealthModifyJmpBackAddr = 0;
DWORD infiniteheat_HeatModifyJmpBackAddr = 0;
DWORD overdrive_FreezeOverdriveTimeleftJmpBackAddr = 0;
DWORD overdrive_KeepTimerGoingJmpBackAddr = 0;


void SetCustomGamemode(int mode)
{
	LOG(2, "SetCustomGamemode\n");
	activatedGameMode = mode;
}

void ActivateCustomGameMode()
{
	LOG(2, "ActivateCustomGameMode\n");

	ResetAllHooks(); //making sure we dont leave anything turned on

	//if we are spectators, set up activatedgamemode
	if (Containers::gameVals.thisPlayerNum == 0)
		activatedGameMode = P1_activatedGameMode;

	if (activatedGameMode < GameModes.size())
		GameModes[activatedGameMode].init_func();
	else
		EndCustomGamemode();
}

void InitCustomGamemode()
{
	LOG(2, "InitCustomGamemode\n");
	//if (activatedGameMode != customGameMode_none)
	//	InitIngamePointers();

	if (Containers::gameVals.P1CharObjPointer == 0 || Containers::gameVals.P2CharObjPointer == 0) //|| customGameModeInitialized)
		return;

	if (*Containers::gameVals.pGameMode == GAME_MODE_ONLINE) //online
	{
		if (activatedGameMode == opponent_activatedGameMode || P1_activatedGameMode == P2_activatedGameMode) // only run in online, and if opponent has selected the same mode as us
		{
			ActivateCustomGameMode();
		}
		else
		{
			ImGuiSystem::AddLog("[system] Players couldn't agree on the same game mode. Resetting back to 'Normal'\n");
			EndCustomGamemode();
		}
	}
	else //offline
	{
		ActivateCustomGameMode();
	}
}

void ResetAllHooks()
{
	LOG(2, "ResetAllHooks\n");

	for (int i = 0; i < GameModes.size(); i++)
		for (int j = 0; j < GameModes[i].hook_labels.size(); j++)
			HookManager::DeactivateHook(GameModes[i].hook_labels[j]);

	int result = HookManager::OverWriteBytes((char*)steroid_HeatModifyJmpBackAddr, (char*)steroid_HeatModifyJmpBackAddr + 0x28, "\x98\x3A\x00\x00", "xxxx", "\x10\x27\x00\x00"); //change heat limits of 15000 back to 10000
#ifndef RELEASE_VER
	if (result != 3)
		LOG(2, "Error, steroid_HeatModify OverWriteBytes didnt reset all 3 addresses, only %d!!\n", result);
#endif // !RELEASE_VER
}

void EndCustomGamemode()
{
	LOG(2, "EndCustomGamemode\n");
	activatedGameMode = customGameMode_none;
	opponent_activatedGameMode = customGameMode_none;
	P1_activatedGameMode = customGameMode_none;
	P2_activatedGameMode = customGameMode_none;
	ResetAllHooks();
}

void InitSteroidMode()
{
	LOG(2, "InitSteroidMode\n");
	ImGuiSystem::AddLog("[system] Starting Steroid Mode\n");

	Containers::gameVals.P1CharObjPointer->maxHP *= steroid_hp_multiplier;
	Containers::gameVals.P2CharObjPointer->maxHP *= steroid_hp_multiplier;
	Containers::gameVals.P1CharObjPointer->currentHP *= steroid_hp_multiplier;
	Containers::gameVals.P2CharObjPointer->currentHP *= steroid_hp_multiplier;

	HookManager::ActivateHook("steroid_OverdriveCharge");
	HookManager::ActivateHook("steroid_HeatModify");
	HookManager::ActivateHook("steroid_HealthModify");

	int result = HookManager::OverWriteBytes((char*)steroid_HeatModifyJmpBackAddr, (char*)steroid_HeatModifyJmpBackAddr + 0x28, "\x10\x27\x00\x00", "xxxx", "\x98\x3A\x00\x00"); //change heat limits of 10000 to 15000
#ifndef RELEASE_VER
	if (result != 3)
		LOG(2, "Error, steroid_HeatModify OverWriteBytes didnt reset all 3 addresses!!\n");
#endif // !RELEASE_VER
}

void __declspec(naked)steroid_OverdriveCharge()
{
#ifndef RELEASE_VER
	__asm pushad
	LOG(7, "steroid_OverdriveCharge\n");
	__asm popad
#endif // !RELEASE_VER

	static int addedOverdriveValue = 0;

	__asm
	{
		cmp esi, 7530h //do not change the value of 30000 if OD finisher hits
		je EXIT
		mov[addedOverdriveValue], esi
	}

	__asm pushad
	addedOverdriveValue *= steroid_overdriveChargeMultiplier;
	__asm popad

	__asm
	{
		mov esi, [addedOverdriveValue]
EXIT:
		add[edi + 5AE4h], esi
		jmp[steroid_OverdriveChargeJmpBackAddr]
	}
}

void __declspec(naked)steroid_HeatModify()
{
#ifndef RELEASE_VER
	__asm pushad
	LOG(7, "steroid_HeatModify\n");
	__asm popad
#endif // !RELEASE_VER

	static int addedHeatValue = 0;
	_asm
	{
		mov[addedHeatValue], esi
		cmp esi, 0
		jl EXIT
		cmp esi, 1
		je LOW_HP_HEAT_GENERATION
		cmp esi, 2
		je HAKUMEN_AIR_HEAT_GENERATION
		cmp esi, 4
		je HAKUMEN_GROUND_HEAT_GENERATION
	}

	__asm pushad
	addedHeatValue *= steroid_heatModifyMultiplier;
	__asm popad

	__asm
	{
		jmp EXIT
LOW_HP_HEAT_GENERATION:
	}

	__asm pushad
	addedHeatValue *= steroid_heatLowHPChargeMultiplier;
	__asm popad

	__asm
	{
		jmp EXIT
HAKUMEN_AIR_HEAT_GENERATION:
		mov[addedHeatValue], 3
		jmp EXIT
	}

	__asm
	{
		jmp EXIT
HAKUMEN_GROUND_HEAT_GENERATION:
		mov [addedHeatValue], 5
	}

	__asm
	{
EXIT:
		mov esi, [addedHeatValue]
		add[ebx + 5AD4h], esi
		jmp[steroid_HeatModifyJmpBackAddr]
	}
}

void __declspec(naked)steroid_HealthModify()
{
#ifndef RELEASE_VER
	__asm pushad
	LOG(7, "steroid_HealthModify\n");
	__asm popad
#endif // !RELEASE_VER

	static int newHP = 0;
	static int previousHP = 0;
	static int maxHP = 0;

	__asm pushad
	__asm
	{
		mov[newHP], eax
		mov edx, [ecx + 9D0h] //previous hp addr
		mov [previousHP], edx
		mov edx, [ecx + 9D8h] //max hp addr
		mov [maxHP], edx
	}

	//if (newHP > previousHP) // we healed (like ragna for example)
	//	newHP += (newHP - previousHP) * steroid_hp_multiplier;
	if (newHP > maxHP)
		newHP = maxHP;
	__asm popad

	__asm
	{
		mov eax, [newHP]
		mov[ecx + 9D4h], eax //current hp in eax
		jmp[steroid_HealthModifyJmpBackAddr]
	}
}

void InitVampireMode()
{
	LOG(2, "InitVampireMode\n");
	ImGuiSystem::AddLog("[system] Starting Vampire Mode\n");

	HookManager::ActivateHook("vampire_HealthModify");
	HookManager::ActivateHook("vampire_HealthDrain");
}

void __declspec(naked)vampire_HealthModify()
{
	//handles the lifesteal for all chars
#ifndef RELEASE_VER
	__asm pushad
	LOG(7, "vampire_HealthModify\n");
	__asm popad
#endif // !RELEASE_VER

	static int newHP = 0;
	//static int previousHP = 0;
	static int opponentHealing = 0;
	static CChar* thisPlayerObj = 0;
	static CChar* otherPlayerObj = 0;

	__asm
	{
		pushad
		mov [newHP], eax
		//mov edx, [ecx + 9D0h] //previous hp addr
		//mov [previousHP], edx <- decided to simply use thisplayerObj instead
		mov [thisPlayerObj], ecx
	}

	if (*Containers::gameVals.pMatchState == MATCH_STATE_FIGHT && newHP < thisPlayerObj->previousHP) // only if we took damage. avoiding running on healing (like ragna for example)
	{
		//LOG(2, "Vampire: \n");
		if (thisPlayerObj != Containers::gameVals.P1CharObjPointer && thisPlayerObj != Containers::gameVals.P2CharObjPointer)
		{
			//LOG(2, "NEITHER PLAYER OBJECTS!\n");
			__asm
			{
				jmp EXIT
			}
		}

		opponentHealing = (thisPlayerObj->previousHP - newHP) * vampire_healing_percent;

		if (thisPlayerObj == Containers::gameVals.P1CharObjPointer)
		{
			//LOG(2, "\tP1 took %d damage\nHealing P2 with %d HP...:\n", (thisPlayerObj->previousHP - newHP), opponentHealing);
			otherPlayerObj = Containers::gameVals.P2CharObjPointer;
			//LOG(2, "\tP2 currentHP: %d\n", otherPlayerObj->currentHP);
			//LOG(2, "\tP2 maxHP: %d\n", otherPlayerObj->maxHP);
			//LOG(2, "\tP2 ")
		}
		else
		{
			//LOG(2, "\tP2 took %d damage\nHealing P1 with %d HP...:\n", (thisPlayerObj->previousHP - newHP), opponentHealing);
			otherPlayerObj = Containers::gameVals.P1CharObjPointer;
			//LOG(2, "\tP1 currentHP: %d\n", otherPlayerObj->currentHP);
			//LOG(2, "\tP1 maxHP: %d\n", otherPlayerObj->maxHP);
			//LOG(2, "\tP1 ");
		}

		//LOG(2, "Healing: ((previousHP (%d) - newHP (%d)) * vampire_healing(%.3f)) = %d\n", thisPlayerObj->previousHP, newHP, vampire_healing_percent, opponentHealing);
		//if (thisPlayerObj->previousHP > thisPlayerObj->maxHP)
		//	LOG(2, "\tERROR, PREVIOUS HP TOO HIGH!\n");

		otherPlayerObj->currentHP += opponentHealing;
		//LOG(2, "\tafter healing: %d\n", otherPlayerObj->currentHP);
		if (otherPlayerObj->currentHP > otherPlayerObj->maxHP)
			otherPlayerObj->currentHP = otherPlayerObj->maxHP;

		//LOG(2, "\tsetting previousHP from: %d\n", otherPlayerObj->previousHP);
		otherPlayerObj->previousHP = otherPlayerObj->currentHP;
		thisPlayerObj->previousHP = newHP;
		//LOG(2, "\tsetting previousHP to: %d\n", otherPlayerObj->previousHP);
	}

	__asm
	{
EXIT:
		popad
		mov[ecx + 9D4h], eax //current hp in eax
		jmp[vampire_HealthModifyJmpBackAddr]
	}
}

void __declspec(naked)vampire_HealthDrain()
{
	//handles the % hp loss per second
#ifndef RELEASE_VER
	__asm pushad
	LOG(7, "vampire_HealthDrain\n");
	__asm popad
#endif // !RELEASE_VER

	static float vampirism_timer = 0.0;
	static int previous_real_timer = 0;

	__asm pushad

	if (*Containers::gameVals.pMatchState == MATCH_STATE_FIGHT) //on match only
	{
		if (*Containers::gameVals.pGameMode == GAME_MODE_TRAINING || *Containers::gameVals.pMatchTimer == 5939) //if we are in training mode, calculate it a different way, since we have no match timer
		{
			//do not reduce hp while distortions / od finishers / astrals are happening:
			if(!Containers::gameVals.P1CharObjPointer->is_doing_distortion && 
				!Containers::gameVals.P2CharObjPointer->is_doing_distortion)
				vampirism_timer += ImGui::GetIO().DeltaTime;
		}
		else
		{
			//uninitialized value or new match started:
			if (previous_real_timer == 0 || previous_real_timer < *Containers::gameVals.pMatchTimer)
				previous_real_timer = *Containers::gameVals.pMatchTimer;

			vampirism_timer += (previous_real_timer - *Containers::gameVals.pMatchTimer);
			previous_real_timer = *Containers::gameVals.pMatchTimer;
		}

		if (vampirism_timer > 60) // 1 sec
		{
			//LOG(2, "!Vampire taking 1sec damage\n");
			int P1_lostHP = Containers::gameVals.P1CharObjPointer->maxHP * vampire_health_loss_percent_per_sec;
			int P2_lostHP = Containers::gameVals.P2CharObjPointer->maxHP * vampire_health_loss_percent_per_sec;
			Containers::gameVals.P1CharObjPointer->currentHP -= P1_lostHP;
			Containers::gameVals.P2CharObjPointer->currentHP -= P2_lostHP;
			if (Containers::gameVals.P1CharObjPointer->currentHP < 1) //prevent dying of this effect
				Containers::gameVals.P1CharObjPointer->currentHP = 1;
			if (Containers::gameVals.P2CharObjPointer->currentHP < 1) //prevent dying of this effect
				Containers::gameVals.P2CharObjPointer->currentHP = 1;

			Containers::gameVals.P1CharObjPointer->previousHP = Containers::gameVals.P1CharObjPointer->currentHP;
			Containers::gameVals.P2CharObjPointer->previousHP = Containers::gameVals.P2CharObjPointer->currentHP;

			vampirism_timer = 0.0;
		}
	}
	__asm popad

	__asm
	{
		mov ebx, [esi + 9D4h]
		jmp[vampire_HealthDrainJmpBackAddr]
	}
}

void InitExVampireMode()
{
	LOG(2, "InitExVampireMode\n");
	ImGuiSystem::AddLog("[system] Starting exVampire Mode\n");

	HookManager::ActivateHook("exVampire_HealthModify");
	HookManager::ActivateHook("exVampire_HealthDrain");
}

void __declspec(naked)exVampire_HealthModify()
{
	//handles the lifesteal for all chars
#ifndef RELEASE_VER
	__asm pushad
	LOG(7, "exVampire_HealthModify\n");
	__asm popad
#endif // !RELEASE_VER

	static int newHP = 0;
	//static int previousHP = 0;
	static int opponentHealing = 0;
	static CChar* thisPlayerObj = 0;
	static CChar* otherPlayerObj = 0;

	__asm
	{
		pushad
		mov[newHP], eax
		//mov edx, [ecx + 9D0h] //previous hp addr
		//mov [previousHP], edx <- decided to simply use thisplayerObj instead
		mov[thisPlayerObj], ecx
	}

	if (*Containers::gameVals.pMatchState == MATCH_STATE_FIGHT && newHP < thisPlayerObj->previousHP) // only if we took damage. avoiding running on healing (like ragna for example)
	{
		//LOG(2, "exVampire: \n");
		if (thisPlayerObj != Containers::gameVals.P1CharObjPointer && thisPlayerObj != Containers::gameVals.P2CharObjPointer)
		{
			//LOG(2, "NEITHER PLAYER OBJECTS!\n");
			__asm
			{
				jmp EXIT
			}
		}

		opponentHealing = (thisPlayerObj->previousHP - newHP) * exVampire_healing_percent;

		if (thisPlayerObj == Containers::gameVals.P1CharObjPointer)
		{
			//LOG(2, "\tP1 took %d damage\nHealing P2 with %d HP...:\n", (thisPlayerObj->previousHP - newHP), opponentHealing);
			otherPlayerObj = Containers::gameVals.P2CharObjPointer;
			//LOG(2, "\tP2 currentHP: %d\n", otherPlayerObj->currentHP);
			//LOG(2, "\tP2 maxHP: %d\n", otherPlayerObj->maxHP);
			//LOG(2, "\tP2 ")
		}
		else
		{
			//LOG(2, "\tP2 took %d damage\nHealing P1 with %d HP...:\n", (thisPlayerObj->previousHP - newHP), opponentHealing);
			otherPlayerObj = Containers::gameVals.P1CharObjPointer;
			//LOG(2, "\tP1 currentHP: %d\n", otherPlayerObj->currentHP);
			//LOG(2, "\tP1 maxHP: %d\n", otherPlayerObj->maxHP);
			//LOG(2, "\tP1 ");
		}

		//LOG(2, "Healing: ((previousHP (%d) - newHP (%d)) * exVampire_healing(%.3f)) = %d\n", thisPlayerObj->previousHP, newHP, exVampire_healing_percent, opponentHealing);
		//if (thisPlayerObj->previousHP > thisPlayerObj->maxHP)
		//	LOG(2, "\tERROR, PREVIOUS HP TOO HIGH!\n");

		otherPlayerObj->currentHP += opponentHealing;
		//LOG(2, "\tafter healing: %d\n", otherPlayerObj->currentHP);
		if (otherPlayerObj->currentHP > otherPlayerObj->maxHP)
			otherPlayerObj->maxHP = otherPlayerObj->currentHP;

		//LOG(2, "\tsetting previousHP from: %d\n", otherPlayerObj->previousHP);
		otherPlayerObj->previousHP = otherPlayerObj->currentHP;
		thisPlayerObj->previousHP = newHP;
		//LOG(2, "\tsetting previousHP to: %d\n", otherPlayerObj->previousHP);
	}

	__asm
	{
	EXIT:
		popad
		mov [ecx + 9D4h], eax //current hp in eax
		jmp [exVampire_HealthModifyJmpBackAddr]
	}
}

void __declspec(naked)exVampire_HealthDrain()
{
	//handles the % hp loss per second
#ifndef RELEASE_VER
	__asm pushad
	LOG(7, "exVampire_HealthDrain\n");
	__asm popad
#endif // !RELEASE_VER

	static float vampirism_timer = 0.0;
	static int previous_real_timer = 0;

	__asm pushad

	if (*Containers::gameVals.pMatchState == MATCH_STATE_FIGHT) //on match only
	{
		if (*Containers::gameVals.pGameMode == GAME_MODE_TRAINING || *Containers::gameVals.pMatchTimer == 5939) //if we are in training mode, calculate it a different way, since we have no match timer. pMatchTimer is set to 5939 when infinite timer
		{
			//do not reduce hp while distortions / od finishers / astrals are happening:
			if (!Containers::gameVals.P1CharObjPointer->is_doing_distortion &&
				!Containers::gameVals.P2CharObjPointer->is_doing_distortion)
				vampirism_timer += ImGui::GetIO().DeltaTime;
		}
		else
		{
			//uninitialized value or new match started:
			if (previous_real_timer == 0 || previous_real_timer < *Containers::gameVals.pMatchTimer)
				previous_real_timer = *Containers::gameVals.pMatchTimer;

			vampirism_timer += (previous_real_timer - *Containers::gameVals.pMatchTimer);
			previous_real_timer = *Containers::gameVals.pMatchTimer;
		}

		if (vampirism_timer > 60) // 1 sec
		{
			//LOG(2, "!exVampire taking 1sec damage\n");
			int P1_lostHP = Containers::gameVals.P1CharObjPointer->maxHP * exVampire_health_loss_percent_per_sec;
			int P2_lostHP = Containers::gameVals.P2CharObjPointer->maxHP * exVampire_health_loss_percent_per_sec;
			Containers::gameVals.P1CharObjPointer->currentHP -= P1_lostHP;
			Containers::gameVals.P2CharObjPointer->currentHP -= P2_lostHP;
			if (Containers::gameVals.P1CharObjPointer->currentHP < 1) //prevent dying of this effect
				Containers::gameVals.P1CharObjPointer->currentHP = 1;
			if (Containers::gameVals.P2CharObjPointer->currentHP < 1) //prevent dying of this effect
				Containers::gameVals.P2CharObjPointer->currentHP = 1;

			Containers::gameVals.P1CharObjPointer->previousHP = Containers::gameVals.P1CharObjPointer->currentHP;
			Containers::gameVals.P2CharObjPointer->previousHP = Containers::gameVals.P2CharObjPointer->currentHP;

			vampirism_timer = 0.0;
		}
	}
	__asm popad

	__asm
	{
		mov ebx, [esi + 9D4h]
		jmp[exVampire_HealthDrainJmpBackAddr]
	}
}

void InitOnePunchMode()
{
	LOG(2, "InitOnePunchMode\n");
	ImGuiSystem::AddLog("[system] Starting One Punch Mode\n");

	*Containers::gameVals.pMatchRounds = 5;
	*Containers::gameVals.pMatchTimer = 60 * 15;
	//new round, recharge 50% of the burst
	Containers::gameVals.P1CharObjPointer->overdrive_meter += 50000;
	if (Containers::gameVals.P1CharObjPointer->overdrive_meter > 100000)
		Containers::gameVals.P1CharObjPointer->overdrive_meter = 100000;

	Containers::gameVals.P2CharObjPointer->overdrive_meter += 50000;
	if (Containers::gameVals.P2CharObjPointer->overdrive_meter > 100000)
		Containers::gameVals.P2CharObjPointer->overdrive_meter = 100000;
	HookManager::ActivateHook("onepunch_HealthModify");
}

void __declspec(naked)onepunch_HealthModify()
{
#ifndef RELEASE_VER
	__asm pushad
	LOG(7, "onepunch_HealthModify\n");
	__asm popad
#endif // !RELEASE_VER

	static int newHP = 0;

	__asm mov[newHP], eax
	__asm pushad
	if (*Containers::gameVals.pMatchState == MATCH_STATE_FIGHT) //on match only
		newHP = 0;
	__asm popad

	__asm
	{
		mov eax, [newHP]
		mov[ecx + 9D4h], eax //current hp in eax
		jmp[onepunch_HealthModifyJmpBackAddr]
	}
}

void InitTwoPunchMode()
{
	LOG(2, "InitTwoPunchMode\n");
	ImGuiSystem::AddLog("[system] Starting Two Punch Mode\n");

	*Containers::gameVals.pMatchRounds = 5;
	*Containers::gameVals.pMatchTimer = 60 * 15;
	//new round, recharge 50% of the burst
	Containers::gameVals.P1CharObjPointer->overdrive_meter += 50000;
	if (Containers::gameVals.P1CharObjPointer->overdrive_meter > 100000)
		Containers::gameVals.P1CharObjPointer->overdrive_meter = 100000;

	Containers::gameVals.P2CharObjPointer->overdrive_meter += 50000;
	if (Containers::gameVals.P2CharObjPointer->overdrive_meter > 100000)
		Containers::gameVals.P2CharObjPointer->overdrive_meter = 100000;

	HookManager::ActivateHook("twopunch_HealthModify");
}

void __declspec(naked)twopunch_HealthModify()
{
#ifndef RELEASE_VER
	__asm pushad
	LOG(7, "twopunch_HealthModify\n");
	__asm popad
#endif // !RELEASE_VER

	static int newHP = 0;
	static CChar* thisPlayerObj = 0;

	__asm
	{
		mov[newHP], eax
		mov[thisPlayerObj], ecx
		pushad
	}

	if (*Containers::gameVals.pMatchState == MATCH_STATE_FIGHT) //on match only
	{
		if (thisPlayerObj != Containers::gameVals.P1CharObjPointer && thisPlayerObj != Containers::gameVals.P2CharObjPointer)
		{
			//LOG(2, "NEITHER PLAYER OBJECTS!\n");
			__asm jmp EXIT
		}
		if (newHP > thisPlayerObj->previousHP) // we healed, ignore it.
			newHP = thisPlayerObj->previousHP;
		else //took damage
		{
			newHP = thisPlayerObj->previousHP; //we want to do exactly 50% max hp damage
			newHP -= thisPlayerObj->maxHP / 2;
			thisPlayerObj->previousHP = newHP;
		}
	}

	__asm
	{
EXIT:
		popad
		mov eax, [newHP]
		mov[ecx + 9D4h], eax //current hp in eax
		jmp[twopunch_HealthModifyJmpBackAddr]
	}
}

void InitFivePunchMode()
{
	LOG(2, "InitFivePunchMode\n");
	ImGuiSystem::AddLog("[system] Starting Five Punch Mode\n");

	*Containers::gameVals.pMatchRounds = 5;
	*Containers::gameVals.pMatchTimer = 60 * 60;
	//new round, recharge 50% of the burst
	Containers::gameVals.P1CharObjPointer->overdrive_meter += 50000;
	if (Containers::gameVals.P1CharObjPointer->overdrive_meter > 100000)
		Containers::gameVals.P1CharObjPointer->overdrive_meter = 100000;

	Containers::gameVals.P2CharObjPointer->overdrive_meter += 50000;
	if (Containers::gameVals.P2CharObjPointer->overdrive_meter > 100000)
		Containers::gameVals.P2CharObjPointer->overdrive_meter = 100000;

	HookManager::ActivateHook("fivepunch_HealthModify");
}

void __declspec(naked)fivepunch_HealthModify()
{
#ifndef RELEASE_VER
	__asm pushad
	LOG(7, "fivepunch_HealthModify\n");
	__asm popad
#endif // !RELEASE_VER

	static int newHP = 0;
	static CChar* thisPlayerObj = 0;

	__asm
	{
		mov[newHP], eax
		mov[thisPlayerObj], ecx
		pushad
	}

	if (*Containers::gameVals.pMatchState == MATCH_STATE_FIGHT) //on match only
	{
		if (thisPlayerObj != Containers::gameVals.P1CharObjPointer && thisPlayerObj != Containers::gameVals.P2CharObjPointer)
		{
			//LOG(2, "NEITHER PLAYER OBJECTS!\n");
			__asm jmp EXIT
		}
		if (newHP > thisPlayerObj->previousHP) // we healed, ignore it.
			newHP = thisPlayerObj->previousHP;
		else //took damage
		{
			newHP = thisPlayerObj->previousHP; //we want to do exactly 20% max hp damage
			newHP -= thisPlayerObj->maxHP / 5;
			thisPlayerObj->previousHP = newHP;
		}
	}

	__asm
	{
	EXIT:
		popad
		mov eax, [newHP]
		mov[ecx + 9D4h], eax //current hp in eax
		jmp[fivepunch_HealthModifyJmpBackAddr]
	}
}

void Inittugofwar()
{
	LOG(2, "InitTugOfWarMode\n");
	ImGuiSystem::AddLog("[system] Starting Tug of War Mode\n");

	*Containers::gameVals.pMatchRounds = 5;
	*Containers::gameVals.pMatchTimer = 60 * 60;

	Containers::gameVals.P1CharObjPointer->currentHP /= 2;
	Containers::gameVals.P2CharObjPointer->currentHP /= 2;

	//new round, recharge 50% of the burst
	Containers::gameVals.P1CharObjPointer->overdrive_meter += 50000;
	if (Containers::gameVals.P1CharObjPointer->overdrive_meter > 100000)
		Containers::gameVals.P1CharObjPointer->overdrive_meter = 100000;

	Containers::gameVals.P2CharObjPointer->overdrive_meter += 50000;
	if (Containers::gameVals.P2CharObjPointer->overdrive_meter > 100000)
		Containers::gameVals.P2CharObjPointer->overdrive_meter = 100000;

	HookManager::ActivateHook("tugofwar_HealthModify");
}

void __declspec(naked)tugofwar_HealthModify()
{
#ifndef RELEASE_VER
	__asm pushad
	LOG(7, "tugofwar_HealthModify\n");
	__asm popad
#endif // !RELEASE_VER

	static int newHP = 0;
	static CChar* thisPlayerObj = 0;
	static CChar* otherPlayerObj = 0;

	__asm
	{
		mov[newHP], eax
		mov[thisPlayerObj], ecx
		pushad
	}

	if (*Containers::gameVals.pMatchState == MATCH_STATE_FIGHT) //on match only
	{
		if (thisPlayerObj != Containers::gameVals.P1CharObjPointer && thisPlayerObj != Containers::gameVals.P2CharObjPointer)
		{
			//LOG(2, "NEITHER PLAYER OBJECTS!\n");
			__asm jmp EXIT
		}
		if (newHP > thisPlayerObj->previousHP) // we healed, ignore it.
			newHP = thisPlayerObj->previousHP;
		else //took damage
		{
			newHP = thisPlayerObj->previousHP; //we want to do exactly 20% max hp damage
			newHP -= thisPlayerObj->maxHP / 10;

			if (thisPlayerObj == Containers::gameVals.P1CharObjPointer)
				otherPlayerObj = Containers::gameVals.P2CharObjPointer;
			else
				otherPlayerObj = Containers::gameVals.P1CharObjPointer;

			//heal the other player
			otherPlayerObj->currentHP += otherPlayerObj->maxHP / 10;
			//prevent healing past maxhp
			if (otherPlayerObj->currentHP > otherPlayerObj->maxHP)
				otherPlayerObj->maxHP = otherPlayerObj->currentHP;

			otherPlayerObj->previousHP = otherPlayerObj->currentHP;
		}

		thisPlayerObj->previousHP = newHP;

	}

	__asm
	{
EXIT:
		popad
		mov eax, [newHP]
		mov[ecx + 9D4h], eax //current hp in eax
		jmp[tugofwar_HealthModifyJmpBackAddr]
	}
}

void InitInfiniteHeatMode()
{
	LOG(2, "InitInfiniteHeatMode\n");
	ImGuiSystem::AddLog("[system] Starting Infinite Heat Mode\n");

	//*Containers::gameVals.pMatchTimer = 60 * 60;
	//new round, recharge 50% of the burst
	Containers::gameVals.P1CharObjPointer->heat = 10000;
	Containers::gameVals.P2CharObjPointer->heat = 10000;

	HookManager::ActivateHook("infiniteheat_HeatModify");
}

void __declspec(naked)infiniteheat_HeatModify()
{
#ifndef RELEASE_VER
	__asm pushad
	LOG(7, "infiniteheat_HeatModify\n");
	__asm popad
#endif // !RELEASE_VER

	__asm
	{
		mov esi, 0
		add[ebx + 5AD4h], esi
		jmp[infiniteheat_HeatModifyJmpBackAddr]
	}
}

void InitOverdriveMode()
{
	LOG(2, "InitOverdriveMode\n");
	ImGuiSystem::AddLog("[system] Starting Overdrive Mode\n");

	//*Containers::gameVals.pMatchRounds = 5;
	//*Containers::gameVals.pMatchTimer = 60 * 60;
	//new round, recharge 100% of the burst
	//Containers::gameVals.P1CharObjPointer->overdrive_meter = 0;
	//Containers::gameVals.P2CharObjPointer->overdrive_meter = 0;
	//Containers::gameVals.P1CharObjPointer->overdrive_timeleft = 500;
	//Containers::gameVals.P2CharObjPointer->overdrive_timeleft = 500;

	HookManager::ActivateHook("overdrive_FreezeOverdriveTimeleft");
	HookManager::ActivateHook("overdrive_KeepTimerGoing");
}

void __declspec(naked)overdrive_FreezeOverdriveTimeleft()
{
#ifndef RELEASE_VER
	__asm pushad
	LOG(7, "overdrive_FreezeOverdriveTimeleft\n");
	__asm popad
#endif // !RELEASE_VER

	__asm
	{
		//dec dword ptr[ebx + 1C0h]
		jmp[overdrive_FreezeOverdriveTimeleftJmpBackAddr]
	}
}

void __declspec(naked)overdrive_KeepTimerGoing()
{
#ifndef RELEASE_VER
	__asm pushad
	LOG(7, "overdrive_KeepTimerGoing\n");
	__asm popad
#endif // !RELEASE_VER
	__asm
	{
		nop
		jmp[overdrive_KeepTimerGoingJmpBackAddr]
	}
}

void InitCustomGamemodesVector()
{
	GameMode_t normal{ "Normal", customGameMode_none, NULL, EndCustomGamemode };

	GameMode_t steroid{ "Steroid", customGameMode_steroid,
		"Playable in Online, Training, and Versus modes\nApplies to both players:\n" \
		"- Max HP: +150%%\n- Max Heat Gauge: +50%%\n- Heat gain: +20%%\n"\
		"- Automatic Heat gain on low HP: +100%%\n- Burst Gauge gain: +100%%",
		InitSteroidMode, { "steroid_OverdriveCharge", "steroid_HeatModify", "steroid_HealthModify" } };

	GameMode_t vampire{ "Vampire", customGameMode_vampire,
		"Playable in Online, Training, and Versus modes\nApplies to both players:\n" \
		"- 60%% of damage dealt is healed back\n- Losing 1%% max HP per second",
		InitVampireMode, {"vampire_HealthModify", "vampire_HealthDrain"} };

	GameMode_t ex_vampire{ "Extreme Vampire", customGameMode_ex_vampire,
		"Playable in Online, Training, and Versus modes\nApplies to both players:\n" \
		"- 200%% of damage dealt is healed back\n- Healing beyond full HP increases max HP\n" \
		"- Losing 1.5%% max HP per second",
		InitExVampireMode, {"exVampire_HealthModify", "exVampire_HealthDrain"} };

	GameMode_t onepunch{ "One Punch", customGameMode_onepunch,
		"Playable in Online, and Versus modes\nApplies to both players:\n" \
		"- Any damage results in instakill, unless blocked with barrier\n" \
		"- Each round 50%% of the Burst Gauge is regenerated\n- Each round lasts 15 seconds\n- 5 rounds",
		InitOnePunchMode, {"onepunch_HealthModify"} };

	GameMode_t twopunch{ "Two Punch", customGameMode_twopunch,
		"Playable in Online, and Versus modes\nApplies to both players:\n" \
		"- Taking damage twice results in death, unless blocked with barrier\n" \
		"- Each round 50%% of the Burst Gauge is regenerated\n- Each round lasts 15 seconds\n- 5 rounds",
		InitTwoPunchMode, {"twopunch_HealthModify"} };

	GameMode_t fivepunch{ "Five Punch", customGameMode_fivepunch,
		"Playable in Online, and Versus modes\nApplies to both players:\n" \
		"- Taking damage five times results in death, unless blocked with barrier\n" \
		"- Each round 50%% of the Burst Gauge is regenerated\n- Each round lasts 60 seconds\n- 5 rounds",
		InitFivePunchMode, {"fivepunch_HealthModify"} };

	GameMode_t tugofwar{"Tug of War", customGameMode_tugofwar,
		"Playable in Online, and Versus modes\nApplies to both players:\n" \
		"- Start with 50%% HP\n- All damages hurt/heal 10%% HP, unless blocked with barrier\n" \
		"- Each round 50%% of the Burst Gauge is regenerated\n- Each round lasts 60 seconds\n- 5 rounds",
		Inittugofwar, {"tugofwar_HealthModify"} };

	GameMode_t infiniteheat{ "Infinite Heat", customGameMode_infiniteheat,
		"Playable in Online, Training, and Versus modes\nApplies to both players:\n" \
		"- Heat Gauge is always full",
		InitInfiniteHeatMode, { "infiniteheat_HeatModify" } };

	GameMode_t overdrive{ "Overdrive", customGameMode_overdrive,
		"Playable in Online, Training, and Versus modes\nApplies to both players:\n" \
		"- Overdrive is always on",
		InitOverdriveMode, {"overdrive_FreezeOverdriveTimeleft", "overdrive_KeepTimerGoing"} };

	GameModes.push_back(normal);
	GameModes.push_back(steroid);
	GameModes.push_back(vampire);
	GameModes.push_back(ex_vampire);
	GameModes.push_back(onepunch);
	GameModes.push_back(twopunch);
	GameModes.push_back(fivepunch);
	GameModes.push_back(tugofwar);
	GameModes.push_back(infiniteheat);
	//GameModes.push_back(overdrive);
}

void CustomGamemodeHooks()
{
	LOG(2, "CustomGamemodeHooks\n");

	steroid_OverdriveChargeJmpBackAddr = HookManager::SetHook("steroid_OverdriveCharge", "\x01\xb7\xe4\x5a\x00\x00\x8b\x87\xe4\x5a\x00\x00", "xxxxxxxxxxxx", 6, steroid_OverdriveCharge, false);
	steroid_HeatModifyJmpBackAddr = HookManager::SetHook("steroid_HeatModify", "\x01\xb3\xd4\x5a\x00\x00\x8b\x83\xd4\x5a\x00\x00", "xxxxxxxxxxxx", 6, steroid_HeatModify, false);
	steroid_HealthModifyJmpBackAddr = HookManager::SetHook("steroid_HealthModify", "\x89\x81\xd4\x09\x00\x00\x5d", "xxxxxxx", 6, steroid_HealthModify, false);

	vampire_HealthModifyJmpBackAddr = HookManager::SetHook("vampire_HealthModify", "\x89\x81\xd4\x09\x00\x00\x5d", "xxxxxxx", 6, vampire_HealthModify, false);
	vampire_HealthDrainJmpBackAddr = HookManager::SetHook("vampire_HealthDrain", "\x8b\x9e\xd4\x09\x00\x00\x6b\xc9\x64", "xxxxxxxxx", 6, vampire_HealthDrain, false);

	exVampire_HealthModifyJmpBackAddr = HookManager::SetHook("exVampire_HealthModify", "\x89\x81\xd4\x09\x00\x00\x5d", "xxxxxxx", 6, exVampire_HealthModify, false);
	exVampire_HealthDrainJmpBackAddr = HookManager::SetHook("exVampire_HealthDrain", "\x8b\x9e\xd4\x09\x00\x00\x6b\xc9\x64", "xxxxxxxxx", 6, exVampire_HealthDrain, false);

	onepunch_HealthModifyJmpBackAddr = HookManager::SetHook("onepunch_HealthModify", "\x89\x81\xd4\x09\x00\x00\x5d", "xxxxxxx", 6, onepunch_HealthModify, false);

	twopunch_HealthModifyJmpBackAddr = HookManager::SetHook("twopunch_HealthModify", "\x89\x81\xd4\x09\x00\x00\x5d", "xxxxxxx", 6, twopunch_HealthModify, false);

	fivepunch_HealthModifyJmpBackAddr = HookManager::SetHook("fivepunch_HealthModify", "\x89\x81\xd4\x09\x00\x00\x5d", "xxxxxxx", 6, fivepunch_HealthModify, false);

	tugofwar_HealthModifyJmpBackAddr = HookManager::SetHook("tugofwar_HealthModify", "\x89\x81\xd4\x09\x00\x00\x5d", "xxxxxxx", 6, tugofwar_HealthModify, false);

	infiniteheat_HeatModifyJmpBackAddr = HookManager::SetHook("infiniteheat_HeatModify", "\x01\xb3\xd4\x5a\x00\x00\x8b\x83\xd4\x5a\x00\x00", "xxxxxxxxxxxx", 6, infiniteheat_HeatModify, false);

	overdrive_FreezeOverdriveTimeleftJmpBackAddr = HookManager::SetHook("overdrive_FreezeOverdriveTimeleft", "\xff\x8b\xc0\x01\x00\x00\x83\xbb\xc0\x01\x00\x00\x00", "xxxxxxxxxxxxx", 6, overdrive_FreezeOverdriveTimeleft, false);
	overdrive_KeepTimerGoingJmpBackAddr = HookManager::SetHook("overdrive_KeepTimerGoing", "\x39\x45\xcc\x0f\x84", "xxxxx", 9, overdrive_KeepTimerGoing, false);

	InitCustomGamemodesVector();
}