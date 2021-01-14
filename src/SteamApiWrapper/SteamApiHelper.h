#pragma once

#include <string>
#include <steam_api.h>

class SteamApiHelper
{
public:
	SteamApiHelper(ISteamUserStats* pSteamUserStats, ISteamFriends* pSteamFriends);
	void UpdateNumberOfCurrentPlayers();
	const int GetCurrentPlayersCount() const;
	std::string GetCurrentPlayersCountString() const;
	bool IsSteamOverlayActive() const;

private:
	void OnUpdateNumberOfCurrentPlayers(NumberOfCurrentPlayers_t* pCallback, bool bIOFailure);
	CCallResult< SteamApiHelper, NumberOfCurrentPlayers_t > m_numberOfCurrentPlayersCallResult;

	STEAM_CALLBACK(SteamApiHelper, OnGameOverlayActivated, GameOverlayActivated_t);
	
	int m_currentPlayersCount = -1;
	bool m_isSteamOverlayActive = false;

	// Interfaces
	ISteamUserStats* m_pSteamUserStats;
	ISteamFriends* m_pSteamFriends;
};