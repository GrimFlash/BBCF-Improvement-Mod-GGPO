#include "MatchState.h"

#include "Core/interfaces.h"
#include "Core/logger.h"
#include "Overlay/Window/PaletteEditorWindow.h"
#include "Overlay/WindowContainer/WindowType.h"
#include "Overlay/WindowManager.h"
#include "Rollback/SavedGameState.h"

void MatchState::OnMatchInit()
{
	if (!WindowManager::GetInstance().IsInitialized())
	{
		return;
	}

	LOG(2, "MatchState::OnMatchInit\n");

	g_interfaces.pPaletteManager->LoadPaletteSettingsFile();
	g_interfaces.pPaletteManager->OnMatchInit(g_interfaces.player1, g_interfaces.player2);

	if (g_interfaces.pRoomManager->IsRoomFunctional())
	{
		// Prevent loading palettes.ini custom palette on opponent

		uint16_t thisPlayerMatchPlayerIndex = g_interfaces.pRoomManager->GetThisPlayerMatchPlayerIndex();

		if (thisPlayerMatchPlayerIndex != 0)
		{
			g_interfaces.pPaletteManager->RestoreOrigPal(g_interfaces.player1.GetPalHandle());
		}

		if (thisPlayerMatchPlayerIndex != 1)
		{
			g_interfaces.pPaletteManager->RestoreOrigPal(g_interfaces.player2.GetPalHandle());
		}

		// Send our custom palette and load opponent's
		g_interfaces.pOnlinePaletteManager->OnMatchInit();

		// Activate settled game mode
		g_interfaces.pOnlineGameModeManager->OnMatchInit();

		// Add players to steam's "recent games" list
		for (const RoomMemberEntry* player : g_interfaces.pRoomManager->GetOtherRoomMemberEntriesInCurrentMatch())
		{
			g_interfaces.pSteamFriendsWrapper->SetPlayedWith(CSteamID(player->steamId));
		}
	}

	g_gameVals.isFrameFrozen = false;

	WindowManager::GetInstance().GetWindowContainer()->GetWindow<PaletteEditorWindow>(WindowType_PaletteEditor)->OnMatchInit();

	initGameStateAddresses();
}

void MatchState::OnMatchRematch()
{
	LOG(2, "MatchState::OnMatchRematch\n");

	g_interfaces.pPaletteManager->OnMatchRematch(
		g_interfaces.player1,
		g_interfaces.player2
	);
}

void MatchState::OnMatchEnd()
{
	LOG(2, "MatchState::OnMatchEnd\n");

	g_interfaces.pGameModeManager->EndGameMode();

	g_interfaces.pPaletteManager->OnMatchEnd(
		g_interfaces.player1.GetPalHandle(),
		g_interfaces.player2.GetPalHandle()
	);

	g_interfaces.pOnlinePaletteManager->ClearSavedPalettePacketQueues();
	g_interfaces.pOnlineGameModeManager->ClearPlayerGameModeChoices();
}

void MatchState::OnUpdate()
{
	LOG(7, "MatchState::OnUpdate\n");

	g_interfaces.pPaletteManager->OnUpdate(
		g_interfaces.player1.GetPalHandle(),
		g_interfaces.player2.GetPalHandle()
	);
}