#include "OnlinePaletteManager.h"

#include "Palette/impl_format.h"

#include "Core/logger.h"

OnlinePaletteManager::OnlinePaletteManager(PaletteManager* pPaletteManager, CharPaletteHandle* pP1CharPalHandle,
	CharPaletteHandle* pP2CharPalHandle, RoomManager* pRoomManager)
	: m_pPaletteManager(pPaletteManager), m_pP1CharPalHandle(pP1CharPalHandle), 
	m_pP2CharPalHandle(pP2CharPalHandle), m_pRoomManager(pRoomManager)
{
}

void OnlinePaletteManager::SendPalettePackets()
{
	LOG(2, "OnlinePaletteManager::SendPalettePackets\n");

	if (m_pRoomManager->IsThisPlayerSpectator())
		return;

	uint16_t thisPlayerMatchPlayerIndex = m_pRoomManager->GetThisPlayerMatchPlayerIndex();
	CharPaletteHandle& charPalHandle = GetPlayerCharPaletteHandle(thisPlayerMatchPlayerIndex);

	SendPaletteInfoPacket(charPalHandle, thisPlayerMatchPlayerIndex);
	SendPaletteDataPackets(charPalHandle, thisPlayerMatchPlayerIndex);
}

void OnlinePaletteManager::RecvPaletteDataPacket(Packet* packet)
{
	LOG(2, "OnlinePaletteManager::RecvPaletteDataPacket\n");

	uint16_t matchPlayerIndex = m_pRoomManager->GetPlayerMatchPlayerIndexByRoomMemberIndex(packet->roomMemberIndex);
	CharPaletteHandle& charPalHandle = GetPlayerCharPaletteHandle(matchPlayerIndex);

	if (charPalHandle.IsNullPointerPalBasePtr())
	{
		m_unprocessedPaletteFiles.push(UnprocessedPaletteFile(matchPlayerIndex, (PaletteFile)packet->part, (char*)packet->data));
		return;
	}

	m_pPaletteManager->ReplacePaletteFile((const char*)packet->data, (PaletteFile)packet->part, charPalHandle);
}

void OnlinePaletteManager::RecvPaletteInfoPacket(Packet* packet)
{
	LOG(2, "OnlinePaletteManager::RecvPaletteInfoPacket\n");

	uint16_t matchPlayerIndex = m_pRoomManager->GetPlayerMatchPlayerIndexByRoomMemberIndex(packet->roomMemberIndex);
	CharPaletteHandle& charPalHandle = GetPlayerCharPaletteHandle(matchPlayerIndex);

	if (charPalHandle.IsNullPointerPalBasePtr())
	{
		m_unprocessedPaletteInfos.push(UnprocessedPaletteInfo(matchPlayerIndex, (IMPL_info_t*)packet->data));
		return;
	}

	m_pPaletteManager->SetCurrentPalInfo(charPalHandle, *(IMPL_info_t*)packet->data);
}

void OnlinePaletteManager::ProcessSavedPalettePackets()
{
	LOG(2, "OnlinePaletteManager::ProcessSavedPalettePackets\n");

	if (!m_pRoomManager->IsRoomFunctional())
		return;

	ProcessSavedPaletteInfoPackets();
	ProcessSavedPaletteDataPackets();
}

void OnlinePaletteManager::ClearSavedPalettePacketQueues()
{
	LOG(2, "OnlinePaletteManager::ClearSavedPalettePacketQueues\n");

	m_unprocessedPaletteInfos = {};
	m_unprocessedPaletteFiles = {};
}

void OnlinePaletteManager::OnMatchInit()
{
	LOG(2, "OnlinePaletteManager::OnMatchInit\n");

	SendPalettePackets();
	ProcessSavedPalettePackets();
}

void OnlinePaletteManager::SendPaletteInfoPacket(CharPaletteHandle& charPalHandle, uint16_t roomMemberIndex)
{
	LOG(2, "OnlinePaletteManager::SendPaletteInfoPacket\n");

	Packet packet = Packet(
		(char*)&m_pPaletteManager->GetCurrentPalInfo(charPalHandle),
		(uint16_t)sizeof(IMPL_info_t),
		PacketType_PaletteInfo,
		roomMemberIndex
	);

	m_pRoomManager->SendPacketToSameMatchIMPlayers(&packet);
}

void OnlinePaletteManager::SendPaletteDataPackets(CharPaletteHandle& charPalHandle, uint16_t roomMemberIndex)
{
	LOG(2, "OnlinePaletteManager::SendPaletteDataPackets\n");

	for (int palFileIndex = 0; palFileIndex < IMPL_PALETTE_FILES_COUNT; palFileIndex++)
	{
		const char* palAddr = m_pPaletteManager->GetCurPalFileAddr((PaletteFile)palFileIndex, charPalHandle);

		Packet packet = Packet(
			(char*)palAddr,
			(uint16_t)IMPL_PALETTE_DATALEN,
			PacketType_PaletteData,
			roomMemberIndex,
			palFileIndex
		);

		m_pRoomManager->SendPacketToSameMatchIMPlayers(&packet);
	}
}

void OnlinePaletteManager::ProcessSavedPaletteInfoPackets()
{
	LOG(2, "OnlinePaletteManager::ProcessSavedPaletteInfoPackets\n");

	for (int i = 0; i < m_unprocessedPaletteInfos.size(); i++)
	{
		UnprocessedPaletteInfo& palInfo = m_unprocessedPaletteInfos.front();

		CharPaletteHandle& charPalHandle = GetPlayerCharPaletteHandle(palInfo.matchPlayerIndex);

		m_pPaletteManager->SetCurrentPalInfo(charPalHandle, palInfo.palInfo);

		m_unprocessedPaletteInfos.pop();
	}
}

void OnlinePaletteManager::ProcessSavedPaletteDataPackets()
{
	LOG(2, "OnlinePaletteManager::ProcessSavedPaletteDataPackets\n");

	for (int i = 0; i < m_unprocessedPaletteFiles.size(); i++)
	{
		UnprocessedPaletteFile& palfile = m_unprocessedPaletteFiles.front();

		CharPaletteHandle& charPalHandle = GetPlayerCharPaletteHandle(palfile.matchPlayerIndex);

		m_pPaletteManager->ReplacePaletteFile(palfile.palData, palfile.palFile, charPalHandle);

		m_unprocessedPaletteFiles.pop();
	}
}

CharPaletteHandle& OnlinePaletteManager::GetPlayerCharPaletteHandle(uint16_t matchPlayerIndex)
{
	return matchPlayerIndex == 0 ? *m_pP1CharPalHandle : *m_pP2CharPalHandle;
}
