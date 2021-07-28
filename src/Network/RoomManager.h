#pragma once
#include "NetworkManager.h"

#include "Game/Room/Room.h"

#include <isteamfriends.h>
#include <string>
#include <vector>

#define MAX_PLAYERS_IN_ROOM 8

struct IMPlayer
{
	int roomMemberIndex;
	CSteamID steamID;
	std::string steamName;
	int matchPlayerIndex;

	IMPlayer() : roomMemberIndex(-1), matchPlayerIndex(-1)
	{
	}

	IMPlayer(int roomMemberIndex, uint64_t steamID, const char* steamName)
		: roomMemberIndex(roomMemberIndex), steamID(steamID),
		steamName(steamName), matchPlayerIndex(-1)
	{
	}

	IMPlayer(IMPlayer other, int matchPlayerIndex)
		: roomMemberIndex(other.roomMemberIndex), steamID(other.steamID),
		steamName(other.steamName), matchPlayerIndex(matchPlayerIndex)
	{
	}
};

class RoomManager
{
public:
	RoomManager(NetworkManager* pNetworkManager, ISteamFriends* pSteamFriends, CSteamID steamID);
	~RoomManager();

	void SendAcknowledge(Packet* packet);
	void AcceptAcknowledge(Packet* packet);
	void JoinRoom(Room* pRoom);
	bool IsRoomFunctional() const;
	void SendPacketToSameMatchIMPlayers(Packet* packet);
	bool IsPacketFromSameRoom(Packet* packet) const;
	bool IsPacketFromSameMatchNonSpectator(Packet* packet) const;
	bool IsThisPlayerSpectator() const;
	bool IsThisPlayerInMatch() const;
	void SetFFAThisPlayerIndex(int* pFFAThisPlayerIndex);
	uint16_t GetThisPlayerMatchPlayerIndex() const;
	uint16_t GetPlayerMatchPlayerIndexByRoomMemberIndex(uint16_t index) const;
	const std::string GetRoomTypeName() const;
	std::vector<IMPlayer> GetIMPlayersInCurrentMatch() const;
	std::vector<IMPlayer> GetIMPlayersInCurrentRoom() const;
	std::vector<const RoomMemberEntry*> GetOtherRoomMemberEntriesInCurrentMatch() const;

private:
	void SendAnnounce();
	void AddIMPlayerToRoom(const IMPlayer& player);
	void RemoveIMPlayerFromRoom(uint16_t index);
	bool IsPacketFromSameMatch(Packet* packet) const;
	bool IsPacketFromSpectator(Packet* packet) const;;
	uint16_t GetThisPlayerRoomMemberIndex() const;
	const RoomMemberEntry* GetThisPlayerRoomMemberEntry() const;
	const RoomMemberEntry* GetRoomMemberEntryByIndex(uint16_t index) const;
	bool IsPlayerInRoom(const IMPlayer& player) const;
	bool IsThisPlayer(const uint64_t otherSteamID) const;
	const char* GetPlayerSteamName(uint64_t steamID) const;

	std::vector<IMPlayer> m_imPlayers;
	CSteamID m_thisPlayerSteamID;

	// Free-for-All fix
	int* m_pFFAThisPlayerIndex;

	// Interfaces
	NetworkManager* m_pNetworkManager;
	ISteamFriends* m_pSteamFriends;
	Room* m_pRoom;

	// For debug purposes
	friend class DebugWindow;
};
