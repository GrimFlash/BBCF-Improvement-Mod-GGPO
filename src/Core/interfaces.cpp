#include "interfaces.h"

#include "logger.h"
#include "utils.h"

interfaces_t g_interfaces = {};
gameProc_t g_gameProc = {};
temps_t g_tempVals = {};
gameVals_t g_gameVals = {};

void InitManagers()
{
	LOG(1, "InitManagers\n");

	if (g_interfaces.pSteamNetworkingWrapper &&
		g_interfaces.pSteamUserWrapper &&
		!g_interfaces.pNetworkManager)
	{
		g_interfaces.pNetworkManager = new NetworkManager(
			g_interfaces.pSteamNetworkingWrapper,
			g_interfaces.pSteamUserWrapper->GetSteamID()
		);
	}

	if (g_interfaces.pNetworkManager &&
		g_interfaces.pSteamUserWrapper &&
		g_interfaces.pSteamFriendsWrapper &&
		!g_interfaces.pRoomManager)
	{
		g_interfaces.pRoomManager = new RoomManager(
			g_interfaces.pNetworkManager,
			g_interfaces.pSteamFriendsWrapper,
			g_interfaces.pSteamUserWrapper->GetSteamID()
		);
	}

	if (g_interfaces.pPaletteManager &&
		g_interfaces.pRoomManager &&
		!g_interfaces.pOnlinePaletteManager)
	{
		g_interfaces.pOnlinePaletteManager = new OnlinePaletteManager(
			g_interfaces.pPaletteManager,
			&g_interfaces.player1.GetPalHandle(),
			&g_interfaces.player2.GetPalHandle(),
			g_interfaces.pRoomManager
		);
	}

	if (!g_interfaces.pGameModeManager)
	{
		g_interfaces.pGameModeManager = new GameModeManager();
	}

	if (g_interfaces.pGameModeManager &&
		g_interfaces.pRoomManager &&
		!g_interfaces.pOnlineGameModeManager)
	{
		g_interfaces.pOnlineGameModeManager = new OnlineGameModeManager(
			g_interfaces.pGameModeManager,
			g_interfaces.pRoomManager
		);
	}
}

void CleanupInterfaces()
{
	LOG(1, "CleanupInterfaces\n");

	SAFE_DELETE(g_interfaces.pNetworkManager);
	SAFE_DELETE(g_interfaces.pPaletteManager);
	SAFE_DELETE(g_interfaces.pRoomManager);
	SAFE_DELETE(g_interfaces.pOnlinePaletteManager);
	SAFE_DELETE(g_interfaces.pOnlineGameModeManager);
	SAFE_DELETE(g_interfaces.pGameModeManager);

	SAFE_DELETE(g_interfaces.pD3D9ExWrapper);

	SAFE_DELETE(g_interfaces.pSteamFriendsWrapper);
	SAFE_DELETE(g_interfaces.pSteamMatchmakingWrapper);
	SAFE_DELETE(g_interfaces.pSteamNetworkingWrapper);
	SAFE_DELETE(g_interfaces.pSteamUserStatsWrapper);
	SAFE_DELETE(g_interfaces.pSteamUserWrapper);
	SAFE_DELETE(g_interfaces.pSteamUtilsWrapper);
	SAFE_DELETE(g_interfaces.pSteamApiHelper);
}
