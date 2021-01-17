#pragma once
#include <cstdint>

#pragma pack(push, 1) // Prevent padding by compiler

class RoomMemberEntry
{
public:
	uint8_t memberIndex; //0x0000
	uint8_t N00009344; //0x0001
	uint8_t N00009342; //0x0002
	uint8_t N00009340; //0x0003
	uint64_t steamId; //0x0004
	char pad_000C[36]; //0x000C
	uint32_t matchId; //0x0030
	uint8_t matchPlayerIndex; //0x0034
	uint8_t N00009349; //0x0035
	uint8_t N0000934C; //0x0036
	uint8_t N0000934A; //0x0037
	char pad_0038[64]; //0x0038
	uint64_t N0000914B; //0x0078
	char pad_0080[24]; //0x0080
}; //Size: 0x0098

#pragma pack(pop)
