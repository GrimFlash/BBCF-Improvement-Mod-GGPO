//#include 
#include "../include/update_check.h"
#include <WinHttpClient.h>
#include <regex>
#include "../include/ImGui/ImGuiSystem.h"

//10 seconds timout
#define TIMEOUT 10000

std::string newVersionNum = "";
std::string ingamePlayersNum = "<No data>";

std::string GetNewVersionNum()
{
	if (newVersionNum != "")
	{
		return newVersionNum;
	}
	else
		return "";
}

void CheckUpdate()
{
	LOG(2, "CheckUpdate\n");
	//////////////////////////////////
	// Set URL.
	WinHttpClient client(L"http://steamcommunity.com/app/586140/discussions/0/1291817208497395528/");
	client.SetTimeouts(TIMEOUT, TIMEOUT, TIMEOUT, TIMEOUT);

	// Send http request, a GET request by default.
	client.SendHttpRequest();
	
	// The response header.
	//wstring httpResponseHeader = client.GetResponseHeader();

	// The response content.
	wstring httpResponseContent = client.GetResponseContent();

	//we only need the top part of head that contains the title
	//const char* msg = (char *)_bstr_t(httpResponseContent.substr(0, 950).c_str());
	std::string str = _bstr_t(httpResponseContent.substr(0, 950).c_str());
	
	//look for vx.xx , returning like v1.23 for example
	std::regex r("<title>.+(v\\d\.\\d\\d)"); // entire match will be 2 numbers
	std::smatch m;
	std::regex_search(str, m, r);

	//ImGuiSystem::AddLog("on site: ");
	//ImGuiSystem::AddLog(m[1].str().c_str());
	//ImGuiSystem::AddLog("\n");
	//ImGuiSystem::AddLog("installed: ");
	//ImGuiSystem::AddLog(version_num);
	
	if (m[1].str() == "")
	{
		ImGuiSystem::AddLog("[error] Update check failed.\n");
		return;
	}

	if (strcmp(m[1].str().c_str(), version_num) != 0)
	{
		newVersionNum = m[1].str();
		LOG(2, "New version found: %s\n", newVersionNum.c_str());
		ImGuiSystem::AddLog("[system] Update available: BBCF Improvement Mod %s has been released!\n", newVersionNum.c_str());
		ImGuiSystem::IsUpdateAvailable = true;
	}
	else
	{
		ImGuiSystem::AddLog("[system] BBCF Improvement Mod is up-to-date\n");
	}
}

void FetchTotalIngamePlayers()
{
	WinHttpClient client(L"https://steamdb.info/app/586140/");
	client.SetTimeouts(TIMEOUT, TIMEOUT, TIMEOUT, TIMEOUT);

	client.SendHttpRequest();
	
	wstring httpResponseContent = client.GetResponseContent();

	std::string str = _bstr_t(httpResponseContent.substr(12000, 18000).c_str());

	std::regex r("><\\/span>\\s(\\d.*)\\sIn-Game");
	std::smatch m;
	std::regex_search(str, m, r);

	//if(m[1].str() != "")
	ingamePlayersNum = m[1].str();

	LOG(2, "%s ingame players\n", ingamePlayersNum.c_str());
	ImGuiSystem::AddLog("[system] Total players in-game: %s\n", ingamePlayersNum.c_str());
}

std::string GetIngamePlayersNum()
{
	return ingamePlayersNum;
}