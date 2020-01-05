#pragma once
#include "stdafx.h"
#include <string>

extern const int DEBUG_LEVEL; //7=lowest priority, 0=highest
extern const char *version_num;
extern const int internal_version_num;

struct settingsIni_t
{
	std::string togglebutton;
	bool notificationpopups;
	int renderwidth;
	int renderheight;
	int antialiasing;
	int vsync;
	int regionlock;
	int viewport;
	int menusize;
	int cpuusagefix;
	int checkupdates;
};

struct savedSettings_t
{
	RECT newSourceRect;
	D3DVIEWPORT9 newViewport;
	D3DXVECTOR2 origViewportRes;
	bool isDuelFieldSprite;
	bool isFiltering;
};

class Settings
{
public:
	static settingsIni_t settingsIni;
	static savedSettings_t savedSettings;

	static void applySettingsIni(D3DPRESENT_PARAMETERS* pPresentationParameters);
	static bool loadSettingsIni();
	static void initSavedSettings();
	static void setViewportSize(D3DVIEWPORT9 Viewport);
	static short getToggleButtonValue();
};