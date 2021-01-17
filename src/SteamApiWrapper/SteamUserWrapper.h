#pragma once
#include <isteamuser.h>

#include <Windows.h>

interface SteamUserWrapper : public ISteamUser
{
public:
	SteamUserWrapper(ISteamUser**);
	~SteamUserWrapper();

	ISteamUser* m_SteamUser;

	HSteamUser GetHSteamUser();
	bool BLoggedOn();
	CSteamID GetSteamID();
	int InitiateGameConnection(void *pAuthBlob, int cbMaxAuthBlob, CSteamID steamIDGameServer, uint32 unIPServer, uint16 usPortServer, bool bSecure);
	void TerminateGameConnection(uint32 unIPServer, uint16 usPortServer);
	void TrackAppUsageEvent(CGameID gameID, int eAppUsageEvent, const char *pchExtraInfo = "");
	bool GetUserDataFolder(char *pchBuffer, int cubBuffer);
	void StartVoiceRecording();
	void StopVoiceRecording();
	EVoiceResult GetAvailableVoice(uint32 *pcbCompressed, uint32 *pcbUncompressed_Deprecated = 0, uint32 nUncompressedVoiceDesiredSampleRate_Deprecated = 0);
	EVoiceResult GetVoice(bool bWantCompressed, void *pDestBuffer, uint32 cbDestBufferSize, uint32 *nBytesWritten, bool bWantUncompressed_Deprecated = false, void *pUncompressedDestBuffer_Deprecated = 0, uint32 cbUncompressedDestBufferSize_Deprecated = 0, uint32 *nUncompressBytesWritten_Deprecated = 0, uint32 nUncompressedVoiceDesiredSampleRate_Deprecated = 0);
	EVoiceResult DecompressVoice(const void *pCompressed, uint32 cbCompressed, void *pDestBuffer, uint32 cbDestBufferSize, uint32 *nBytesWritten, uint32 nDesiredSampleRate);
	uint32 GetVoiceOptimalSampleRate();
	HAuthTicket GetAuthSessionTicket(void *pTicket, int cbMaxTicket, uint32 *pcbTicket);
	EBeginAuthSessionResult BeginAuthSession(const void *pAuthTicket, int cbAuthTicket, CSteamID steamID);
	void EndAuthSession(CSteamID steamID);
	void CancelAuthTicket(HAuthTicket hAuthTicket);
	EUserHasLicenseForAppResult UserHasLicenseForApp(CSteamID steamID, AppId_t appID);
	bool BIsBehindNAT();
	void AdvertiseGame(CSteamID steamIDGameServer, uint32 unIPServer, uint16 usPortServer);
	CALL_RESULT(EncryptedAppTicketResponse_t)
		SteamAPICall_t RequestEncryptedAppTicket(void *pDataToInclude, int cbDataToInclude);
	bool GetEncryptedAppTicket(void *pTicket, int cbMaxTicket, uint32 *pcbTicket);
	int GetGameBadgeLevel(int nSeries, bool bFoil);
	int GetPlayerSteamLevel();
	CALL_RESULT(StoreAuthURLResponse_t)
		SteamAPICall_t RequestStoreAuthURL(const char *pchRedirectURL);
	bool BIsPhoneVerified();
	bool BIsTwoFactorEnabled();
	bool BIsPhoneIdentifying();
	bool BIsPhoneRequiringVerification();
};