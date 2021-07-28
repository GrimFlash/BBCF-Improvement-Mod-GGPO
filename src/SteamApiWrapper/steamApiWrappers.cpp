#include "steamApiWrappers.h"

#include "Core/interfaces.h"
#include "Core/logger.h"

bool InitSteamApiWrappers()
{
	LOG(1, "InitSteamApiWrappers\n");

	if (g_tempVals.ppSteamMatchmaking &&
		!g_interfaces.pSteamMatchmakingWrapper)
	{
		g_interfaces.pSteamMatchmakingWrapper = new SteamMatchmakingWrapper(g_tempVals.ppSteamMatchmaking);
	}

	if (g_tempVals.ppSteamFriends &&
		!g_interfaces.pSteamFriendsWrapper)
	{
		g_interfaces.pSteamFriendsWrapper = new SteamFriendsWrapper(g_tempVals.ppSteamFriends);
	}

	if (g_tempVals.ppSteamUser &&
		!g_interfaces.pSteamUserWrapper)
	{
		g_interfaces.pSteamUserWrapper = new SteamUserWrapper(g_tempVals.ppSteamUser);
	}

	if (g_tempVals.ppSteamNetworking &&
		!g_interfaces.pSteamNetworkingWrapper)
	{
		g_interfaces.pSteamNetworkingWrapper = new SteamNetworkingWrapper(g_tempVals.ppSteamNetworking);
	}

	if (g_tempVals.ppSteamUserStats &&
		!g_interfaces.pSteamUserStatsWrapper)
	{
		g_interfaces.pSteamUserStatsWrapper = new SteamUserStatsWrapper(g_tempVals.ppSteamUserStats);
	}

	if (g_tempVals.ppSteamUtils &&
		!g_interfaces.pSteamUtilsWrapper)
	{
		g_interfaces.pSteamUtilsWrapper = new SteamUtilsWrapper(g_tempVals.ppSteamUtils);
	}

	if (g_interfaces.pSteamUserStatsWrapper &&
		g_interfaces.pSteamFriendsWrapper &&
		!g_interfaces.pSteamApiHelper)
	{
		g_interfaces.pSteamApiHelper = new SteamApiHelper(g_interfaces.pSteamUserStatsWrapper, g_interfaces.pSteamFriendsWrapper);
	}

	return true;
}