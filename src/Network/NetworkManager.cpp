#include "NetworkManager.h"

#include "RoomManager.h"

#include "Core/interfaces.h"
#include "Core/logger.h"
#include "Core/utils.h"
#include "Game/gamestates.h"
#include "Overlay/Logger/ImGuiLogger.h"

NetworkManager::NetworkManager(SteamNetworkingWrapper* SteamNetworking, CSteamID steamID)
{
	m_pSteamNetworking = SteamNetworking;
	m_steamID = steamID;
}

NetworkManager::~NetworkManager()
{
}

bool NetworkManager::SendPacket(CSteamID* steamID, Packet* packet)
{
	LOG(2, "NetworkManager::SendPacket\n");

	packet->steamID = m_steamID.ConvertToUint64();

	LOG(2, "\tSending packet:\n");
	LOG(2, "\tversion: %s\n", RawMemoryArrayToString((unsigned char*)&packet->version, sizeof(packet->version)));
	LOG(2, "\tpacketType: %s\n", RawMemoryArrayToString((unsigned char*)&packet->packetType, sizeof(packet->packetType)));
	LOG(2, "\tpart: %s\n", RawMemoryArrayToString((unsigned char*)&packet->part, sizeof(packet->part)));
	LOG(2, "\tpacketSize: %s\n", RawMemoryArrayToString((unsigned char*)&packet->packetSize, sizeof(packet->packetSize)));
	LOG(2, "\troomPlayerIndex: %s\n", RawMemoryArrayToString((unsigned char*)&packet->roomMemberIndex, sizeof(packet->roomMemberIndex)));
	LOG(2, "\tsteamID: %s\n", RawMemoryArrayToString((unsigned char*)&packet->steamID, sizeof(packet->steamID)));
	LOG(2, "\tdataSize: %s\n", RawMemoryArrayToString((unsigned char*)&packet->dataSize, sizeof(packet->dataSize)));
	//LOG(2, "\tdata: %s\n", RawMemoryArrayToString((unsigned char*)&packet->data, sizeof(packet->data)));

	EP2PSend sendType = k_EP2PSendUnreliable;

	return m_pSteamNetworking->SendP2PPacket(*steamID, packet, packet->packetSize, sendType, 0);
}

void NetworkManager::RecvPacket(Packet* packet)
{
	LOG(7, "NetworkManager::RecvPacket\n");

	if (!g_interfaces.pRoomManager->IsPacketFromSameRoom(packet))
	{
		LOG(2, "[error] Packet received from not a room member. RoomPlayerIndex: %d, SteamID: %llu\n",
			packet->roomMemberIndex, packet->steamID);

		return;
	}

	switch (packet->packetType)
	{
	case PacketType_IMID_Announce:
		g_interfaces.pRoomManager->SendAcknowledge(packet);
		break;

	case PacketType_IMID_Acknowledge:
		g_interfaces.pRoomManager->AcceptAcknowledge(packet);
		break;

	case PacketType_PaletteInfo:
		if (g_interfaces.pRoomManager->IsPacketFromSameMatchNonSpectator(packet))
		{
			g_interfaces.pOnlinePaletteManager->RecvPaletteInfoPacket(packet);
		}
		break;

	case PacketType_PaletteData:
		if (g_interfaces.pRoomManager->IsPacketFromSameMatchNonSpectator(packet))
		{
			g_interfaces.pOnlinePaletteManager->RecvPaletteDataPacket(packet);
		}
		break;

	case PacketType_GameMode:
		if (g_interfaces.pRoomManager->IsPacketFromSameMatchNonSpectator(packet) &&
			*g_gameVals.pGameState == GameState_CharacterSelectionScreen)
		{
			g_interfaces.pOnlineGameModeManager->RecvGameModePacket(packet);
		}
		break;

	default:
		LOG(2, "Unknown packet type received: %d\n", packet->packetType);
		g_imGuiLogger->Log("[error] Unknown packet type received (%d)\n", packet->packetType);
	}
}

bool NetworkManager::IsIMPacket(Packet* packet)
{
	return packet->version == IM_PACKET_VERSION;
}