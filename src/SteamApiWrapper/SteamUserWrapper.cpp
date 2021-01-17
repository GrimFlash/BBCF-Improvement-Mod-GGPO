#include "SteamUserWrapper.h"

#include "Core/logger.h"
#include "Core/utils.h"

SteamUserWrapper::SteamUserWrapper(ISteamUser** pSteamUser)
{
	LOG(7, "SteamUserWrapper\n");
	LOG(7, "\t- before: *pSteamUser: 0x%p, thispointer: 0x%p\n", *pSteamUser, this);

	m_SteamUser = *pSteamUser;
	void* thisAddress = this;
	WriteToProtectedMemory((uintptr_t)pSteamUser, (char*)&thisAddress, 4); //basically *pSteamUser = this;

	LOG(7, "\t- after: *pSteamUser: 0x%p, m_SteamUser: 0x%p\n", *pSteamUser, m_SteamUser);
}

SteamUserWrapper::~SteamUserWrapper()
{
}

HSteamUser SteamUserWrapper::GetHSteamUser()
{
	LOG(7, "GetHSteamUser\n");
	return m_SteamUser->GetHSteamUser();
}

bool SteamUserWrapper::BLoggedOn()
{
	LOG(7, "BLoggedOn\n");
	return m_SteamUser->BLoggedOn();
}

CSteamID SteamUserWrapper::GetSteamID()
{
	LOG(7, "GetSteamID\n");
	return m_SteamUser->GetSteamID();
}

int SteamUserWrapper::InitiateGameConnection(void *pAuthBlob, int cbMaxAuthBlob, CSteamID steamIDGameServer, uint32 unIPServer, uint16 usPortServer, bool bSecure)
{
	LOG(7, "InitiateGameConnection\n");
	return m_SteamUser->InitiateGameConnection(pAuthBlob, cbMaxAuthBlob, steamIDGameServer, unIPServer, usPortServer, bSecure);
}

void SteamUserWrapper::TerminateGameConnection(uint32 unIPServer, uint16 usPortServer)
{
	LOG(7, "TerminateGameConnection\n");
	return m_SteamUser->TerminateGameConnection(unIPServer, usPortServer);
}

void SteamUserWrapper::TrackAppUsageEvent(CGameID gameID, int eAppUsageEvent, const char *pchExtraInfo)
{
	LOG(7, "TrackAppUsageEvent\n");
	return m_SteamUser->TrackAppUsageEvent(gameID, eAppUsageEvent, pchExtraInfo);
}

bool SteamUserWrapper::GetUserDataFolder(char *pchBuffer, int cubBuffer)
{
	LOG(7, "GetUserDataFolder\n");
	return m_SteamUser->GetUserDataFolder(pchBuffer, cubBuffer);
}

void SteamUserWrapper::StartVoiceRecording()
{
	LOG(7, "StartVoiceRecording\n");
	return m_SteamUser->StartVoiceRecording();
}

void SteamUserWrapper::StopVoiceRecording()
{
	LOG(7, "StopVoiceRecording\n");
	return m_SteamUser->StopVoiceRecording();
}

EVoiceResult SteamUserWrapper::GetAvailableVoice(uint32 *pcbCompressed, uint32 *pcbUncompressed_Deprecated, uint32 nUncompressedVoiceDesiredSampleRate_Deprecated)
{
	LOG(7, "GetAvailableVoice\n");
	return m_SteamUser->GetAvailableVoice(pcbCompressed, pcbUncompressed_Deprecated, nUncompressedVoiceDesiredSampleRate_Deprecated);
}

EVoiceResult SteamUserWrapper::GetVoice(bool bWantCompressed, void *pDestBuffer, uint32 cbDestBufferSize, uint32 *nBytesWritten, bool bWantUncompressed_Deprecated, 
	void *pUncompressedDestBuffer_Deprecated, uint32 cbUncompressedDestBufferSize_Deprecated, uint32 *nUncompressBytesWritten_Deprecated, uint32 nUncompressedVoiceDesiredSampleRate_Deprecated)
{
	LOG(7, "GetVoice\n");
	return m_SteamUser->GetVoice(bWantCompressed, pDestBuffer, cbDestBufferSize, nBytesWritten, bWantUncompressed_Deprecated, pUncompressedDestBuffer_Deprecated, 
		cbUncompressedDestBufferSize_Deprecated, nUncompressBytesWritten_Deprecated, nUncompressedVoiceDesiredSampleRate_Deprecated);
}

EVoiceResult SteamUserWrapper::DecompressVoice(const void *pCompressed, uint32 cbCompressed, void *pDestBuffer, uint32 cbDestBufferSize, uint32 *nBytesWritten, uint32 nDesiredSampleRate)
{
	LOG(7, "DecompressVoice\n");
	return m_SteamUser->DecompressVoice(pCompressed, cbCompressed, pDestBuffer, cbDestBufferSize, nBytesWritten, nDesiredSampleRate);
}

uint32 SteamUserWrapper::GetVoiceOptimalSampleRate()
{
	LOG(7, "GetVoiceOptimalSampleRate\n");
	return m_SteamUser->GetVoiceOptimalSampleRate();
}

HAuthTicket SteamUserWrapper::GetAuthSessionTicket(void *pTicket, int cbMaxTicket, uint32 *pcbTicket)
{
	LOG(7, "GetAuthSessionTicket\n");
	return m_SteamUser->GetAuthSessionTicket(pTicket, cbMaxTicket, pcbTicket);
}

EBeginAuthSessionResult SteamUserWrapper::BeginAuthSession(const void *pAuthTicket, int cbAuthTicket, CSteamID steamID)
{
	LOG(7, "BeginAuthSession\n");
	return m_SteamUser->BeginAuthSession(pAuthTicket, cbAuthTicket, steamID);
}

void SteamUserWrapper::EndAuthSession(CSteamID steamID)
{
	LOG(7, "EndAuthSession\n");
	return m_SteamUser->EndAuthSession(steamID);
}

void SteamUserWrapper::CancelAuthTicket(HAuthTicket hAuthTicket)
{
	LOG(7, "CancelAuthTicket\n");
	return m_SteamUser->CancelAuthTicket(hAuthTicket);
}

EUserHasLicenseForAppResult SteamUserWrapper::UserHasLicenseForApp(CSteamID steamID, AppId_t appID)
{
	LOG(7, "UserHasLicenseForApp\n");
	return m_SteamUser->UserHasLicenseForApp(steamID, appID);
}

bool SteamUserWrapper::BIsBehindNAT()
{
	LOG(7, "BIsBehindNAT");
	return m_SteamUser->BIsBehindNAT();
}

void SteamUserWrapper::AdvertiseGame(CSteamID steamIDGameServer, uint32 unIPServer, uint16 usPortServer)
{
	LOG(7, "AdvertiseGame\n");
	return m_SteamUser->AdvertiseGame(steamIDGameServer, unIPServer, usPortServer);
}

CALL_RESULT(EncryptedAppTicketResponse_t)
SteamAPICall_t SteamUserWrapper::RequestEncryptedAppTicket(void *pDataToInclude, int cbDataToInclude)
{
	LOG(7, "RequestEncryptedAppTicket\n");
	return m_SteamUser->RequestEncryptedAppTicket(pDataToInclude, cbDataToInclude);
}

bool SteamUserWrapper::GetEncryptedAppTicket(void *pTicket, int cbMaxTicket, uint32 *pcbTicket)
{
	LOG(7, "GetEncryptedAppTicket\n");
	return m_SteamUser->GetEncryptedAppTicket(pTicket, cbMaxTicket, pcbTicket);
}

int SteamUserWrapper::GetGameBadgeLevel(int nSeries, bool bFoil)
{
	LOG(7, "GetGameBadgeLevel\n");
	return m_SteamUser->GetGameBadgeLevel(nSeries, bFoil);
}

int SteamUserWrapper::GetPlayerSteamLevel()
{
	LOG(7, "GetPlayerSteamLevel\n");
	return m_SteamUser->GetPlayerSteamLevel();
}

CALL_RESULT(StoreAuthURLResponse_t)
SteamAPICall_t SteamUserWrapper::RequestStoreAuthURL(const char *pchRedirectURL)
{
	LOG(7, "RequestStoreAuthURL\n");
	return m_SteamUser->RequestStoreAuthURL(pchRedirectURL);
}

bool SteamUserWrapper::BIsPhoneVerified()
{
	LOG(7, "BIsPhoneVerified\n");
	return m_SteamUser->BIsPhoneVerified();
}

bool SteamUserWrapper::BIsTwoFactorEnabled()
{
	LOG(7, "BIsTwoFactorEnabled\n");
	return m_SteamUser->BIsTwoFactorEnabled();
}

bool SteamUserWrapper::BIsPhoneIdentifying()
{
	LOG(7, "BIsPhoneIdentifying\n");
	return m_SteamUser->BIsPhoneIdentifying();
}

bool SteamUserWrapper::BIsPhoneRequiringVerification()
{
	LOG(7, "BIsPhoneRequiringVerification\n");
	return m_SteamUser->BIsPhoneRequiringVerification();
}
