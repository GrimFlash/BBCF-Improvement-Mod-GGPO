#include "hooks_customGameModes.h"

#include "HookManager.h"

#include "Core/interfaces.h"
#include "Core/logger.h"
#include "CustomGameMode/customGameMode.h"
#include "Game/gamestates.h"

#include <imgui.h>

DWORD steroid_OverdriveChargeJmpBackAddr = 0;
void __declspec(naked)steroid_OverdriveCharge()
{
	LOG_ASM(7, "steroid_OverdriveCharge\n");

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

DWORD steroid_HeatModifyJmpBackAddr = 0;
void __declspec(naked)steroid_HeatModify()
{
	LOG_ASM(7, "steroid_HeatModify\n")

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
		mov[addedHeatValue], 5
	}

	__asm
	{
EXIT:
		mov esi, [addedHeatValue]
		add[ebx + 5AD4h], esi
		jmp[steroid_HeatModifyJmpBackAddr]
	}
}

DWORD steroid_HealthModifyJmpBackAddr = 0;
void __declspec(naked)steroid_HealthModify()
{
	LOG_ASM(7, "steroid_HealthModify\n");

	static int newHP = 0;
	static int previousHP = 0;
	static int maxHP = 0;

	__asm pushad
	__asm
	{
		mov[newHP], eax
		mov edx, [ecx + 9D0h] //previous hp addr
		mov[previousHP], edx
		mov edx, [ecx + 9D8h] //max hp addr
		mov[maxHP], edx
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

// Handles the lifesteal for all chars
DWORD vampire_HealthModifyJmpBackAddr = 0;
void __declspec(naked)vampire_HealthModify()
{
	LOG_ASM(7, "vampire_HealthModify\n");

	static int newHP = 0;
	//static int previousHP = 0;
	static int opponentHealing = 0;
	static CharData* thisPlayerObj = 0;
	static CharData* otherPlayerObj = 0;

	__asm
	{
		pushad
		mov[newHP], eax
		//mov edx, [ecx + 9D0h] //previous hp addr
		//mov [previousHP], edx <- decided to simply use thisplayerObj instead
		mov[thisPlayerObj], ecx
	}

	if (*g_gameVals.pMatchState == MatchState_Fight && newHP < thisPlayerObj->previousHP) // only if we took damage. avoiding running on healing (like ragna for example)
	{
		//LOG(2, "Vampire: \n");
		if (thisPlayerObj != g_interfaces.player1.GetData() && thisPlayerObj != g_interfaces.player2.GetData())
		{
			//LOG(2, "NEITHER PLAYER OBJECTS!\n");
			__asm jmp EXIT
		}

		opponentHealing = (thisPlayerObj->previousHP - newHP) * vampire_healing_percent;

		if (thisPlayerObj == g_interfaces.player1.GetData())
		{
			otherPlayerObj = g_interfaces.player2.GetData();
		}
		else
		{
			otherPlayerObj = g_interfaces.player1.GetData();
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

// Handles the % hp loss per second
DWORD vampire_HealthDrainJmpBackAddr = 0;
void __declspec(naked)vampire_HealthDrain()
{
	LOG_ASM(7, "vampire_HealthDrain\n");

	static float vampirism_timer = 0.0;
	static int previous_real_timer = 0;
	static int P1_lostHP = 0;
	static int P2_lostHP = 0;
	static CharData* player1;
	static CharData* player2;

	__asm pushad

	if (*g_gameVals.pMatchState == MatchState_Fight) //on match only
	{
		player1 = g_interfaces.player1.GetData();
		player2 = g_interfaces.player2.GetData();

		if (*g_gameVals.pGameMode == GameMode_Training || *g_gameVals.pMatchTimer == 5939) //if we are in training mode, calculate it a different way, since we have no match timer
		{
			//do not reduce hp while distortions / od finishers / astrals are happening:
			if (!player1->isDoingDistortion && !player2->isDoingDistortion)
				vampirism_timer += ImGui::GetIO().DeltaTime;
		}
		else
		{
			//uninitialized value or new match started:
			if (previous_real_timer == 0 || previous_real_timer < *g_gameVals.pMatchTimer)
				previous_real_timer = *g_gameVals.pMatchTimer;

			vampirism_timer += (previous_real_timer - *g_gameVals.pMatchTimer);
			previous_real_timer = *g_gameVals.pMatchTimer;
		}

		if (vampirism_timer > 60) // 1 sec
		{
			//LOG(2, "!Vampire taking 1sec damage\n");
			P1_lostHP = player1->maxHP * vampire_health_loss_percent_per_sec;
			P2_lostHP = player2->maxHP * vampire_health_loss_percent_per_sec;

			player1->currentHP -= P1_lostHP;
			player2->currentHP -= P2_lostHP;

			if (player1->currentHP < 1) //prevent dying of this effect
				player1->currentHP = 1;

			if (player2->currentHP < 1) //prevent dying of this effect
				player2->currentHP = 1;

			player1->previousHP = player1->currentHP;
			player2->previousHP = player2->currentHP;

			vampirism_timer = 0.0;
		}
	}

	__asm
	{
		popad
		mov ebx, [esi + 9D4h]
		jmp[vampire_HealthDrainJmpBackAddr]
	}
}

DWORD exVampire_HealthModifyJmpBackAddr = 0;
void __declspec(naked)exVampire_HealthModify()
{
	//handles the lifesteal for all chars
	LOG_ASM(7, "exVampire_HealthModify\n");

	static int newHP = 0;
	//static int previousHP = 0;
	static int opponentHealing = 0;
	static CharData* thisPlayerObj = 0;
	static CharData* otherPlayerObj = 0;

	__asm
	{
		pushad
		mov[newHP], eax
		//mov edx, [ecx + 9D0h] //previous hp addr
		//mov [previousHP], edx <- decided to simply use thisplayerObj instead
		mov[thisPlayerObj], ecx
	}

	if (*g_gameVals.pMatchState == MatchState_Fight && newHP < thisPlayerObj->previousHP) // only if we took damage. avoiding running on healing (like ragna for example)
	{
		//LOG(2, "exVampire: \n");
		if (thisPlayerObj != g_interfaces.player1.GetData() && thisPlayerObj != g_interfaces.player2.GetData())
		{
			//LOG(2, "NEITHER PLAYER OBJECTS!\n");
			__asm jmp EXIT
		}

		opponentHealing = (thisPlayerObj->previousHP - newHP) * exVampire_healing_percent;

		if (thisPlayerObj == g_interfaces.player1.GetData())
		{
			otherPlayerObj = g_interfaces.player2.GetData();
		}
		else
		{
			otherPlayerObj = g_interfaces.player1.GetData();
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
		mov[ecx + 9D4h], eax //current hp in eax
		jmp[exVampire_HealthModifyJmpBackAddr]
	}
}

DWORD exVampire_HealthDrainJmpBackAddr = 0;
void __declspec(naked)exVampire_HealthDrain()
{
	//handles the % hp loss per second
	LOG_ASM(7, "exVampire_HealthDrain\n");

	static float vampirism_timer = 0.0;
	static int previous_real_timer = 0;
	static int P1_lostHP = 0;
	static int P2_lostHP = 0;
	static CharData* player1;
	static CharData* player2;

	__asm pushad

	if (*g_gameVals.pMatchState == MatchState_Fight) //on match only
	{
		player1 = g_interfaces.player1.GetData();
		player2 = g_interfaces.player2.GetData();

		//if we are in training mode, calculate it a different way, since we have no match timer. pMatchTimer is set to 5939 when infinite timer
		if (*g_gameVals.pGameMode == GameMode_Training || *g_gameVals.pMatchTimer == 5939)
		{
			//do not reduce hp while distortions / od finishers / astrals are happening:
			if (!player1->isDoingDistortion && !player2->isDoingDistortion)
			{
				vampirism_timer += ImGui::GetIO().DeltaTime;
			}
		}
		else
		{
			//uninitialized value or new match started:
			if (previous_real_timer == 0 || previous_real_timer < *g_gameVals.pMatchTimer)
			{
				previous_real_timer = *g_gameVals.pMatchTimer;
			}

			vampirism_timer += (previous_real_timer - *g_gameVals.pMatchTimer);
			previous_real_timer = *g_gameVals.pMatchTimer;
		}

		if (vampirism_timer > 60) // 1 sec
		{
			P1_lostHP = player1->maxHP * exVampire_health_loss_percent_per_sec;
			P2_lostHP = player2->maxHP * exVampire_health_loss_percent_per_sec;

			player1->currentHP -= P1_lostHP;
			player2->currentHP -= P2_lostHP;

			if (player1->currentHP < 1) //prevent dying of this effect
				player1->currentHP = 1;

			if (player2->currentHP < 1) //prevent dying of this effect
				player2->currentHP = 1;

			player1->previousHP = player1->currentHP;
			player2->previousHP = player2->currentHP;

			vampirism_timer = 0.0;
		}
	}

	__asm
	{
		popad
		mov ebx, [esi + 9D4h]
		jmp[exVampire_HealthDrainJmpBackAddr]
	}
}

DWORD onepunch_HealthModifyJmpBackAddr = 0;
void __declspec(naked)onepunch_HealthModify()
{
	LOG_ASM(7, "onepunch_HealthModify\n");

	static int newHP = 0;
	static CharData* thisPlayerObj = 0;

	__asm
	{
		mov[newHP], eax
		mov[thisPlayerObj], ecx
		pushad
	}

	if (*g_gameVals.pMatchState != MatchState_Fight || //on match only
		(thisPlayerObj != g_interfaces.player1.GetData() && thisPlayerObj != g_interfaces.player2.GetData())) // not a player object
	{
		__asm jmp EXIT
	}

	// In training mode this fix is needed as HP is being set by this method on start
	if ((*g_gameVals.pGameMode == GameMode_Training && newHP == thisPlayerObj->maxHP))
	{
		__asm jmp EXIT
	}

	newHP = 0;

	__asm
	{
EXIT:
		popad
		mov eax, [newHP]
		mov[ecx + 9D4h], eax //current hp in eax
		jmp[onepunch_HealthModifyJmpBackAddr]
	}
}

DWORD twopunch_HealthModifyJmpBackAddr = 0;
void __declspec(naked)twopunch_HealthModify()
{
	LOG_ASM(7, "twopunch_HealthModify\n");

	static int newHP = 0;
	static CharData* thisPlayerObj = 0;

	__asm
	{
		mov[newHP], eax
		mov[thisPlayerObj], ecx
		pushad
	}

	if (*g_gameVals.pMatchState != MatchState_Fight || //on match only
		(thisPlayerObj != g_interfaces.player1.GetData() && thisPlayerObj != g_interfaces.player2.GetData())) // not a player object
	{
		__asm jmp EXIT
	}

	// In training mode this fix is needed as HP is being set by this method on start
	if (*g_gameVals.pGameMode == GameMode_Training && newHP == thisPlayerObj->maxHP)
	{
		__asm jmp EXIT
	}

	if (newHP > thisPlayerObj->previousHP) // we healed, ignore it.
	{
		newHP = thisPlayerObj->previousHP;
	}
	else //took damage
	{
		newHP = thisPlayerObj->previousHP; //we want to do exactly 50% max hp damage
		newHP -= thisPlayerObj->maxHP / 2;
		thisPlayerObj->previousHP = newHP;
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

DWORD fivepunch_HealthModifyJmpBackAddr = 0;
void __declspec(naked)fivepunch_HealthModify()
{
	LOG_ASM(7, "fivepunch_HealthModify\n");

	static int newHP = 0;
	static CharData* thisPlayerObj = 0;

	__asm
	{
		mov[newHP], eax
		mov[thisPlayerObj], ecx
		pushad
	}

	if (*g_gameVals.pMatchState != MatchState_Fight ||
		(thisPlayerObj != g_interfaces.player1.GetData() && thisPlayerObj != g_interfaces.player2.GetData())) //on match only
	{
		__asm jmp EXIT
	}

	// In training mode this fix is needed as HP is being set by this method on start
	if (*g_gameVals.pGameMode == GameMode_Training && newHP == thisPlayerObj->maxHP)
	{
		__asm jmp EXIT
	}

	if (newHP > thisPlayerObj->previousHP) // we healed, ignore it.
	{
		newHP = thisPlayerObj->previousHP;
	}
	else //took damage
	{
		newHP = thisPlayerObj->previousHP; //we want to do exactly 20% max hp damage
		newHP -= thisPlayerObj->maxHP / 5;
		thisPlayerObj->previousHP = newHP;
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

DWORD tugofwar_HealthModifyJmpBackAddr = 0;
void __declspec(naked)tugofwar_HealthModify()
{
	LOG_ASM(7, "tugofwar_HealthModify\n");

	static int newHP = 0;
	static CharData* thisPlayerObj = 0;
	static CharData* otherPlayerObj = 0;

	__asm
	{
		mov[newHP], eax
		mov[thisPlayerObj], ecx
		pushad
	}

	if (*g_gameVals.pMatchState != MatchState_Fight ||
		(thisPlayerObj != g_interfaces.player1.GetData() && thisPlayerObj != g_interfaces.player2.GetData())) //on match only
	{
		__asm jmp EXIT
	}

	// In training mode this fix is needed as HP is being set by this method on start
	if (*g_gameVals.pGameMode == GameMode_Training && newHP == thisPlayerObj->maxHP)
	{
		__asm jmp EXIT
	}

	if (newHP > thisPlayerObj->previousHP) // we healed, ignore it.
	{
		newHP = thisPlayerObj->previousHP;
	}
	else //took damage
	{
		newHP = thisPlayerObj->previousHP; //we want to do exactly 20% max hp damage
		newHP -= thisPlayerObj->maxHP / 10;

		if (thisPlayerObj == g_interfaces.player1.GetData())
		{
			otherPlayerObj = g_interfaces.player2.GetData();
		}
		else
		{
			otherPlayerObj = g_interfaces.player1.GetData();
		}

		//heal the other player
		otherPlayerObj->currentHP += otherPlayerObj->maxHP / 10;

		//prevent healing past maxhp
		if (otherPlayerObj->currentHP > otherPlayerObj->maxHP)
			otherPlayerObj->maxHP = otherPlayerObj->currentHP;

		otherPlayerObj->previousHP = otherPlayerObj->currentHP;
	}

	thisPlayerObj->previousHP = newHP;

	__asm
	{
EXIT:
		popad
		mov eax, [newHP]
		mov[ecx + 9D4h], eax //current hp in eax
		jmp[tugofwar_HealthModifyJmpBackAddr]
	}
}


DWORD infiniteheat_HeatModifyJmpBackAddr = 0;
void __declspec(naked)infiniteheat_HeatModify()
{
	LOG_ASM(7, "infiniteheat_HeatModify\n");

	__asm
	{
		mov esi, 0
		add[ebx + 5AD4h], esi
		jmp[infiniteheat_HeatModifyJmpBackAddr]
	}
}

DWORD training_healthModifyFixJmpBackAddr = 0;
void __declspec(naked)training_healthModifyFix()
{
	LOG_ASM(7, "training_healthModifyFix\n");

	__asm
	{
		//mov esi, 69h // Random value to trigger next conditional jump
		//jmp[training_healthModifyFixJmpBackAddr]

		// TODO: find better solution for this
		jmp [training_healthModifyFixJmpBackAddr]
	}
}

void placeHooks_CustomGameModes()
{
	LOG(2, "placeHooks_CustomGameModes\n");

	steroid_OverdriveChargeJmpBackAddr = HookManager::SetHook("steroid_OverdriveCharge", "\x01\xb7\xe4\x5a\x00\x00\x8b\x87\xe4\x5a\x00\x00",
		"xxxxxxxxxxxx", 6, steroid_OverdriveCharge, false);

	steroid_HeatModifyJmpBackAddr = HookManager::SetHook("steroid_HeatModify", "\x01\xb3\xd4\x5a\x00\x00\x8b\x83\xd4\x5a\x00\x00",
		"xxxxxxxxxxxx", 6, steroid_HeatModify, false);

	steroid_HealthModifyJmpBackAddr = HookManager::SetHook("steroid_HealthModify", "\x89\x81\xd4\x09\x00\x00\x5d",
		"xxxxxxx", 6, steroid_HealthModify, false);

	vampire_HealthModifyJmpBackAddr = HookManager::SetHook("vampire_HealthModify", "\x89\x81\xd4\x09\x00\x00\x5d",
		"xxxxxxx", 6, vampire_HealthModify, false);

	vampire_HealthDrainJmpBackAddr = HookManager::SetHook("vampire_HealthDrain", "\x8b\x9e\xd4\x09\x00\x00\x6b\xc9\x64",
		"xxxxxxxxx", 6, vampire_HealthDrain, false);

	exVampire_HealthModifyJmpBackAddr = HookManager::SetHook("exVampire_HealthModify", "\x89\x81\xd4\x09\x00\x00\x5d",
		"xxxxxxx", 6, exVampire_HealthModify, false);

	exVampire_HealthDrainJmpBackAddr = HookManager::SetHook("exVampire_HealthDrain", "\x8b\x9e\xd4\x09\x00\x00\x6b\xc9\x64",
		"xxxxxxxxx", 6, exVampire_HealthDrain, false);

	onepunch_HealthModifyJmpBackAddr = HookManager::SetHook("onepunch_HealthModify", "\x89\x81\xd4\x09\x00\x00\x5d",
		"xxxxxxx", 6, onepunch_HealthModify, false);

	twopunch_HealthModifyJmpBackAddr = HookManager::SetHook("twopunch_HealthModify", "\x89\x81\xd4\x09\x00\x00\x5d",
		"xxxxxxx", 6, twopunch_HealthModify, false);

	fivepunch_HealthModifyJmpBackAddr = HookManager::SetHook("fivepunch_HealthModify", "\x89\x81\xd4\x09\x00\x00\x5d",
		"xxxxxxx", 6, fivepunch_HealthModify, false);

	tugofwar_HealthModifyJmpBackAddr = HookManager::SetHook("tugofwar_HealthModify", "\x89\x81\xd4\x09\x00\x00\x5d",
		"xxxxxxx", 6, tugofwar_HealthModify, false);

	infiniteheat_HeatModifyJmpBackAddr = HookManager::SetHook("infiniteheat_HeatModify", "\x01\xb3\xd4\x5a\x00\x00\x8b\x83\xd4\x5a\x00\x00",
		"xxxxxxxxxxxx", 6, infiniteheat_HeatModify, false);

	////////////////// Fix for training mode
	// Could not make a better fix due to unreliable byte pattern for sigscan

	// Get return address
	HookManager::SetHook("getTrainingHealthModifyFixJmpBackAddr", "\x8b\x75\xfc\x85\xf6\x0f\x84\x00\x00\x00\x00\x68\x00\x00\x00\x00",
		"xxxxxxx????x????", 0, nullptr, false);

	HookManager::SetHook("training_healthModifyFix", "\x83\x7b\x30\x00\x8b\xf0\x74\x00",
		"xxxxxxx?", 6, training_healthModifyFix, false);

	training_healthModifyFixJmpBackAddr = HookManager::GetStartAddress("getTrainingHealthModifyFixJmpBackAddr");

	//////////////////
}
