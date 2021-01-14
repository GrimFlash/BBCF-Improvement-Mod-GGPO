#include "url_downloader.h"

#include "url_downloader.h"

#include "Core/utils.h"
#include "Overlay/Logger/ImGuiLogger.h"

#include <wininet.h>

#pragma comment(lib,"wininet.lib")

std::string DownloadUrl(std::wstring& wUrl)
{
	std::string url(wUrl.begin(), wUrl.end());

	HINTERNET connect = InternetOpen(L"MyBrowser", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	if (!connect)
	{
		g_imGuiLogger->Log("[error] DownloadUrl failed. Connection Failed or Syntax error with URL\n'%s'\n", url.c_str());
		return "";
	}

	HINTERNET OpenAddress = InternetOpenUrl(connect, wUrl.c_str(), NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION, 0);

	if (!OpenAddress)
	{
		DWORD ErrorNum = GetLastError();
		g_imGuiLogger->Log("[error] DownloadUrl failed. Failed to open URL\n'%s'\ncode: %d\n", url.c_str(), ErrorNum);
		InternetCloseHandle(connect);
		return "";
	}

	std::string receivedData = "";

	char DataReceived[4096];
	DWORD NumberOfBytesRead = 0;
	while (InternetReadFile(OpenAddress, DataReceived, 4096, &NumberOfBytesRead) && NumberOfBytesRead)
	{
		receivedData.append(DataReceived, NumberOfBytesRead);
	}

	InternetCloseHandle(OpenAddress);
	InternetCloseHandle(connect);

	return receivedData;
}

unsigned long DownloadUrlBinary(std::wstring& wUrl, void** outBuffer)
{
	std::string url(wUrl.begin(), wUrl.end());

	HINTERNET connect = InternetOpen(L"MyBrowser", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	if (!connect)
	{
		g_imGuiLogger->Log(
			"[error] DownloadUrlBinary failed. Connection Failed or Syntax error with URL\n'%s'\n",
			url.c_str()
		);
		return 0;
	}

	HINTERNET OpenAddress = InternetOpenUrl(connect, wUrl.c_str(), NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION, 0);

	if (!OpenAddress)
	{
		DWORD ErrorNum = GetLastError();
		g_imGuiLogger->Log(
			"[error] DownloadUrlBinary failed. Failed to open URL\n'%s'\ncode: %d\n",
			url.c_str(), ErrorNum
		);
		InternetCloseHandle(connect);
		return 0;
	}

	DWORD numberOfBytesRead = 0;
	DWORD returnedBytesRead = 0;
	bool result = false;
	do
	{
		char buffer[2000];
		result = InternetReadFile(OpenAddress, buffer, sizeof(buffer), &numberOfBytesRead);

		// Re-allocate memory
		char* tempData = new char[returnedBytesRead + numberOfBytesRead];
		memcpy(tempData, *outBuffer, returnedBytesRead);
		memcpy(tempData + returnedBytesRead, buffer, numberOfBytesRead);
		SAFE_DELETE_ARRAY(*outBuffer);
		*outBuffer = tempData;

		returnedBytesRead += numberOfBytesRead;

	} while (result && numberOfBytesRead);

	InternetCloseHandle(OpenAddress);
	InternetCloseHandle(connect);

	return returnedBytesRead;
}
