#pragma once
#include "SteamApiWrapper/SteamNetworkingWrapper.h"
#include <steam_api.h>

struct im_packet_t
{
	unsigned char type;
	unsigned short playernum;
	unsigned short part;
	unsigned short datalen;
	unsigned char data[1400];
};

struct im_packet_internal_t
{
	unsigned short length1;
	unsigned short length2;
	unsigned char code;			//always 0x98 (version 1.23 uses code '\x99')
	unsigned char type;			// see packetType enum
	unsigned short playernum;	//'\x01' or'\x02'
	unsigned short part;		//'\x01', '\x02' etc
	unsigned short datalen;
	unsigned char data[1400];
};

enum packetType : unsigned char
{
	packetType_IMID = 0x01, // reserved for sending information about player's BBCFIM version
	packetType_palette = 0x02,
	packetType_effect = 0x03,
	packetType_bloom = 0x04,
	packetType_gamemode = 0x05
};

class NetworkManager
{
public:
	NetworkManager(SteamNetworkingWrapper* SteamNetworking);
	~NetworkManager();
	bool SendPacket(CSteamID* playerID, im_packet_t *packet);
	void PacketProcesser(im_packet_internal_t *packet);
	//returns the size of the packet that will be sent internally
	int GetFinalTransferSizeOfPacket(im_packet_t *packet);
private:
	SteamNetworkingWrapper* pSteamNetworking;
	int FillInternalPacket(im_packet_internal_t *internal_packet, im_packet_t *packet);
};