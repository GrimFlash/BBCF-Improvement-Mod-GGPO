#include "../include/additional_hooks.h"
#include <detours.h>
#include <steam_api.h>
#include "../include/D3D9ExWrapper/ID3D9Wrapper_Sprite.h"
#include "../include/D3D9ExWrapper/ID3DXWrapper_Effect.h"
#include "../include/SteamApiWrapper/SteamMatchmakingWrapper.h"
#include "../include/SteamApiWrapper/SteamNetworkingWrapper.h"
#include "../include/ImGui/ImGuiSystem.h"
#include "../include/utils.h"
#include "../include/hook_manager.h"
#include "../include/containers.h"
#include "../include/custom_palette.h"
#include "../include/custom_gamemodes.h"
#include "../include/gamestates_defines.h"
#include <atlstr.h>

#pragma comment(lib, "detours.lib")
#pragma comment(lib, "steam_api.lib")

typedef HRESULT(APIENTRY *D3DXCreateEffect_t)(LPDIRECT3DDEVICE9, LPCVOID, UINT, CONST D3DXMACRO*, LPD3DXINCLUDE, DWORD, LPD3DXEFFECTPOOL, LPD3DXEFFECT*, LPD3DXBUFFER*);
typedef HRESULT(WINAPI *D3DXCreateSprite_t)(LPDIRECT3DDEVICE9 pDevice, LPD3DXSPRITE* ppSprite);

typedef SteamAPICall_t(__fastcall *RequestLobbyList_t)(ISteamMatchmaking*);
typedef bool (WINAPI *SteamAPI_Init_t)();

typedef HWND(__stdcall *CreateWindowExW_t)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
	DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
typedef ATOM(WINAPI *RegisterClassExW_t)(_In_ const WNDCLASSEX *lpwcx);

typedef BOOL(WINAPI *ReadFile_t)(_In_ HANDLE hFile, _Out_ LPVOID lpBuffer, _In_ DWORD nNumberOfBytesToRead, _Out_opt_ LPDWORD lpNumberOfBytesRead, _Inout_opt_ LPOVERLAPPED lpOverlapped);
typedef HANDLE(WINAPI *CreateFileA_t)(_In_ LPCSTR lpFileName, _In_ DWORD dwDesiredAccess, _In_ DWORD dwShareMode, _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition, _In_ DWORD dwFlagsAndAttributes, _In_opt_ HANDLE hTemplateFile);
typedef HANDLE(WINAPI *CreateFileW_t)(_In_ LPCTSTR lpFileName, _In_ DWORD dwDesiredAccess, _In_ DWORD dwShareMode, _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition, _In_ DWORD dwFlagsAndAttributes, _In_opt_ HANDLE hTemplateFile);
typedef BOOL (WINAPI *CloseHandle_t)(_In_ HANDLE hObject);

D3DXCreateEffect_t orig_D3DXCreateEffect;
D3DXCreateSprite_t orig_D3DXCreateSprite;

RequestLobbyList_t orig_RequestLobbyList;
SteamAPI_Init_t orig_SteamAPI_Init;

CreateWindowExW_t orig_CreateWindowExW;
RegisterClassExW_t orig_RegisterClassExW;
ReadFile_t orig_ReadFile;
CreateFileA_t orig_CreateFileA;
CreateFileW_t orig_CreateFileW;
CloseHandle_t orig_CloseHandle;

BOOL WINAPI hook_CloseHandle(_In_ HANDLE hObject)
{
	LOG(2, "hook_CloseHandle hObject: 0x%x\n", hObject);
	BOOL ret = orig_CloseHandle(hObject);
	return ret;
}


HANDLE WINAPI hook_CreateFileA(_In_ LPCSTR lpFileName, _In_ DWORD dwDesiredAccess, _In_ DWORD dwShareMode, _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition, _In_ DWORD dwFlagsAndAttributes, _In_opt_ HANDLE hTemplateFile)
{
	LOG(2, "hook_CreateFileA: ");
	CStringA cstrText(lpFileName);
	LOG(2, "\tFileName: %s\n", cstrText);

	HANDLE ret = orig_CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	LOG(2, "RET CreateFileA: 0x%x\n", ret);
	return ret;
}

HANDLE WINAPI hook_CreateFileW(_In_ LPCTSTR lpFileName, _In_ DWORD dwDesiredAccess, _In_ DWORD dwShareMode, _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition, _In_ DWORD dwFlagsAndAttributes, _In_opt_ HANDLE hTemplateFile)
{
	LOG(2, "hook_CreateFileW: ");
	CStringA cstrText(lpFileName);
	LOG(2, "\tFileName: %s\n", cstrText);

	HANDLE ret = orig_CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	LOG(2, "RET CreateFileW: 0x%x\n", ret);
	return ret;
}

BOOL WINAPI hook_ReadFile(_In_ HANDLE hFile, _Out_ LPVOID lpBuffer, _In_ DWORD nNumberOfBytesToRead, _Out_opt_ LPDWORD lpNumberOfBytesRead, _Inout_opt_ LPOVERLAPPED lpOverlapped)
{
	LOG(2, "hook_ReadFile: ");
	LOG(2, "\tHANDLE: 0x%x , lpBuffer: 0x%x , nNumberOfBytesToRead: 0x%x\n", hFile, lpBuffer, nNumberOfBytesToRead);
	BOOL ret = orig_ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
	return ret;
}

HRESULT APIENTRY hook_D3DXCreateEffect(LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataLen,
	CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect,
	LPD3DXBUFFER* ppCompilationErrors)
{
	LOG(7, "D3DXCreateEffect\n");
	HRESULT hR = orig_D3DXCreateEffect(pDevice, pSrcData, SrcDataLen, pDefines, pInclude, Flags, pPool, ppEffect, ppCompilationErrors);
	if (SUCCEEDED(hR))
	{
		ID3DXEffectWrapper *ret = new ID3DXEffectWrapper(&ppEffect);
	}

	return hR;
}

HRESULT WINAPI hook_D3DXCreateSprite(LPDIRECT3DDEVICE9 pDevice, LPD3DXSPRITE* ppSprite)
{
	LOG(7, "D3DXCreateSprite\n");
	HRESULT hR = orig_D3DXCreateSprite(pDevice, ppSprite);
	if (SUCCEEDED(hR))
	{
		ID3DXSpriteWrapper *ret = new ID3DXSpriteWrapper(&ppSprite);
	}
	return hR;
}

SteamAPICall_t __fastcall hook_RequestLobbyList(ISteamMatchmaking* pSteamMatchmaking)
{
	LOG(7, "STEAM LOBBY!\n");
	ELobbyDistanceFilter filter;
	switch (Settings::settingsIni.regionlock)
	{
	case 2:
		filter = k_ELobbyDistanceFilterFar;
		break;
	case 3:
		filter = k_ELobbyDistanceFilterWorldwide;
		break;
	case 1:
	default:
		filter = k_ELobbyDistanceFilterDefault;
		break;
	}

	SteamMatchmaking()->AddRequestLobbyListDistanceFilter(filter);

	return orig_RequestLobbyList(pSteamMatchmaking);
}

void hookSteamAPIMatchmaking()
{
	DWORD* pFunct = GetInterfaceFuncPtr((DWORD*)SteamMatchmaking(), "%p", &ISteamMatchmaking::RequestLobbyList);
	PBYTE pRequestLobbyList = (PBYTE)*pFunct;
	checkHookSuccess((PBYTE)pRequestLobbyList, "RequestLobbyList");
	orig_RequestLobbyList = (RequestLobbyList_t)DetourFunction(pRequestLobbyList, (LPBYTE)hook_RequestLobbyList);
}

DWORD SteamMatchmakingFuncJmpBackAddr = 0;
void __declspec(naked)GetSteamMatchmaking()
{
	__asm pushad
	LOG(2, "GetSteamMatchmaking\n");
	__asm popad
	__asm
	{
		call dword ptr[eax + 28h]
		/////
		pushad
		add esi, 10h
		//mov steamMatchmakingPtrAddr, esi
		mov Containers::tempVals.ppSteamMatchmaking, esi
		popad
		/////
		mov[esi + 10h], eax
		jmp[SteamMatchmakingFuncJmpBackAddr]
	}
}

DWORD SteamNetworkingFuncJmpBackAddr = 0;
void __declspec(naked)GetSteamNetworking()
{
	__asm pushad
	LOG(2, "GetSteamNetworking\n");
	__asm popad
	__asm
	{
		call dword ptr[eax + 40h]
		/////
		pushad
		add esi, 20h
		//mov steamNetworkingPtrAddr, esi
		mov Containers::tempVals.ppSteamNetworking, esi
		popad
		/////
		mov[esi + 20h], eax
		jmp[SteamNetworkingFuncJmpBackAddr]
	}
}

DWORD SteamUserFuncJmpBackAddr = 0;
void __declspec(naked)GetSteamUser()
{
	__asm pushad
	LOG(2, "GetSteamUser\n");
	__asm popad
	__asm
	{
		call dword ptr[eax + 14h]
		/////
		pushad
		add esi, 4h
		mov Containers::tempVals.ppSteamUser, esi
		popad
		/////
		mov[esi + 4h], eax
		jmp[SteamUserFuncJmpBackAddr]
	}
}

DWORD SteamFriendsFuncJmpBackAddr = 0;
void __declspec(naked)GetSteamFriends()
{
	__asm pushad
	LOG(2, "GetSteamFriends\n");
	__asm popad
	__asm
	{
		call dword ptr[eax + 20h]
		/////
		pushad
		add esi, 8h
		mov Containers::tempVals.ppSteamFriends, esi
		popad
		/////
		mov[esi + 8h], eax
		jmp[SteamFriendsFuncJmpBackAddr]
	}
}

DWORD SteamUtilsFuncJmpBackAddr = 0;
void __declspec(naked)GetSteamUtils()
{
	__asm pushad
	LOG(2, "GetSteamUtils\n");
	__asm popad
	__asm
	{
		call dword ptr[eax + 24h]
		/////
		pushad
		add esi, 0Ch
		mov Containers::tempVals.ppSteamUtils, esi
		popad
		/////
		mov[esi + 0Ch], eax
		jmp[SteamUtilsFuncJmpBackAddr]
	}
}

DWORD SteamUserStatsFuncJmpBackAddr = 0;
void __declspec(naked)GetSteamUserStats()
{
	__asm pushad
	LOG(2, "GetSteamUserStats\n");
	__asm popad
	__asm
	{
		call dword ptr[eax + 34h]
		/////
		pushad
		add esi, 14h
		mov Containers::tempVals.ppSteamUserStats, esi
		popad
		/////
		mov[esi + 14h], eax
		jmp[SteamUserStatsFuncJmpBackAddr]
	}
}

bool WINAPI hook_SteamAPI_Init()
{
	LOG(1, "SteamAPI_Init\n");
	bool ret = orig_SteamAPI_Init();

	//hooking SteamMatchmaking and SteamNetworking
	SteamMatchmakingFuncJmpBackAddr = HookManager::SetHook("SteamMatchmaking", "\xff\x50\x28\x89\x46\x10\x85\xc0", "xxxxxxxx", 6, GetSteamMatchmaking);
	SteamNetworkingFuncJmpBackAddr = HookManager::SetHook("SteamNetworking", "\xff\x50\x40\x89\x46\x20\x85\xc0", "xxxxxxxx", 6, GetSteamNetworking);
	SteamUserFuncJmpBackAddr = HookManager::SetHook("SteamUser", "\xff\x50\x14\x89\x46\x04", "xxxxxx", 6, GetSteamUser);
	SteamFriendsFuncJmpBackAddr = HookManager::SetHook("SteamFriends", "\xff\x50\x20\x89\x46\x08", "xxxxxx", 6, GetSteamFriends);
	SteamUtilsFuncJmpBackAddr = HookManager::SetHook("SteamUtils", "\xff\x50\x24\x89\x46\x0c", "xxxxxx", 6, GetSteamUtils);
	SteamUserStatsFuncJmpBackAddr = HookManager::SetHook("SteamUserStats", "\xff\x50\x34\x89\x46\x14", "xxxxxx", 6, GetSteamUserStats);

	return ret;
}

HWND WINAPI hook_CreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
	DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	LOG(7, "CreateWindowExW\n");
	static int counter = 1;
	HWND hWnd = orig_CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	if (SUCCEEDED(hWnd))
	{
		LOG(7, "\tSuccess: 0x%x\n", hWnd);
		if (counter == 2) // 2nd created window should be the correct one according to process hacker
		{
			LOG(2, "Correct window: 0x%x\n", hWnd);
			Containers::gameProc.hWndGameWindow = hWnd;
		}
	}
	counter++;
	return hWnd;
}

ATOM WINAPI hook_RegisterClassExW(_In_ const WNDCLASSEX *lpwcx)
{
	LOG(2, "RegisterClassExW\n");
	LOG(2, "\t0x%x\t%s\n", lpwcx->lpfnWndProc, lpwcx->lpszClassName);
	
	return orig_RegisterClassExW(lpwcx);
}

bool additional_hooks_detours()
{
	HMODULE hM_d3dx9_43 = GetModuleHandleA("d3dx9_43.dll");

	PBYTE pD3DXCreateEffect = (PBYTE)GetProcAddress(hM_d3dx9_43, "D3DXCreateEffect");
	PBYTE pD3DXCreateSprite = (PBYTE)GetProcAddress(hM_d3dx9_43, "D3DXCreateSprite");

	HMODULE hM_steam_api = GetModuleHandleA("steam_api.dll");
	PBYTE pSteamAPI_Init = (PBYTE)GetProcAddress(hM_steam_api, "SteamAPI_Init");

	HMODULE hM_user32 = GetModuleHandleA("user32.dll");
	PBYTE pCreateWindowExW = (PBYTE)GetProcAddress(hM_user32, "CreateWindowExW");
	//PBYTE pRegisterClassExW = (PBYTE)GetProcAddress(hM_user32, "RegisterClassExW");
	//HMODULE hM_kernel32 = GetModuleHandleA("kernel32.dll");
	//PBYTE pReadFile = (PBYTE)GetProcAddress(hM_kernel32, "ReadFile");
	//PBYTE pCreateFileA = (PBYTE)GetProcAddress(hM_kernel32, "CreateFileA");
	//PBYTE pCreateFileW = (PBYTE)GetProcAddress(hM_kernel32, "CreateFileW");
	//PBYTE pCloseHandle = (PBYTE)GetProcAddress(hM_kernel32, "CloseHandle");

	if (!checkHookSuccess((PBYTE)pD3DXCreateEffect, "D3DXCreateEffect"))
		return false;
	if (!checkHookSuccess((PBYTE)pD3DXCreateSprite, "D3DXCreateSprite"))
		return false;
	if (!checkHookSuccess((PBYTE)pSteamAPI_Init, "SteamAPI_Init"))
		return false;
	if (!checkHookSuccess((PBYTE)pCreateWindowExW, "CreateWindowExW"))
		return false;
	//if (!checkHookSuccess((PBYTE)pRegisterClassExW, "RegisterClassExW"))
	//	return false;
	//if (!checkHookSuccess((PBYTE)pReadFile, "ReadFile"))
	//	return false;
	//if (!checkHookSuccess((PBYTE)pCreateFileA, "CreateFileA"))
	//	return false;
	//if (!checkHookSuccess((PBYTE)pCreateFileW, "CreateFileW"))
	//	return false;
	//if (!checkHookSuccess((PBYTE)pCloseHandle, "CloseHandle"))
	//	return false;

	orig_D3DXCreateEffect = (D3DXCreateEffect_t)DetourFunction(pD3DXCreateEffect, (LPBYTE)hook_D3DXCreateEffect);
	orig_D3DXCreateSprite = (D3DXCreateSprite_t)DetourFunction(pD3DXCreateSprite, (LPBYTE)hook_D3DXCreateSprite);
	orig_SteamAPI_Init = (SteamAPI_Init_t)DetourFunction(pSteamAPI_Init, (LPBYTE)hook_SteamAPI_Init);
	orig_CreateWindowExW = (CreateWindowExW_t)DetourFunction(pCreateWindowExW, (LPBYTE)hook_CreateWindowExW);
	//orig_RegisterClassExW = (RegisterClassExW_t)DetourFunction(pRegisterClassExW, (LPBYTE)hook_RegisterClassExW);

	//orig_ReadFile = (ReadFile_t)DetourFunction(pReadFile, (LPBYTE)hook_ReadFile);
	//orig_CreateFileA = (CreateFileA_t)DetourFunction(pCreateFileA, (LPBYTE)hook_CreateFileA);
	//orig_CreateFileW = (CreateFileW_t)DetourFunction(pCreateFileW, (LPBYTE)hook_CreateFileW);
	//orig_CloseHandle = (CloseHandle_t)DetourFunction(pCloseHandle, (LPBYTE)hook_CloseHandle);

	return true;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// NON-DETOURS HOOKS BELOW:

DWORD GetGameStateTitleScreenJmpBackAddr = 0;
void __declspec(naked)GetGameStateTitleScreen()
{
	_asm pushad
	LOG(2, "GetGameStateTitleScreen\n");
	_asm popad

	_asm
	{
		pushad
		add edi, 10Ch
		lea ebx, Containers::gameVals.pGameState
		mov[ebx], edi
	}
	ImGuiSystem::Init(
		Containers::gameProc.hWndGameWindow,
		Containers::g_interfaces.pD3D9ExWrapper);
	__asm
	{
		popad
		mov dword ptr [edi+10Ch], 4 //original bytes
		jmp[GetGameStateTitleScreenJmpBackAddr]
	}
}

void ResetBackToMenu()
{
	//copying the same stuffs here as in GameStateLobby
	if (Containers::gameVals.opponentSteamID)
	{
		Containers::tempVals.opponentIDToBeDeleted = Containers::gameVals.opponentSteamID;
		Containers::gameVals.opponentSteamID = 0;
	}

	Containers::gameVals.thisPlayerNum = 0;
	Containers::gameVals.bOpponentUsingBBCFIM = false;
	Containers::gameVals.bP1UsingBBCFIM = false;
	Containers::gameVals.bP2UsingBBCFIM = false;
	ResettingDefaultPalettes();
	Containers::gameVals.P1_selectedCharID = -1;
	Containers::gameVals.P2_selectedCharID = -1;
	Containers::gameVals.P1CurPalette = 0;
	Containers::gameVals.P2CurPalette = 0;
	Containers::gameVals.startMatchPalettesInit = false;
	Containers::g_interfaces.pSteamApiHelper->UpdateNumberOfCurrentPlayers();
	Containers::tempVals.PlayersCharIDVersusScreenCounter = 0;
	Containers::gameVals.charSelectInit = false;
	Containers::gameVals.playersInMatch.clear();
	Containers::gameVals.totalReadPackets = 0;
	Containers::tempVals.paledit_show_sel_by_highlight = false;
	Containers::tempVals.paledit_show_placeholder = false;

	EndCustomGamemode();
}

DWORD GetGameStateMenuScreenJmpBackAddr = 0;
void __declspec(naked)GetGameStateMenuScreen()
{
	_asm pushad
	LOG(2, "GetGameStateMenuScreen\n");
	_asm popad

	_asm
	{
		pushad
		add eax, 10Ch
		lea ebx, Containers::gameVals.pGameState
		mov[ebx], eax
	}
	ImGuiSystem::Init(
		Containers::gameProc.hWndGameWindow,
		Containers::g_interfaces.pD3D9ExWrapper);

	ResetBackToMenu();

	__asm
	{
		popad
		mov dword ptr[eax + 10Ch], 1Bh //original bytes
		jmp[GetGameStateMenuScreenJmpBackAddr]
	}
}

DWORD GetGameStateLobbyJmpBackAddress = 0;
void __declspec(naked)GetGameStateLobby()
{
	__asm pushad
	LOG(2, "GetGameStateLobby\n");
	__asm popad

	__asm pushad

	ResetBackToMenu();

	__asm popad

	__asm
	{
		mov dword ptr[eax + 10Ch], 1Fh
		jmp[GetGameStateLobbyJmpBackAddress]
	}
}

DWORD GetGameStateMatchStartJmpBackAddr = 0;
void __declspec(naked)GetGameStateMatchStart()
{
	_asm pushad
	LOG(2, "GetGameStateMatchStart\n");
	_asm popad
	__asm pushad

	Containers::gameVals.startMatchPalettesInit = false;
	Containers::gameVals.isP1BloomOn = 0;
	Containers::gameVals.isP2BloomOn = 0;
	Containers::tempVals.PlayersCharIDVersusScreenCounter = 0;

	ImGuiSystem::AddLogSeparator();

	if (*Containers::gameVals.pGameMode == GAME_MODE_ONLINE)
	{
		//Containers::g_interfaces.pSteamApiHelper->GetRequestUserInformation(*Containers::gameVals.opponentSteamID);
		//ImGuiSystem::AddLog("[system] Online match against '%s' has started\n", Containers::g_interfaces.pSteamFriendsWrapper->GetFriendPersonaName(*Containers::gameVals.opponentSteamID));
		ImGuiSystem::AddLog("[system] Online match has started\n");
#ifndef RELEASE_VER
		ImGuiSystem::AddLog("[debug] Local player is from %s\n", Containers::g_interfaces.pSteamUtilsWrapper->GetIPCountry());
#endif
	}
	else
	{
		ImGuiSystem::AddLog("[system] Local match has started\n");
	}

	__asm
	{
		popad
		mov dword ptr[eax + 10Ch], 0Fh
		jmp[GetGameStateMatchStartJmpBackAddr]
	}
}

DWORD GetGameStateVictoryScreenJmpBackAddr = 0;
void __declspec(naked)GetGameStateVictoryScreen()
{
	__asm pushad
	LOG(2, "GetGameStateVictoryScreen\n");

	ResettingDefaultPalettes();
	Containers::gameVals.startMatchPalettesInit = false;
	//restore original palette indexes so they will be correct on rematch
	if(Containers::gameVals.P1PaletteIndex)
		*Containers::gameVals.P1PaletteIndex = Containers::gameVals.origP1PaletteIndex;
	if(Containers::gameVals.P2PaletteIndex)
		*Containers::gameVals.P2PaletteIndex = Containers::gameVals.origP2PaletteIndex;

	__asm popad

	__asm
	{
		mov dword ptr[eax + 10Ch], 10h
		jmp[GetGameStateVictoryScreenJmpBackAddr]
	}
}

DWORD GetGameStateVersusScreenJmpBackAddr = 0;
void __declspec(naked)GetGameStateVersusScreen()
{
	__asm pushad
	LOG(2, "GetGameStateVersusScreen\n");
	__asm popad

	Containers::tempVals.PlayersCharIDVersusScreenCounter = 0;

	__asm
	{
		mov dword ptr[eax + 10Ch], 0Eh
		jmp[GetGameStateVersusScreenJmpBackAddr]
	}
}

DWORD GetGameStateReplayMenuScreenJmpBackAddr = 0;
void __declspec(naked)GetGameStateReplayMenuScreen()
{
	__asm pushad
	LOG(2, "GetGameStateReplayMenuScreen\n");

	ResettingDefaultPalettes();
	Containers::gameVals.P1_selectedCharID = -1;
	Containers::gameVals.P2_selectedCharID = -1;
	Containers::gameVals.P1CurPalette = 0;
	Containers::gameVals.P2CurPalette = 0;
	Containers::gameVals.startMatchPalettesInit = false;
	Containers::tempVals.PlayersCharIDVersusScreenCounter = 0;

	EndCustomGamemode();
	__asm popad

	__asm
	{
		mov dword ptr[eax + 10Ch], 1Ah
		jmp[GetGameStateReplayMenuScreenJmpBackAddr];
	}
}

DWORD WindowMsgHandlerJmpBackAddr = 0;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void __declspec(naked)PassMsgToImGui()
{
	__asm
	{
		mov edi, [ebp + 0Ch]
		mov ebx, ecx
	}
	__asm pushad
	LOG(7, "PassMsgToImGui\n");
	__asm popad

	__asm
	{
		pushad
		movzx eax, ImGuiSystem::Initialized
		cmp eax, 0
		je SKIP
		push[ebp + 10h] //lparam
		push edi //wParam
		push esi //msg
		push dword ptr[ebx + 1Ch] //hwnd
		call ImGui_ImplWin32_WndProcHandler
		pop ebx //manually clearing stack...
		pop ebx
		pop ebx
		pop ebx
		cmp eax, 1
		je EXIT
SKIP:
		popad
		jmp[WindowMsgHandlerJmpBackAddr]
	}
EXIT:
	__asm
	{
		popad
		mov eax, 1
		retn 0Ch
	}
}

DWORD PacketProcessingFuncJmpBackAddr = 0;
void __declspec(naked)PacketProcessingFunc()
{
	im_packet_internal_t* packet;
	__asm pushad
	LOG(7, "PacketProcessingFunc\n");
	__asm popad

	__asm mov packet, ecx;

	__asm
	{
		movzx eax, [ecx + 2] // <-- - load the packet code into eax
		cmp eax, 98h // <-- - palette packet code of BBCFIM 2.00 > is 0x98
		je CUSTOMPACKET
		cmp eax, 99h // <-- - palette packet code of BBCFIM 2.00 < is 0x99 (unsupported version but we still process the packet)
		jne SHORT NOTCUSTOMPACKET
CUSTOMPACKET:
		pushad
		sub ecx, 2 //<-- - set ecx at packet.length2
		mov packet, ecx //<-- - align packet pointer ontop of packet.length2
	}
	Containers::g_interfaces.pNetworkManager->PacketProcesser(packet);
	_asm
	{
		popad
		jmp SHORT EXIT
NOTCUSTOMPACKET:
		mov edx, [ebp - 08h]
		lea edi, [ebp - 24h]
		mov eax, [edx]
		push edi
		push ecx
		mov ecx, edx
		call dword ptr[eax + 00000090h]
EXIT:
		jmp[PacketProcessingFuncJmpBackAddr]
	}
}

DWORD GetPlayerAvatarBaseAddr = 0;
void __declspec(naked)GetPlayerAvatarBaseFunc()
{
	__asm pushad
	LOG(2, "GetPlayerAvatarBaseFunc\n");
	__asm popad

	__asm
	{
		mov[ebx + 0CAh], eax
		pushad
		mov eax, ebx
		add eax, 0CAh
		add eax, 6h
		mov Containers::gameVals.playerAvatarBaseAddr, eax
	}

	Containers::gameVals.playerAvatarAddr = reinterpret_cast<int*>(Containers::gameVals.playerAvatarBaseAddr + 0x610C);
	Containers::gameVals.playerAvatarColAddr = reinterpret_cast<int*>(Containers::gameVals.playerAvatarBaseAddr + 0x6110);
	Containers::gameVals.playerAvatarAcc1 = reinterpret_cast<BYTE*>(Containers::gameVals.playerAvatarBaseAddr + 0x61C4);
	Containers::gameVals.playerAvatarAcc2 = reinterpret_cast<BYTE*>(Containers::gameVals.playerAvatarBaseAddr + 0x61C5);

	LOG(2, "Containers::gameVals.playerAvatarBaseAddr: 0x%x\n", Containers::gameVals.playerAvatarBaseAddr);
	LOG(2, "Containers::gameVals.playerAvatarAddr: 0x%x\n", Containers::gameVals.playerAvatarAddr);
	LOG(2, "Containers::gameVals.playerAvatarColAddr: 0x%x\n", Containers::gameVals.playerAvatarColAddr);
	LOG(2, "Containers::gameVals.playerAvatarAcc1: 0x%x\n", Containers::gameVals.playerAvatarAcc1);
	LOG(2, "Containers::gameVals.playerAvatarAcc2: 0x%x\n", Containers::gameVals.playerAvatarAcc2);
	//restore the original optcodes after grabbing the addresses, nothing else to do here
	HookManager::DeactivateHook("GetPlayerAvatarBaseFunc");

	_asm
	{
		popad
		jmp[GetPlayerAvatarBaseAddr]
	}
}

//DWORD CpuBottleneckFixJmpBackAddr = 0;
//int ran = 0;
//void __declspec(naked)CpuBottleneckFix()
//{
//	__asm
//	{
//		add esi, 18h
//		cmp ran, 1
//		je EXIT
//		push esi
//		call QueryPerformanceFrequency
//		mov ran, 1
//EXIT:
//		jmp[CpuBottleneckFixJmpBackAddr]
//	}
//}

DWORD CpuBottleneckFix_newJmpBackAddr = 0;
void __declspec(naked)CpuBottleneckFix_new()
{
	__asm pushad
	Sleep(1);
	__asm popad
	__asm
	{
		push esi
		lea eax, [ebp-8h]
		push eax
		jmp[CpuBottleneckFix_newJmpBackAddr]
	}
}

DWORD GetGameModeIndexPointerJmpBackAddr = 0;
void __declspec(naked)GetGameModeIndexPointer()
{
	__asm pushad
	LOG(2, "GetGameModeIndexPointer\n");
	__asm popad

	__asm
	{ 
		push eax
		add eax, 108h
		mov [Containers::gameVals.pGameMode], eax
		pop eax
		mov dword ptr[eax + 108h], 0Dh
		jmp[GetGameModeIndexPointerJmpBackAddr]
	}
}

DWORD GetSetMatchVariablesJmpBackAddr = 0;
void __declspec(naked)GetSetMatchVariables()
{
	__asm pushad
	LOG(2, "GetSetMatchVariables\n");
	__asm popad

	__asm
	{
		//grab the pointers
		push ecx
		add ecx, 30h
		mov [Containers::gameVals.pMatchState], ecx //ecx +30h
		sub ecx, 18h
		mov [Containers::gameVals.pMatchTimer], ecx //ecx +18h
		sub ecx, 14h
		mov [Containers::gameVals.pMatchRounds], ecx //ecx +4h
		pop ecx
	}

	__asm
	{
		mov dword ptr[ecx + 20Ch], 0
		jmp[GetSetMatchVariablesJmpBackAddr]
	}
}

DWORD MatchStateFightStartJmpBackAddr = 0;
void __declspec(naked)MatchStateFightStart()
{
	__asm pushad
	LOG(2, "MatchStateFightStart\n");
	__asm popad

	__asm pushad
	if (activatedGameMode == customGameMode_overdrive)
	{
		Containers::gameVals.P1CharObjPointer->overdrive_timeleft = 1;
		Containers::gameVals.P2CharObjPointer->overdrive_timeleft = 1;
	}
	__asm popad
	__asm
	{
		mov dword ptr[esi + 30h], 3
		jmp[MatchStateFightStartJmpBackAddr]
	}
}

DWORD GetStageSelectAddrJmpBackAddr = 0;
void __declspec(naked)GetStageSelectAddr()
{
	__asm pushad
	LOG(2, "GetStageSelectAddr\n");
	__asm popad

	__asm
	{
		mov dword ptr[ecx + 0F54h], 0
		push eax
		mov eax, ecx
		add eax, 0A0h
		mov Containers::gameVals.stageListMemory, eax
		add eax, 0EB0h
		mov Containers::gameVals.stageSelect_X, eax
		add eax, 4h
		mov Containers::gameVals.stageSelect_Y, eax
		pop eax
		jmp[GetStageSelectAddrJmpBackAddr]
	}
}

DWORD GetMusicSelectAddrJmpBackAddr = 0;
void __declspec(naked)GetMusicSelectAddr()
{
	__asm pushad
	LOG(2, "GetMusicSelectAddr\n");
	__asm popad

	__asm
	{
		mov dword ptr[ecx + 4], 0
		mov Containers::gameVals.musicSelect_X, ecx
		push eax
		mov eax, ecx
		add eax, 4h
		mov Containers::gameVals.musicSelect_Y, eax
		pop eax
		jmp[GetMusicSelectAddrJmpBackAddr]
	}
}

DWORD GetP1ScreenPosXJmpBackAddr = 0;
void __declspec(naked)GetP1ScreenPosX()
{
	//__asm pushad
	//LOG(2, "GetP1ScreenPosX\n");
	//__asm popad

	__asm
	{
		mov esi, [edi]

		push esi
		add esi, 1C0h
		mov Containers::gameVals.P1ScreenPosX, esi
	}
	//__asm pushad
	//LOG(2, "Containers::gameVals.P1ScreenPosX: 0x%x\n", Containers::gameVals.P1ScreenPosX);
	//__asm popad
	__asm
	{
		add esi, 4
		mov Containers::gameVals.P1ScreenPosY, esi
		pop esi

		add esi, edx
		push ecx
		jmp[GetP1ScreenPosXJmpBackAddr]
	}
}

bool additional_hooks_manual()
{
	LOG(2, "additional_hooks_manual\n");
	
	//EXTRA HOOKS THAT ARE ONLY ACCESSIBLE AFTER GAME RESTARTS AND D3D9 IS UP
	HookPaletteFunctions();

	//hook steammatchmaking
	if (Containers::tempVals.ppSteamMatchmaking)
	{
		Containers::g_interfaces.pSteamMatchmakingWrapper = new SteamMatchmakingWrapper(Containers::tempVals.ppSteamMatchmaking);
	}

	//hook steamnetworking
	if (Containers::tempVals.ppSteamNetworking)
	{
		Containers::g_interfaces.pSteamNetworkingWrapper = new SteamNetworkingWrapper(Containers::tempVals.ppSteamNetworking);
		Containers::g_interfaces.pNetworkManager = new NetworkManager(Containers::g_interfaces.pSteamNetworkingWrapper);
	}

	if (Containers::tempVals.ppSteamFriends)
	{
		Containers::g_interfaces.pSteamFriendsWrapper = new SteamFriendsWrapper(Containers::tempVals.ppSteamFriends);
	}

	if (Containers::tempVals.ppSteamUser)
	{
		Containers::g_interfaces.pSteamUserWrapper = new SteamUserWrapper(Containers::tempVals.ppSteamUser);
	}

	if (Containers::tempVals.ppSteamUserStats)
	{
		Containers::g_interfaces.pSteamUserStatsWrapper = new SteamUserStatsWrapper(Containers::tempVals.ppSteamUserStats);
	}

	if (Containers::tempVals.ppSteamUtils)
	{
		Containers::g_interfaces.pSteamUtilsWrapper = new SteamUtilsWrapper(Containers::tempVals.ppSteamUtils);
	}

	if(Settings::settingsIni.cpuusagefix)
		CpuBottleneckFix_newJmpBackAddr = HookManager::SetHook("CpuBottleneckFix_new", "\x56\x8d\x45\xf8\x50\x8b\xf1", "xxxxxxx", 5, CpuBottleneckFix_new);

	GetGameStateTitleScreenJmpBackAddr = HookManager::SetHook("GetGameStateTitleScreen", "\xc7\x87\x0c\x01\x00\x00\x04\x00\x00\x00\x83\xc4\x1c", "xxxxxxxxxxxxx", 10, GetGameStateTitleScreen);
	GetGameStateMenuScreenJmpBackAddr = HookManager::SetHook("GetGameStateMenuScreen", "\xc7\x80\x0c\x01\x00\x00\x1b\x00\x00\x00\xe8\x00\x00\x00\x00", "xxxxxxxxxxx????", 10, GetGameStateMenuScreen);
	GetGameStateLobbyJmpBackAddress = HookManager::SetHook("GetGameStateLobby", "\xc7\x80\x0c\x01\x00\x00\x1f\x00\x00\x00\xe8", "xxxxxxxxxxx", 10, GetGameStateLobby);
	GetGameStateMatchStartJmpBackAddr = HookManager::SetHook("GetGameStateMatchStart", "\xc7\x80\x0c\x01\x00\x00\x0f\x00\x00\x00\xe8", "xxxxxxxxxxx", 10, GetGameStateMatchStart);
	GetGameStateVictoryScreenJmpBackAddr = HookManager::SetHook("GetGameStateVictoryScreen", "\xc7\x80\x0c\x01\x00\x00\x10\x00\x00\x00\xe8", "xxxxxxxxxxx", 10, GetGameStateVictoryScreen);
	GetGameStateVersusScreenJmpBackAddr = HookManager::SetHook("GetGameStateVersusScreen", "\xc7\x80\x0c\x01\x00\x00\x0e\x00\x00\x00\xe8", "xxxxxxxxxxx", 10, GetGameStateVersusScreen);
	GetGameStateReplayMenuScreenJmpBackAddr = HookManager::SetHook("GetGameStateReplayMenuScreen", "\xc7\x80\x0c\x01\x00\x00\x1a\x00\x00\x00\xe8", "xxxxxxxxxxx", 10, GetGameStateReplayMenuScreen);
	WindowMsgHandlerJmpBackAddr = HookManager::SetHook("WindowMsgHandler", "\x8b\x7d\x0c\x8b\xd9\x83\xfe\x10\x77\x00\x74\x00\x8b\xc6", "xxxxxxxxx?x?xx", 5, PassMsgToImGui);
	PacketProcessingFuncJmpBackAddr = HookManager::SetHook("PacketProcessingFunc", "\x8b\x55\xf8\x8d\x7d\xdc\x8b\x02", "xxxxxxxx", 18, PacketProcessingFunc);
	GetPlayerAvatarBaseAddr = HookManager::SetHook("GetPlayerAvatarBaseFunc", "\x89\x83\xca\x00\x00\x00\xe8", "xxxxxxx", 6, GetPlayerAvatarBaseFunc);
	GetGameModeIndexPointerJmpBackAddr = HookManager::SetHook("GetGameModeIndexPointer", "\xc7\x80\x08\x01\x00\x00\x0d\x00\x00\x00\x6a\x00", "xxxxxxxxxxxx", 10, GetGameModeIndexPointer);
	GetSetMatchVariablesJmpBackAddr = HookManager::SetHook("GetSetMatchVariables", "\xc7\x81\x0c\x02\x00\x00\x00\x00\x00\x00\x8b\xce", "xxxxxxxxxxxx", 10, GetSetMatchVariables);
	MatchStateFightStartJmpBackAddr = HookManager::SetHook("MatchStateFightStart", "\xc7\x46\x30\x03\x00\x00\x00\xb8", "xxxxxxxx", 7, MatchStateFightStart);
	GetStageSelectAddrJmpBackAddr = HookManager::SetHook("GetStageSelectAddr", "\xc7\x81\x54\x0f\x00\x00\x00\x00\x00\x00\x8d\x41\x0c", "xxxxxxxxxxxxx", 10, GetStageSelectAddr);
	GetMusicSelectAddrJmpBackAddr = HookManager::SetHook("GetMusicSelectAddr", "\xc7\x41\x04\x00\x00\x00\x00\x8d\x41\x0c", "xxxxxxxxxx", 7, GetMusicSelectAddr);

	GetP1ScreenPosXJmpBackAddr = HookManager::SetHook("GetP1ScreenPosX", "\x8b\x37\x03\xf2\x51", "xxxxx", 5, GetP1ScreenPosX);

	HookManager::RegisterHook("GetMoneyAddr", "\xff\x35\x00\x00\x00\x00\x8d\x45\xb4\x68\x00\x00\x00\x00\x50", "xx????xxxx????x", 6);
	Containers::gameVals.pGameMoney = (int*)HookManager::GetBytesFromAddr("GetMoneyAddr", 2, 4);

	CustomGamemodeHooks();

	return true;
}