#include "../include/custom_palette.h"
#include "../include/utils.h"
#include "../include/SteamApiWrapper/SteamNetworkingWrapper.h"
#include "../include/settings.h"
#include "../include/hook_manager.h"
#include "game/game.h"
#include <steam_api.h>
#include "../include/ImGui/ImGuiSystem.h"
#include "../include/containers.h"
#include "../include/internal_palette_datas.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <atlstr.h>
#include <string>
#include "../include/custom_gamemodes.h"
#include "../include/gamestates_defines.h"
#pragma comment(lib, "steam_api.lib")

LPCWSTR ingame_chars[36]
{
	L"Ragna",		//00
	L"Jin",			//01
	L"Noel",		//02
	L"Rachel",		//03
	L"Taokaka",		//04
	L"Tager",		//05
	L"Litchi",		//06
	L"Arakune",		//07
	L"Bang",		//08
	L"Carl",		//09
	L"Hakumen",		//10
	L"Nu",			//11
	L"Tsubaki",		//12
	L"Hazama",		//13
	L"Mu",			//14
	L"Makoto",		//15
	L"Valkenhayn",	//16
	L"Platinum",	//17
	L"Relius",		//18
	L"Izayoi",		//19
	L"Amane",		//20
	L"Bullet",		//21
	L"Azrael",		//22
	L"Kagura",		//23
	L"Kokonoe",		//24
	L"Terumi",		//25
	L"Celica",		//26
	L"Lambda",		//27
	L"Hibiki",		//28
	L"Nine",		//29
	L"Naoto",		//30
	L"Izanami",		//31
	L"Susanoo",		//32
	L"Es",			//33
	L"Mai",			//34
	L"Jubei"		//35
};

char HPAL_header[HPAL_HEADER_LEN] = { '\x48', '\x50', '\x41', '\x4C', '\x25', '\x01', '\x00', '\x00', '\x20', '\x04', '\x00','\x00', '\x00',
							'\x01', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x01', '\x00', 
							'\x00', '\x10', '\x00', '\x00', '\x00', '\x00', '\x00', '\xB4', '\x00', '\xFF'};

int debug_id = 0;

void CreatePaletteDirectories();
void LoadPaletteFiles();
void LoadCustomPalettesIntoContainer(int charIndex, LPCWSTR path);
void SaveDefaultP1P2Pals();
void GrabP1P2PalettePointers();
void GetPlayersPaletteIndexSPPointer();
void GetCPUsPaletteIndexSPPointer();
void GetP1PaletteIndexMPPointer();
void GetP2PaletteIndexMPPointer();
bool HookPaletteFunctions();
void DereferencePalettePointer1(DWORD* palettePointer, DWORD paletteIndex);
void DereferencePalettePointer2(DWORD* palettePointer, DWORD paletteIndex);
void DEBUGDereferencePalettePointer1(DWORD* palettePointer, DWORD paletteIndex);
void DereferenceP1P2PalettePointers();
void PacketProcessingFunc();
void SendCustomDatas();
void GetOpponentSteamID();
void GetOwnSteamID();
void StartMatchWheelOfFate();
void GetGameStateMatchStart();
void MatchIntroStartsPlayingFunc();

void GetP1P2CharIDCharSelectSP();
void GetOpponentCharIDCharSelectMP();
void GetPlayersCharIDVersusScreenSP();
void GetCPUsCharIDVersusScreenSP();
void GetP1CharIDVersusScreenMP();
void GetP2CharIDVersusScreenMP();

void GetGameStateCharacterSelect();
void GetP1P2CharSelectPalettePointerIndexes();
void BackupP1PaletteEffects();
void BackupP2PaletteEffects();
DWORD* getPaletteArray(DWORD* base, DWORD palIndex, DWORD fileID);
void ReplacePaletteInMemory(DWORD* Dst, const void* Src);
void ReplaceP1PaletteEffects();
void ReplaceP2PaletteEffects();
void HandleSavedPackets();
void GetP1CharIDTrainingModeCharIDChange();
void GetP2CharIDTrainingModeCharIDChange();
void InitCustomPaletteVector();
void SendCustomEffectBloom();
void OverwriteStagesList();

//DWORD jmpBackAddr = hookAddress + hookLength;
DWORD GetPlayersPaletteIndexSPJmpBackAddr = 0;
DWORD GetCPUsPaletteIndexSPJmpBackAddr = 0;
DWORD P1PaletteIndexMPJmpBackAddr = 0;
DWORD P2PaletteIndexMPJmpBackAddr = 0;
DWORD GrabP1P2PalettePointersJmpBackAddr = 0;
DWORD GetOpponentSteamIDJmpBackAddr = 0;
DWORD GetOwnSteamIDJmpBackAddr = 0;
DWORD StartMatchWheelOfFateJmpBackAddr = 0;
DWORD MatchIntroStartsPlayingJmpBackAddr = 0;
DWORD GetP1P2CharIDCharSelectSPJmpBackAddr = 0;
DWORD GetOpponentCharIDCharSelectMPJmpBackAddr = 0;
DWORD GetPlayersCharIDVersusScreenSPJmpBackAddr = 0;
DWORD GetCPUsCharIDVersusScreenSPJmpBackAddr = 0;
DWORD GetP1CharIDVersusScreenMPJmpBackAddr = 0;
DWORD GetP2CharIDVersusScreenMPJmpBackAddr = 0;
DWORD GetGameStateCharacterSelectJmpBackAddr = 0;
DWORD GetP1CharIDTrainingModeCharIDChangeJmpBackAddr = 0;
DWORD GetP2CharIDTrainingModeCharIDChangeJmpBackAddr = 0;
int restoredGetP2CharIDTrainingModeCharAddr = 0;

DWORD GetP1P2CharSelectPalettePointerIndexesJmpBackAddr = 0;

DWORD* P1CharSelectPalIndex = 0;
DWORD* P2CharSelectPalIndex = 0;
DWORD* CharSelectPalettesBasePointer = 0;
DWORD GetCharacterSelScreenPalPointerJmpBackAddr = 0;
int restoredCallAddr = 0;
void __declspec(naked)GetCharacterSelScreenPalPointer()
{
	__asm pushad
	LOG(2, "GetCharacterSelScreenPalPointer\n");
	LOG(2, "\trestoredCallAddr: 0x%x\n", restoredCallAddr);
	__asm popad

	__asm
	{
		mov CharSelectPalettesBasePointer, eax
		mov ecx, eax
		call restoredCallAddr
		jmp[GetCharacterSelScreenPalPointerJmpBackAddr]
	}
}

DWORD GetOpponentSteamIDAtCharSelectJmpBackAddr = 0;
CSteamID* steamid;
void __declspec(naked)GetOpponentSteamIDAtCharSelect()
{
	__asm
	{
		mov [esi], eax
		mov steamid, eax
		pushad
	}
	LOG(2, "steamid: 0x%x\n", steamid);
	__asm
	{
		popad
		mov eax, [edi + 4]
		jmp[GetOpponentSteamIDAtCharSelectJmpBackAddr]
	}
}

DWORD GetCharObjPointersJmpBackAddr = 0;
void __declspec(naked)GetCharObjPointers()
{
	__asm pushad
	LOG(2, "GetCharObjPointers\n");
	// TODO: make better effort to integrate with existing systems and/or move stuff around to make sense
	game::InitGameStatePointers();
	__asm popad

	__asm
	{	
		pushad
		mov ebx, eax 
		mov eax, 4 
		mul edi
		add eax, ebx
		add eax, 25E8h //calculated eax + edi * 4 + 25E8h
		mov eax, [eax] //get from static addresses 00C19DF0 and 00C19DF4 the addresses of player objects
		cmp edi, 1
		je PLAYER2
PLAYER1:
		mov Containers::gameVals.P1CharObjPointer, eax
		jmp EXIT
PLAYER2:
		mov Containers::gameVals.P2CharObjPointer, eax
EXIT:
		popad
		mov[eax + edi * 4 + 25E8h], esi
		jmp[GetCharObjPointersJmpBackAddr]
	}
}

DWORD ForceBloomOnJmpBackAddr = 0;
int restoredForceBloomOffAddr = 0;
void __declspec(naked)ForceBloomOn()
{

	__asm pushad
	LOG(7, "ForceBloomOn\n");

	if (Containers::gameVals.P1_selectedCharID != -1)
	{
		if (Containers::gameVals.origP1PaletteIndex == 21)
			Containers::gameVals.isP1BloomOn = 1;
		else if (Containers::gameVals.P1_selected_custom_pal > 0)
			Containers::gameVals.isP1BloomOn = Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID][Containers::gameVals.P1_selected_custom_pal][9][0];
	}
	if (Containers::gameVals.P2_selectedCharID != -1)
	{
		if (Containers::gameVals.origP2PaletteIndex == 21)
			Containers::gameVals.isP2BloomOn = 1;
		else if (Containers::gameVals.P2_selected_custom_pal > 0)
			Containers::gameVals.isP2BloomOn = Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID][Containers::gameVals.P2_selected_custom_pal][9][0];
	}
	__asm popad

	__asm
	{
		mov eax, [Containers::gameVals.P1CharObjPointer]
		cmp edi, eax
		jne PLAYER2
PLAYER1:
		cmp Containers::gameVals.isP1BloomOn, 1
		je TURN_BLOOM_ON
		jmp[restoredForceBloomOffAddr]
PLAYER2:
		cmp Containers::gameVals.isP2BloomOn, 1
		je TURN_BLOOM_ON
		jmp[restoredForceBloomOffAddr]
TURN_BLOOM_ON:
		jmp[ForceBloomOnJmpBackAddr]
	}
}

DWORD CPUsPaletteIndexAndCharIDArcadeFixJmpBackAddr = 0;
void __declspec(naked)CPUsPaletteIndexAndCharIDArcadeFix()
{
	__asm pushad
	LOG(2, "CPUsPaletteIndexAndCharIDArcadeFix\n");
	if (Containers::gameVals.isP1CPU)
		Containers::gameVals.P1DefaultPalBackup[0] = "";
	else
		Containers::gameVals.P2DefaultPalBackup[0] = "";
	__asm popad

	__asm
	{
		mov ebx, edi
		add ebx, 1650h
		cmp Containers::gameVals.isP1CPU, 1
		je SET_TO_PLAYER1
		mov Containers::gameVals.P2PaletteIndex, ebx
		mov Containers::gameVals.P2_selectedCharID, eax
		jmp EXIT
SET_TO_PLAYER1:
		mov Containers::gameVals.P1PaletteIndex, ebx
		mov Containers::gameVals.P1_selectedCharID, eax
EXIT:
		mov[edi + 1648h], eax
		jmp[CPUsPaletteIndexAndCharIDArcadeFixJmpBackAddr]
	}
}

DWORD GetIsP1CPUJmpBackAddr = 0;
void __declspec(naked)GetIsP1CPU()
{
	__asm pushad
	LOG(2, "GetIsP1CPU\n");
	__asm popad

	__asm
	{
		mov[eax + 1688h], edi
		mov Containers::gameVals.isP1CPU, edi;
		jmp[GetIsP1CPUJmpBackAddr]
	}
}

bool HookPaletteFunctions()
{ 
	//single player
	GetPlayersPaletteIndexSPJmpBackAddr = HookManager::SetHook("GetPlayersPaletteIndexSPPointerFunc", "\x89\x87\x50\x16\x00\x00\x8b\x83\x48\x06\x00\x00", 
		"xxxxxxxxxxxx", 6, GetPlayersPaletteIndexSPPointer);
	GetCPUsPaletteIndexSPJmpBackAddr = HookManager::SetHook("GetCPUsPaletteIndexSPPointerFunc", "\x89\x86\x50\x16\x00\x00\x8b\x84\x0a\x48\x06\x00\x00",
		"xxxxxxxxxxxxx", 6, GetCPUsPaletteIndexSPPointer);
	CPUsPaletteIndexAndCharIDArcadeFixJmpBackAddr = HookManager::SetHook("P2PaletteIndexSPVersusFix",
		"\x89\x87\x48\x16\x00\x00\xc7\x87\x50\x16\x00\x00\x00\x00\x00\x00", "xxxxxxxxxxxxxxxx", 6, CPUsPaletteIndexAndCharIDArcadeFix);
	//multiplayer
	P1PaletteIndexMPJmpBackAddr = HookManager::SetHook("P1PaletteIndexMPPointerFunc", "\x89\x48\x08\x0f\xb6\xb3\xa8\x03\x00\x00\x6a\x00", 
		"xxxxxxxxxxxx", 10, GetP1PaletteIndexMPPointer);
	P2PaletteIndexMPJmpBackAddr = HookManager::SetHook("P2PaletteIndexMPPointerFunc", "\x89\x48\x08\x0f\xb6\xb3\x14\x04\x00\x00\x6a\x01", 
		"xxxxxxxxxxxx", 10, GetP2PaletteIndexMPPointer);
	GrabP1P2PalettePointersJmpBackAddr = HookManager::SetHook("GrabP1P2PalettePointerFunc", "\x89\x81\x30\x08\x00\x00\x8b\xc8\xe8\x00\x00\x00\x00\x5f", 
		"xxxxxxxxx????x", 6, GrabP1P2PalettePointers);
	/*GetOpponentSteamIDJmpBackAddr = HookManager::SetHook("GetOpponentSteamID", "\x89\x43\x3c\x8b\x41\x04\x57\x8b\xce", "xxxxxxxxx", 6, GetOpponentSteamID);*/
	GetOwnSteamIDJmpBackAddr = HookManager::SetHook("GetOwnSteamID", "\x89\x07\x8b\x46\x04\x89\x47\x04\x8d\x46\x08", "xxxxxxxxxxx", 5, GetOwnSteamID);

	StartMatchWheelOfFateJmpBackAddr = HookManager::SetHook("StartMatchWheelOfFate", "\xc7\x43\x30\x02\x00\x00\x00\xc7\x43\x34\x01\x00\x00\x00\xe8\x00\x00\x00\x00\x83", 
		"xxxxxxxxxxxxxxx????x", 7, StartMatchWheelOfFate);
	MatchIntroStartsPlayingJmpBackAddr = HookManager::SetHook("MatchIntroStartsPlaying", "\x83\xa0\x6c\x62\x40\x00\xfd\x83\x66\x24\xef\x5f\x5e\x33\xc0",
		"xxxxxxxxxxxxxxx", 7, MatchIntroStartsPlayingFunc);

	GetGameStateCharacterSelectJmpBackAddr = HookManager::SetHook("GetGameStateCharacterSelect", "\xc7\x83\x0c\x01\x00\x00\x06\x00\x00\x00\xe8", "xxxxxxxxxxx", 10, GetGameStateCharacterSelect);

	GetP1P2CharIDCharSelectSPJmpBackAddr = HookManager::SetHook("GetP1P2CharIDCharSelectSP", "\x89\x84\xbe\x0c\x0a\x00\x00\xe8", "xxxxxxxx", 7, GetP1P2CharIDCharSelectSP);
	//GetOpponentCharIDCharSelectMPJmpBackAddr = HookManager::SetHook("GetOpponentCharIDCharSelectMP", "\x89\x84\x9e\x0c\x0a\x00\x00\x8b\x45\xfc", 
	//	"xxxxxxxxxx", 7, GetOpponentCharIDCharSelectMP);

	GetPlayersCharIDVersusScreenSPJmpBackAddr = HookManager::SetHook("GetPlayersCharIDVersusScreenSP", "\x89\x87\x48\x16\x00\x00\x8b\x83\x44\x06\x00\x00", 
		"xxxxxxxxxxxx", 6, GetPlayersCharIDVersusScreenSP);
	GetCPUsCharIDVersusScreenSPJmpBackAddr = HookManager::SetHook("GetCPUsCharIDVersusScreenSP", "\x89\x86\x48\x16\x00\x00\x8b\x84\x0a\x44\x06\x00\x00",
		"xxxxxxxxxxxxx", 6, GetCPUsCharIDVersusScreenSP);
	GetP1CharIDVersusScreenMPJmpBackAddr = HookManager::SetHook("GetP1CharIDVersusScreenMP", "\x89\x08\x6a\x00\x8b\xcf", "xxxxxx", 6, GetP1CharIDVersusScreenMP);
	GetP2CharIDVersusScreenMPJmpBackAddr = HookManager::SetHook("GetP2CharIDVersusScreenMP", "\x89\x08\x6a\x01\x8b\xcf", "xxxxxx", 6, GetP2CharIDVersusScreenMP);

	GetP1CharIDTrainingModeCharIDChangeJmpBackAddr = HookManager::SetHook("GetP1CharIDTrainingModeCharChange", "\x8b\x4d\xe4\x89\x08\x68", "xxxxxx", 5, GetP1CharIDTrainingModeCharIDChange);

	GetP2CharIDTrainingModeCharIDChangeJmpBackAddr = HookManager::SetHook("GetP2CharIDTrainingModeCharChange", 
		"\x57\x89\x30\xe8\x00\x00\x00\x00\x8b\xc8", "xxxx????xx", 8, GetP2CharIDTrainingModeCharIDChange, false);
	restoredGetP2CharIDTrainingModeCharAddr = GetP2CharIDTrainingModeCharIDChangeJmpBackAddr + HookManager::GetBytesFromAddr("GetP2CharIDTrainingModeCharChange", 4, 4); 
	HookManager::ActivateHook("GetP2CharIDTrainingModeCharChange");

	GetCharObjPointersJmpBackAddr = HookManager::SetHook("GetCharObjPointers", "\x89\xb4\xb8\xe8\x25\x00\x00\x8b\x45\xfc", "xxxxxxxxxx", 7, GetCharObjPointers);

	ForceBloomOnJmpBackAddr = HookManager::SetHook("ForceBloomOn", "\x83\xfe\x15\x75", "xxxx", 5, ForceBloomOn, false);
	restoredForceBloomOffAddr = ForceBloomOnJmpBackAddr + HookManager::GetBytesFromAddr("ForceBloomOn", 4, 1);
	HookManager::ActivateHook("ForceBloomOn");

	GetIsP1CPUJmpBackAddr = HookManager::SetHook("GetIsP1CPU", "\x89\xb8\x88\x16\x00\x00\x8b\x83\xa4\x64\x40\x00", "xxxxxxxxxxxx", 6, GetIsP1CPU);

	/*GetOpponentSteamIDAtCharSelectJmpBackAddr = HookManager::SetHook("GetOpponentSteamIDAtCharSelect", "\x89\x06\x8b\x47\x04\x8b\xcf", "xxxxxxx", 5, GetOpponentSteamIDAtCharSelect);*/

	//character selection screen, showing custom palette
	//GetCharacterSelScreenPalPointerJmpBackAddr = HookManager::SetHook("GetCharacterSelScreenPalPointer", "\x8b\xc8\xe8\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x6a\x0c",
	//	"xxx????x????xx", 7, GetCharacterSelScreenPalPointer, false);
	//restoredCallAddr = GetCharacterSelScreenPalPointerJmpBackAddr + HookManager::GetBytesFromAddr("GetCharacterSelScreenPalPointer", 3, 4); //restore callable address
	//HookManager::ActivateHook("GetCharacterSelScreenPalPointer");

	//GetP1P2CharSelectPalettePointerIndexesJmpBackAddr = HookManager::SetHook("GetP1P2CharSelectPalettePointerIndexes",
	//	"\x89\x8e\x14\x0a\x00\x00\x89\x8e\x2c\x0a\x00\x00", "xxxxxxxxxxxx", 6, GetP1P2CharSelectPalettePointerIndexes);

	return true;
}

void __declspec(naked)GetP1CharIDTrainingModeCharIDChange()
{
	__asm pushad
	LOG(2, "GetP1CharIDTrainingModeCharIDChange\n");
	Containers::gameVals.P1DefaultPalBackup[0] = "";
	Containers::tempVals.paledit_show_sel_by_highlight = false;
	Containers::tempVals.paledit_show_placeholder = false;
	__asm popad

	__asm
	{
		mov ecx, [ebp - 1Ch]
		cmp[Containers::gameVals.isP1CPU], 1
		je PLAYER2
PLAYER1:
		cmp Containers::gameVals.P1_selectedCharID, ecx
		JNE CHANGE1
KEEP1: 
		push ecx
		push eax
		mov ecx, Containers::gameVals.origP1PaletteIndex //*Containers::gameVals.P1PaletteIndex = Containers::gameVals.origP1PaletteIndex;
		mov eax, Containers::gameVals.P1PaletteIndex
		mov [eax], ecx
		pop eax
		pop ecx
		jmp[EXIT]
CHANGE1:
		mov Containers::gameVals.P1_selectedCharID, ecx
		mov Containers::gameVals.P1_selected_custom_pal, 0
		mov Containers::gameVals.origP1PaletteIndex, 0
		jmp[EXIT]
PLAYER2:
		cmp Containers::gameVals.P2_selectedCharID, ecx
		JNE CHANGE2
KEEP2:
		push ecx
		push eax
		mov ecx, Containers::gameVals.origP2PaletteIndex //*Containers::gameVals.P2PaletteIndex = Containers::gameVals.origP2PaletteIndex;
		mov eax, Containers::gameVals.P2PaletteIndex
		mov [eax], ecx
		pop eax
		pop ecx
		jmp[EXIT]
CHANGE2:
		mov Containers::gameVals.P2_selectedCharID, ecx
		mov Containers::gameVals.P2_selected_custom_pal, 0
		mov Containers::gameVals.origP2PaletteIndex, 0
EXIT:
		mov[eax], ecx
		jmp[GetP1CharIDTrainingModeCharIDChangeJmpBackAddr]
	}
}

void __declspec(naked)GetP2CharIDTrainingModeCharIDChange()
{
	__asm pushad
	LOG(2, "GetP2CharIDTrainingModeCharIDChange\n");
	Containers::gameVals.P2DefaultPalBackup[0] = "";
	Containers::tempVals.paledit_show_sel_by_highlight = false;
	Containers::tempVals.paledit_show_placeholder = false;
	__asm popad

	__asm
	{
		push edi
		mov [eax], esi
		cmp[Containers::gameVals.isP1CPU], 1
		je PLAYER1
		cmp Containers::gameVals.P2_selectedCharID, esi
		JNE CHANGE2
KEEP2:
		push esi
		push eax
		mov esi, Containers::gameVals.origP2PaletteIndex //*Containers::gameVals.P2PaletteIndex = Containers::gameVals.origP2PaletteIndex;
		mov eax, Containers::gameVals.P2PaletteIndex
		mov [eax], esi
		pop eax
		pop esi
		jmp[EXIT]
CHANGE2:
		mov Containers::gameVals.P2_selectedCharID, esi
		mov Containers::gameVals.P2_selected_custom_pal, 0
		mov Containers::gameVals.origP2PaletteIndex, 0
		jmp[EXIT]
PLAYER1:
		cmp Containers::gameVals.P1_selectedCharID, esi
		JNE CHANGE1
KEEP1:
		push esi
		push eax
		mov esi, Containers::gameVals.origP1PaletteIndex //*Containers::gameVals.P1PaletteIndex = Containers::gameVals.origP1PaletteIndex;
		mov eax, Containers::gameVals.P1PaletteIndex
		mov[eax], esi
		pop eax
		pop esi
		jmp[EXIT]
CHANGE1:
		mov Containers::gameVals.P1_selectedCharID, esi
		mov Containers::gameVals.P1_selected_custom_pal, 0
		mov Containers::gameVals.origP1PaletteIndex, 0
EXIT:
		call restoredGetP2CharIDTrainingModeCharAddr
		jmp[GetP2CharIDTrainingModeCharIDChangeJmpBackAddr]
	}
}

void DereferenceP1P2PalettePointers()
{
	LOG(2, "DereferenceP1P2PalettePointers\n");

	LOG(2, "P1PaletteBase: 0x%x, P1PaletteIndex: 0x%x, P2PaletteBase: 0x%x, P2PaletteIndex: 0x%x\n",
		Containers::gameVals.P1PaletteBase, Containers::gameVals.P1PaletteIndex, 
		Containers::gameVals.P2PaletteBase, Containers::gameVals.P2PaletteIndex);
	//DEBUGDereferencePalettePointer(P1Palette, *P1PaletteIndex);
	//DereferencePalettePointer1(P1Palette, *P1PaletteIndex);
	Containers::gameVals.P1CurPalette = getPaletteArray(Containers::gameVals.P1PaletteBase, *Containers::gameVals.P1PaletteIndex, 0);
	LOG(2, "P1Palette: 0x%x\n", Containers::gameVals.P1CurPalette);

	Containers::gameVals.P2CurPalette = getPaletteArray(Containers::gameVals.P2PaletteBase, *Containers::gameVals.P2PaletteIndex, 0);
	LOG(2, "P2Palette: 0x%x\n", Containers::gameVals.P2CurPalette);

	Containers::gameVals.P1Palette3 = getPaletteArray(Containers::gameVals.P1PaletteBase, 2, 0);
	LOG(2, "P1Palette3: 0x%x\n", Containers::gameVals.P1Palette3);

	Containers::gameVals.P1Palette4 = getPaletteArray(Containers::gameVals.P1PaletteBase, 3, 0);
	LOG(2, "P1Palette4: 0x%x\n", Containers::gameVals.P1Palette4);

	Containers::gameVals.P2Palette5 = getPaletteArray(Containers::gameVals.P2PaletteBase, 4, 0);
	LOG(2, "P2Palette5: 0x%x\n", Containers::gameVals.P2Palette5);

	Containers::gameVals.P2Palette6 = getPaletteArray(Containers::gameVals.P2PaletteBase, 5, 0);
	LOG(2, "P2Palette6: 0x%x\n", Containers::gameVals.P2Palette6);
	
	//DereferencePalettePointer2(P2Palette, *P2PaletteIndex);
	LOG(2, "P1PalettePointer: 0x%x , index: %d -- P2PalettePointer: 0x%x , index: %d\n",
		Containers::gameVals.P1CurPalette, *Containers::gameVals.P1PaletteIndex, 
		Containers::gameVals.P2CurPalette, *Containers::gameVals.P2PaletteIndex);
	LOG(2, "P1PalettePointer contains: %s\n", RawMemoryArrayToString((unsigned char*)Containers::gameVals.P1CurPalette, 30));
	LOG(2, "P2PalettePointer contains: %s\n", RawMemoryArrayToString((unsigned char*)Containers::gameVals.P2CurPalette, 30));
}

//fileID 0 to get sprite palette, fileID 1 to get first effect file, fileID 2 to get second effect file etc...
DWORD* getPaletteArray(DWORD* base, DWORD palIndex, DWORD fileID)
{
	return (DWORD*)*((DWORD*)*(base + 1) + (palIndex * 8) + fileID) + 8;
}

DWORD* getCharSelPaletteArray(DWORD* base, DWORD pid, bool isP1)
{
	LOG(2, "getCharSelPaletteArray\n");
	//LOG(2, "\tbase: 0x%x\n", base);
	//LOG(2, "\tpid: %d\n", pid);

	int charIndex = 0;
	int palIndex = 0;
	if (isP1)
	{
		charIndex = Containers::gameVals.P1_selectedCharID;
		palIndex = *P1CharSelectPalIndex;
	}
	else
	{
		charIndex = Containers::gameVals.P2_selectedCharID;
		palIndex = *P2CharSelectPalIndex;
	}

	//LOG(2, "(*base): 0x%x\n", (*base));
	//LOG(2, "(DWORD*)((*base) + 0x3D8 + (charIndex * 0x4)): 0x%x\n", (DWORD*)((*base) + 0x3D8 + (charIndex * 0x4)));
	//LOG(2, "*(DWORD*)((*base) + 0x3D8 + (charIndex * 0x4)): 0x%x\n", *(DWORD*)((*base) + 0x3D8 + (charIndex * 0x4)));
	//LOG(2, "(DWORD*)(*(DWORD*)((*base) + 0x3D8 + (charIndex * 0x4)) + 0x4): 0x%x\n", (DWORD*)(*(DWORD*)((*base) + 0x3D8 + (charIndex * 0x4)) + 0x4));
	//LOG(2, "*(DWORD*)(*(DWORD*)((*base) + 0x3D8 + (charIndex * 0x4)) + 0x4): 0x%x\n", *(DWORD*)(*(DWORD*)((*base) + 0x3D8 + (charIndex * 0x4)) + 0x4));
	//LOG(2, "(DWORD*)(*(DWORD*)(*(DWORD*)((*base) + 0x3D8 + (charIndex * 0x4)) + 0x4) + (palIndex * 0x4)): 0x%x\n",
	//	(DWORD*)(*(DWORD*)(*(DWORD*)((*base) + 0x3D8 + (charIndex * 0x4)) + 0x4) + (palIndex * 0x4)));

	//LOG(2, "*(DWORD*)(*(DWORD*)(*(DWORD*)((*base) + 0x3D8 + (charIndex * 0x4)) + 0x4) + (palIndex * 0x4)): 0x%x\n",
	//	*(DWORD*)(*(DWORD*)(*(DWORD*)((*base) + 0x3D8 + (charIndex * 0x4)) + 0x4) + (palIndex * 0x4)));

	//LOG(2, "*(DWORD*)(*(DWORD*)(*(DWORD*)((*base) + 0x3D8 + (charIndex * 0x4)) + 0x4) + (palIndex * 0x4)) + 0x820: 0x%x\n",
	//	*(DWORD*)(*(DWORD*)(*(DWORD*)((*base) + 0x3D8 + (charIndex * 0x4)) + 0x4) + (palIndex * 0x4)) + 0x820);

	return (DWORD*)(*(DWORD*)(*(DWORD*)(*(DWORD*)((*base) + 0x3D8 + (charIndex * 0x4)) + 0x4) + (palIndex * 0x4)) + 0x820);
}

void DereferencePalettePointer1(DWORD* palettePointer, DWORD paletteIndex)
{
	__asm
	{
		mov eax, [palettePointer] //lvl0 pointer
		add eax, 4
		mov ebx, [eax] //lvl1 pointer 
		mov esi, [paletteIndex]
		mov eax, 20h // mul by 0x20
		mul esi
		mov ecx, [ebx + eax] //lvl2 pointer
		add ecx, 20h // push it by 0x20 offset
		mov[Containers::gameVals.P1CurPalette], ecx
	}
}

void DereferencePalettePointer2(DWORD* palettePointer, DWORD paletteIndex)
{
	__asm
	{
		mov eax, [palettePointer] //lvl0 pointer
		add eax, 4
		mov ebx, [eax] //lvl1 pointer 
		mov esi, [paletteIndex]
		mov eax, 20h // mul by 0x20
		mul esi
		mov ecx, [ebx + eax] //lvl2 pointer
		add ecx, 20h // push it by 0x20 offset
		mov[Containers::gameVals.P2CurPalette], ecx
	}
}

void DEBUGDereferencePalettePointer1(DWORD* palettePointer, DWORD paletteIndex)
{
	int temp;

	__asm mov eax, [palettePointer] //lvl0
	__asm add eax, 4

	__asm mov[temp], eax // extra
	__asm push eax//extra
	LOG(2, "lvl0 palettePointer: 0x%x\n", temp);//extra
	__asm pop eax//extra

	__asm mov ebx, [eax] //lvl1

	__asm mov[temp], ebx // extra
	__asm push ebx //extra
	LOG(2, "lvl1 palettePointer: 0x%x\n", temp); //extra
	__asm pop ebx //extra

	__asm mov esi, [paletteIndex]
	__asm mov eax, 20h // mul by 0x20
	__asm mul esi
	__asm mov ecx, [ebx + eax] //lvl2

	__asm mov[temp], ecx // extra
	__asm push ecx //extra
	LOG(2, "lvl2 palettePointer: 0x%x\n", temp); //extra
	__asm pop ecx //extra

	__asm add ecx, 20h // push it by 0x20 offset

	__asm mov[temp], ecx // extra
	__asm push ecx //extra
	LOG(2, "lvl2 pointer pushed by 0x20: 0x%x\n", temp); //extra
	__asm pop ecx //extra

	__asm mov[Containers::gameVals.P1CurPalette], ecx

	__asm mov eax, [Containers::gameVals.P1CurPalette]// extra
	__asm mov[temp], eax //extra
	LOG(2, "lvl palettePointer: 0x%x\n", temp); //extra
}

void __declspec(naked)GetP2CharIDVersusScreenMP()
{
	__asm pushad
	LOG(2, "GetP2CharIDVersusScreenMP\n");
	__asm popad
	__asm
	{
		mov Containers::gameVals.P2_selectedCharID, ecx
		mov [eax], ecx
		push 1
		mov ecx, edi
		jmp[GetP2CharIDVersusScreenMPJmpBackAddr]
	}
}

void __declspec(naked)GetP1CharIDVersusScreenMP()
{
	__asm pushad
	LOG(2, "GetP1CharIDVersusScreenMP\n");
	__asm popad

	__asm
	{
		mov Containers::gameVals.P1_selectedCharID, ecx
		mov [eax], ecx
		push 0
		mov ecx, edi
		jmp[GetP1CharIDVersusScreenMPJmpBackAddr]
	}
}

void __declspec(naked)GetCPUsCharIDVersusScreenSP()
{
#ifndef RELEASE_VER
	__asm pushad
	__asm mov debug_id, eax
	LOG(2, "GetCPUsCharIDVersusScreenSP, id:%d\n", debug_id);
	__asm popad
#endif

	__asm
	{
		cmp Containers::gameVals.isP1CPU, 1
		jne SET_TO_PLAYER2
		mov Containers::gameVals.P1_selectedCharID, eax
		jmp EXIT
SET_TO_PLAYER2:
		mov Containers::gameVals.P2_selectedCharID, eax
EXIT:
		mov[esi + 1648h], eax
		jmp[GetCPUsCharIDVersusScreenSPJmpBackAddr]
	}
}

void __declspec(naked)GetPlayersCharIDVersusScreenSP()
{
#ifndef RELEASE_VER
	__asm pushad
	__asm mov debug_id, eax
	LOG(2, "GetPlayersCharIDVersusScreenSP, id:%d\n", debug_id);
	__asm popad
#endif

	__asm
	{
		cmp Containers::gameVals.isP1CPU, 1
		je SET_TO_PLAYER2
		cmp Containers::tempVals.PlayersCharIDVersusScreenCounter, 1
		je SET_TO_PLAYER2
		mov Containers::gameVals.P1_selectedCharID, eax
		jmp EXIT
SET_TO_PLAYER2:
		mov Containers::gameVals.P2_selectedCharID, eax
EXIT:
		mov[edi + 1648h], eax
		jmp[GetPlayersCharIDVersusScreenSPJmpBackAddr]
	}
}

//void __declspec(naked)GetOpponentCharIDCharSelectMP()
//{
//	//!!!!!!!WARNING!!!!!!!!
//	//thisPlayerNum is not set yet when this function runs!!!
//
//	__asm pushad
//	LOG(2, "GetOpponentCharIDCharSelectMP\n");
//	__asm popad
//	__asm
//	{
//		pushad
//		cmp thisPlayerNum, 1
//		jne PLAYER2
//PLAYER1:
//		mov Containers::gameVals.P1_selectedCharID, eax
//		jmp EXIT
//PLAYER2:
//		mov Containers::gameVals.P2_selectedCharID, eax
//EXIT:
//		popad
//		mov[esi + ebx * 4 + 0A0Ch], eax
//		jmp[GetOpponentCharIDCharSelectMPJmpBackAddr]
//	}
//}

void __declspec(naked)GetP1P2CharSelectPalettePointerIndexes()
{
	__asm pushad
	LOG(2, "GetP1P2CharSelectPalettePointerIndexes\n");
	__asm popad

	__asm
	{
		pushad
		add esi, 0A14h
		mov P1CharSelectPalIndex, esi
		add esi, 4h
		mov P2CharSelectPalIndex, esi
		popad

		mov[esi + 0A14h], ecx
		jmp[GetP1P2CharSelectPalettePointerIndexesJmpBackAddr]
	}
}

void __declspec(naked)GetP1P2CharIDCharSelectSP()
{
	int curSelection;

	__asm pushad
	LOG(7, "GetP1P2CharIDCharSelectSP\n");
	OverwriteStagesList();
	__asm popad

	__asm
	{
		pushad
		mov curSelection, eax
		cmp edi, 0
		jne PLAYER2
PLAYER1:
	}
	//reset custom palette selection if player hovers over an another character, so we avoid array out of bounds
	Containers::gameVals.thisPlayerNum = 1;
	if (curSelection != Containers::gameVals.P1_selectedCharID)
		Containers::gameVals.P1_selected_custom_pal = 0;
	__asm
	{
		mov Containers::gameVals.P1_selectedCharID, eax
		jmp EXIT
PLAYER2:
	}
	//reset custom palette selection if player hovers over an another character, so we avoid array out of bounds
	Containers::gameVals.thisPlayerNum = 2;
	if (curSelection != Containers::gameVals.P2_selectedCharID)
		Containers::gameVals.P2_selected_custom_pal = 0;
	__asm
	{
		mov Containers::gameVals.P2_selectedCharID, eax
EXIT:
		popad
		mov [esi + edi * 4 + 0A0Ch], eax
		jmp[GetP1P2CharIDCharSelectSPJmpBackAddr]
	}
}

void __declspec(naked)GetGameStateCharacterSelect()
{
	__asm pushad
	LOG(2, "GetGameStateCharacterSelect\n");
	__asm popad
	
	__asm pushad

	Containers::gameVals.startMatchPalettesInit = false;
	ResettingDefaultPalettes();

	//as of right now, the opponent's steamid is not known until the match starts
	//if (Containers::gameVals.opponentSteamID)
	//	ImGuiSystem::AddLog("[system] Online match has started.\n");
	if (Containers::tempVals.opponentIDToBeDeleted)
	{
		LOG(2, "Deleting previous opponentSteamID, resetting...\n");
		delete Containers::tempVals.opponentIDToBeDeleted;
		Containers::tempVals.opponentIDToBeDeleted = 0;
	}
	__asm popad

	__asm
	{
		mov dword ptr[ebx + 10Ch], 6
		jmp[GetGameStateCharacterSelectJmpBackAddr]
	}
}

void __declspec(naked)MatchIntroStartsPlayingFunc()
{
	//this function runs whenever the camera is forcibly taken control by the game

	__asm pushad
	LOG(2, "MatchIntroStartsPlayingFunc\n");

	InitCustomGamemode();

	if (!Containers::gameVals.startMatchPalettesInit && 
		Containers::gameVals.P1_selectedCharID != -1 &&
		Containers::gameVals.P2_selectedCharID != -1)
	{
		LOG(2, "Applying custom palettes, sending to opponent...\n");

		SaveDefaultP1P2Pals();

		////prevent sending both palettes in MP, since we only want to send OUR player's palette
		ReplaceP1Palette(false);
		ReplaceP2Palette(false);
		////send now once
		SendCustomDatas();
		Containers::gameVals.startMatchPalettesInit = true;
		HandleSavedPackets();
	}
	__asm
	{
		popad
		and dword ptr[eax + 40626Ch], 0FFFFFFFDh
		jmp[MatchIntroStartsPlayingJmpBackAddr]
	}
}

void __declspec(naked)GetOwnSteamID()
{
	__asm pushad
	LOG(2, "GetOwnSteamID\n");
	__asm popad
	__asm
	{
		mov[edi], eax
		cmp Containers::gameVals.ownSteamID, 0
		jne EXIT
		mov Containers::gameVals.ownSteamID, edi
	}
	__asm pushad
	LOG(2, "OwnSteamID: 0x%x\n", *Containers::gameVals.ownSteamID);
	__asm popad
	__asm
	{
EXIT:
		mov eax, [esi + 4]
		jmp[GetOwnSteamIDJmpBackAddr]
	}
}

void __declspec(naked)StartMatchWheelOfFate()
{
	__asm pushad
	LOG(2, "StartMatchWheelOfFate\n");

	__asm
	{
		popad
		mov[ebx + 30h], 00000002
		jmp[StartMatchWheelOfFateJmpBackAddr]
	}
}

//void __declspec(naked)GetOpponentSteamID()
//{
//	static int debugVar = 0;
//
//	__asm pushad
//	LOG(2, "GetOpponentSteamID\n");
//	__asm popad
//	__asm
//	{
//		mov[ebx + 3Ch], eax
//		pushad
//		mov[debugVar], eax
//	}
//	
//	LOG(2, "\tdebugVar: 0x%x\n", debugVar);
//	__asm popad
//	__asm pushad
//	__asm
//	{
//		cmp Containers::gameVals.opponentSteamID, 0 //only run below code if opponentSteamID pointer is 0
//		jne EXIT
//		mov eax, [ebx + 3Ch]
//		mov ecx, [Containers::gameVals.ownSteamID] //ownSteamID pointer address in ecx
//		mov edx, [ecx]
//		cmp eax, edx // value of ownSteamID in edx
//		jne PLAYER2
//PLAYER1:
//		cmp Containers::gameVals.thisPlayerNum, 0
//		jne SET_OPPONENT_ID
//		mov Containers::gameVals.thisPlayerNum, 1
//		jmp EXIT
//PLAYER2:
//		cmp Containers::gameVals.thisPlayerNum, 0
//		jne SET_OPPONENT_ID
//		mov Containers::gameVals.thisPlayerNum, 2
//SET_OPPONENT_ID:
//		add ebx, 3Ch
//		mov Containers::gameVals.opponentSteamID, ebx
//	}
//	__asm pushad
//	LOG(2, "opponentPlayer steamID addr: 0x%x\n", Containers::gameVals.opponentSteamID);
//	LOG(2, "opponentPlayer steamID: 0x%x\n", *Containers::gameVals.opponentSteamID);
//	//Containers::g_interfaces.pSteamFriendsWrapper->SetPlayedWith(*opponentSteamID);
//	__asm popad
//	__asm
//	{
//EXIT:
//		popad
//		mov eax, [ecx + 04]
//		jmp[GetOpponentSteamIDJmpBackAddr]
//	}
//}

void HandleSavedPackets()
{
	LOG(2, "HandleSavedPackets\n");
	while (Containers::tempVals.tempPackets.size() > 0)
	{
#ifndef RELEASE_VER
		LOG(2, "Processing previously saved tempPackets...\n");
		ImGuiSystem::AddLog("[debug] Processing previously saved tempPacket\n");
#endif
		Containers::g_interfaces.pNetworkManager->PacketProcesser(&Containers::tempVals.tempPackets[Containers::tempVals.tempPackets.size() - 1]);
		Containers::tempVals.tempPackets.pop_back();
	}
}

void LoadPalettePacket(im_packet_internal_t *packet)
{
	LOG(2, "LoadPalettePacket\n");

	if (!Containers::gameVals.startMatchPalettesInit)
	{
		LOG(2, "PalettesInit has not finished yet, saving palette...\n");
		Containers::tempVals.tempPackets.push_back(*packet);
		return;
	}

	//ImGuiSystem::AddLog("[system] Custom palette packet received from player%d (%s)\n",
	//	packet->playernum, Containers::g_interfaces.pSteamFriendsWrapper->GetFriendPersonaName(*Containers::gameVals.opponentSteamID));
	ImGuiSystem::AddLog("[system] Custom palette packet received from player%d\n", packet->playernum);

	LOG(2, "\tReceived palette packet: \n");
	LOG(2, "\tlength1: %s\n", RawMemoryArrayToString((unsigned char*)&packet->length1, sizeof(packet->length1)));
	LOG(2, "\tlength2: %s\n", RawMemoryArrayToString((unsigned char*)&packet->length2, sizeof(packet->length2)));
	LOG(2, "\tcode: %s\n", RawMemoryArrayToString((unsigned char*)&packet->code, sizeof(packet->code)));
	LOG(2, "\ttype: %s\n", RawMemoryArrayToString((unsigned char*)&packet->type, sizeof(packet->type)));
	LOG(2, "\tplayernum: %s\n", RawMemoryArrayToString((unsigned char*)&packet->playernum, sizeof(packet->playernum)));
	LOG(2, "\tpart: %s\n", RawMemoryArrayToString((unsigned char*)&packet->part, sizeof(packet->part)));
	LOG(2, "\tdatalen: %s\n", RawMemoryArrayToString((unsigned char*)&packet->datalen, sizeof(packet->datalen)));
	LOG(2, "\tdata: %s\n", RawMemoryArrayToString((unsigned char*)&packet->data, packet->datalen));

	if (packet->playernum == 1) //player1
	{
		LOG(2, "Received P1Palette\n");
		LOG(2, "Copying to P1Palette3[0]: 0x%x\n", Containers::gameVals.P1Palette3);
		ReplacePaletteInMemory(Containers::gameVals.P1Palette3, &packet->data[0]);
		LOG(2, "Copying to P1Palette4[0]: 0x%x\n", Containers::gameVals.P1Palette4);
		ReplacePaletteInMemory(Containers::gameVals.P1Palette4, &packet->data[0]);

		if (*Containers::gameVals.P1PaletteIndex != 2) //need to switch paletteindex in order to reflect changes ingame
		{
			LOG(2, "Setting P1PaletteIndex to 2 (3rd palette)\n");
			*Containers::gameVals.P1PaletteIndex = 2;
		}
		else
		{
			LOG(2, "Setting P1PaletteIndex to 3 (4th palette)\n");
			*Containers::gameVals.P1PaletteIndex = 3;
		}
	}
	else //player2
	{
		LOG(2, "Received P2Palette\n");
		LOG(2, "Copying to P2Palette5[0]: 0x%x\n", Containers::gameVals.P2Palette5);
		ReplacePaletteInMemory(Containers::gameVals.P2Palette5, &packet->data[0]);
		LOG(2, "Copying to P2Palette6[0]: 0x%x\n", Containers::gameVals.P2Palette6);
		ReplacePaletteInMemory(Containers::gameVals.P2Palette6, &packet->data[0]);

		if (*Containers::gameVals.P2PaletteIndex != 4) //need to switch paletteindex in order to reflect changes ingame
		{
			LOG(2, "Setting P2PaletteIndex to 4 (5th palette)\n");
			*Containers::gameVals.P2PaletteIndex = 4;
		}
		else
		{
			LOG(2, "Setting P2PaletteIndex to 5 (6th palette)\n");
			*Containers::gameVals.P2PaletteIndex = 5;
		}
	}
}

void LoadEffectPacket(im_packet_internal_t *packet)
{
	LOG(2, "LoadEffectPacket\n");

	if (!Containers::gameVals.startMatchPalettesInit)
	{
		LOG(2, "PalettesInit has not finished yet, saving effect...\n");
		Containers::tempVals.tempPackets.push_back(*packet);
		return;
	}

	DWORD* P1Pal3EffectPart = getPaletteArray(Containers::gameVals.P1PaletteBase, 2, packet->part);
	DWORD* P1Pal4EffectPart = getPaletteArray(Containers::gameVals.P1PaletteBase, 3, packet->part);
	DWORD* P2Pal5EffectPart = getPaletteArray(Containers::gameVals.P2PaletteBase, 4, packet->part);
	DWORD* P2Pal6EffectPart = getPaletteArray(Containers::gameVals.P2PaletteBase, 5, packet->part);

	//if(strcmp((char*)packet->data, CURPALETTE) == 0)

#ifndef RELEASE_VER
	//ImGuiSystem::AddLog("[debug] Custom effect packet part%d received from player%d (%s)\n",
	//	packet->part, packet->playernum, Containers::g_interfaces.pSteamFriendsWrapper->GetFriendPersonaName(*Containers::gameVals.opponentSteamID));
	ImGuiSystem::AddLog("[debug] Custom effect packet part%d received from player%d \n", packet->part, packet->playernum);
#endif

	LOG(2, "\tReceived effect packet: \n");
	LOG(2, "\tlength1: %s\n", RawMemoryArrayToString((unsigned char*)&packet->length1, sizeof(packet->length1)));
	LOG(2, "\tlength2: %s\n", RawMemoryArrayToString((unsigned char*)&packet->length2, sizeof(packet->length2)));
	LOG(2, "\tcode: %s\n", RawMemoryArrayToString((unsigned char*)&packet->code, sizeof(packet->code)));
	LOG(2, "\ttype: %s\n", RawMemoryArrayToString((unsigned char*)&packet->type, sizeof(packet->type)));
	LOG(2, "\tplayernum: %s\n", RawMemoryArrayToString((unsigned char*)&packet->playernum, sizeof(packet->playernum)));
	LOG(2, "\tpart: %s\n", RawMemoryArrayToString((unsigned char*)&packet->part, sizeof(packet->part)));
	LOG(2, "\tdatalen: %s\n", RawMemoryArrayToString((unsigned char*)&packet->datalen, sizeof(packet->datalen)));
	LOG(2, "\tdata: %s\n", RawMemoryArrayToString((unsigned char*)&packet->data, packet->datalen));

	if (packet->playernum == 1) //player1
	{
		LOG(2, "Received P1Effect\n");
		LOG(2, "Copying to P1Pal3EffectPart%d: 0x%x\n", packet->part, P1Pal3EffectPart);
		ReplacePaletteInMemory(P1Pal3EffectPart, &packet->data[0]);
		LOG(2, "Copying to P1Pal4EffectPart%d: 0x%x\n", packet->part, P1Pal4EffectPart);
		ReplacePaletteInMemory(P1Pal4EffectPart, &packet->data[0]);
	}
	else //player2
	{
		LOG(2, "Received P2Effect\n");
		LOG(2, "Copying to P2Pal5EffectPart%d: 0x%x\n", packet->part, P2Pal5EffectPart);
		ReplacePaletteInMemory(P2Pal5EffectPart, &packet->data[0]);
		LOG(2, "Copying to P2Pal6EffectPart%d: 0x%x\n", packet->part, P2Pal6EffectPart);
		ReplacePaletteInMemory(P2Pal6EffectPart, &packet->data[0]);
	}
}

void LoadBloomPacket(im_packet_internal_t *packet)
{
	LOG(2, "LoadBloomPacket\n");

	if (!Containers::gameVals.startMatchPalettesInit)
	{
		LOG(2, "PalettesInit has not finished yet, saving bloom flag...\n");
		Containers::tempVals.tempPackets.push_back(*packet);
		return;
	}

//#ifndef RELEASE_VER
//	ImGuiSystem::AddLog("[debug] Custom bloom packet part%d received from player%d (%s)\n",
//		packet->part, packet->playernum, Containers::g_interfaces.pSteamFriendsWrapper->GetFriendPersonaName(*Containers::gameVals.opponentSteamID));
//#endif
//
//	LOG(2, "\tReceived effect packet: \n");
//	LOG(2, "\tlength1: %s\n", RawMemoryArrayToString((unsigned char*)&packet->length1, sizeof(packet->length1)));
//	LOG(2, "\tlength2: %s\n", RawMemoryArrayToString((unsigned char*)&packet->length2, sizeof(packet->length2)));
//	LOG(2, "\tcode: %s\n", RawMemoryArrayToString((unsigned char*)&packet->code, sizeof(packet->code)));
//	LOG(2, "\ttype: %s\n", RawMemoryArrayToString((unsigned char*)&packet->type, sizeof(packet->type)));
//	LOG(2, "\tplayernum: %s\n", RawMemoryArrayToString((unsigned char*)&packet->playernum, sizeof(packet->playernum)));
//	LOG(2, "\tpart: %s\n", RawMemoryArrayToString((unsigned char*)&packet->part, sizeof(packet->part)));
//	LOG(2, "\tdatalen: %s\n", RawMemoryArrayToString((unsigned char*)&packet->datalen, sizeof(packet->datalen)));
//	LOG(2, "\tdata: %s\n", RawMemoryArrayToString((unsigned char*)&packet->data, packet->datalen));

	if (packet->playernum == 1) //player1
	{
		LOG(2, "Received P1 bloom effect\n");
		Containers::gameVals.isP1BloomOn = packet->data[0];
	}
	else //player2
	{
		LOG(2, "Received P2 bloom effect\n");
		Containers::gameVals.isP2BloomOn = packet->data[0];
	}
}

void SendCustomEffectBloom()
{
	LOG(2, "SendCustomEffectBloom\n");

	int isBloomOn = 0;
	int otherPlayerNum = 0;
	EP2PSend sendtype = k_EP2PSendUnreliable;
	im_packet_t packet;

	if (Containers::gameVals.thisPlayerNum == 1)
	{
		if (Containers::gameVals.P1_selectedCharID != -1)
			isBloomOn = Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID][Containers::gameVals.P1_selected_custom_pal][9][0];
		otherPlayerNum = 2;
	}
	else
	{
		if (Containers::gameVals.P2_selectedCharID != -1)
			isBloomOn = Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID][Containers::gameVals.P2_selected_custom_pal][9][0];
		otherPlayerNum = 1;
	}

	packet.playernum = Containers::gameVals.thisPlayerNum;
	packet.type = packetType_bloom;
	packet.datalen = 1;
	packet.part = 1;
	packet.data[0] = isBloomOn;

	bool ret = Containers::g_interfaces.pNetworkManager->SendPacket(Containers::gameVals.opponentSteamID, &packet);
#ifndef RELEASE_VER
	if (ret)
	{
		//ImGuiSystem::AddLog("[system] Custom bloom packet sent to player%d (%s)\n",
		//	otherPlayerNum, Containers::g_interfaces.pSteamFriendsWrapper->GetFriendPersonaName(*Containers::gameVals.opponentSteamID));
		ImGuiSystem::AddLog("[system] Custom bloom packet sent to player%d\n", otherPlayerNum);
	}
#endif

}

void OverwriteStagesList()
{
	if (!Containers::gameVals.charSelectInit && 
		(*Containers::gameVals.pGameMode == GAME_MODE_ONLINE || 
		*Containers::gameVals.pGameMode == GAME_MODE_TRAINING ||
		*Containers::gameVals.pGameMode == GAME_MODE_VERSUS))
	{
		LOG(2, "OverwriteStagesList\n");
		memcpy(Containers::gameVals.stageListMemory, allstagesunlockedmemory, ALL_STAGES_UNLOCKED_MEMORY_SIZE * sizeof(char));
		Containers::gameVals.charSelectInit = true;
	}
}

void SendCustomPalette()
{
	LOG(2, "SendCustomPalette\n");

	DWORD* PalettePointer;
	int otherPlayerNum = 0;
	EP2PSend sendtype = k_EP2PSendUnreliable;
	im_packet_t packet;

	if (Containers::gameVals.thisPlayerNum == 1)
	{
		PalettePointer = Containers::gameVals.P1CurPalette;
		otherPlayerNum = 2;
	}
	else
	{
		PalettePointer = Containers::gameVals.P2CurPalette;
		otherPlayerNum = 1;
	}

	packet.playernum = Containers::gameVals.thisPlayerNum;
	packet.type = packetType_palette;
	packet.datalen = PALETTE_DATALEN;
	packet.part = 1;
	memcpy(&packet.data[0], PalettePointer, PALETTE_DATALEN);

	bool ret = Containers::g_interfaces.pNetworkManager->SendPacket(Containers::gameVals.opponentSteamID, &packet);
	if (ret)
	{
		//ImGuiSystem::AddLog("[system] Custom palette packet sent to player%d (%s)\n",
		//	otherPlayerNum, Containers::g_interfaces.pSteamFriendsWrapper->GetFriendPersonaName(*Containers::gameVals.opponentSteamID));
		ImGuiSystem::AddLog("[system] Custom palette packet sent to player%d\n", otherPlayerNum);
	}
}

void SendCustomEffects()
{
	LOG(2, "SendCustomEffect\n");

	DWORD* CurPalEffectPart;
	int otherPlayerNum = 0;
	EP2PSend sendtype = k_EP2PSendUnreliable;
	im_packet_t packet;

	for (int i = 1; i < 8; i++)
	{
		if (Containers::gameVals.thisPlayerNum == 1)
		{
			CurPalEffectPart = getPaletteArray(Containers::gameVals.P1PaletteBase, *Containers::gameVals.P1PaletteIndex, i);
			otherPlayerNum = 2;
		}
		else
		{
			CurPalEffectPart = getPaletteArray(Containers::gameVals.P2PaletteBase, *Containers::gameVals.P2PaletteIndex, i);
			otherPlayerNum = 1;
		}

		packet.playernum = Containers::gameVals.thisPlayerNum;
		packet.type = packetType_effect;
		packet.datalen = PALETTE_DATALEN;
		packet.part = i;
		memcpy(&packet.data[0], CurPalEffectPart, PALETTE_DATALEN);

		bool ret = Containers::g_interfaces.pNetworkManager->SendPacket(Containers::gameVals.opponentSteamID, &packet);
#ifndef RELEASE_VER
		if (ret)
		{
			//ImGuiSystem::AddLog("[debug] Custom effect packets sent to player%d (%s)\n",
			//	otherPlayerNum, Containers::g_interfaces.pSteamFriendsWrapper->GetFriendPersonaName(*Containers::gameVals.opponentSteamID));
			ImGuiSystem::AddLog("[debug] Custom effect packets sent to player%d\n", otherPlayerNum);
		}
#endif
	}
}

void SendCustomDatas()
{
	LOG(2, "SendCustomDatas\n");
	if (Containers::gameVals.opponentSteamID == 0 || Containers::gameVals.thisPlayerNum == 0)
	{
		LOG(2, "opponentSteamID or thisPlayerNum is 0\n");
		return;
	}

	LOG(2, "SendPalettes to 0x%x\n", *Containers::gameVals.opponentSteamID);
	LOG(2, "ThisPlayer = %d\n", Containers::gameVals.thisPlayerNum);

	SendCustomPalette();
	SendCustomEffects();
	SendCustomEffectBloom();
}

//single player
void __declspec(naked) GetPlayersPaletteIndexSPPointer()
{
	static int counter = 0;
#ifndef RELEASE_VER
	__asm pushad
	__asm 
	{
		mov eax, edi
		add eax, 1650h
		mov debug_id, eax
	}
	LOG(2, "GetPlayersPaletteIndexSPPointer, addr:0x%x\n", debug_id);
	__asm popad
#endif

	__asm
	{
		push eax
		mov eax, edi // <---- esi to edi fixed here!!!!!!!!!!!!!!!!!!!!!!!
		add eax, 1650h
		cmp Containers::gameVals.isP1CPU, 1
		je SET_TO_PLAYER2
		cmp Containers::tempVals.PlayersCharIDVersusScreenCounter, 1
		je SET_TO_PLAYER2
		mov Containers::gameVals.P1PaletteIndex, eax //grab address
		pop eax
		mov Containers::gameVals.origP1PaletteIndex, eax //get value of paletteIndex
		jmp EXIT
SET_TO_PLAYER2:
		mov Containers::gameVals.P2PaletteIndex, eax //grab address
		pop eax
		mov Containers::gameVals.origP2PaletteIndex, eax //get value of paletteIndex
EXIT:
		add Containers::tempVals.PlayersCharIDVersusScreenCounter, 1
		mov[edi + 1650h], eax //original opcodes here
		jmp[GetPlayersPaletteIndexSPJmpBackAddr] //jump back
	}
}

//single player
void __declspec(naked) GetCPUsPaletteIndexSPPointer()
{
#ifndef RELEASE_VER
	__asm pushad
	__asm
	{
		mov eax, esi
		add eax, 1650h
		mov debug_id, eax
	}
	LOG(2, "GetCPUsPaletteIndexSPPointer, addr:0x%x\n", debug_id);
	__asm popad
#endif

	__asm
	{
		push eax
		mov eax, esi
		add eax, 1650h
		cmp Containers::gameVals.isP1CPU, 1
		jne SET_TO_PLAYER2
		mov Containers::gameVals.P1PaletteIndex, eax
		pop eax
		mov Containers::gameVals.origP2PaletteIndex, eax //get value of paletteIndex
		jmp EXIT
SET_TO_PLAYER2:
		mov Containers::gameVals.P2PaletteIndex, eax //grab address
		pop eax
		mov Containers::gameVals.origP2PaletteIndex, eax //get value of paletteIndex
EXIT:
		mov[esi + 1650h], eax //original opcodes here
		jmp[GetCPUsPaletteIndexSPJmpBackAddr] //jump back
	}
}

void __declspec(naked) GetP1PaletteIndexMPPointer()
{
	__asm pushad
	LOG(2, "GetP1PaletteIndexMPPointer\n");
	__asm popad

	__asm
	{
		push eax
		add eax, 8
		mov[Containers::gameVals.P1PaletteIndex], eax
		pop eax
		//..
		mov[eax + 8], ecx //orig
		movzx esi, byte ptr[ebx + 000003A8h]
		jmp[P1PaletteIndexMPJmpBackAddr] //jump back
	}
}

void __declspec(naked) GetP2PaletteIndexMPPointer()
{
	__asm pushad
	LOG(2, "GetP2PaletteIndexMPPointer\n");
	__asm popad

	__asm
	{
		push eax
		add eax, 8
		mov[Containers::gameVals.P2PaletteIndex], eax
		pop eax
		//..
		mov[eax + 8], ecx
		movzx esi, byte ptr[ebx + 00000414h]
		jmp[P2PaletteIndexMPJmpBackAddr] //jump back
	}
}

void __declspec(naked) GrabP1P2PalettePointers()
{
	static int counter = 0;

	__asm pushad
	LOG(2, "GrabP1P2PalettePointers\n");
	__asm popad

	__asm
	{
		//dostuffs here
		pushad // save registers

		cmp DWORD PTR counter, 0
		jne SHORT COUNTERCOMPARE2
		mov DWORD PTR Containers::gameVals.P1PaletteBase, eax
		jmp SHORT COUNTERCOMPARE3
COUNTERCOMPARE2:
		cmp DWORD PTR counter, 1
		jne SHORT COUNTERCOMPARE3
		mov DWORD PTR Containers::gameVals.P2PaletteBase, eax
COUNTERCOMPARE3:
		mov eax, DWORD PTR counter
		add eax, 1
		mov DWORD PTR counter, eax
		cmp DWORD PTR counter, 3
		jne SHORT EXIT
EXIT:
	}
	if (counter == 3)
	{
		if (Containers::gameVals.P1PaletteIndex != 0 && Containers::gameVals.P2PaletteIndex != 0)
		{
			//backup the values of palette indexes as we will switch them, so we know what they were originally
			if (Containers::gameVals.startMatchPalettesInit == false)
			{
				Containers::gameVals.origP1PaletteIndex = *Containers::gameVals.P1PaletteIndex;
				Containers::gameVals.origP2PaletteIndex = *Containers::gameVals.P2PaletteIndex;
			}
			DereferenceP1P2PalettePointers();
		}
		counter = 0;
	}
	__asm
	{
		popad // restore registers
		//...
		mov[ecx + 830h], eax //original opcodes here
		jmp[GrabP1P2PalettePointersJmpBackAddr] //jump back
	}
}

void CreateCustomFolders()
{
	CreatePaletteDirectories();
	//CreateInterfaceDirectories();
	//CreateCustomFilesDirectories();
}

void CreatePaletteDirectories()
{
	LOG(2, "CreatePaletteDirectories\n");

	CreateDirectory(L"BBCF_IM", NULL);
	CreateDirectory(L"BBCF_IM\\Palettes", NULL);

	std::wstring path;
	for (int i = 0; i < 36; i++)
	{
		path = std::wstring(L"BBCF_IM\\Palettes\\") + ingame_chars[i];
		CreateDirectory(path.c_str(), NULL);
	}
}

void InitCustomPaletteVector()
{
	LOG(2, "InitCustomPaletteVector\n");

	for (int i = 0; i < 36; i++)
	{
		//make the character palette array's 0th element an empty one, that will be used to set back to the default palette
		std::vector<std::string> default_item;
		default_item.push_back("Default");	//0 name
		default_item.push_back(""); // 1	palette
		default_item.push_back("");			// 2	effect1
		default_item.push_back("");			// 3	effect2
		default_item.push_back("");			// 4	effect3
		default_item.push_back("");			// 5	effect4
		default_item.push_back("");			// 6	effect5
		default_item.push_back("");			// 7	effect6
		default_item.push_back("");			// 8	effect7
		default_item.push_back("\x00");		// 9	bloom
		default_item.push_back("");			// 10	creator
		default_item.push_back("");			// 11	description

		Containers::gameVals.customPalettes[i].push_back(default_item);
	}
}

void LoadPaletteFiles()
{
	LOG(2, "LoadPaletteFiles\n");
	ImGuiSystem::AddLogSeparator();
	ImGuiSystem::AddLog("[system] Loading custom palettes...\n");

	InitCustomPaletteVector();
	LoadInternalPalettes();

	std::wstring path;
	for (int i = 0; i < 36; i++)
	{
		path = std::wstring(L"BBCF_IM\\Palettes\\") + ingame_chars[i] + L"\\*";
		LoadCustomPalettesIntoContainer(i, path.c_str());
	}
	ImGuiSystem::AddLog("[system] Finished loading custom palettes\n");
	ImGuiSystem::AddLogSeparator();
}

void LoadCustomPalettesIntoContainer(int charIndex, LPCWSTR path)
{
	//Containers::gameVals.customPalettes[charIndex][palnum][0/1]
	std::wstring wws(path);
	std::string ppath(wws.begin(), wws.end());
	LOG(2, "LoadPalettesIntoContainer %s\n", ppath);

	HANDLE hFind;
	WIN32_FIND_DATA data;

	hFind = FindFirstFile(path, &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {

			//ignore current and parent directories
			if (_tcscmp(data.cFileName, TEXT(".")) == 0 || _tcscmp(data.cFileName, TEXT("..")) == 0)
				continue;
			//recursively search subfolders
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				std::wstring subfolderpath(path);
				subfolderpath.pop_back(); //delete "*" at the end
				subfolderpath += data.cFileName;
				subfolderpath += L"\\*";
				LoadCustomPalettesIntoContainer(charIndex, subfolderpath.c_str());
				continue;
			}
				//continue;

			std::wstring ws(data.cFileName);
			std::string fileName(ws.begin(), ws.end());
			std::string fullPath(ppath);
			fullPath.erase(fullPath.end() - 1);
			fullPath += fileName;
			
			LOG(2, "\tFILE: %s", fileName.c_str());
			LOG(2, "\t\tFull path: %s\n", fullPath.c_str());
			////read binary file into string
			std::ifstream file(fullPath, std::ios::binary);
			if (!file.is_open())
			{
				LOG(2, "\tCouldn't open %s!\n", strerror(errno));
				ImGuiSystem::AddLog("[error] Unable to open '%s' : %s\n", fileName.c_str(), strerror(errno));
				continue;
			}
			if (fileName.find(".hpl") == std::string::npos)
			{
				ImGuiSystem::AddLog("[error] Unable to open '%s' : not a .hpl file\n", fileName.c_str());
				continue;
			}

			std::stringstream buffer;
			buffer << file.rdbuf();

			if (fileName.find("_effectbloom") != std::string::npos && Containers::gameVals.customPalettes[charIndex].size() > 1)
			{
				Containers::gameVals.customPalettes[charIndex][Containers::gameVals.customPalettes[charIndex].size() - 1][9].assign("\x01");
				ImGuiSystem::AddLog("[system] Loaded '%s'\n", fileName.c_str());
				continue;
			}

			//error check for palettes
			if (buffer.str().length() < PALETTE_DATALEN)
			{
				LOG(2, "ERROR, unrecognized file contents!\n");
				ImGuiSystem::AddLog("[error] '%s' unrecognized file contents, data length less than 3FCh!\n", fileName.c_str());
				continue;
			}

			std::string fileData(buffer.str(), 36, PALETTE_DATALEN);
			
			//effect file:
			if (fileName.find("_effect0") != std::string::npos)
			{
				if (Containers::gameVals.customPalettes[charIndex].size() < 2)
				{
					ImGuiSystem::AddLog("[error] '%s' has no custom character palette found to match with! Create a character palette named '%s' to load this effect file on!\n", 
						fileName.c_str(), (fileName.substr(0, fileName.rfind("_effect0")) + ".hpl").c_str());
					LOG(2, "\t\tEffect file has no custom character palette to match with!\n");
					continue;
				}

				//if previously pushed character palette's name wasnt the effect file's name
				if (Containers::gameVals.customPalettes[charIndex][Containers::gameVals.customPalettes[charIndex].size()-1][0] != fileName.substr(0, fileName.rfind("_effect0")))
				{
					ImGuiSystem::AddLog("[error] '%s' has no custom character palette found to match with! Create a character palette named '%s' to load this effect file on!\n",
						fileName.c_str(), (fileName.substr(0, fileName.rfind("_effect0")) + ".hpl").c_str());
					LOG(2, "\t\tEffect file has no custom character palette to match with!\n");
					continue;
				}

				LOG(2, "\t\tEffect file! size(): %d ", Containers::gameVals.customPalettes[charIndex].size());
				int pos = fileName.find("_effect0");
				std::string index = fileName.substr(pos + 7, 2);
				LOG(2, "index: %s ", index.c_str());
				if (!(std::istringstream(index) >> pos)) pos = -1;
				LOG(2, "pos: %d\n", pos);
				if (pos > 7 || pos == -1)
				{
					LOG(2, "ERROR, WRONG INDEX OF EFFECT FILE!\n");
					ImGuiSystem::AddLog("[error] '%s' has wrong index!\n", fileName.c_str());
					continue;
				}
				Containers::gameVals.customPalettes[charIndex][Containers::gameVals.customPalettes[charIndex].size() - 1][pos + 1] = fileData;
			}
			else //palette file:
			{
				std::vector<std::string> item;
				item.push_back(fileName.substr(0, fileName.rfind('.')));	// 0	filename without the extension
				item.push_back(fileData);	// 1	palette
				item.push_back("");			// 2	effect1
				item.push_back("");			// 3	effect2
				item.push_back("");			// 4	effect3
				item.push_back("");			// 5	effect4
				item.push_back("");			// 6	effect5
				item.push_back("");			// 7	effect6
				item.push_back("");			// 8	effect7
				item.push_back("\x00");		// 9	bloom
				item.push_back("");			// 10	creator
				item.push_back("");			// 11	description
				Containers::gameVals.customPalettes[charIndex].push_back(item);
			}
			ImGuiSystem::AddLog("[system] Loaded '%s'\n", fileName.c_str());
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
}

int FindCustomPaletteIndex(std::string paletteName, int charID)
{
	int foundPaletteIndex = 0;
	for (int i = 0; i < Containers::gameVals.customPalettes[charID].size(); i++)
	{
		if (Containers::gameVals.customPalettes[charID][i][0] == paletteName)
		{
			foundPaletteIndex = i;
			break;
		}
	}

	return foundPaletteIndex;
}

void ReloadCustomPalettes()
{
	//clearing arrays
	for (int i = 0; i < 36; i++)
	{
		Containers::gameVals.customPalettes[i].clear();
	}
	Containers::gameVals.customPalettes.clear();
	Containers::gameVals.P1_selected_custom_pal = 0;
	Containers::gameVals.P2_selected_custom_pal = 0;

	LoadPaletteFiles();

	if (Containers::gameVals.P1_selectedCharID != -1 && Containers::gameVals.P2_selectedCharID != -1)
	{
		ReplaceP1Palette();
		ReplaceP2Palette();
	}
}

void ReplaceP1Palette(bool sendToOpponent)
{
	LOG(2, "ReplaceP1Palette\n");

	if (*Containers::gameVals.pGameState != GAME_STATE_IN_MATCH || Containers::gameVals.P1CurPalette == 0)
	{
		if (*Containers::gameVals.pGameState == GAME_STATE_IN_MATCH && Containers::gameVals.P1CurPalette == 0)
		{
			LOG(2, "P1CurPalette is 0 !\n");
			ImGuiSystem::AddLog("[error] P1CurPalette is 0! Please report this bug.\n");
		}
		return;
	}

	if (Containers::gameVals.P1_selectedCharID == -1)
	{
		LOG(2, "P1_selectedCharID is -1 !\n");
		ImGuiSystem::AddLog("[error] P1_selectedCharID is -1 ! Please report this bug.\n");
		return;
	}

	ReplaceP1PaletteEffects();

	if (Containers::gameVals.P1_selected_custom_pal == 0)
	{
		//load default palette from backup buf instead of palette array
		ReplacePaletteInMemory(Containers::gameVals.P1Palette3, Containers::gameVals.P1DefaultPalBackup[0].c_str());
		ReplacePaletteInMemory(Containers::gameVals.P1Palette4, Containers::gameVals.P1DefaultPalBackup[0].c_str());
	}
	else
	{
		//load palette from the palette array
		ReplacePaletteInMemory(Containers::gameVals.P1Palette3, Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID][Containers::gameVals.P1_selected_custom_pal][1].c_str());
		ReplacePaletteInMemory(Containers::gameVals.P1Palette4, Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID][Containers::gameVals.P1_selected_custom_pal][1].c_str());
	}

	if (*Containers::gameVals.P1PaletteIndex != 2) //need to switch paletteindex in order to reflect changes ingame
		*Containers::gameVals.P1PaletteIndex = 2;
	else
		*Containers::gameVals.P1PaletteIndex = 3;

	//point current palette to the new one
	Containers::gameVals.P1CurPalette = getPaletteArray(Containers::gameVals.P1PaletteBase, *Containers::gameVals.P1PaletteIndex, 0);

	if (*Containers::gameVals.pGameMode == GAME_MODE_TRAINING && Containers::gameVals.paletteEditor_selectedPlayer == 0)
		memcpy(Containers::tempVals.PaletteEditorP1PalBackup,
			getPaletteArray(Containers::gameVals.P1PaletteBase, *Containers::gameVals.P1PaletteIndex, Containers::gameVals.paletteEditor_selectedFile),
			PALETTE_DATALEN);

	std::wstring ws(ingame_chars[Containers::gameVals.P1_selectedCharID]);
	std::string charName(ws.begin(), ws.end());
	ImGuiSystem::AddLog("[system] P1 (%s) palette set to '%s'\n",
		charName.c_str(), Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID][Containers::gameVals.P1_selected_custom_pal][0].c_str());

	if(sendToOpponent)
		SendCustomDatas();
}

void ReplaceP2Palette(bool sendToOpponent)
{
	LOG(2, "ReplaceP2Palette\n");

	if (*Containers::gameVals.pGameState != GAME_STATE_IN_MATCH || Containers::gameVals.P2CurPalette == 0)
	{
		if (*Containers::gameVals.pGameState == GAME_STATE_IN_MATCH && Containers::gameVals.P2CurPalette == 0)
		{
			LOG(2, "P2CurPalette is 0 !\n");
			ImGuiSystem::AddLog("[error] P2CurPalette is 0! Please report this bug.\n");
		}
		return;
	}

	if (Containers::gameVals.P2_selectedCharID == -1)
	{
		LOG(2, "P2_selectedCharID is -1 !\n");
		ImGuiSystem::AddLog("[error] P2_selectedCharID is -1 ! Please report this bug.\n");
		return;
	}

	ReplaceP2PaletteEffects();

	if (Containers::gameVals.P2_selected_custom_pal == 0)
	{
		//load default palette from backup buf instead of palette array
		ReplacePaletteInMemory(Containers::gameVals.P2Palette5, Containers::gameVals.P2DefaultPalBackup[0].c_str());
		ReplacePaletteInMemory(Containers::gameVals.P2Palette6, Containers::gameVals.P2DefaultPalBackup[0].c_str());
	}
	else
	{
		//load palette from the palette array
		ReplacePaletteInMemory(Containers::gameVals.P2Palette5, Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID][Containers::gameVals.P2_selected_custom_pal][1].c_str());
		ReplacePaletteInMemory(Containers::gameVals.P2Palette6, Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID][Containers::gameVals.P2_selected_custom_pal][1].c_str());
	}

	if (*Containers::gameVals.P2PaletteIndex != 4) //need to switch paletteindex in order to reflect changes ingame
		*Containers::gameVals.P2PaletteIndex = 4;
	else
		*Containers::gameVals.P2PaletteIndex = 5;

	//point current palette to the new one
	Containers::gameVals.P2CurPalette = getPaletteArray(Containers::gameVals.P2PaletteBase, *Containers::gameVals.P2PaletteIndex, 0);

	if (*Containers::gameVals.pGameMode == GAME_MODE_TRAINING && Containers::gameVals.paletteEditor_selectedPlayer == 1)
		memcpy(Containers::tempVals.PaletteEditorP2PalBackup,
			getPaletteArray(Containers::gameVals.P2PaletteBase, *Containers::gameVals.P2PaletteIndex, Containers::gameVals.paletteEditor_selectedFile),
			PALETTE_DATALEN);

	std::wstring ws(ingame_chars[Containers::gameVals.P2_selectedCharID]);
	std::string charName(ws.begin(), ws.end());
	ImGuiSystem::AddLog("[system] P2 (%s) palette set to '%s'\n",
		charName.c_str(), Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID][Containers::gameVals.P2_selected_custom_pal][0].c_str());

	if (sendToOpponent)
		SendCustomDatas();
}

void ReplacePaletteInMemory(DWORD* Dst, const void* Src)
{
	memcpy(Dst, Src, PALETTE_DATALEN);
	memcpy(Dst + 512, Src, PALETTE_DATALEN);
}

void SaveDefaultP1P2Pals()
{
	//use this func right before applying the custom palettes!!
	LOG(2, "SaveDefaultP1P2Pals\n");

	//if backup palettes are already set then return and dont do anything
	if (Containers::gameVals.P1DefaultPalBackup[0] == "")
	{
		LOG(2, "Backing up P1 palette\n");
		Containers::gameVals.P1DefaultPalBackup[0].assign((char*)Containers::gameVals.P1CurPalette, PALETTE_DATALEN);
		Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID][0][1].assign(Containers::gameVals.P1DefaultPalBackup[0], 0, PALETTE_DATALEN);
		BackupP1PaletteEffects();
	}

	if (Containers::gameVals.P2DefaultPalBackup[0] == "")
	{
		LOG(2, "Backing up P2 palette\n");
		Containers::gameVals.P2DefaultPalBackup[0].assign((char*)Containers::gameVals.P2CurPalette, PALETTE_DATALEN);
		Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID][0][1].assign(Containers::gameVals.P2DefaultPalBackup[0], 0, PALETTE_DATALEN);
		BackupP2PaletteEffects();
	}
}

void ResettingDefaultPalettes()
{
	LOG(2, "ResettingDefaultPalettes\n");
	for (int i = 0; i < 36; i++)
	{
		Containers::gameVals.customPalettes[i][0][1] = "";
	}
	Containers::gameVals.P1DefaultPalBackup[0] = "";
	Containers::gameVals.P2DefaultPalBackup[0] = "";
	//Containers::gameVals.P2_selected_cus = 0;
	//Containers::gameVals.P2_selected_cus = 0;
}

void BackupP1PaletteEffects()
{
	LOG(2, "BackupP1PaletteEffects\n");

	//player1
	for(int i = 1; i < 8; i++)
	{
		DWORD* P1CurPalEffectPart = getPaletteArray(Containers::gameVals.P1PaletteBase, *Containers::gameVals.P1PaletteIndex, i);

		DWORD* P1Pal3EffectPart = getPaletteArray(Containers::gameVals.P1PaletteBase, 2, i);
		DWORD* P1Pal4EffectPart = getPaletteArray(Containers::gameVals.P1PaletteBase, 3, i);

		ReplacePaletteInMemory(P1Pal3EffectPart, P1CurPalEffectPart);
		ReplacePaletteInMemory(P1Pal4EffectPart, P1CurPalEffectPart);

		Containers::gameVals.P1DefaultPalBackup[i].assign((char*)P1CurPalEffectPart, PALETTE_DATALEN);
	}
}

void BackupP2PaletteEffects()
{
	LOG(2, "BackupP2PaletteEffects\n");
	//player2
	for (int i = 1; i < 8; i++)
	{
		DWORD* P2CurPalEffectPart = getPaletteArray(Containers::gameVals.P2PaletteBase, *Containers::gameVals.P2PaletteIndex, i);

		DWORD* P2Pal5EffectPart = getPaletteArray(Containers::gameVals.P2PaletteBase, 4, i);
		DWORD* P2Pal6EffectPart = getPaletteArray(Containers::gameVals.P2PaletteBase, 5, i);

		ReplacePaletteInMemory(P2Pal5EffectPart, P2CurPalEffectPart);
		ReplacePaletteInMemory(P2Pal6EffectPart, P2CurPalEffectPart);

		Containers::gameVals.P2DefaultPalBackup[i].assign((char*)P2CurPalEffectPart, PALETTE_DATALEN);
	}
}

void ReplaceP1PaletteEffects()
{
	LOG(2, "ReplaceP1PaletteEffects\n");

	for (int i = 1; i < 8; i++)
	{
		std::string PalettePartString;
		DWORD* P1Pal3EffectPart = getPaletteArray(Containers::gameVals.P1PaletteBase, 2, i);
		DWORD* P1Pal4EffectPart = getPaletteArray(Containers::gameVals.P1PaletteBase, 3, i);

		if (Containers::gameVals.P1_selected_custom_pal == 0)
			PalettePartString = Containers::gameVals.P1DefaultPalBackup[i];
		else
			PalettePartString = Containers::gameVals.customPalettes[Containers::gameVals.P1_selectedCharID][Containers::gameVals.P1_selected_custom_pal][i + 1];

		//if palette string was empty, then use the default palette's effects
		if (PalettePartString == "")
			PalettePartString = Containers::gameVals.P1DefaultPalBackup[i];

		ReplacePaletteInMemory(P1Pal3EffectPart, PalettePartString.c_str());
		ReplacePaletteInMemory(P1Pal4EffectPart, PalettePartString.c_str());
	}
}

void ReplaceP2PaletteEffects()
{
	LOG(2, "ReplaceP2PaletteEffects\n");

	for (int i = 1; i < 8; i++)
	{
		std::string PalettePartString;
		DWORD* P2Pal5EffectPart = getPaletteArray(Containers::gameVals.P2PaletteBase, 4, i);
		DWORD* P2Pal6EffectPart = getPaletteArray(Containers::gameVals.P2PaletteBase, 5, i);

		if (Containers::gameVals.P2_selected_custom_pal == 0)
			PalettePartString = Containers::gameVals.P2DefaultPalBackup[i];
		else
			PalettePartString = Containers::gameVals.customPalettes[Containers::gameVals.P2_selectedCharID][Containers::gameVals.P2_selected_custom_pal][i + 1];

		//if palette string was empty, then use the default palette's effects
		if (PalettePartString == "")
			PalettePartString = Containers::gameVals.P2DefaultPalBackup[i];

			ReplacePaletteInMemory(P2Pal5EffectPart, PalettePartString.c_str());
			ReplacePaletteInMemory(P2Pal6EffectPart, PalettePartString.c_str());
	}
}

void ReplaceP1Palette_PaletteEditor(char* pPaletteData, int file)
{
	if (*Containers::gameVals.pGameMode != GAME_MODE_TRAINING && *Containers::gameVals.pGameState != GAME_STATE_IN_MATCH && !Containers::gameVals.P1CurPalette)
		return;

	LOG(7, "ReplaceP1Palette_PaletteEditor\n");

	//Get pointers to the palettes in memory
	DWORD* P1Pal3FilePart = getPaletteArray(Containers::gameVals.P1PaletteBase, 2, file);
	DWORD* P1Pal4FilePart = getPaletteArray(Containers::gameVals.P1PaletteBase, 3, file);

	//Replace with our own
	ReplacePaletteInMemory(P1Pal3FilePart, pPaletteData);
	ReplacePaletteInMemory(P1Pal4FilePart, pPaletteData);

	if (*Containers::gameVals.P1PaletteIndex != 2) //need to switch paletteindex in order to reflect changes ingame
		*Containers::gameVals.P1PaletteIndex = 2;
	else
		*Containers::gameVals.P1PaletteIndex = 3;

	//point current palette to the new one
	Containers::gameVals.P1CurPalette = getPaletteArray(Containers::gameVals.P1PaletteBase, *Containers::gameVals.P1PaletteIndex, 0);
}

void ReplaceP2Palette_PaletteEditor(char* pPaletteData, int file)
{
	if (*Containers::gameVals.pGameMode != GAME_MODE_TRAINING && *Containers::gameVals.pGameState != GAME_STATE_IN_MATCH && !Containers::gameVals.P2CurPalette)
		return;

	LOG(7, "ReplaceP2Palette_PaletteEditor\n");

	//Get pointers to the palettes in memory
	DWORD* P2Pal5FilePart = getPaletteArray(Containers::gameVals.P2PaletteBase, 4, file);
	DWORD* P2Pal6FilePart = getPaletteArray(Containers::gameVals.P2PaletteBase, 5, file);

	//Replace with our own
	ReplacePaletteInMemory(P2Pal5FilePart, pPaletteData);
	ReplacePaletteInMemory(P2Pal6FilePart, pPaletteData);

	if (*Containers::gameVals.P2PaletteIndex != 4) //need to switch paletteindex in order to reflect changes ingame
		*Containers::gameVals.P2PaletteIndex = 4;
	else
		*Containers::gameVals.P2PaletteIndex = 5;

	//point current palette to the new one
	Containers::gameVals.P2CurPalette = getPaletteArray(Containers::gameVals.P2PaletteBase, *Containers::gameVals.P2PaletteIndex, 0);
}