#pragma once
#include <isteamfriends.h>

#include <Windows.h>

interface SteamFriendsWrapper : public ISteamFriends
{
public:
	SteamFriendsWrapper(ISteamFriends**);
	~SteamFriendsWrapper();

	ISteamFriends* m_SteamFriends;

	const char *GetPersonaName();
	CALL_RESULT(SetPersonaNameResponse_t)
		SteamAPICall_t SetPersonaName(const char *pchPersonaName);
	EPersonaState GetPersonaState();
	int GetFriendCount(int iFriendFlags);
	CSteamID GetFriendByIndex(int iFriend, int iFriendFlags);
	EFriendRelationship GetFriendRelationship(CSteamID steamIDFriend);
	EPersonaState GetFriendPersonaState(CSteamID steamIDFriend);
	const char *GetFriendPersonaName(CSteamID steamIDFriend);
	bool GetFriendGamePlayed(CSteamID steamIDFriend, OUT_STRUCT() FriendGameInfo_t *pFriendGameInfo);
	const char *GetFriendPersonaNameHistory(CSteamID steamIDFriend, int iPersonaName);
	int GetFriendSteamLevel(CSteamID steamIDFriend);
	const char *GetPlayerNickname(CSteamID steamIDPlayer);
	int GetFriendsGroupCount();
	FriendsGroupID_t GetFriendsGroupIDByIndex(int iFG);
	const char *GetFriendsGroupName(FriendsGroupID_t friendsGroupID);
	int GetFriendsGroupMembersCount(FriendsGroupID_t friendsGroupID);
	void GetFriendsGroupMembersList(FriendsGroupID_t friendsGroupID, OUT_ARRAY_CALL(nMembersCount, GetFriendsGroupMembersCount, friendsGroupID) CSteamID *pOutSteamIDMembers, int nMembersCount);
	bool HasFriend(CSteamID steamIDFriend, int iFriendFlags);
	int GetClanCount();
	CSteamID GetClanByIndex(int iClan);
	const char *GetClanName(CSteamID steamIDClan);
	const char *GetClanTag(CSteamID steamIDClan);
	bool GetClanActivityCounts(CSteamID steamIDClan, int *pnOnline, int *pnInGame, int *pnChatting);
	SteamAPICall_t DownloadClanActivityCounts(ARRAY_COUNT(cClansToRequest) CSteamID *psteamIDClans, int cClansToRequest);
	int GetFriendCountFromSource(CSteamID steamIDSource);
	CSteamID GetFriendFromSourceByIndex(CSteamID steamIDSource, int iFriend);
	bool IsUserInSource(CSteamID steamIDUser, CSteamID steamIDSource);
	void SetInGameVoiceSpeaking(CSteamID steamIDUser, bool bSpeaking);
	void ActivateGameOverlay(const char *pchDialog);
	void ActivateGameOverlayToUser(const char *pchDialog, CSteamID steamID);
	void ActivateGameOverlayToWebPage(const char *pchURL);
	void ActivateGameOverlayToStore(AppId_t nAppID, EOverlayToStoreFlag eFlag);
	void SetPlayedWith(CSteamID steamIDUserPlayedWith);
	void ActivateGameOverlayInviteDialog(CSteamID steamIDLobby);
	int GetSmallFriendAvatar(CSteamID steamIDFriend);
	int GetMediumFriendAvatar(CSteamID steamIDFriend);
	int GetLargeFriendAvatar(CSteamID steamIDFriend);
	bool RequestUserInformation(CSteamID steamIDUser, bool bRequireNameOnly);
	CALL_RESULT(ClanOfficerListResponse_t)
		SteamAPICall_t RequestClanOfficerList(CSteamID steamIDClan);
	CSteamID GetClanOwner(CSteamID steamIDClan);
	int GetClanOfficerCount(CSteamID steamIDClan);
	CSteamID GetClanOfficerByIndex(CSteamID steamIDClan, int iOfficer);
	uint32 GetUserRestrictions();
	bool SetRichPresence(const char *pchKey, const char *pchValue);
	void ClearRichPresence();
	const char *GetFriendRichPresence(CSteamID steamIDFriend, const char *pchKey);
	int GetFriendRichPresenceKeyCount(CSteamID steamIDFriend);
	const char *GetFriendRichPresenceKeyByIndex(CSteamID steamIDFriend, int iKey);
	void RequestFriendRichPresence(CSteamID steamIDFriend);
	bool InviteUserToGame(CSteamID steamIDFriend, const char *pchConnectString);
	int GetCoplayFriendCount();
	CSteamID GetCoplayFriend(int iCoplayFriend);
	int GetFriendCoplayTime(CSteamID steamIDFriend);
	AppId_t GetFriendCoplayGame(CSteamID steamIDFriend);
	// chat interface for games
	// this allows in-game access to group (clan) chats from in the game
	// the behavior is somewhat sophisticated, because the user may or may not be already in the group chat from outside the game or in the overlay
	// use ActivateGameOverlayToUser( "chat", steamIDClan ) to open the in-game overlay version of the chat
	CALL_RESULT(JoinClanChatRoomCompletionResult_t) 
		SteamAPICall_t JoinClanChatRoom(CSteamID steamIDClan);
	bool LeaveClanChatRoom(CSteamID steamIDClan);
	int GetClanChatMemberCount(CSteamID steamIDClan);
	CSteamID GetChatMemberByIndex(CSteamID steamIDClan, int iUser);
	bool SendClanChatMessage(CSteamID steamIDClanChat, const char *pchText);
	int GetClanChatMessage(CSteamID steamIDClanChat, int iMessage, void *prgchText, int cchTextMax, EChatEntryType *peChatEntryType, OUT_STRUCT() CSteamID *psteamidChatter);
	bool IsClanChatAdmin(CSteamID steamIDClanChat, CSteamID steamIDUser);
	bool IsClanChatWindowOpenInSteam(CSteamID steamIDClanChat);
	bool OpenClanChatWindowInSteam(CSteamID steamIDClanChat);
	bool CloseClanChatWindowInSteam(CSteamID steamIDClanChat);
	// peer-to-peer chat interception
	// this is so you can show P2P chats inline in the game
	bool SetListenForFriendsMessages(bool bInterceptEnabled);
	bool ReplyToFriendMessage(CSteamID steamIDFriend, const char *pchMsgToSend);
	int GetFriendMessage(CSteamID steamIDFriend, int iMessageID, void *pvData, int cubData, EChatEntryType *peChatEntryType);
	CALL_RESULT(FriendsGetFollowerCount_t)
		SteamAPICall_t GetFollowerCount(CSteamID steamID);
	CALL_RESULT(FriendsIsFollowing_t)
		SteamAPICall_t IsFollowing(CSteamID steamID);
	CALL_RESULT(FriendsEnumerateFollowingList_t)
		SteamAPICall_t EnumerateFollowingList(uint32 unStartIndex);
};