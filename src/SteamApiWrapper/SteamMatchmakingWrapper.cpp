#include "../../include/SteamApiWrapper/SteamMatchmakingWrapper.h"
#include "../../include/utils.h"
#include "../../include/containers.h"
#include "../../include/gamestates_defines.h"

SteamMatchmakingWrapper::SteamMatchmakingWrapper(ISteamMatchmaking** pSteamMatchmaking)
{
	LOG(2, "SteamMatchmakingWrapper\n");
	LOG(2, "\t- before: *pSteamMatchmaking: 0x%p, thispointer: 0x%p\n", *pSteamMatchmaking, this);

	m_SteamMatchmaking = *pSteamMatchmaking;
	void* thisAddress = this;
	WriteToMemory((uintptr_t)pSteamMatchmaking, (char*)&thisAddress, 4); //basically *pSteamMatchmaking = this;

	LOG(2, "\t- after: *pSteamMatchmaking: 0x%p, m_SteamMatchmaking: 0x%p\n", *pSteamMatchmaking, m_SteamMatchmaking);
}

SteamMatchmakingWrapper::~SteamMatchmakingWrapper()
{
}

int SteamMatchmakingWrapper::GetFavoriteGameCount()
{
	LOG(7, "SteamMatchmakingWrapper GetFavoriteGameCount\n");
	return m_SteamMatchmaking->GetFavoriteGameCount();
}

bool SteamMatchmakingWrapper::GetFavoriteGame(int iGame, AppId_t *pnAppID, uint32 *pnIP, uint16 *pnConnPort, uint16 *pnQueryPort, uint32 *punFlags, uint32 *pRTime32LastPlayedOnServer)
{
	LOG(7, "SteamMatchmakingWrapper GetFavoriteGame\n");
	return m_SteamMatchmaking->GetFavoriteGame(iGame, pnAppID, pnIP, pnConnPort, pnQueryPort, punFlags, pRTime32LastPlayedOnServer);
}

int SteamMatchmakingWrapper::AddFavoriteGame(AppId_t nAppID, uint32 nIP, uint16 nConnPort, uint16 nQueryPort, uint32 unFlags, uint32 rTime32LastPlayedOnServer)
{
	LOG(7, "SteamMatchmakingWrapper AddFavoriteGame\n");
	return m_SteamMatchmaking->AddFavoriteGame(nAppID, nIP, nConnPort, nQueryPort, unFlags, rTime32LastPlayedOnServer);
}

bool SteamMatchmakingWrapper::RemoveFavoriteGame(AppId_t nAppID, uint32 nIP, uint16 nConnPort, uint16 nQueryPort, uint32 unFlags)
{
	LOG(7, "SteamMatchmakingWrapper RemoveFavoriteGame\n");
	return m_SteamMatchmaking->RemoveFavoriteGame(nAppID, nIP, nConnPort, nQueryPort, unFlags);
}

SteamAPICall_t SteamMatchmakingWrapper::RequestLobbyList()
{
	LOG(7, "SteamMatchmakingWrapper RequestLobbyList\n");
	return m_SteamMatchmaking->RequestLobbyList();
}

void SteamMatchmakingWrapper::AddRequestLobbyListStringFilter(const char *pchKeyToMatch, const char *pchValueToMatch, ELobbyComparison eComparisonType)
{
	LOG(7, "SteamMatchmakingWrapper AddRequestLobbyListStringFilter\n");
	return m_SteamMatchmaking->AddRequestLobbyListStringFilter(pchKeyToMatch, pchValueToMatch, eComparisonType);
}

void SteamMatchmakingWrapper::AddRequestLobbyListNumericalFilter(const char *pchKeyToMatch, int nValueToMatch, ELobbyComparison eComparisonType)
{
	LOG(7, "SteamMatchmakingWrapper AddRequestLobbyListNumericalFilter\n");
	return m_SteamMatchmaking->AddRequestLobbyListNumericalFilter(pchKeyToMatch, nValueToMatch, eComparisonType);
}
void SteamMatchmakingWrapper::AddRequestLobbyListNearValueFilter(const char *pchKeyToMatch, int nValueToBeCloseTo)
{
	LOG(7, "SteamMatchmakingWrapper AddRequestLobbyListNearValueFilter\n");
	return m_SteamMatchmaking->AddRequestLobbyListNearValueFilter(pchKeyToMatch, nValueToBeCloseTo);
}
void SteamMatchmakingWrapper::AddRequestLobbyListFilterSlotsAvailable(int nSlotsAvailable)
{
	LOG(7, "SteamMatchmakingWrapper AddRequestLobbyListFilterSlotsAvailable\n");
	return m_SteamMatchmaking->AddRequestLobbyListFilterSlotsAvailable(nSlotsAvailable);
}

void SteamMatchmakingWrapper::AddRequestLobbyListDistanceFilter(ELobbyDistanceFilter eLobbyDistanceFilter)
{
	LOG(7, "SteamMatchmakingWrapper AddRequestLobbyListDistanceFilter\n");

	ELobbyDistanceFilter filter;
	switch (Settings::settingsIni.regionlock)
	{
	case 2:
		filter = k_ELobbyDistanceFilterFar;
		break;
	case 3:
		filter = k_ELobbyDistanceFilterWorldwide;
		break;
	case 1:
	default:
		filter = k_ELobbyDistanceFilterDefault;
		break;
	}
	if (eLobbyDistanceFilter != filter)
	{

		LOG(7, "\teLobbyDistanceFilter modified from %d to %d\n", (int)eLobbyDistanceFilter, (int)filter);
		return m_SteamMatchmaking->AddRequestLobbyListDistanceFilter(filter);
	}
	return m_SteamMatchmaking->AddRequestLobbyListDistanceFilter(eLobbyDistanceFilter);
}

void SteamMatchmakingWrapper::AddRequestLobbyListResultCountFilter(int cMaxResults)
{
	LOG(7, "SteamMatchmakingWrapper AddRequestLobbyListResultCountFilter\n");
	return m_SteamMatchmaking->AddRequestLobbyListResultCountFilter(cMaxResults);
}

void SteamMatchmakingWrapper::AddRequestLobbyListCompatibleMembersFilter(CSteamID steamIDLobby)
{
	LOG(7, "SteamMatchmakingWrapper AddRequestLobbyListCompatibleMembersFilter\n");
	return m_SteamMatchmaking->AddRequestLobbyListCompatibleMembersFilter(steamIDLobby);
}

CSteamID SteamMatchmakingWrapper::GetLobbyByIndex(int iLobby)
{
	LOG(7, "SteamMatchmakingWrapper GetLobbyByIndex\n");
	return m_SteamMatchmaking->GetLobbyByIndex(iLobby);
}

SteamAPICall_t SteamMatchmakingWrapper::CreateLobby(ELobbyType eLobbyType, int cMaxMembers)
{
	LOG(7, "SteamMatchmakingWrapper CreateLobby\n");
	return m_SteamMatchmaking->CreateLobby(eLobbyType, cMaxMembers);
}

SteamAPICall_t SteamMatchmakingWrapper::JoinLobby(CSteamID steamIDLobby)
{
	LOG(7, "SteamMatchmakingWrapper JoinLobby\n");
	LOG(7, "\tsteamIDLobby: 0x%x\n", steamIDLobby);
	return m_SteamMatchmaking->JoinLobby(steamIDLobby);
}

void SteamMatchmakingWrapper::LeaveLobby(CSteamID steamIDLobby)
{
	LOG(7, "SteamMatchmakingWrapper LeaveLobby\n");
	return m_SteamMatchmaking->LeaveLobby(steamIDLobby);
}

bool SteamMatchmakingWrapper::InviteUserToLobby(CSteamID steamIDLobby, CSteamID steamIDInvitee)
{
	LOG(7, "SteamMatchmakingWrapper InviteUserToLobby\n");
	return m_SteamMatchmaking->InviteUserToLobby(steamIDLobby, steamIDInvitee);
}

int SteamMatchmakingWrapper::GetNumLobbyMembers(CSteamID steamIDLobby)
{
	LOG(7, "SteamMatchmakingWrapper GetNumLobbyMembers\n");
	return m_SteamMatchmaking->GetNumLobbyMembers(steamIDLobby);
}

CSteamID SteamMatchmakingWrapper::GetLobbyMemberByIndex(CSteamID steamIDLobby, int iMember)
{
	LOG(7, "SteamMatchmakingWrapper GetLobbyMemberByIndex\n");
	return m_SteamMatchmaking->GetLobbyMemberByIndex(steamIDLobby, iMember);
}

uint32 totalGetPackets = 0;
const char* SteamMatchmakingWrapper::GetLobbyData(CSteamID steamIDLobby, const char *pchKey)
{
	LOG(7, "SteamMatchmakingWrapper GetLobbyData\n");
	const char* ret = m_SteamMatchmaking->GetLobbyData(steamIDLobby, pchKey);
	//totalGetPackets++;
	//LOG(7, "\tcount: %d\n", totalGetPackets);
	//LOG(7, "\tpchKey: %s\n", pchKey);
	return ret;
}

uint32 totalSetPackets = 0;
bool SteamMatchmakingWrapper::SetLobbyData(CSteamID steamIDLobby, const char *pchKey, const char *pchValue)
{
	LOG(7, "SteamMatchmakingWrapper SetLobbyData\n");
	//totalSetPackets++;
	//LOG(7, "\tcount: %d\n", totalSetPackets);
	return m_SteamMatchmaking->SetLobbyData(steamIDLobby, pchKey, pchValue);
}

int SteamMatchmakingWrapper::GetLobbyDataCount(CSteamID steamIDLobby)
{
	LOG(7, "SteamMatchmakingWrapper GetLobbyDataCount\n");
	return m_SteamMatchmaking->GetLobbyDataCount(steamIDLobby);
}

bool SteamMatchmakingWrapper::GetLobbyDataByIndex(CSteamID steamIDLobby, int iLobbyData, char *pchKey, int cchKeyBufferSize, char *pchValue, int cchValueBufferSize)
{
	LOG(7, "SteamMatchmakingWrapper GetLobbyDataByIndex\n");
	return m_SteamMatchmaking->GetLobbyDataByIndex(steamIDLobby, iLobbyData, pchKey, cchKeyBufferSize, pchValue, cchValueBufferSize);
}

bool SteamMatchmakingWrapper::DeleteLobbyData(CSteamID steamIDLobby, const char *pchKey)
{
	LOG(7, "SteamMatchmakingWrapper DeleteLobbyData\n");
	return m_SteamMatchmaking->DeleteLobbyData(steamIDLobby, pchKey);
}

const char* SteamMatchmakingWrapper::GetLobbyMemberData(CSteamID steamIDLobby, CSteamID steamIDUser, const char *pchKey)
{
	LOG(7, "SteamMatchmakingWrapper GetLobbyMemberData\n");
	//DWORD returnAddress = 0;
	//__asm
	//{
	//	push eax
	//	mov eax, [ebp + 4]
	//	mov[returnAddress], eax
	//	pop eax
	//}
	//LOG(7, "\treturn address: 0x%x\n", returnAddress);
	//LOG(7, "\tsteamIDLobby: 0x%x, steamIDUser: 0x%x, pchKey: %s\n", steamIDLobby, steamIDUser, pchKey);

	//opponentPlayer = new CSteamID(steamIDRemote);

	return m_SteamMatchmaking->GetLobbyMemberData(steamIDLobby, steamIDUser, pchKey);
}

void SteamMatchmakingWrapper::SetLobbyMemberData(CSteamID steamIDLobby, const char *pchKey, const char *pchValue)
{
	LOG(7, "SteamMatchmakingWrapper SetLobbyMemberData\n");
	return m_SteamMatchmaking->SetLobbyMemberData(steamIDLobby, pchKey, pchValue);
}

bool SteamMatchmakingWrapper::SendLobbyChatMsg(CSteamID steamIDLobby, const void *pvMsgBody, int cubMsgBody)
{
	LOG(7, "SteamMatchmakingWrapper SendLobbyChatMsg\n");
	return m_SteamMatchmaking->SendLobbyChatMsg(steamIDLobby, pvMsgBody, cubMsgBody);
}

int SteamMatchmakingWrapper::GetLobbyChatEntry(CSteamID steamIDLobby, int iChatID, OUT_STRUCT() CSteamID *pSteamIDUser, void *pvData, int cubData, EChatEntryType *peChatEntryType)
{
	LOG(7, "SteamMatchmakingWrapper GetLobbyChatEntry\n");
	return m_SteamMatchmaking->GetLobbyChatEntry(steamIDLobby, iChatID, pSteamIDUser, pvData, cubData, peChatEntryType);
}

bool SteamMatchmakingWrapper::RequestLobbyData(CSteamID steamIDLobby)
{
	LOG(7, "SteamMatchmakingWrapper RequestLobbyData\n");
	return m_SteamMatchmaking->RequestLobbyData(steamIDLobby);
}

void SteamMatchmakingWrapper::SetLobbyGameServer(CSteamID steamIDLobby, uint32 unGameServerIP, uint16 unGameServerPort, CSteamID steamIDGameServer)
{
	LOG(7, "SteamMatchmakingWrapper SetLobbyGameServer\n");
	return m_SteamMatchmaking->SetLobbyGameServer(steamIDLobby, unGameServerIP, unGameServerPort, steamIDGameServer);
}

bool SteamMatchmakingWrapper::GetLobbyGameServer(CSteamID steamIDLobby, uint32 *punGameServerIP, uint16 *punGameServerPort, OUT_STRUCT() CSteamID *psteamIDGameServer)
{
	LOG(7, "SteamMatchmakingWrapper GetLobbyGameServer\n");
	return m_SteamMatchmaking->GetLobbyGameServer(steamIDLobby, punGameServerIP, punGameServerPort, psteamIDGameServer);
}

bool SteamMatchmakingWrapper::SetLobbyMemberLimit(CSteamID steamIDLobby, int cMaxMembers)
{
	LOG(7, "SteamMatchmakingWrapper SetLobbyMemberLimit\n");
	return m_SteamMatchmaking->SetLobbyMemberLimit(steamIDLobby, cMaxMembers);
}

int SteamMatchmakingWrapper::GetLobbyMemberLimit(CSteamID steamIDLobby)
{
	LOG(7, "SteamMatchmakingWrapper GetLobbyMemberLimit\n");
	return m_SteamMatchmaking->GetLobbyMemberLimit(steamIDLobby);
}

bool SteamMatchmakingWrapper::SetLobbyType(CSteamID steamIDLobby, ELobbyType eLobbyType)
{
	LOG(7, "SteamMatchmakingWrapper SetLobbyType\n");
	return m_SteamMatchmaking->SetLobbyType(steamIDLobby, eLobbyType);
}

bool SteamMatchmakingWrapper::SetLobbyJoinable(CSteamID steamIDLobby, bool bLobbyJoinable)
{
	LOG(7, "SteamMatchmakingWrapper SetLobbyJoinable\n");
	return m_SteamMatchmaking->SetLobbyJoinable(steamIDLobby, bLobbyJoinable);
}

CSteamID SteamMatchmakingWrapper::GetLobbyOwner(CSteamID steamIDLobby)
{
	LOG(7, "SteamMatchmakingWrapper GetLobbyOwner\n");

	//for (int i = 0; i < m_SteamMatchmaking->GetNumLobbyMembers(steamIDLobby); i++)
	//{
	//	CSteamID member = m_SteamMatchmaking->GetLobbyMemberByIndex(steamIDLobby, i);
	//	LOG(7, "SteamLobbyMemberByIndex %d: 0x%x\n", i ,member);
	//}

	return m_SteamMatchmaking->GetLobbyOwner(steamIDLobby);
}

bool SteamMatchmakingWrapper::SetLobbyOwner(CSteamID steamIDLobby, CSteamID steamIDNewOwner)
{
	LOG(7, "SteamMatchmakingWrapper SetLobbyOwner\n");
	return m_SteamMatchmaking->SetLobbyOwner(steamIDLobby, steamIDNewOwner);
}

bool SteamMatchmakingWrapper::SetLinkedLobby(CSteamID steamIDLobby, CSteamID steamIDLobbyDependent)
{
	LOG(7, "SteamMatchmakingWrapper SetLinkedLobby\n");
	return m_SteamMatchmaking->SetLinkedLobby(steamIDLobby, steamIDLobbyDependent);
}

#ifdef _PS3
void SteamMatchmakingWrapper::CheckForPSNGameBootInvite(unsigned int iGameBootAttributes)
{
	LOG(7, "SteamMatchmakingWrapper CheckForPSNGameBootInvite\n");
	return m_SteamMatchmaking->CheckForPSNGameBootInvite(iGameBootAttributes);
}
#endif