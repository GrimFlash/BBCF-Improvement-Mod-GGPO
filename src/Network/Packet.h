#pragma once
#include <cstdint>
#include <cstring>

#define IM_PACKET_VERSION 0x95
#define IM_PACKET_VERSION_ASM 95h

constexpr int MAX_DATA_SIZE = 1200;

enum PacketType : uint16_t
{
	PacketType_IMID_Announce,
	PacketType_IMID_Acknowledge,
	PacketType_PaletteInfo,
	PacketType_PaletteData,
	PacketType_GameMode
};

// BBCF packets' first two fields must be the packet size
struct Packet
{
	uint16_t __packetSize; // BBCF packets' first two fields must be the packet size, the game performs validation on them upon receiving
	uint16_t packetSize;
	uint16_t version = IM_PACKET_VERSION;
	PacketType packetType;
	uint64_t steamID;
	uint16_t part;
	uint16_t roomMemberIndex;
	uint32_t dataSize;
	unsigned char data[MAX_DATA_SIZE];

	Packet(void* dataSrc, uint16_t dataSize, PacketType packetType, uint16_t roomMemberIndex, uint16_t part = 0)
		: dataSize(dataSize), packetType(packetType), roomMemberIndex(roomMemberIndex), part(part)
	{
		// HeaderSize + dataSize
		packetSize = sizeof(Packet) - MAX_DATA_SIZE + dataSize;
		__packetSize = packetSize;

		if (dataSrc && dataSize)
		{
			memcpy_s(data, MAX_DATA_SIZE, dataSrc, dataSize);
		}
	}
};