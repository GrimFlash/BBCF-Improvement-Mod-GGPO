#pragma once
#include <isteamuserstats.h>

#include <Windows.h>

interface SteamUserStatsWrapper : public ISteamUserStats
{
public:
	SteamUserStatsWrapper(ISteamUserStats**);
	~SteamUserStatsWrapper();

	ISteamUserStats* m_SteamUserStats;

	CALL_BACK(UserStatsReceived_t)
		bool RequestCurrentStats();
	bool GetStat(const char *pchName, int32 *pData);
	bool GetStat(const char *pchName, float *pData);
	bool SetStat(const char *pchName, int32 nData);
	bool SetStat(const char *pchName, float fData);
	bool UpdateAvgRateStat(const char *pchName, float flCountThisSession, double dSessionLength);
	bool GetAchievement(const char *pchName, bool *pbAchieved);
	bool SetAchievement(const char *pchName);
	bool ClearAchievement(const char *pchName);
	bool GetAchievementAndUnlockTime(const char *pchName, bool *pbAchieved, uint32 *punUnlockTime);
	bool StoreStats();
	int GetAchievementIcon(const char *pchName);
	const char *GetAchievementDisplayAttribute(const char *pchName, const char *pchKey);
	bool IndicateAchievementProgress(const char *pchName, uint32 nCurProgress, uint32 nMaxProgress);
	uint32 GetNumAchievements();
	const char *GetAchievementName(uint32 iAchievement);
	CALL_RESULT(UserStatsReceived_t)
		SteamAPICall_t RequestUserStats(CSteamID steamIDUser);
	bool GetUserStat(CSteamID steamIDUser, const char *pchName, int32 *pData);
	bool GetUserStat(CSteamID steamIDUser, const char *pchName, float *pData);
	bool GetUserAchievement(CSteamID steamIDUser, const char *pchName, bool *pbAchieved);
	bool GetUserAchievementAndUnlockTime(CSteamID steamIDUser, const char *pchName, bool *pbAchieved, uint32 *punUnlockTime);
	bool ResetAllStats(bool bAchievementsToo);
	CALL_RESULT(LeaderboardFindResult_t)
		SteamAPICall_t FindOrCreateLeaderboard(const char *pchLeaderboardName, ELeaderboardSortMethod eLeaderboardSortMethod, ELeaderboardDisplayType eLeaderboardDisplayType);
	CALL_RESULT(LeaderboardFindResult_t)
		SteamAPICall_t FindLeaderboard(const char *pchLeaderboardName);
	const char *GetLeaderboardName(SteamLeaderboard_t hSteamLeaderboard);
	int GetLeaderboardEntryCount(SteamLeaderboard_t hSteamLeaderboard);
	ELeaderboardSortMethod GetLeaderboardSortMethod(SteamLeaderboard_t hSteamLeaderboard);
	ELeaderboardDisplayType GetLeaderboardDisplayType(SteamLeaderboard_t hSteamLeaderboard);
	CALL_RESULT(LeaderboardScoresDownloaded_t)
		SteamAPICall_t DownloadLeaderboardEntries(SteamLeaderboard_t hSteamLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd);
	METHOD_DESC(Downloads leaderboard entries for an arbitrary set of users - ELeaderboardDataRequest is k_ELeaderboardDataRequestUsers)
		CALL_RESULT(LeaderboardScoresDownloaded_t)
		SteamAPICall_t DownloadLeaderboardEntriesForUsers(SteamLeaderboard_t hSteamLeaderboard,
			ARRAY_COUNT_D(cUsers, Array of users to retrieve) CSteamID *prgUsers, int cUsers);
	bool GetDownloadedLeaderboardEntry(SteamLeaderboardEntries_t hSteamLeaderboardEntries, int index, LeaderboardEntry_t *pLeaderboardEntry, int32 *pDetails, int cDetailsMax);
	CALL_RESULT(LeaderboardScoreUploaded_t)
		SteamAPICall_t UploadLeaderboardScore(SteamLeaderboard_t hSteamLeaderboard, ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int32 nScore, const int32 *pScoreDetails, int cScoreDetailsCount);
	CALL_RESULT(LeaderboardUGCSet_t)
		SteamAPICall_t AttachLeaderboardUGC(SteamLeaderboard_t hSteamLeaderboard, UGCHandle_t hUGC);
	CALL_RESULT(NumberOfCurrentPlayers_t)
		SteamAPICall_t GetNumberOfCurrentPlayers();
	CALL_RESULT(GlobalAchievementPercentagesReady_t)
		SteamAPICall_t RequestGlobalAchievementPercentages();
	int GetMostAchievedAchievementInfo(char *pchName, uint32 unNameBufLen, float *pflPercent, bool *pbAchieved);
	int GetNextMostAchievedAchievementInfo(int iIteratorPrevious, char *pchName, uint32 unNameBufLen, float *pflPercent, bool *pbAchieved);
	bool GetAchievementAchievedPercent(const char *pchName, float *pflPercent);
	CALL_RESULT(GlobalStatsReceived_t)
		SteamAPICall_t RequestGlobalStats(int nHistoryDays);
	bool GetGlobalStat(const char *pchStatName, int64 *pData);
	bool GetGlobalStat(const char *pchStatName, double *pData);
	int32 GetGlobalStatHistory(const char *pchStatName, ARRAY_COUNT(cubData) int64 *pData, uint32 cubData);
	int32 GetGlobalStatHistory(const char *pchStatName, ARRAY_COUNT(cubData) double *pData, uint32 cubData);

#ifdef _PS3
	bool InstallPS3Trophies();
	uint64 GetTrophySpaceRequiredBeforeInstall();
	bool SetUserStatsData(const void *pvData, uint32 cubData);
	bool GetUserStatsData(void *pvData, uint32 cubData, uint32 *pcubWritten);
#endif
};