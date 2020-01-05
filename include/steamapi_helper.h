#pragma once
#include "stdafx.h"
#include <steam_api.h>

//note: commented out functions are not working

class SteamApiHelper
{
public:
	int current_players;
	SteamApiHelper(ISteamUserStats* pSteamUserStats, ISteamFriends* pSteamFriends);
	void UpdateNumberOfCurrentPlayers(); 
	//void GetRequestUserInformation(CSteamID steamID);
private:
	ISteamUserStats* pSteamUserStats;
	ISteamFriends* pSteamFriends;
	void OnUpdateNumberOfCurrentPlayers(NumberOfCurrentPlayers_t *pCallback, bool bIOFailure);
	CCallResult< SteamApiHelper, NumberOfCurrentPlayers_t > m_NumberOfCurrentPlayersCallResult;
	//void OnGetRequestUserInformation(PersonaStateChange_t *pCallback, bool bIOFailure);
	//CCallResult< SteamApiHelper, PersonaStateChange_t > m_RequestUserInformationCallResult;
}; 