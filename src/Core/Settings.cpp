#include "Settings.h"
#include "logger.h"

#include "Core/interfaces.h"

#include <atlstr.h>
#include <ctime>

#define VIEWPORT_DEFAULT 1

settingsIni_t Settings::settingsIni = {};
savedSettings_t Settings::savedSettings = {};

void Settings::applySettingsIni(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	if (settingsIni.viewport != VIEWPORT_DEFAULT)
	{
		pPresentationParameters->BackBufferHeight = settingsIni.renderheight;
		pPresentationParameters->BackBufferWidth = settingsIni.renderwidth;
	}

	switch (Settings::settingsIni.antialiasing)
	{
	case 0:
		pPresentationParameters->MultiSampleType = D3DMULTISAMPLE_NONE;
		pPresentationParameters->MultiSampleQuality = 0;
		break;
		//case 2:
		//	pPresentationParameters->MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
		//	break;
	case 4:
		pPresentationParameters->MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;
		break;
	case 5:
	default:
		break;
	}

	//pPresentationParameters->Windowed = !Settings::settingsIni.fullscreen;

	pPresentationParameters->PresentationInterval = settingsIni.vsync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;
	
	//pPresentationParameters->Windowed = !settingsIni.fullscreen;
	//if (settingsIni.fullscreen)
	//{
	//	pPresentationParameters->FullScreen_RefreshRateInHz = 60; // savedSettings.adapterRefreshRate;
	//}
}

int Settings::readSettingsFilePropertyInt(LPCWSTR key, LPCWSTR defaultVal, LPCWSTR filename)
{
	CString strNotificationPopups;
	GetPrivateProfileString(_T("Settings"), key, defaultVal, strNotificationPopups.GetBuffer(MAX_PATH), MAX_PATH, filename);
	strNotificationPopups.ReleaseBuffer();
	return _ttoi(strNotificationPopups);
}

float Settings::readSettingsFilePropertyFloat(LPCWSTR key, LPCWSTR defaultVal, LPCWSTR filename)
{
	CString strCustomHUDScale;
	GetPrivateProfileString(_T("Settings"), key, defaultVal, strCustomHUDScale.GetBuffer(MAX_PATH), MAX_PATH, filename);
	strCustomHUDScale.ReleaseBuffer();
	return _ttof(strCustomHUDScale);
}

std::string Settings::readSettingsFilePropertyString(LPCWSTR key, LPCWSTR defaultVal, LPCWSTR filename)
{
	CString strBuffer;
	GetPrivateProfileString(_T("Settings"), key, defaultVal, strBuffer.GetBuffer(MAX_PATH), MAX_PATH, filename);
	strBuffer.ReleaseBuffer();
	CT2CA pszConvertedAnsiString(strBuffer);
	return pszConvertedAnsiString.m_psz;
}

bool Settings::loadSettingsFile()
{
	CString strINIPath;

	_wfullpath((wchar_t*)strINIPath.GetBuffer(MAX_PATH), L"settings.ini", MAX_PATH);
	strINIPath.ReleaseBuffer();

	if (GetFileAttributes(strINIPath) == 0xFFFFFFFF)
	{
		MessageBoxA(NULL, "Settings INI File Was Not Found!", "Error", MB_OK);
		return false;
	}

	void* iniPtr = 0;

	//X-Macro
#define SETTING(_type, _var, _inistring, _defaultval) \
	iniPtr = &settingsIni.##_var; \
	if(strcmp(#_type, "bool") == 0 || strcmp(#_type, "int") == 0) { \
		*(int*)iniPtr = readSettingsFilePropertyInt(L##_inistring, L##_defaultval, strINIPath); } \
	else if(strcmp(#_type, "float") == 0) { \
		*(float*)iniPtr = readSettingsFilePropertyFloat(L##_inistring, L##_defaultval, strINIPath); } \
	else if (strcmp(#_type, "std::string") == 0) { \
		*(std::string*)iniPtr = readSettingsFilePropertyString(L##_inistring, L##_defaultval, strINIPath); }
#include "settings.def"
#undef SETTING

	// Set buttons back to default if their values are incorrect
	if (settingsIni.togglebutton.length() != 2 || settingsIni.togglebutton[0] != 'F')
		settingsIni.togglebutton = "F1";

	if (settingsIni.toggleOnlineButton.length() != 2 || settingsIni.toggleOnlineButton[0] != 'F')
		settingsIni.toggleOnlineButton = "F2";

	if (settingsIni.toggleHUDbutton.length() != 2 || settingsIni.toggleHUDbutton[0] != 'F')
		settingsIni.toggleHUDbutton = "F3";

	return true;
}

void Settings::initSavedSettings()
{
	LOG(7, "initSavedSettings\n");

	switch (settingsIni.viewport)
	{
	case 2:
		LOG(7, " - case 2\n");
		savedSettings.newSourceRect.right = settingsIni.renderwidth;
		savedSettings.newSourceRect.bottom = settingsIni.renderheight;
		savedSettings.newViewport.Width = settingsIni.renderwidth;;
		savedSettings.newViewport.Height = settingsIni.renderheight;
		break;
	case 3:
		LOG(7, " - case 3\n");
		savedSettings.newSourceRect.right = 1280;
		savedSettings.newSourceRect.bottom = 768;
		savedSettings.newViewport.Width = 1280;
		savedSettings.newViewport.Height = 768;
		break;
	case 1:
	default:
		LOG(7, " - case 1, default\n");
		//in this case the value is set in Direct3DDevice9ExWrapper::CreateRenderTargetEx!
		break;
	}

	savedSettings.origViewportRes.x = 0.0;
	savedSettings.origViewportRes.y = 0.0;

	savedSettings.isDuelFieldSprite = false;

	savedSettings.isFiltering = false;
}

short Settings::getButtonValue(std::string button)
{
	if (button == "F1")
		return 112;
	if (button == "F2")
		return 113;
	if (button == "F3")
		return 114;
	if (button == "F4")
		return 115;
	if (button == "F5")
		return 116;
	if (button == "F6")
		return 117;
	if (button == "F7")
		return 118;
	if (button == "F8")
		return 119;
	if (button == "F9")
		return 120;

	//default to F1
	button = "F1";
	return 112;
}
