#include "update_check.h"

#include "url_downloader.h"

#include "Core/info.h"
#include "Core/logger.h"
#include "Overlay/Logger/ImGuiLogger.h"
#include "Overlay/WindowManager.h"

#include <handleapi.h>
#include <processthreadsapi.h>
#include <regex>

std::string newVersionNum = "";

std::string GetNewVersionNum()
{
	if (newVersionNum != "")
	{
		return newVersionNum;
	}

	return "";
}

void CheckUpdate()
{
	std::wstring wUrl = MOD_LINK_FORUM;
	std::string data = DownloadUrl(wUrl);

	if (strcmp(data.c_str(), "") == 0)
	{
		g_imGuiLogger->Log("[error] Update check failed. No data downloaded.\n");
		LOG(2, "Update check failed.No data downloaded.\n");
		return;
	}

	data = data.substr(0, 950).c_str();

	// Fits on: <title>[BBCF IMPROVEMENT MOD] (v2.06
	// and captures: v2.06
	std::regex r("<title>.+(v\\d\.\\d\\d)");
	std::smatch m;
	std::regex_search(data, m, r);

	if (m[1].str() == "")
	{
		g_imGuiLogger->Log("[error] Update check failed. Regex no match.\n");
		return;
	}

	if (strcmp(m[1].str().c_str(), MOD_VERSION_NUM) != 0)
	{
		newVersionNum = m[1].str();

		LOG(2, "New version found: %s\n", newVersionNum.c_str());
		g_imGuiLogger->Log("[system] Update available: BBCF Improvement Mod %s has been released!\n",
			newVersionNum.c_str());

		WindowManager::GetInstance().GetWindowContainer()->GetWindow(WindowType_UpdateNotifier)->Open();
	}
	else
	{
		g_imGuiLogger->Log("[system] BBCF Improvement Mod is up-to-date\n");
	}
}

void StartAsyncUpdateCheck()
{
	if (MOD_FORCE_DISABLE_UPDATE_CHECK)
		return;

	if (Settings::settingsIni.checkupdates)
	{
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)CheckUpdate, nullptr, 0, nullptr));
	}
}
