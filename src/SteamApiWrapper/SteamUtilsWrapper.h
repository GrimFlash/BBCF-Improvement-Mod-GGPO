#pragma once
#include <isteamutils.h>

#include <Windows.h>

interface SteamUtilsWrapper : public ISteamUtils
{
public:
	SteamUtilsWrapper(ISteamUtils**);
	~SteamUtilsWrapper();

	ISteamUtils* m_SteamUtils;

	uint32 GetSecondsSinceAppActive();
	uint32 GetSecondsSinceComputerActive();
	EUniverse GetConnectedUniverse();
	uint32 GetServerRealTime();
	const char *GetIPCountry();
	bool GetImageSize(int iImage, uint32 *pnWidth, uint32 *pnHeight);
	bool GetImageRGBA(int iImage, uint8 *pubDest, int nDestBufferSize);
	bool GetCSERIPPort(uint32 *unIP, uint16 *usPort);
	uint8 GetCurrentBatteryPower();
	uint32 GetAppID();
	void SetOverlayNotificationPosition(ENotificationPosition eNotificationPosition);
	bool IsAPICallCompleted(SteamAPICall_t hSteamAPICall, bool *pbFailed);
	ESteamAPICallFailure GetAPICallFailureReason(SteamAPICall_t hSteamAPICall);
	bool GetAPICallResult(SteamAPICall_t hSteamAPICall, void *pCallback, int cubCallback, int iCallbackExpected, bool *pbFailed);
	// Deprecated. Applications should use SteamAPI_RunCallbacks() instead. Game servers do not need to call this function.
	STEAM_PRIVATE_API(void RunFrame(); )
	uint32 GetIPCCallCount();
	void SetWarningMessageHook(SteamAPIWarningMessageHook_t pFunction);
	bool IsOverlayEnabled();
	bool BOverlayNeedsPresent();
	CALL_RESULT(CheckFileSignature_t)
		SteamAPICall_t CheckFileSignature(const char *szFileName);
	bool ShowGamepadTextInput(EGamepadTextInputMode eInputMode, EGamepadTextInputLineMode eLineInputMode, const char *pchDescription, uint32 unCharMax, const char *pchExistingText);
	uint32 GetEnteredGamepadTextLength();
	bool GetEnteredGamepadTextInput(char *pchText, uint32 cchText);
	const char *GetSteamUILanguage();
	bool IsSteamRunningInVR();
	void SetOverlayNotificationInset(int nHorizontalInset, int nVerticalInset);
	bool IsSteamInBigPictureMode();
	void StartVRDashboard();
	bool IsVRHeadsetStreamingEnabled();
	void SetVRHeadsetStreamingEnabled(bool bEnabled);
};