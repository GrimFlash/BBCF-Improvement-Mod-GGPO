#pragma once
#include "RoomMemberEntry.h"

#include <cstdint>

enum RoomStatus
{
	RoomStatus_Unavailable,
	RoomStatus_Initializing,
	RoomStatus_Functional,
	RoomStatus_Terminating
};

// Seems like a bitfield
enum RoomType : uint8_t
{
	RoomType_Ranked = 0x10,
	RoomType_MatchSpectate = 0x11,
	RoomType_Lobby = 0x12,
	RoomType_FFA = 0x21,
	RoomType_Training = 0x31,
	RoomType_Replay = 0x41
};

class Room
{
public:
	RoomStatus roomStatus; //0x0000
	uint8_t capacity; //0x0004
	uint8_t invitation; //0x0005
	uint8_t N000090AF; //0x0006
	uint8_t N000090AD; //0x0007
	wchar_t roomName[16]; //0x0008
	char pad_0028[8]; //0x0028
	RoomType roomType; //0x0030
	uint8_t N0001167D; //0x0031
	uint8_t N00011681; //0x0032
	uint8_t N0001167E; //0x0033
	uint8_t roundsToWinPlusMatchLimitBitfield; //0x0034
	uint8_t roundTime; //0x0035
	uint8_t skipTimeLimit; //0x0036
	uint8_t N00009510; //0x0037
	uint8_t memberCount; //0x0038
	uint8_t N0000910B; //0x0039
	uint8_t N0000910E; //0x003A
	uint8_t N0000910C; //0x003B
	uint32_t secondsElapsed; //0x003C
	char pad_0040[8]; //0x0040
	RoomMemberEntry member1; //0x0048
	RoomMemberEntry member2; //0x00E0
	RoomMemberEntry member3; //0x0178
	RoomMemberEntry member4; //0x0210
	RoomMemberEntry member5; //0x02A8
	RoomMemberEntry member6; //0x0340
	RoomMemberEntry member7; //0x03D8
	RoomMemberEntry member8; //0x0470
}; //Size: 0x0508
