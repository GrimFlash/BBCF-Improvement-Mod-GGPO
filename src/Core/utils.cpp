#include "utils.h"

#include <cstdio>
#include <fstream>
#include <Psapi.h>

void WriteToProtectedMemory(uintptr_t addressToWrite, char* valueToWrite, int byteNum)
{
	//used to change our file access type, stores the old
	//access type and restores it after memory is written
	unsigned long OldProtection;
	//give that address read and write permissions and store the old permissions at oldProtection
	VirtualProtect((LPVOID)(addressToWrite), byteNum, PAGE_EXECUTE_READWRITE, &OldProtection);

	//write the memory into the program and overwrite previous value
	memcpy((LPVOID)addressToWrite, valueToWrite, byteNum);

	//reset the permissions of the address back to oldProtection after writting memory
	VirtualProtect((LPVOID)(addressToWrite), byteNum, OldProtection, NULL);
}

char* RawMemoryArrayToString(unsigned char* srcBuf, int length)
{
	static char output[2000];
	//maybe should memset 0 the output array
	//memset(output, 0, 2000);
	char *ptr = &output[0];
	int i = 0;
	for (; i < length; i++)
	{
		ptr += sprintf(ptr, "%02X ", srcBuf[i]);
	}
	return output;
}

//Get all module related info, this will include the base DLL. 
//and the size of the module
MODULEINFO GetModuleInfo(LPCWSTR szModule)
{
	MODULEINFO modinfo = { 0 };
	HMODULE hModule = GetModuleHandle(szModule);
	if (hModule == 0)
		return modinfo;
	GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));
	return modinfo;
}

DWORD FindPattern(LPCWSTR module, char *pattern, char *mask)
{
	//Get all module related information
	MODULEINFO mInfo = GetModuleInfo(module);

	//Assign our base and module size
	//Having the values right is ESSENTIAL, this makes sure
	//that we don't scan unwanted memory and leading our game to crash
	DWORD base = (DWORD)mInfo.lpBaseOfDll;
	DWORD size = (DWORD)mInfo.SizeOfImage;

	//Get length for our mask, this will allow us to loop through our array
	DWORD patternLength = (DWORD)strlen(mask);

	for (DWORD i = 0; i < size - patternLength; i++)
	{
		bool found = true;
		for (DWORD j = 0; j < patternLength; j++)
		{
			//if we have a ? in our mask then we have true by default, 
			//or if the bytes match then we keep searching until finding it or not
			found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
		}
		//found = true, our entire pattern was found
		//return the memory addy so we can write to it
		if (found)
		{
			return base + i;
		}
	}
	return NULL;
}

//use GetInterfaceFuncPtr like this:
//HMODULE hM2 = GetModuleHandleA("steam_api.dll");
//PBYTE pSteamAPI_Init = (PBYTE)GetProcAddress(hM2, "SteamAPI_Init");
//orig_SteamAPI_Init = (SteamAPI_Init_t)DetourFunction(pSteamAPI_Init, (LPBYTE)hook_SteamAPI_Init);
DWORD* GetInterfaceFuncPtr(DWORD* pDeviceInterface, const char *fmt, ...)
{
	va_list	va_alist;
	char	buf[32];

	memset(buf, 0, sizeof(buf));

	va_start(va_alist, fmt);
	_vsnprintf_s(buf, sizeof(buf), fmt, va_alist);
	va_end(va_alist);

	char *op = (char *)strtoul(buf, NULL, 16);

	while (1)
	{
		if (*op == '\xFF')
		{
			op++;

			if (*op == '\xA0')
			{
				int iIndex = 0;

				op++;
				memcpy(&iIndex, op, (4));
				return (DWORD*)(*pDeviceInterface + iIndex);
			}

			op++;
			break;
		}
		op++;
	}

	if (((int)*op) < 0)
		return (DWORD*)*pDeviceInterface;

	return (DWORD*)(*pDeviceInterface + ((int)*op)); //Credits: as2 (G-D) 
}

//need at least 5 bytes to hook!
//usage:
//int hookLength = xxx;
//DWORD hookAddr = 0x23435;
//Hook((void)hookAddr, ourFunc, hookLength);
//
//DWORD jmpBackAddr = hookAddress + hookLength;
//void __declspec(naked) ourFunc()
//{
//	__asm
//	{
//		//dostuffs here
//		//...
//		//original opcodes here
//		jmp[jmpBackAddr]
//	}
//}
bool Hook(void* toHook, void* ourFunc, int len)
{
	if (len < 5)
	{
		return false;
	}

	DWORD curProtection;
	VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);

	memset(toHook, 0x90, len);

	DWORD relativeAddress = ((DWORD)ourFunc - (DWORD)toHook) - 5;

	*(BYTE*)toHook = 0xE9;
	*(DWORD*)((DWORD)toHook + 1) = relativeAddress;

	DWORD temp;
	VirtualProtect(toHook, len, curProtection, &temp);

	return true;
}

std::string FormatText(const char* message, ...)
{
	if (!message) { return std::string(""); }

	char buf[1000];
	va_list args;
	va_start(args, message);
	
	vsprintf(buf, message, args);
	va_end(args);

	return std::string(buf);
}


//source: https://stackoverflow.com/questions/40629345/fill-array-dynamicly-with-gradient-color-c
unsigned int rgb(double hue)
{
	int h = int(hue * 256 * 6);
	int x = h % 0x100;

	int r = 0, g = 0, b = 0;
	switch (h / 256)
	{
	case 0: r = 255; g = x;       break;
	case 1: g = 255; r = 255 - x; break;
	case 2: g = 255; b = x;       break;
	case 3: b = 255; g = 255 - x; break;
	case 4: b = 255; r = x;       break;
	case 5: r = 255; b = 255 - x; break;
	}

	return r + (g << 8) + (b << 16);
}

DWORD QuickChecksum(DWORD *pData, int size)
{
	if (!pData) { return 0x0; }

	DWORD sum;
	DWORD tmp;
	sum = *pData;

	for (int i = 1; i < (size / 4); i++)
	{
		tmp = pData[i];
		tmp = (DWORD)(sum >> 29) + tmp;
		tmp = (DWORD)(sum >> 17) + tmp;
		sum = (DWORD)(sum << 3) ^ tmp;
	}

	return sum;
}

bool utils_WriteFile(const char* path, void* inBuffer, unsigned long bufferSize, bool binaryFile, bool append)
{
	std::ofstream file;

	int openMode = 2;

	if (binaryFile)
		openMode = std::fstream::binary;
	if (append)
		openMode |= std::fstream::ate;

	file.open(path, openMode);

	if (!file.is_open())
	{
		return false;
	}

	file.write((char*)inBuffer, bufferSize);
	file.close();

	return true;
}

bool utils_ReadFile(const char* path, void* outBuffer, unsigned long bufferSize, bool binaryFile)
{
	std::ifstream file;

	int openMode = 1;

	if (binaryFile)
		openMode = std::fstream::binary;

	file.open(path, openMode);

	if (!file.is_open())
	{
		return false;
	}

	file.read((char*)outBuffer, bufferSize);
	file.close();

	return true;
}

std::string utf16_to_utf8(const std::wstring& wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

int SafeDereferencePtr(int* ptr)
{
	if (ptr != nullptr)
		return *ptr;

	return 0;
}
