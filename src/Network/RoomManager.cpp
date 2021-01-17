#include "RoomManager.h"

#include "Core/logger.h"

RoomManager::RoomManager(NetworkManager* pNetworkManager, ISteamFriends* pSteamFriends, CSteamID steamID)
	: m_pNetworkManager(pNetworkManager), m_pSteamFriends(pSteamFriends),
	m_thisPlayerSteamID(steamID), m_pFFAThisPlayerIndex(nullptr), m_pRoom(nullptr)
{
	m_imPlayers.resize(8);
}

RoomManager::~RoomManager() {}

void RoomManager::SendAnnounce()
{
	LOG(2, "RoomManager::SendAnnounce\n");

	Packet packet = Packet(NULL, NULL, PacketType_IMID_Announce, GetThisPlayerRoomMemberIndex());

	// Send to every other player in the room
	for (int i = 0; i < MAX_PLAYERS_IN_ROOM; i++)
	{
		const RoomMemberEntry* pRoomMemberEntry = GetRoomMemberEntryByIndex(i);

		if (pRoomMemberEntry && !IsThisPlayer(pRoomMemberEntry->steamId))
		{
			m_pNetworkManager->SendPacket(&CSteamID(pRoomMemberEntry->steamId), &packet);
		}
	}
}

void RoomManager::SendAcknowledge(Packet* packet)
{
	LOG(2, "RoomManager::SendAcknowledge\n");

	IMPlayer otherPlayer = IMPlayer(packet->roomMemberIndex, packet->steamID, GetPlayerSteamName(packet->steamID));
	AddIMPlayerToRoom(otherPlayer);

	Packet ackPacket = Packet(NULL, NULL, PacketType_IMID_Acknowledge, GetThisPlayerRoomMemberIndex());
	m_pNetworkManager->SendPacket(&otherPlayer.steamID, &ackPacket);
}

void RoomManager::AcceptAcknowledge(Packet* packet)
{
	LOG(2, "RoomManager::AcceptAcknowledge\n");

	IMPlayer otherPlayer = IMPlayer(packet->roomMemberIndex, packet->steamID, GetPlayerSteamName(packet->steamID));
	AddIMPlayerToRoom(otherPlayer);
}

void RoomManager::JoinRoom(Room* pRoom)
{
	LOG(2, "RoomManager::JoinRoom\n");

	m_pRoom = pRoom;

	m_imPlayers.clear();
	m_imPlayers.resize(8);

	IMPlayer thisPlayer = IMPlayer(GetThisPlayerRoomMemberIndex(), m_thisPlayerSteamID.ConvertToUint64(), GetPlayerSteamName(m_thisPlayerSteamID.ConvertToUint64()));
	AddIMPlayerToRoom(thisPlayer);

	SendAnnounce();
}

bool RoomManager::IsRoomFunctional() const
{
	LOG(7, "RoomManager::IsRoomFunctional\n");

	return m_pRoom != nullptr && m_pRoom->roomStatus == RoomStatus_Functional;
}

void RoomManager::SendPacketToSameMatchIMPlayers(Packet* packet)
{
	LOG(2, "RoomManager::SendPacketToSameMatchIMPlayers\n");

	packet->roomMemberIndex = GetThisPlayerRoomMemberIndex();

	for (IMPlayer& imPlayer : GetIMPlayersInCurrentMatch())
	{
		// Remove from IM users list if player has left the room
		if (!IsPlayerInRoom(imPlayer))
		{
			RemoveIMPlayerFromRoom(imPlayer.roomMemberIndex);
			continue;
		}

		// Send to all other IM players
		if (!IsThisPlayer(imPlayer.steamID.ConvertToUint64()))
		{
			m_pNetworkManager->SendPacket(&imPlayer.steamID, packet);
		}
	}
}

bool RoomManager::IsPacketFromSameRoom(Packet* packet) const
{
	LOG(7, "RoomManager::IsPacketFromSameRoom\n");

	if (!IsRoomFunctional())
		return false;

	return IsPlayerInRoom(IMPlayer(packet->roomMemberIndex, packet->steamID, GetPlayerSteamName(packet->steamID)));
}

bool RoomManager::IsPacketFromSameMatchNonSpectator(Packet* packet) const
{
	LOG(7, "RoomManager::IsPacketFromSameMatchNonSpectator\n");

	return IsPacketFromSameMatch(packet) && !IsPacketFromSpectator(packet);
}

bool RoomManager::IsThisPlayerSpectator() const
{
	LOG(7, "RoomManager::IsThisPlayerSpectator\n");

	return GetThisPlayerRoomMemberEntry()->matchPlayerIndex > 1;
}

bool RoomManager::IsThisPlayerInMatch() const
{
	LOG(7, "RoomManager::IsThisPlayerInMatch\n");

	const RoomMemberEntry* thisPlayerMemberEntry = GetThisPlayerRoomMemberEntry();
	int matchId = thisPlayerMemberEntry ? thisPlayerMemberEntry->matchId : 0;

	return matchId != 0;
}

void RoomManager::SetFFAThisPlayerIndex(int* pFFAThisPlayerIndex)
{
	LOG(7, "RoomManager::SetFFAThisPlayerIndex\n");

	m_pFFAThisPlayerIndex = pFFAThisPlayerIndex;
}

uint16_t RoomManager::GetThisPlayerMatchPlayerIndex() const
{
	LOG(7, "RoomManager::GetThisPlayerMatchPlayerIndex\n");

	if (m_pRoom->roomType == RoomType_FFA)
		return *m_pFFAThisPlayerIndex;

	return GetThisPlayerRoomMemberEntry()->matchPlayerIndex;
}

uint16_t RoomManager::GetPlayerMatchPlayerIndexByRoomMemberIndex(uint16_t index) const
{
	LOG(7, "RoomManager::GetPlayerMatchPlayerIndexByRoomMemberIndex\n");

	// Resolve playerindex via m_pFFAThisPlayerIndex if room type is FFA
	if (m_pRoom->roomType == RoomType_FFA)
	{
		if (index == GetThisPlayerRoomMemberIndex())
			return *m_pFFAThisPlayerIndex;

		return !*m_pFFAThisPlayerIndex;;
	}

	return GetRoomMemberEntryByIndex(index)->matchPlayerIndex;
}

const char* RoomManager::GetPlayerSteamName(uint64_t steamID) const
{
	LOG(7, "RoomManager::GetPlayerSteamName\n");

	return m_pSteamFriends->GetFriendPersonaName(steamID);
}

const std::string RoomManager::GetRoomTypeName() const
{
	switch (m_pRoom->roomType)
	{
	case RoomType_Ranked:
		return "Ranked";
	case RoomType_MatchSpectate:
		return "Match / Spectate Room";
	case RoomType_Lobby:
		return "Lobby";
	case RoomType_FFA:
		return "Free-for-All Room";
	case RoomType_Training:
		return "Training Room";
	case RoomType_Replay: 
		return "Replay Theater";
	default:
		return "Unknown type";
	}
}

std::vector<IMPlayer> RoomManager::GetIMPlayersInCurrentMatch() const
{
	LOG(7, "RoomManager::GetIMPlayersInCurrentMatch\n");

	std::vector<IMPlayer> currentMatchIMPlayers;
	uint32_t matchId = GetThisPlayerRoomMemberEntry()->matchId;

	for (const IMPlayer& imPlayer : m_imPlayers)
	{
		if (imPlayer.roomMemberIndex == -1)
			continue;

		const RoomMemberEntry* pRoomMemberEntry = GetRoomMemberEntryByIndex(imPlayer.roomMemberIndex);

		if (!pRoomMemberEntry)
			continue;

		bool isInSameMatch = matchId == pRoomMemberEntry->matchId;

		if (isInSameMatch)
		{
			currentMatchIMPlayers.push_back(
				IMPlayer(imPlayer, GetPlayerMatchPlayerIndexByRoomMemberIndex(pRoomMemberEntry->memberIndex))
			);
		}
	}

	return currentMatchIMPlayers;
}

std::vector<IMPlayer> RoomManager::GetIMPlayersInCurrentRoom() const
{
	LOG(7, "RoomManager::GetIMPlayersInCurrentRoom\n");

	std::vector<IMPlayer> currentRoomIMPlayers;

	for (const IMPlayer& imPlayer : m_imPlayers)
	{
		if (imPlayer.roomMemberIndex == -1)
			continue;
		
		const RoomMemberEntry* pRoomMemberEntry = GetRoomMemberEntryByIndex(imPlayer.roomMemberIndex);

		if (!pRoomMemberEntry)
			continue;

		currentRoomIMPlayers.push_back(imPlayer);
	}

	return currentRoomIMPlayers;
}

std::vector<const RoomMemberEntry*> RoomManager::GetOtherRoomMemberEntriesInCurrentMatch() const
{
	std::vector<const RoomMemberEntry*> currentMatchRoomMembers;

	for (int i = 0; i < MAX_PLAYERS_IN_ROOM; i++)
	{
		const RoomMemberEntry* pRoomMemberEntry = GetRoomMemberEntryByIndex(i);

		if (pRoomMemberEntry && !IsThisPlayer(pRoomMemberEntry->steamId))
		{
			currentMatchRoomMembers.push_back(pRoomMemberEntry);
		}
	}

	return currentMatchRoomMembers;
}

void RoomManager::AddIMPlayerToRoom(const IMPlayer& imPlayer)
{
	m_imPlayers[imPlayer.roomMemberIndex] = imPlayer;
}

void RoomManager::RemoveIMPlayerFromRoom(uint16_t index)
{
	m_imPlayers[index] = IMPlayer();
}

bool RoomManager::IsPacketFromSameMatch(Packet* packet) const
{
	const RoomMemberEntry* pRoomMemberEntry = GetRoomMemberEntryByIndex(packet->roomMemberIndex);

	if (!pRoomMemberEntry)
		return false;

	return pRoomMemberEntry->matchId == GetThisPlayerRoomMemberEntry()->matchId;
}

// IsPacketFromSameMatch should be called beforehand
bool RoomManager::IsPacketFromSpectator(Packet* packet) const
{
	if (m_pRoom->roomType == RoomType_FFA)
		return false;

	return GetRoomMemberEntryByIndex(packet->roomMemberIndex)->matchPlayerIndex > 1;
}

uint16_t RoomManager::GetThisPlayerRoomMemberIndex() const
{
	for (int i = 0; i < MAX_PLAYERS_IN_ROOM; i++)
	{
		const RoomMemberEntry* pRoomMemberEntry = GetRoomMemberEntryByIndex(i);

		if (pRoomMemberEntry && IsThisPlayer(pRoomMemberEntry->steamId))
		{
			return i;
		}
	}

	return -1;
}

const RoomMemberEntry* RoomManager::GetThisPlayerRoomMemberEntry() const
{
	for (int i = 0; i < MAX_PLAYERS_IN_ROOM; i++)
	{
		const RoomMemberEntry* pRoomMemberEntry = GetRoomMemberEntryByIndex(i);

		if (pRoomMemberEntry && IsThisPlayer(pRoomMemberEntry->steamId))
		{
			return pRoomMemberEntry;
		}
	}

	return nullptr;
}

const RoomMemberEntry* RoomManager::GetRoomMemberEntryByIndex(uint16_t index) const
{
	char* deref = (char*)(&m_pRoom->member1) + (index * sizeof(RoomMemberEntry));
	RoomMemberEntry* roomMemberEntry = (RoomMemberEntry*)deref;

	if (roomMemberEntry->steamId == 0)
		return nullptr;

	return roomMemberEntry;
}

bool RoomManager::IsPlayerInRoom(const IMPlayer& player) const
{
	for (int i = 0; i < MAX_PLAYERS_IN_ROOM; i++)
	{
		const RoomMemberEntry* pRoomMemberEntry = GetRoomMemberEntryByIndex(i);

		if (!pRoomMemberEntry)
			continue;

		if (pRoomMemberEntry->memberIndex == player.roomMemberIndex &&
			pRoomMemberEntry->steamId == player.steamID.ConvertToUint64())
		{
			return true;
		}
	}

	return false;
}

bool RoomManager::IsThisPlayer(const uint64_t otherSteamID) const
{
	return otherSteamID == m_thisPlayerSteamID.ConvertToUint64();
}
