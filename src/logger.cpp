#include "../include/logger.h"
#include <time.h>
#include <fstream>
#include <sstream>
#include <iomanip> 
#include <string> 
bool checkHookSuccess(PBYTE addr, const char* funcName)
{
	if (!addr)
	{
		LOG(2, "FAILED to hook %s\n", funcName);
		return false;

	}
	LOG(2, "Successfully hooked %s at 0x%p\n", funcName, addr);
	return true;
}

#ifndef RELEASE_VER

FILE* g_oFile;

inline void logger(const char* message, ...)
{
	if (!message) { return; }

	va_list args;
	va_start(args, message);
	vfprintf(g_oFile, message, args);
	va_end(args);

	fflush(g_oFile);
}

std::string uint_to_hex(unsigned int i)
{
	std::stringstream stream;
	stream << "0x"
		<< std::setfill('0') << std::setw(sizeof(unsigned int) * 2)
		<< std::hex << i;
	return stream.str();
}
void logGameState(uintptr_t* time, uintptr_t* p1, uintptr_t* p2, uintptr_t* XScreenScroll, uintptr_t* YScreenScroll, uintptr_t* objectData[])
{
	fprintf(g_oFile, ("Time Address: "+uint_to_hex((unsigned int)time) + " Value: "+ std::to_string(*time) +"\n").c_str());
	fprintf(g_oFile, ("P1 Address: " + uint_to_hex((unsigned int)p1) + " Value: " + uint_to_hex(*p1) + "\n").c_str());
	fprintf(g_oFile, ("P2 Address: " + uint_to_hex((unsigned int)p2) + " Value: " + uint_to_hex(*p2) + "\n").c_str());
	fprintf(g_oFile, ("X Screen Scroll 2 Address: " + uint_to_hex((unsigned int)XScreenScroll) + " Value: " + std::to_string(*XScreenScroll) + "\n").c_str());
	fprintf(g_oFile, ("Y Screen Scroll 2 Address: " + uint_to_hex((unsigned int)YScreenScroll) + " Value: " + std::to_string(*YScreenScroll) + "\n").c_str());
	fflush(g_oFile);
}


char* getFullDate()
{
	time_t timer;
	char* buffer = (char *)malloc(sizeof(char)* 26);
	if (!buffer)
	{
		return NULL;
	}
	struct tm* tm_info;

	time(&timer);
	tm_info = localtime(&timer);

	strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
	return buffer;
}

void openLogger()
{
	g_oFile = fopen("DEBUG.txt", "w");
	char* time = getFullDate();
	LOG(1, "\n\n\n\n");

	if (time)
	{
		LOG(1, "BBCF_FIX START - %s\n", time);
		free(time);
	}
	else
	{
		LOG(1, "BBCF_FIX START - {Couldn't get the current time}\n");
	}
	LOG(1, "/////////////////////////////////////\n");
	LOG(1, "/////////////////////////////////////\n\n");
}

void closeLogger()
{
	char* time = getFullDate();
	if (time)
	{
		LOG(1, "BBCF_FIX STOP - %s\n", time);
		free(time);
	}
	else
	{
		LOG(1, "BBCF_FIX STOP - {Couldn't get the current time}\n");
	}
	if (g_oFile)
		fclose(g_oFile);
}

void logSettingsIni()
{
	LOG(1, "settings.ini config:\n");
	LOG(1, "\t- ToggleButton: %s\n", Settings::settingsIni.togglebutton.c_str());
	LOG(1, "\t- NotificationPopups: %d\n", Settings::settingsIni.notificationpopups);
	LOG(1, "\t- RenderingWidth: %d\n", Settings::settingsIni.renderwidth);
	LOG(1, "\t- RenderingHeight: %d\n", Settings::settingsIni.renderheight);
	LOG(1, "\t- Viewport: %d\n", Settings::settingsIni.viewport);
	LOG(1, "\t- AntiAliasing: %d\n", Settings::settingsIni.antialiasing);
	LOG(1, "\t- V-Sync: %d\n", Settings::settingsIni.vsync);
	LOG(1, "\t- MenuSize: %d\n", Settings::settingsIni.menusize);
	LOG(1, "\t- Regionlock: %d\n", Settings::settingsIni.regionlock);
	LOG(1, "\t- CpuUsageFix: %d\n", Settings::settingsIni.cpuusagefix);
	LOG(1, "\t- CheckUpdates: %d\n", Settings::settingsIni.checkupdates);
}

void logD3DPParams(D3DPRESENT_PARAMETERS* pPresentationParameters, bool isOriginalSettings)
{
	if (isOriginalSettings)
	{
		LOG(1, "Original D3D PresentationParameters:\n");
	}
	else
	{
		LOG(1, "Modified D3D PresentationParameters:\n");
	}
	LOG(1, "\t- BackBufferWidth: %u\n", pPresentationParameters->BackBufferWidth);
	LOG(1, "\t- BackBufferHeight: %u\n", pPresentationParameters->BackBufferHeight);
	LOG(1, "\t- BackBufferFormat: %u\n", pPresentationParameters->BackBufferFormat);
	LOG(1, "\t- BackBufferCount: %u\n", pPresentationParameters->BackBufferCount);
	LOG(1, "\t- SwapEffect: %u\n", pPresentationParameters->SwapEffect);
	LOG(1, "\t- MultiSampleType: %u\n", pPresentationParameters->MultiSampleType);
	LOG(1, "\t- MultiSampleQuality: %d\n", pPresentationParameters->MultiSampleQuality);
	LOG(1, "\t- EnableAutoDepthStencil: %d\n", pPresentationParameters->EnableAutoDepthStencil);
	LOG(1, "\t- FullScreen_RefreshRateInHz: %u\n", pPresentationParameters->FullScreen_RefreshRateInHz);
	LOG(1, "\t- hDeviceWindow: 0x%p\n", pPresentationParameters->hDeviceWindow);
	LOG(1, "\t- Windowed: %d\n", pPresentationParameters->Windowed);
	LOG(1, "\t- Flags: %d\n", pPresentationParameters->Flags);
	LOG(1, "\t- PresentationInterval: %u\n", pPresentationParameters->PresentationInterval);
}

#else

void openLogger()
{
}
void closeLogger()
{
}
void logSettingsIni()
{
}
void logD3DPParams(D3DPRESENT_PARAMETERS* pPresentationParameters, bool isOriginalSettings)
{
}

#endif