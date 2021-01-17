#pragma once
#include <string>

std::string DownloadUrl(std::wstring& wUrl);

// Returns number of bytes read
unsigned long DownloadUrlBinary(std::wstring& wUrl, void** outBuffer);
