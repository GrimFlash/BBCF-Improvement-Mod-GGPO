#pragma once

#include "RoomManager.h"

#include "CustomGameMode/GameModeManager.h"

class OnlineGameModeManager
{
public:
	OnlineGameModeManager(GameModeManager* pGameModeManager, RoomManager* pRoomManager);
	void SendGameModePacket();
	void RecvGameModePacket(Packet* packet);
	void SetThisPlayerGameMode(CustomGameMode gameMode);
	const CustomGameMode GetPlayer1GameModeChoice() const { return m_player1GameModeChoice; }
	const CustomGameMode GetPlayer2GameModeChoice() const { return m_player2GameModeChoice; }
	const CustomGameMode GetSettledGameModeChoice() const;
	void ClearPlayerGameModeChoices();
	void OnMatchInit();

private:
	CustomGameMode m_player1GameModeChoice;
	CustomGameMode m_player2GameModeChoice;

	// Interfaces
	RoomManager* m_pRoomManager;
	GameModeManager* m_pGameModeManager;
};
