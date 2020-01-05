#include "../include/settings.h"
#include "../include/containers.h"
#include <ctime>
#include <atlstr.h>

const int DEBUG_LEVEL = 2; //7=lowest priority, 0=highest
const char *version_num = "v2.07";
const int internal_version_num = 207;

settingsIni_t Settings::settingsIni = {};
savedSettings_t Settings::savedSettings = {};

void Settings::applySettingsIni(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	if (settingsIni.viewport != 1)
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

	switch (settingsIni.vsync)
	{
	case 0:
		//vsync off
		pPresentationParameters->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		break;
	case 1:
	default:
		// vsync on 
		pPresentationParameters->PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
		break;
	}
	
	//pPresentationParameters->Windowed = !settingsIni.fullscreen;
	//if (settingsIni.fullscreen)
	//{
	//	pPresentationParameters->FullScreen_RefreshRateInHz = 60; // savedSettings.adapterRefreshRate;
	//}
}

bool Settings::loadSettingsIni()
{
	CString strINIPath;

	_wfullpath((wchar_t*)strINIPath.GetBuffer(MAX_PATH), L"settings.ini", MAX_PATH);
	strINIPath.ReleaseBuffer();

	if (GetFileAttributes(strINIPath) == 0xFFFFFFFF)
	{
		MessageBoxA(NULL, "Settings INI File Was Not Found!", "Error", MB_OK);
		return false;
	}

	//////////////////////////////////// 
	// Read INI For Game Path 
	//////////////////////////////////// 
	CString strToggleButton;
	GetPrivateProfileString(_T("Settings"), _T("ToggleButton"), _T("F1"), strToggleButton.GetBuffer(MAX_PATH), MAX_PATH, strINIPath);
	strToggleButton.ReleaseBuffer();
	CT2CA pszConvertedAnsiString(strToggleButton);
	settingsIni.togglebutton = pszConvertedAnsiString;
	if (settingsIni.togglebutton.length() > 2)
		settingsIni.togglebutton = "F1";
	if (settingsIni.togglebutton[0] != 'F')
		settingsIni.togglebutton = "F1";

	CString strNotificationPopups;
	GetPrivateProfileString(_T("Settings"), _T("NotificationPopups"), _T("1"), strNotificationPopups.GetBuffer(MAX_PATH), MAX_PATH, strINIPath);
	strNotificationPopups.ReleaseBuffer();
	settingsIni.notificationpopups = _ttoi(strNotificationPopups);

	CString strRenderingWidth;
	GetPrivateProfileString(_T("Settings"), _T("RenderingWidth"), _T("1920"), strRenderingWidth.GetBuffer(MAX_PATH), MAX_PATH, strINIPath);
	strRenderingWidth.ReleaseBuffer();
	settingsIni.renderwidth = _ttoi(strRenderingWidth);

	CString strRenderingHeight;
	GetPrivateProfileString(_T("Settings"), _T("RenderingHeight"), _T("1080"), strRenderingHeight.GetBuffer(MAX_PATH), MAX_PATH, strINIPath);
	strRenderingHeight.ReleaseBuffer();
	settingsIni.renderheight = _ttoi(strRenderingHeight);

	CString strAA;
	GetPrivateProfileString(_T("Settings"), _T("AntiAliasing"), _T("-1"), strAA.GetBuffer(MAX_PATH), MAX_PATH, strINIPath);
	strAA.ReleaseBuffer();
	settingsIni.antialiasing = _ttoi(strAA);

	CString strVsync;
	GetPrivateProfileString(_T("Settings"), _T("V-sync"), _T("1"), strVsync.GetBuffer(MAX_PATH), MAX_PATH, strINIPath);
	strVsync.ReleaseBuffer();
	settingsIni.vsync = _ttoi(strVsync);

	CString strRegionlock;
	GetPrivateProfileString(_T("Settings"), _T("Regionlock"), _T("3"), strRegionlock.GetBuffer(MAX_PATH), MAX_PATH, strINIPath);
	strRegionlock.ReleaseBuffer();
	settingsIni.regionlock = _ttoi(strRegionlock);

	CString strViewport;
	GetPrivateProfileString(_T("Settings"), _T("Viewport"), _T("1"), strViewport.GetBuffer(MAX_PATH), MAX_PATH, strINIPath);
	strViewport.ReleaseBuffer();
	settingsIni.viewport = _ttoi(strViewport); 

	CString strMenuSize;
	GetPrivateProfileString(_T("Settings"), _T("MenuSize"), _T("2"), strMenuSize.GetBuffer(MAX_PATH), MAX_PATH, strINIPath);
	strMenuSize.ReleaseBuffer();
	settingsIni.menusize = _ttoi(strMenuSize);

	CString strCpuUsageFix;
	GetPrivateProfileString(_T("Settings"), _T("CpuUsageFix"), _T("0"), strCpuUsageFix.GetBuffer(MAX_PATH), MAX_PATH, strINIPath);
	strCpuUsageFix.ReleaseBuffer();
	settingsIni.cpuusagefix= _ttoi(strCpuUsageFix);

	CString strCheckUpdates;
	GetPrivateProfileString(_T("Settings"), _T("CheckUpdates"), _T("1"), strCheckUpdates.GetBuffer(MAX_PATH), MAX_PATH, strINIPath);
	strCheckUpdates.ReleaseBuffer();
	settingsIni.checkupdates = _ttoi(strCheckUpdates);

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

short Settings::getToggleButtonValue()
{
	if (settingsIni.togglebutton == "F1")
		return 112;
	else if (settingsIni.togglebutton == "F2")
		return 113;
	else if (settingsIni.togglebutton == "F3")
		return 114;
	else if (settingsIni.togglebutton == "F4")
		return 115;
	else if (settingsIni.togglebutton == "F5")
		return 116;
	else if (settingsIni.togglebutton == "F6")
		return 117;
	else if (settingsIni.togglebutton == "F7")
		return 118;
	else if (settingsIni.togglebutton == "F8")
		return 119;
	else if (settingsIni.togglebutton == "F9")
		return 120;
	else//default to F1
	{
		settingsIni.togglebutton = "F1";
		return 112;
	}
}
