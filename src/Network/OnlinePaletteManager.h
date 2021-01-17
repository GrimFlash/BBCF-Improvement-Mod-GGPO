#pragma once

#include "RoomManager.h"

#include "Palette/PaletteManager.h"

#include <queue>

class OnlinePaletteManager
{
public:
	OnlinePaletteManager(PaletteManager* pPaletteManager, CharPaletteHandle* pP1CharPalHandle,
		CharPaletteHandle* pP2CharPalHandle, RoomManager* pRoomManager);
	void SendPalettePackets();
	void RecvPaletteDataPacket(Packet* packet);
	void RecvPaletteInfoPacket(Packet* packet);
	void ProcessSavedPalettePackets();
	void ClearSavedPalettePacketQueues();
	void OnMatchInit();

private:
	void SendPaletteInfoPacket(CharPaletteHandle& charPalHandle, uint16_t roomMemberIndex);
	void SendPaletteDataPackets(CharPaletteHandle& charPalHandle, uint16_t roomMemberIndex);
	void ProcessSavedPaletteInfoPackets();
	void ProcessSavedPaletteDataPackets();
	CharPaletteHandle& GetPlayerCharPaletteHandle(uint16_t matchPlayerIndex);

	struct UnprocessedPaletteInfo
	{
		uint16_t matchPlayerIndex;
		IMPL_info_t palInfo;

		UnprocessedPaletteInfo(uint16_t matchPlayerIndex_, IMPL_info_t* pPalInfo)
			: matchPlayerIndex(matchPlayerIndex_)
		{
			memcpy_s(&palInfo, sizeof(IMPL_info_t), pPalInfo, sizeof(IMPL_info_t));
		}
	};

	struct UnprocessedPaletteFile
	{
		uint16_t matchPlayerIndex;
		PaletteFile palFile;
		char palData[IMPL_PALETTE_DATALEN];

		UnprocessedPaletteFile(uint16_t matchPlayerIndex_, PaletteFile palFile_, char* pPalSrc)
			: matchPlayerIndex(matchPlayerIndex_), palFile(palFile_)
		{
			memcpy_s(palData, IMPL_PALETTE_DATALEN, pPalSrc, IMPL_PALETTE_DATALEN);
		}
	};

	std::queue<UnprocessedPaletteInfo> m_unprocessedPaletteInfos;
	std::queue<UnprocessedPaletteFile> m_unprocessedPaletteFiles;

	CharPaletteHandle* m_pP1CharPalHandle;
	CharPaletteHandle* m_pP2CharPalHandle;

	// Interfaces
	PaletteManager* m_pPaletteManager;
	RoomManager* m_pRoomManager;
};