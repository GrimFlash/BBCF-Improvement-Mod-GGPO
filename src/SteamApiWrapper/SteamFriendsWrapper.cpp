#include "SteamFriendsWrapper.h"

#include "Core/logger.h"
#include "Core/utils.h"

SteamFriendsWrapper::SteamFriendsWrapper(ISteamFriends** pSteamFriends)
{
	LOG(7, "SteamFriendsWrapper\n");
	LOG(7, "\t- before: *pSteamFriends: 0x%p, thispointer: 0x%p\n", *pSteamFriends, this);

	m_SteamFriends = *pSteamFriends;
	void* thisAddress = this;
	WriteToProtectedMemory((uintptr_t)pSteamFriends, (char*)&thisAddress, 4); //basically *pSteamFriends = this;

	LOG(7, "\t- after: *pSteamFriends: 0x%p, m_SteamFriends: 0x%p\n", *pSteamFriends, m_SteamFriends);
}

SteamFriendsWrapper::~SteamFriendsWrapper()
{
}

const char *SteamFriendsWrapper::GetPersonaName()
{
	LOG(7, "GetPersonaName\n");
	return m_SteamFriends->GetPersonaName();
}

CALL_RESULT(SetPersonaNameResponse_t)
SteamAPICall_t SteamFriendsWrapper::SetPersonaName(const char *pchPersonaName)
{
	LOG(7, "SetPersonaName\n");
	return m_SteamFriends->SetPersonaName(pchPersonaName);
}

EPersonaState SteamFriendsWrapper::GetPersonaState()
{
	LOG(7, "GetPersonaState\n");
	return m_SteamFriends->GetPersonaState();
}

int SteamFriendsWrapper::GetFriendCount(int iFriendFlags)
{
	LOG(7, "GetFriendCount\n");
	return m_SteamFriends->GetFriendCount(iFriendFlags);
}

CSteamID SteamFriendsWrapper::GetFriendByIndex(int iFriend, int iFriendFlags)
{
	LOG(7, "GetFriendByIndex\n");
	return m_SteamFriends->GetFriendByIndex(iFriend, iFriendFlags);
}

EFriendRelationship SteamFriendsWrapper::GetFriendRelationship(CSteamID steamIDFriend)
{
	LOG(7, "GetFriendRelationship\n");
	return m_SteamFriends->GetFriendRelationship(steamIDFriend);
}

EPersonaState SteamFriendsWrapper::GetFriendPersonaState(CSteamID steamIDFriend)
{
	LOG(7, "GetFriendPersonaState\n");
	return m_SteamFriends->GetFriendPersonaState(steamIDFriend);
}

const char *SteamFriendsWrapper::GetFriendPersonaName(CSteamID steamIDFriend)
{
	LOG(7, "GetFriendPersonaName\n");
	return m_SteamFriends->GetFriendPersonaName(steamIDFriend);
}

bool SteamFriendsWrapper::GetFriendGamePlayed(CSteamID steamIDFriend, OUT_STRUCT() FriendGameInfo_t *pFriendGameInfo)
{
	LOG(7, "GetFriendGamePlayed\n");
	return m_SteamFriends->GetFriendGamePlayed(steamIDFriend, OUT_STRUCT() pFriendGameInfo);
}

const char *SteamFriendsWrapper::GetFriendPersonaNameHistory(CSteamID steamIDFriend, int iPersonaName)
{
	LOG(7, "GetFriendPersonaNameHistory\n");
	return m_SteamFriends->GetFriendPersonaNameHistory(steamIDFriend, iPersonaName);
}

int SteamFriendsWrapper::GetFriendSteamLevel(CSteamID steamIDFriend)
{
	LOG(7, "GetFriendSteamLevel\n");
	return m_SteamFriends->GetFriendSteamLevel(steamIDFriend);
}

const char *SteamFriendsWrapper::GetPlayerNickname(CSteamID steamIDPlayer)
{
	LOG(7, "GetPlayerNickname\n");
	return m_SteamFriends->GetPlayerNickname(steamIDPlayer);
}

int SteamFriendsWrapper::GetFriendsGroupCount()
{
	LOG(7, "GetFriendsGroupCount\n");
	return m_SteamFriends->GetFriendsGroupCount();
}

FriendsGroupID_t SteamFriendsWrapper::GetFriendsGroupIDByIndex(int iFG)
{
	LOG(7, "GetFriendsGroupIDByIndex\n");
	return m_SteamFriends->GetFriendsGroupIDByIndex(iFG);
}

const char *SteamFriendsWrapper::GetFriendsGroupName(FriendsGroupID_t friendsGroupID)
{
	LOG(7, "GetFriendsGroupName\n");
	return m_SteamFriends->GetFriendsGroupName(friendsGroupID);
}

int SteamFriendsWrapper::GetFriendsGroupMembersCount(FriendsGroupID_t friendsGroupID)
{
	LOG(7, "GetFriendsGroupMembersCount\n");
	return m_SteamFriends->GetFriendsGroupMembersCount(friendsGroupID);
}

void SteamFriendsWrapper::GetFriendsGroupMembersList(FriendsGroupID_t friendsGroupID, 
	OUT_ARRAY_CALL(nMembersCount, GetFriendsGroupMembersCount, friendsGroupID) CSteamID *pOutSteamIDMembers, int nMembersCount)
{
	LOG(7, "GetFriendsGroupMembersList\n");
	return m_SteamFriends->GetFriendsGroupMembersList(friendsGroupID, OUT_ARRAY_CALL(nMembersCount, GetFriendsGroupMembersCount, friendsGroupID) pOutSteamIDMembers, nMembersCount);
}

bool SteamFriendsWrapper::HasFriend(CSteamID steamIDFriend, int iFriendFlags)
{
	LOG(7, "HasFriend\n");
	return m_SteamFriends->HasFriend(steamIDFriend, iFriendFlags);
}

int SteamFriendsWrapper::GetClanCount()
{
	LOG(7, "GetClanCount\n");
	return m_SteamFriends->GetClanCount();
}

CSteamID SteamFriendsWrapper::GetClanByIndex(int iClan)
{
	LOG(7, "GetClanByIndex\n");
	return m_SteamFriends->GetClanByIndex(iClan);
}

const char *SteamFriendsWrapper::GetClanName(CSteamID steamIDClan)
{
	LOG(7, "GetClanName\n");
	return m_SteamFriends->GetClanName(steamIDClan);
}

const char *SteamFriendsWrapper::GetClanTag(CSteamID steamIDClan)
{
	LOG(7, "GetClanTag\n");
	return m_SteamFriends->GetClanTag(steamIDClan);
}

bool SteamFriendsWrapper::GetClanActivityCounts(CSteamID steamIDClan, int *pnOnline, int *pnInGame, int *pnChatting)
{
	LOG(7, "GetClanActivityCounts\n");
	return m_SteamFriends->GetClanActivityCounts(steamIDClan, pnOnline, pnInGame, pnChatting);
}

SteamAPICall_t SteamFriendsWrapper::DownloadClanActivityCounts(ARRAY_COUNT(cClansToRequest) CSteamID *psteamIDClans, int cClansToRequest)
{
	LOG(7, "DownloadClanActivityCounts\n");
	return m_SteamFriends->DownloadClanActivityCounts(ARRAY_COUNT(cClansToRequest) psteamIDClans, cClansToRequest);
}

int SteamFriendsWrapper::GetFriendCountFromSource(CSteamID steamIDSource)
{
	LOG(7, "GetFriendCountFromSource\n");
	return m_SteamFriends->GetFriendCountFromSource(steamIDSource);
}

CSteamID SteamFriendsWrapper::GetFriendFromSourceByIndex(CSteamID steamIDSource, int iFriend)
{
	LOG(7, "GetFriendFromSourceByIndex\n");
	return m_SteamFriends->GetFriendFromSourceByIndex(steamIDSource, iFriend);
}

bool SteamFriendsWrapper::IsUserInSource(CSteamID steamIDUser, CSteamID steamIDSource)
{
	LOG(7, "IsUserInSource\n");
	return m_SteamFriends->IsUserInSource(steamIDUser, steamIDSource);
}

void SteamFriendsWrapper::SetInGameVoiceSpeaking(CSteamID steamIDUser, bool bSpeaking)
{
	LOG(7, "SetInGameVoiceSpeaking\n");
	return m_SteamFriends->SetInGameVoiceSpeaking(steamIDUser, bSpeaking);
}

void SteamFriendsWrapper::ActivateGameOverlay(const char *pchDialog)
{
	LOG(7, "ActivateGameOverlay\n");
	return m_SteamFriends->ActivateGameOverlay(pchDialog);
}

void SteamFriendsWrapper::ActivateGameOverlayToUser(const char *pchDialog, CSteamID steamID)
{
	LOG(7, "ActivateGameOverlayToUser\n");
	return m_SteamFriends->ActivateGameOverlayToUser(pchDialog, steamID);
}

void SteamFriendsWrapper::ActivateGameOverlayToWebPage(const char *pchURL)
{
	LOG(7, "ActivateGameOverlayToWebPage\n");
	return m_SteamFriends->ActivateGameOverlayToWebPage(pchURL);
}

void SteamFriendsWrapper::ActivateGameOverlayToStore(AppId_t nAppID, EOverlayToStoreFlag eFlag)
{
	LOG(7, "ActivateGameOverlayToStore\n");
	return m_SteamFriends->ActivateGameOverlayToStore(nAppID, eFlag);
}

void SteamFriendsWrapper::SetPlayedWith(CSteamID steamIDUserPlayedWith)
{
	LOG(2, "SetPlayedWith\n");
	return m_SteamFriends->SetPlayedWith(steamIDUserPlayedWith);
}

void SteamFriendsWrapper::ActivateGameOverlayInviteDialog(CSteamID steamIDLobby)
{
	LOG(7, "ActivateGameOverlayInviteDialog\n");
	return m_SteamFriends->ActivateGameOverlayInviteDialog(steamIDLobby);
}

int SteamFriendsWrapper::GetSmallFriendAvatar(CSteamID steamIDFriend)
{
	LOG(7, "GetSmallFriendAvatar\n");
	return m_SteamFriends->GetSmallFriendAvatar(steamIDFriend);
}

int SteamFriendsWrapper::GetMediumFriendAvatar(CSteamID steamIDFriend)
{
	LOG(7, "GetMediumFriendAvatar\n");
	return m_SteamFriends->GetMediumFriendAvatar(steamIDFriend);
}

int SteamFriendsWrapper::GetLargeFriendAvatar(CSteamID steamIDFriend)
{
	LOG(7, "GetLargeFriendAvatar\n");
	return m_SteamFriends->GetLargeFriendAvatar(steamIDFriend);
}

bool SteamFriendsWrapper::RequestUserInformation(CSteamID steamIDUser, bool bRequireNameOnly)
{
	LOG(7, "RequestUserInformation\n");
	return m_SteamFriends->RequestUserInformation(steamIDUser, bRequireNameOnly);
}

CALL_RESULT(ClanOfficerListResponse_t)
SteamAPICall_t SteamFriendsWrapper::RequestClanOfficerList(CSteamID steamIDClan)
{
	LOG(7, "RequestClanOfficerList\n");
	return m_SteamFriends->RequestClanOfficerList(steamIDClan);
}

CSteamID SteamFriendsWrapper::GetClanOwner(CSteamID steamIDClan)
{
	LOG(7, "GetClanOwner\n");
	return m_SteamFriends->GetClanOwner(steamIDClan);
}

int SteamFriendsWrapper::GetClanOfficerCount(CSteamID steamIDClan)
{
	LOG(7, "GetClanOfficerCount\n");
	return m_SteamFriends->GetClanOfficerCount(steamIDClan);
}

CSteamID SteamFriendsWrapper::GetClanOfficerByIndex(CSteamID steamIDClan, int iOfficer)
{
	LOG(7, "GetClanOfficerByIndex\n");
	return m_SteamFriends->GetClanOfficerByIndex(steamIDClan, iOfficer);
}

uint32 SteamFriendsWrapper::GetUserRestrictions()
{
	LOG(7, "GetUserRestrictions\n");
	return m_SteamFriends->GetUserRestrictions();
}

bool SteamFriendsWrapper::SetRichPresence(const char *pchKey, const char *pchValue)
{
	LOG(7, "SetRichPresence\n");
	return m_SteamFriends->SetRichPresence(pchKey, pchValue);
}

void SteamFriendsWrapper::ClearRichPresence()
{
	LOG(7, "ClearRichPresence\n");
	return m_SteamFriends->ClearRichPresence();
}

const char *SteamFriendsWrapper::GetFriendRichPresence(CSteamID steamIDFriend, const char *pchKey)
{
	LOG(7, "GetFriendRichPresence\n");
	return m_SteamFriends->GetFriendRichPresence(steamIDFriend, pchKey);
}

int SteamFriendsWrapper::GetFriendRichPresenceKeyCount(CSteamID steamIDFriend)
{
	LOG(7, "GetFriendRichPresenceKeyCount\n");
	return m_SteamFriends->GetFriendRichPresenceKeyCount(steamIDFriend);
}

const char *SteamFriendsWrapper::GetFriendRichPresenceKeyByIndex(CSteamID steamIDFriend, int iKey)
{
	LOG(7, "GetFriendRichPresenceKeyByIndex\n");
	return m_SteamFriends->GetFriendRichPresenceKeyByIndex(steamIDFriend, iKey);
}

void SteamFriendsWrapper::RequestFriendRichPresence(CSteamID steamIDFriend)
{
	LOG(7, "RequestFriendRichPresence\n");
	return m_SteamFriends->RequestFriendRichPresence(steamIDFriend);
}

bool SteamFriendsWrapper::InviteUserToGame(CSteamID steamIDFriend, const char *pchConnectString)
{
	LOG(7, "InviteUserToGame\n");
	return m_SteamFriends->InviteUserToGame(steamIDFriend, pchConnectString);
}

int SteamFriendsWrapper::GetCoplayFriendCount()
{
	LOG(7, "GetCoplayFriendCount\n");
	return m_SteamFriends->GetCoplayFriendCount();
}

CSteamID SteamFriendsWrapper::GetCoplayFriend(int iCoplayFriend)
{
	LOG(7, "GetCoplayFriend\n");
	return m_SteamFriends->GetCoplayFriend(iCoplayFriend);
}

int SteamFriendsWrapper::GetFriendCoplayTime(CSteamID steamIDFriend)
{
	LOG(7, "GetFriendCoplayTime\n");
	return m_SteamFriends->GetFriendCoplayTime(steamIDFriend);
}

AppId_t SteamFriendsWrapper::GetFriendCoplayGame(CSteamID steamIDFriend)
{
	LOG(7, "GetFriendCoplayGame\n");
	return m_SteamFriends->GetFriendCoplayGame(steamIDFriend);
}

// chat interface for games
// this allows in-game access to group (clan) chats from in the game
// the behavior is somewhat sophisticated, because the user may or may not be already in the group chat from outside the game or in the overlay
// use ActivateGameOverlayToUser( "chat", steamIDClan ) to open the in-game overlay version of the chat
CALL_RESULT(JoinClanChatRoomCompletionResult_t)
SteamAPICall_t SteamFriendsWrapper::JoinClanChatRoom(CSteamID steamIDClan)
{
	LOG(7, "JoinClanChatRoom\n");
	return m_SteamFriends->JoinClanChatRoom(steamIDClan);
}

bool SteamFriendsWrapper::LeaveClanChatRoom(CSteamID steamIDClan)
{
	LOG(7, "LeaveClanChatRoom\n");
	return m_SteamFriends->LeaveClanChatRoom(steamIDClan);
}

int SteamFriendsWrapper::GetClanChatMemberCount(CSteamID steamIDClan)
{
	LOG(7, "GetClanChatMemberCount\n");
	return m_SteamFriends->GetClanChatMemberCount(steamIDClan);
}

CSteamID SteamFriendsWrapper::GetChatMemberByIndex(CSteamID steamIDClan, int iUser)
{
	LOG(7, "GetChatMemberByIndex\n");
	return m_SteamFriends->GetChatMemberByIndex(steamIDClan, iUser);
}

bool SteamFriendsWrapper::SendClanChatMessage(CSteamID steamIDClanChat, const char *pchText)
{
	LOG(7, "SendClanChatMessage\n");
	return m_SteamFriends->SendClanChatMessage(steamIDClanChat, pchText);
}

int SteamFriendsWrapper::GetClanChatMessage(CSteamID steamIDClanChat, int iMessage, void *prgchText, int cchTextMax, EChatEntryType *peChatEntryType, OUT_STRUCT() CSteamID *psteamidChatter)
{
	LOG(7, "GetClanChatMessage\n");
	return m_SteamFriends->GetClanChatMessage(steamIDClanChat, iMessage, prgchText, cchTextMax, peChatEntryType, OUT_STRUCT() psteamidChatter);
}

bool SteamFriendsWrapper::IsClanChatAdmin(CSteamID steamIDClanChat, CSteamID steamIDUser)
{
	LOG(7, "IsClanChatAdmin\n");
	return m_SteamFriends->IsClanChatAdmin(steamIDClanChat, steamIDUser);
}

bool SteamFriendsWrapper::IsClanChatWindowOpenInSteam(CSteamID steamIDClanChat)
{
	LOG(7, "IsClanChatWindowOpenInSteam\n");
	return m_SteamFriends->IsClanChatWindowOpenInSteam(steamIDClanChat);
}

bool SteamFriendsWrapper::OpenClanChatWindowInSteam(CSteamID steamIDClanChat)
{
	LOG(7, "OpenClanChatWindowInSteam\n");
	return m_SteamFriends->OpenClanChatWindowInSteam(steamIDClanChat);
}

bool SteamFriendsWrapper::CloseClanChatWindowInSteam(CSteamID steamIDClanChat)
{
	LOG(7, "CloseClanChatWindowInSteam\n");
	return m_SteamFriends->CloseClanChatWindowInSteam(steamIDClanChat);
}

// peer-to-peer chat interception
// this is so you can show P2P chats inline in the game
bool SteamFriendsWrapper::SetListenForFriendsMessages(bool bInterceptEnabled)
{
	LOG(7, "SetListenForFriendsMessages\n");
	return m_SteamFriends->SetListenForFriendsMessages(bInterceptEnabled);
}

bool SteamFriendsWrapper::ReplyToFriendMessage(CSteamID steamIDFriend, const char *pchMsgToSend)
{
	LOG(7, "ReplyToFriendMessage\n");
	return m_SteamFriends->ReplyToFriendMessage(steamIDFriend, pchMsgToSend);
}

int SteamFriendsWrapper::GetFriendMessage(CSteamID steamIDFriend, int iMessageID, void *pvData, int cubData, EChatEntryType *peChatEntryType)
{
	LOG(7, "GetFriendMessage\n");
	return m_SteamFriends->GetFriendMessage(steamIDFriend, iMessageID, pvData, cubData, peChatEntryType);
}

CALL_RESULT(FriendsGetFollowerCount_t)
SteamAPICall_t SteamFriendsWrapper::GetFollowerCount(CSteamID steamID)
{
	LOG(7, "GetFollowerCount\n");
	return m_SteamFriends->GetFollowerCount(steamID);
}

CALL_RESULT(FriendsIsFollowing_t)
SteamAPICall_t SteamFriendsWrapper::IsFollowing(CSteamID steamID)
{
	LOG(7, "IsFollowing\n");
	return m_SteamFriends->IsFollowing(steamID);
}

CALL_RESULT(FriendsEnumerateFollowingList_t)
SteamAPICall_t SteamFriendsWrapper::EnumerateFollowingList(uint32 unStartIndex)
{
	LOG(7, "EnumerateFollowingList\n");
	return m_SteamFriends->EnumerateFollowingList(unStartIndex);
}
