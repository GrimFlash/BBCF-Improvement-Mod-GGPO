#include "SteamApiHelper.h"

#include "Core/logger.h"
#include "Overlay/Logger/ImGuiLogger.h"

SteamApiHelper::SteamApiHelper(ISteamUserStats* pSteamUserStats, ISteamFriends* pSteamFriends)
	: m_pSteamUserStats(pSteamUserStats), m_pSteamFriends(pSteamFriends)
{
	LOG(2, "SteamApiHelper::SteamApiHelper, *pSteamUserStats: 0x%p, *pSteamFriends: 0x%p\n", *pSteamUserStats, *pSteamFriends);

	UpdateNumberOfCurrentPlayers();
}

// Make the asynchronous request to receive the number of current players. 
void SteamApiHelper::UpdateNumberOfCurrentPlayers()
{
	SteamAPICall_t hSteamAPICall = m_pSteamUserStats->GetNumberOfCurrentPlayers();
	m_numberOfCurrentPlayersCallResult.Set(hSteamAPICall, this, &SteamApiHelper::OnUpdateNumberOfCurrentPlayers);
}

// Callback for UpdateNumberOfCurrentPlayers()
void SteamApiHelper::OnUpdateNumberOfCurrentPlayers(NumberOfCurrentPlayers_t* pCallback, bool bIOFailure)
{
	if (bIOFailure || !pCallback->m_bSuccess)
	{
		g_imGuiLogger->Log("[error] Failed to update the current number of ingame players\n");
		m_currentPlayersCount = -1;
		return;
	}

#ifdef ENABLE_LOGGING
	g_imGuiLogger->Log("[debug] Updated the current number of ingame players: %d\n", pCallback->m_cPlayers);
#endif

	m_currentPlayersCount = pCallback->m_cPlayers;
}

const int SteamApiHelper::GetCurrentPlayersCount() const
{
	return m_currentPlayersCount;
}

std::string SteamApiHelper::GetCurrentPlayersCountString() const
{
	return m_currentPlayersCount < 0 ? "<No data>" : std::to_string(m_currentPlayersCount);
}

bool SteamApiHelper::IsSteamOverlayActive() const
{
	return m_isSteamOverlayActive;
}

void SteamApiHelper::OnGameOverlayActivated(GameOverlayActivated_t* callback)
{
	m_isSteamOverlayActive = callback->m_bActive;
}
