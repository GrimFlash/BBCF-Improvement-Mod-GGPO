#include "../include/bbcf_im_networking.h"
#include "../include/containers.h"
#include "../include/settings.h"
#include "../include/ImGui/ImGuiSystem.h"
#include "../include/custom_gamemodes.h"
#include "../include/gamestates_defines.h"

void Send_BBCFIM_ID(CSteamID opponentSteamID)
{
	LOG(2, "Send_BBCFIM_ID\n");

	EP2PSend sendtype = k_EP2PSendUnreliable;
	im_packet_t packet;

	packet.type = packetType_IMID;
	packet.playernum = Containers::gameVals.thisPlayerNum;
	packet.datalen = sizeof(int) + (strlen(version_num) + 1);

	packet.part = 1;
	memcpy(&packet.data[0], &internal_version_num, sizeof(int));
	memcpy(&packet.data[4], version_num, strlen(version_num) + 1);

	Containers::g_interfaces.pNetworkManager->SendPacket(&opponentSteamID, &packet);

#ifndef RELEASE_VER
	ImGuiSystem::AddLog("[debug] BBCFIM_ID packet sent to: '%s'\n", Containers::g_interfaces.pSteamFriendsWrapper->GetFriendPersonaName(opponentSteamID));
#endif // !RELEASE_VER
}

void Receive_BBCFIM_ID(im_packet_internal_t *packet)
{
	LOG(2, "Receive_BBCFIM_ID\n");

	if (*Containers::gameVals.pGameState != GAME_STATE_CHARACTER_SELECTION_SCREEN)
	{
		LOG(2, "ERROR, RECEIVED BBCFIM ID PACKET WHILE NOT BEING ON THE CHARSELECT SCREEN\n");
		ImGuiSystem::AddLog("[error] BBCFIM ID packet received outside of character selection screen\n");
		return;
	}

	char opponentVersionNum[10];
	std::string text;
	memcpy(opponentVersionNum, &packet->data[4], strlen(version_num) + 1);

	//we are spectators
	if (Containers::gameVals.thisPlayerNum == 0)
	{
		if (packet->playernum == 1)
		{
			text = "Player1 ";
			memcpy(&Containers::gameVals.iP1BBCFIMvernum, &packet->data[0], sizeof(int));
			Containers::gameVals.sP1BBCFIMvernum = opponentVersionNum;
			Containers::gameVals.bP1UsingBBCFIM = true;
		}
		else
		{
			text = "Player2 ";
			memcpy(&Containers::gameVals.iP2BBCFIMvernum, &packet->data[0], sizeof(int));
			Containers::gameVals.sP2BBCFIMvernum = opponentVersionNum;
			Containers::gameVals.bP2UsingBBCFIM = true;
		}

	}
	else //As P1 / P2
	{
		text = "Opponent ";
		memcpy(&Containers::gameVals.iOpponentBBCFIMvernum, &packet->data[0], sizeof(int));
		Containers::gameVals.sOpponentBBCFIMvernum = opponentVersionNum;
		Containers::gameVals.bOpponentUsingBBCFIM = true;
	}

	text += "BBCFIM ";
	text += opponentVersionNum;
	text += " detected";
	ImGuiSystem::SetNotification(text.c_str(), 10, true);
	text += '\n';
	ImGuiSystem::AddLog("[system] %s", text.c_str());
}

void Send_customGamemode_request(CSteamID opponentSteamID)
{
	LOG(2, "Send_customGamemode_request\n");

	EP2PSend sendtype = k_EP2PSendUnreliable;
	im_packet_t packet;

	packet.type = packetType_gamemode;
	packet.playernum = Containers::gameVals.thisPlayerNum;
	packet.datalen = sizeof(int);

	packet.part = 1;
	memcpy(&packet.data[0], &activatedGameMode, sizeof(int));

	Containers::g_interfaces.pNetworkManager->SendPacket(&opponentSteamID, &packet);

#ifndef RELEASE_VER
	ImGuiSystem::AddLog("[debug] customGameMode packet sent to: '%s'\n", Containers::g_interfaces.pSteamFriendsWrapper->GetFriendPersonaName(opponentSteamID));
#endif // !RELEASE_VER
}

void Receive_customGamemode_request(im_packet_internal_t *packet)
{
	LOG(2, "Receive_customGamemode_request\n");

	std::string text;

	//if spectators
	if (Containers::gameVals.thisPlayerNum == 0)
	{
		if (packet->playernum == 1)
		{
			memcpy(&P1_activatedGameMode, &packet->data[0], sizeof(int));
			text = "Player 1 ";
		}
		else
		{
			memcpy(&P2_activatedGameMode, &packet->data[0], sizeof(int));
			text = "Player 2 ";
		}
	}
	else
	{
		memcpy(&opponent_activatedGameMode, &packet->data[0], sizeof(int));
		text = "Opponent ";
	}

	//gamemodes are different
	if (opponent_activatedGameMode != activatedGameMode || P1_activatedGameMode != P2_activatedGameMode)
	{
		text += "has set a custom game mode";
		ImGuiSystem::SetNotification(text.c_str(), 10, true);
		text += '\n';
		ImGuiSystem::AddLog("[system] %s", text.c_str());
	}
}