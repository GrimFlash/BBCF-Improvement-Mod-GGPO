#include "../include/network_manager.h"
#include "../include/ImGui/ImGuiSystem.h"
#include "../include/custom_palette.h"
#include "../include/utils.h"
#include "../include/bbcf_im_networking.h"
#include "../include/containers.h"

NetworkManager::NetworkManager(SteamNetworkingWrapper *SteamNetworking)
{
	pSteamNetworking = SteamNetworking;
}

NetworkManager::~NetworkManager()
{
}

int NetworkManager::FillInternalPacket(im_packet_internal_t *internal_packet, im_packet_t *packet)
{
	LOG(2, "NetworkManager::FillInternalPacket\n");

	internal_packet->code = 0x98;
	internal_packet->type = packet->type;
	internal_packet->playernum = packet->playernum;
	internal_packet->part = packet->part;
	internal_packet->datalen = packet->datalen;
	memcpy(internal_packet->data, packet->data, packet->datalen);
	internal_packet->length1 = packet->datalen + // could just use GetFinalTransferSizeOfPacket here???
		sizeof(internal_packet->datalen) +
		sizeof(internal_packet->part) +
		sizeof(internal_packet->type) +
		sizeof(internal_packet->playernum) +
		sizeof(internal_packet->code) +
		(2 * sizeof(internal_packet->length1));
	internal_packet->length2 = internal_packet->length1;

	return internal_packet->length1;
}

bool NetworkManager::SendPacket(CSteamID *playerID, im_packet_t *packet)
{
	LOG(2, "NetworkManager::SendPacket\n");

	if (packet->playernum == 0)
	{
		LOG(2, "Not sending packet as spectator...\n");
		return false;
	}

	EP2PSend sendtype = k_EP2PSendUnreliable;
	im_packet_internal_t internal_packet;
	FillInternalPacket(&internal_packet, *&packet);

	LOG(2, "\tSending packet: \n");
	LOG(2, "\tlength1: %s\n", RawMemoryArrayToString((unsigned char*)&internal_packet.length1, sizeof(internal_packet.length1)));
	LOG(2, "\tlength2: %s\n", RawMemoryArrayToString((unsigned char*)&internal_packet.length2, sizeof(internal_packet.length2)));
	LOG(2, "\tcode: %s\n", RawMemoryArrayToString((unsigned char*)&internal_packet.code, sizeof(internal_packet.code)));
	LOG(2, "\ttype: %s\n", RawMemoryArrayToString((unsigned char*)&internal_packet.type, sizeof(internal_packet.type)));
	LOG(2, "\tplayernum: %s\n", RawMemoryArrayToString((unsigned char*)&internal_packet.playernum, sizeof(internal_packet.playernum)));
	LOG(2, "\tpart: %s\n", RawMemoryArrayToString((unsigned char*)&internal_packet.part, sizeof(internal_packet.part)));
	LOG(2, "\tdatalen: %s\n", RawMemoryArrayToString((unsigned char*)&internal_packet.datalen, sizeof(internal_packet.datalen)));
	LOG(2, "\tdata: %s\n", RawMemoryArrayToString((unsigned char*)&internal_packet.data, internal_packet.datalen));

	bool ret = false;

	//sending to a single person
	if (packet->type == packetType_IMID)
	{
		ret = pSteamNetworking->SendP2PPacket(*playerID, &internal_packet, internal_packet.length1, sendtype, 0);
		return ret;
	}

	for (int i = 0; i < Containers::gameVals.playersInMatch.size(); i++)
	{
		LOG(2, "Sending packet to: 0x%x\n", Containers::gameVals.playersInMatch[i]);
		ret = pSteamNetworking->SendP2PPacket(Containers::gameVals.playersInMatch[i], &internal_packet, internal_packet.length1, sendtype, 0);
	}

	return ret;
}

void NetworkManager::PacketProcesser(im_packet_internal_t* packet)
{

	LOG(2, "NetworkManager::PacketProcesser\n");

	if (packet->code != 0x98) //do not compare with '\x98', it doesnt work
	{
		LOG(2, "Opponent is using incompatible version of BBCFIM\n");
		ImGuiSystem::AddLog("[error] Opponent is using incompatible version of BBCFIM\n");
		return;
	}

	if (packet->playernum == 0)
	{
		LOG(2, "Packet received from spectator\n");
		return;
	}

	switch (packet->type)
	{
	case packetType_IMID:
		Receive_BBCFIM_ID(packet);
		break;
	case packetType_palette:
		LoadPalettePacket(packet);
		break;
	case packetType_effect:
		LoadEffectPacket(packet);
		break;
	case packetType_bloom:
		LoadBloomPacket(packet);
		break;
	case packetType_gamemode:
		Receive_customGamemode_request(packet);
		break;
	default:
		LOG(2, "packet type not found\n");
		ImGuiSystem::AddLog("[error] Packet type not found\n");
	}
}

int NetworkManager::GetFinalTransferSizeOfPacket(im_packet_t *packet)
{
	if (!packet)
		return 0;

	im_packet_internal_t internal_packet;

	int size = packet->datalen + //this is the data
		sizeof(internal_packet.datalen) +
		sizeof(internal_packet.part) +
		sizeof(internal_packet.type) +
		sizeof(internal_packet.playernum) +
		sizeof(internal_packet.code) +
		(2 * sizeof(internal_packet.length1));

	return size;
}
