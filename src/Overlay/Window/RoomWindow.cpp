#include "RoomWindow.h"

#include "Core/interfaces.h"
#include "Core/utils.h"
#include "Game/gamestates.h"
#include "Overlay/imgui_utils.h"
#include "Overlay/WindowManager.h"
#include "Overlay/Widget/ActiveGameModeWidget.h"
#include "Overlay/Widget/GameModeSelectWidget.h"
#include "Overlay/Widget/StageSelectWidget.h"
#include "Overlay/Window/PaletteEditorWindow.h"

void RoomWindow::BeforeDraw()
{
	ImGui::SetWindowPos(m_windowTitle.c_str(), ImVec2(200, 200), ImGuiCond_FirstUseEver);

	//ImVec2 windowSizeConstraints;
	//switch (Settings::settingsIni.menusize)
	//{
	//case 1:
	//	windowSizeConstraints = ImVec2(250, 190);
	//	break;
	//case 3:
	//	windowSizeConstraints = ImVec2(400, 230);
	//	break;
	//default:
	//	windowSizeConstraints = ImVec2(330, 230);
	//}

	ImVec2 windowSizeConstraints = ImVec2(300, 190);

	ImGui::SetNextWindowSizeConstraints(windowSizeConstraints, ImVec2(1000, 1000));
}

void RoomWindow::Draw()
{
	if (!g_interfaces.pRoomManager->IsRoomFunctional())
	{
		ImGui::TextDisabled("YOU ARE NOT IN A ROOM OR ONLINE MATCH!");
		m_windowTitle = m_origWindowTitle;

		return;
	}

	std::string roomTypeName = g_interfaces.pRoomManager->GetRoomTypeName();
	SetWindowTitleRoomType(roomTypeName);

	ImGui::Text("Online type: %s", roomTypeName.c_str());

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

	if (isStageSelectorEnabledInCurrentState())
	{
		ImGui::VerticalSpacing(10);
		StageSelectWidget();
	}

	if (isOnCharacterSelectionScreen() || isOnVersusScreen() || isInMatch())
	{
		ImGui::VerticalSpacing(10);
		ActiveGameModeWidget();
	}

	if (isGameModeSelectorEnabledInCurrentState())
	{
		bool isThisPlayerSpectator = g_interfaces.pRoomManager->IsRoomFunctional() && g_interfaces.pRoomManager->IsThisPlayerSpectator();

		if (!isThisPlayerSpectator)
		{
			GameModeSelectWidget();
		}
	}

	if (isInMatch())
	{
		ImGui::VerticalSpacing(10);
		WindowManager::GetInstance().GetWindowContainer()->
			GetWindow<PaletteEditorWindow>(WindowType_PaletteEditor)->ShowAllPaletteSelections("Room");
	}

	if (isInMenu())
	{
		ImGui::VerticalSpacing(10);
		DrawRoomImPlayers();
	}

	if (isOnCharacterSelectionScreen() || isOnVersusScreen() || isInMatch())
	{
		ImGui::VerticalSpacing(10);
		DrawMatchImPlayers();
	}

	ImGui::PopStyleVar();
}

void RoomWindow::SetWindowTitleRoomType(const std::string& roomTypeName)
{
	m_windowTitle = "Online - " + roomTypeName + "###Room";
}

void RoomWindow::ShowClickableSteamUser(const char* playerName, const CSteamID& steamId) const
{
	ImGui::TextUnformatted(playerName);
	ImGui::HoverTooltip("Click to open Steam profile");
	if (ImGui::IsItemClicked())
	{
		g_interfaces.pSteamFriendsWrapper->ActivateGameOverlayToUser("steamid", steamId);
	}
}

void RoomWindow::DrawRoomImPlayers()
{
	ImGui::BeginGroup();
	ImGui::TextUnformatted("Improvement Mod users in Room:");
	ImGui::BeginChild("RoomImUsers", ImVec2(230, 150), true);

	for (const IMPlayer& imPlayer : g_interfaces.pRoomManager->GetIMPlayersInCurrentRoom())
	{
		ShowClickableSteamUser(imPlayer.steamName.c_str(), imPlayer.steamID);
		ImGui::NextColumn();
	}

	ImGui::EndChild();
	ImGui::EndGroup();
}

void RoomWindow::DrawMatchImPlayers()
{
	ImGui::BeginGroup();
	ImGui::TextUnformatted("Improvement Mod users in match:");
	ImGui::BeginChild("MatchImUsers", ImVec2(230, 150), true);

	if (g_interfaces.pRoomManager->IsThisPlayerInMatch())
	{
		ImGui::Columns(2);
		for (const IMPlayer& imPlayer : g_interfaces.pRoomManager->GetIMPlayersInCurrentMatch())
		{
			uint16_t matchPlayerIndex = g_interfaces.pRoomManager->GetPlayerMatchPlayerIndexByRoomMemberIndex(imPlayer.roomMemberIndex);
			std::string playerType;

			if (matchPlayerIndex == 0)
				playerType = "Player 1";
			else if (matchPlayerIndex == 1)
				playerType = "Player 2";
			else
				playerType = "Spectator";

			ShowClickableSteamUser(imPlayer.steamName.c_str(), imPlayer.steamID);
			ImGui::NextColumn();

			ImGui::TextUnformatted(playerType.c_str());
			ImGui::NextColumn();
		}
	}

	ImGui::EndChild();
	ImGui::EndGroup();
}
