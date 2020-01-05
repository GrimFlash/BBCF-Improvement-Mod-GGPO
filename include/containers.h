#pragma once
#include "stdafx.h"
#include "SteamApiWrapper\SteamFriendsWrapper.h"
#include "SteamApiWrapper\SteamMatchmakingWrapper.h"
#include "SteamApiWrapper\SteamNetworkingWrapper.h"
#include "SteamApiWrapper\SteamUserStatsWrapper.h"
#include "SteamApiWrapper\SteamUserWrapper.h"
#include "SteamApiWrapper\SteamUtilsWrapper.h"
#include "steamapi_helper.h"
#include <steam_api.h>
#include "D3D9EXWrapper\ID3D9EXWrapper_Device.h"
#include "../include/network_manager.h"
#include "../include/cchar.h"
#include <vector>

//forward declaration
class NetworkManager;
struct im_packet_internal_t;

extern unsigned char stages[79][5];
extern char* allstagesunlockedmemory;
#ifndef ALL_STAGES_UNLOCKED_MEMORY_SIZE
#define ALL_STAGES_UNLOCKED_MEMORY_SIZE 3536//3456
#endif

struct g_interfaces_t
{
	SteamFriendsWrapper* pSteamFriendsWrapper;
	SteamMatchmakingWrapper* pSteamMatchmakingWrapper;
	SteamNetworkingWrapper* pSteamNetworkingWrapper;
	SteamUserWrapper* pSteamUserWrapper;
	SteamUserStatsWrapper* pSteamUserStatsWrapper;
	SteamUtilsWrapper* pSteamUtilsWrapper;
	IDirect3DDevice9Ex* pD3D9ExWrapper;
	NetworkManager* pNetworkManager;
	SteamApiHelper* pSteamApiHelper;
};

struct gameVals_t
{
	//customPalettes[36] that contains vector<string[8]> per custom palette
	//customPalettes[0][0][0] accessses ragna's first palette's name
	//				 ^ charIndex
	//customPalettes[0][0][1] accessses ragna's first palette's data
	//					^ paletteIndex
	//customPalettes[0][0][2] accessses ragna's first effect data
	//					   ^ element
	//ELEMENTS:
	//0 filename str
	//1 palette data
	//2 effect1 data
	//3 effect2 data
	//4 effect3 data
	//5 effect4 data
	//6 effect5 data
	//7 effect6 data
	//8 effect7 data
	//9 bloom bool
	//10 creator str
	//11 description str
	std::vector<std::vector<std::vector<std::string>>> customPalettes;

	//P1DefaultPalBackup[0] contains the sprite palette
	//P1DefaultPalBackup[1-7] contains the effect palettes
	std::vector<std::string> P1DefaultPalBackup;
	std::vector<std::string> P2DefaultPalBackup;


	CSteamID* ownSteamID;
	CSteamID* opponentSteamID;

	bool bOpponentUsingBBCFIM;
	int iOpponentBBCFIMvernum;
	std::string sOpponentBBCFIMvernum;

	bool bP1UsingBBCFIM;
	int iP1BBCFIMvernum;
	std::string sP1BBCFIMvernum;

	bool bP2UsingBBCFIM;
	int iP2BBCFIMvernum;
	std::string sP2BBCFIMvernum;

	int* pGameState;
	int* pGameMoney;
	int* pGameMode;
	int* pMatchState;
	int* pMatchTimer;
	int* pMatchRounds;

	int playerAvatarBaseAddr;
	int* playerAvatarAddr;
	int* playerAvatarColAddr;
	byte* playerAvatarAcc1;
	byte* playerAvatarAcc2;

	CChar* P1CharObjPointer;
	CChar* P2CharObjPointer;
	
	int P1_selectedCharID;
	int P2_selectedCharID;

	int thisPlayerNum;

	int origP1PaletteIndex;
	int origP2PaletteIndex;
	int* P1PaletteIndex;
	int* P2PaletteIndex;

	DWORD* P1PaletteBase;
	DWORD* P2PaletteBase;

	DWORD* P1CurPalette;
	DWORD* P1Palette3;
	DWORD* P1Palette4;

	DWORD* P2CurPalette;
	DWORD* P2Palette5;
	DWORD* P2Palette6;

	int P1_selected_custom_pal;
	int P2_selected_custom_pal;

	char isP1BloomOn = 0;
	char isP2BloomOn = 0;

	bool startMatchPalettesInit;

	int paletteEditor_selectedFile;
	int paletteEditor_selectedPlayer;

	int isP1CPU;

	int setMatchRoundsTo;

	unsigned char* stageListMemory;
	int *stageSelect_X;
	int *stageSelect_Y;
	int *musicSelect_X;
	int *musicSelect_Y;

	bool charSelectInit;

	//spectators included
	std::vector<CSteamID> playersInMatch;

	uint32 totalReadPackets;

	float *P1ScreenPosX;
	float *P1ScreenPosY;
};

struct gameProc_t
{
	HWND hWndGameWindow;
};

//temporary placeholders until wrappers are created / final addresses updated
struct temps_t
{
	ISteamFriends** ppSteamFriends;
	ISteamMatchmaking** ppSteamMatchmaking;
	ISteamNetworking** ppSteamNetworking;
	ISteamUser** ppSteamUser;
	ISteamUserStats** ppSteamUserStats;
	ISteamUtils** ppSteamUtils;

	//temppackets store the unprocessed packets until they are handled properly
	std::vector<im_packet_internal_t> tempPackets;

	char PaletteEditorP1PalBackup[1020];
	char PaletteEditorP2PalBackup[1020];

	bool paledit_show_placeholder;
	bool paledit_show_sel_by_highlight;

	int PlayersCharIDVersusScreenCounter;

	//storing the ID until we can delete it (game crashes if we do it right away after the match ends)
	CSteamID* opponentIDToBeDeleted;
};

class Containers
{
public:
	static g_interfaces_t g_interfaces;
	static gameProc_t gameProc;
	static gameVals_t gameVals;
	static temps_t tempVals;
	static void Init();
	static void Cleanup();
};