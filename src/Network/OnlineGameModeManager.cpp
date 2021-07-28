#include "OnlineGameModeManager.h"

#include "Core/logger.h"

OnlineGameModeManager::OnlineGameModeManager(GameModeManager* pGameModeManager, RoomManager* pRoomManager)
	: m_pGameModeManager(pGameModeManager), m_pRoomManager(pRoomManager),
	m_player1GameModeChoice(CustomGameMode_None), m_player2GameModeChoice(CustomGameMode_None)
{
}

void OnlineGameModeManager::SendGameModePacket()
{
	LOG(2, "OnlineGameModeManager::SendGameModePacket\n");

	if (m_pRoomManager->IsThisPlayerSpectator())
		return;

	uint16_t thisPlayerMatchPlayerIndex = m_pRoomManager->GetThisPlayerMatchPlayerIndex();
	CustomGameMode selectedGameMode = m_pGameModeManager->GetActiveGameMode();

	Packet packet = Packet(&selectedGameMode, sizeof(CustomGameMode), PacketType_GameMode, thisPlayerMatchPlayerIndex);

	m_pRoomManager->SendPacketToSameMatchIMPlayers(&packet);
}

void OnlineGameModeManager::RecvGameModePacket(Packet* packet)
{
	LOG(2, "OnlineGameModeManager::RecvGameModePacket\n");

	uint16_t matchPlayerIndex = m_pRoomManager->GetPlayerMatchPlayerIndexByRoomMemberIndex(packet->roomMemberIndex);
	CustomGameMode selectedGameMode = (CustomGameMode)packet->data[0];
	CustomGameMode* pOtherPlayerGameModeChoice = matchPlayerIndex == 0 ? &m_player1GameModeChoice : &m_player2GameModeChoice;

	*pOtherPlayerGameModeChoice = selectedGameMode;
}

void OnlineGameModeManager::SetThisPlayerGameMode(CustomGameMode gameMode)
{
	LOG(2, "OnlineGameModeManager::SetThisPlayerGameMode\n");

	if (m_pRoomManager->IsThisPlayerSpectator())
		return;

	uint16_t thisPlayerMatchPlayerIndex = m_pRoomManager->GetThisPlayerMatchPlayerIndex();
	CustomGameMode* pThisPlayerGameModeChoice = thisPlayerMatchPlayerIndex == 0 ? &m_player1GameModeChoice : &m_player2GameModeChoice;

	*pThisPlayerGameModeChoice = gameMode;
}

const CustomGameMode OnlineGameModeManager::GetSettledGameModeChoice() const
{
	if (m_player1GameModeChoice == m_player2GameModeChoice)
		return m_player1GameModeChoice;

	return CustomGameMode_None;
}

void OnlineGameModeManager::ClearPlayerGameModeChoices()
{
	LOG(2, "OnlineGameModeManager::ClearPlayerGameModeChoices\n");

	m_player1GameModeChoice = CustomGameMode_None;
	m_player2GameModeChoice = CustomGameMode_None;
}

void OnlineGameModeManager::OnMatchInit()
{
	LOG(2, "OnlineGameModeManager::OnMatchInit\n");

	// Set if both players settled on the same gamemode

	CustomGameMode gameModeToActivate = m_player1GameModeChoice == m_player2GameModeChoice
		? m_player1GameModeChoice
		: CustomGameMode_None;

	m_pGameModeManager->SetActiveGameMode(gameModeToActivate);
}