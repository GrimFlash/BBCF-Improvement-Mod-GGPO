#include "ActiveGameModeWidget.h"

#include "Core/interfaces.h"
#include "Game/gamestates.h"

#include <imgui.h>

void ActiveGameModeWidget()
{
	ImGui::BeginGroup();

	bool isOnlineMatch = g_interfaces.pRoomManager->IsRoomFunctional();
	std::string activeGameMode;

	if (isOnlineMatch)
	{
		CustomGameMode currentGameMode = g_interfaces.pOnlineGameModeManager->GetSettledGameModeChoice();
		activeGameMode = g_interfaces.pGameModeManager->GetGameModeName(currentGameMode);
	}
	else
	{
		activeGameMode = g_interfaces.pGameModeManager->GetCurrentGameModeName();
	}

	ImGui::Text("ACTIVE GAME MODE: %s", activeGameMode.c_str());

	if (isOnlineMatch && isOnCharacterSelectionScreen())
	{
		const std::string player1SelectedGameMode = g_interfaces.pGameModeManager->GetGameModeName(
			g_interfaces.pOnlineGameModeManager->GetPlayer1GameModeChoice()
		);

		const std::string player2SelectedGameMode = g_interfaces.pGameModeManager->GetGameModeName(
			g_interfaces.pOnlineGameModeManager->GetPlayer2GameModeChoice()
		);

		// As spectator show both P1 and P2 selection
		if (g_interfaces.pRoomManager->IsThisPlayerSpectator())
		{
			ImGui::Text("Player 1 selection: %s", player1SelectedGameMode.c_str());
			ImGui::Text("Player 2 selection: %s", player2SelectedGameMode.c_str());
		}
		else // As P1 or P2 only show the opponent's selection
		{
			const std::string& opponentSelectedMode = g_interfaces.pRoomManager->GetThisPlayerMatchPlayerIndex()
				? player1SelectedGameMode
				: player2SelectedGameMode;

			ImGui::Text("Opponent selection: %s", opponentSelectedMode.c_str());
		}
	}

	ImGui::EndGroup();
}
