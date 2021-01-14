#include "HookManager.h"

#include "Core/logger.h"

#include <Psapi.h>

std::vector<functionhook_t> HookManager::hooks;

JMPBACKADDR HookManager::SetHook(const char* label, const char* pattern, const char* mask,
	const int len, void* newFunc, bool activate)
{
	if (len > MAX_LENGTH)
	{
		LOG(2, "Overwritten bytes more than %d (%d)! \n", MAX_LENGTH, len);
		return 0;
	}

	//check if there is already a hook registered with same label
	int index = GetHookStructIndex(label);
	if (index != -1)
	{
		LOG(2, "%s hook already present!\n", label);
		return hooks[index].jmpBackAddr;
	}

	hooks.push_back(functionhook_t{});
	index = hooks.size() - 1;
	hooks[index].label = label;
	hooks[index].pattern = pattern;
	hooks[index].mask = mask;
	hooks[index].length = len;
	hooks[index].newFunc = newFunc;

	DWORD startAddress = FindPattern(pattern, mask);
	hooks[index].startAddress = startAddress;

	if (!startAddress)
	{
		LOG(2, "%s signature scanning returned 0\n", label);
		return 0;
	}

	LOG(2, "%s found at: 0x%p\n", label, startAddress);

	if (!SaveOriginalBytes(index, (void*)startAddress, len))
	{
		LOG(2, "Saving original bytes failed.\n");
		return 0;
	}

	DWORD jmpBackAddr = startAddress + len;
	hooks[index].jmpBackAddr = jmpBackAddr;

	if (activate)
	{
		if (!PlaceHook((void*)startAddress, newFunc, len))
		{
			LOG(2, "%s hook failed.\n", label);
			return 0;
		}
		hooks[index].activated = true;
		LOG(2, "Hook set on %s\n", label);
	}

	return jmpBackAddr;
}

//sets new hooked address to an existing hook struct
bool HookManager::SetHook(const char* label, void* newFunc, bool activate)
{
	int index = GetHookStructIndex(label);
	if (index == -1)
	{
		LOG(2, "%s hook already present!\n", label);
		return false;
	}

	hooks.push_back(functionhook_t());
	hooks[index].newFunc = newFunc;

	if (activate)
	{
		if (!PlaceHook((void*)hooks[index].startAddress, newFunc, hooks[index].length))
		{
			LOG(2, "%s hook failed.\n", label);
			return false;
		}
		hooks[index].activated = true;
		LOG(2, "Hook set on %s\n", label);
	}

	return true;
}

bool HookManager::SaveOriginalBytes(int index, void* startAddress, int len)
{
	DWORD curProtection;
	if (!VirtualProtect(startAddress, len, PAGE_EXECUTE_READWRITE, &curProtection))
		return false;

	for (int i = 0; i < len; i++)
	{
		hooks[index].originalBytes[i] = *(BYTE*)((DWORD)startAddress + i);
	}

	DWORD temp;
	if (!VirtualProtect(startAddress, len, curProtection, &temp))
		return false;

	return true;
}

bool HookManager::IsHookActivated(const char* label)
{
	int index = GetHookStructIndex(label);
	if (index == -1)
	{
		return 0;
	}

	return hooks[index].activated;
}

bool HookManager::ActivateHook(const char* label)
{
	LOG(2, "Activating %s hook.\n", label);
	int index = GetHookStructIndex(label);
	if (index == -1)
	{
		LOG(2, "%s hook not found!\n", label);
		return false;
	}

	//if its already activated then dont do anything
	if (hooks[index].activated)
		return true;

	if (!PlaceHook((void*)hooks[index].startAddress, hooks[index].newFunc, hooks[index].length))
	{
		LOG(2, "%s hook failed.\n", label);
		return false;
	}

	hooks[index].activated = true;
	return true;
}

bool HookManager::DeactivateHook(const char* label)
{
	LOG(2, "Deactivating %s hook.\n", label);
	int index = GetHookStructIndex(label);
	if (index == -1)
	{
		LOG(2, "%s hook not found!\n", label);
		return false;
	}

	if (!hooks[index].activated) // already deactivated
		return true;

	int ret = RestoreOriginalBytes(index);
	if (!ret)
		LOG(2, "RestoreOriginalBytes failed.\n");

	hooks[index].activated = false;
	return ret;
}

JMPBACKADDR HookManager::GetJmpBackAddr(const char* label)
{
	int index = GetHookStructIndex(label);
	if (index == -1)
	{
		LOG(2, "%s hook not found!\n", label);
		return 0;
	}
	return hooks[index].jmpBackAddr;
}

bool HookManager::SetJmpBackAddr(const char* label, DWORD newJmpBackAddr)
{
	int index = GetHookStructIndex(label);
	if (index == -1)
	{
		LOG(2, "SetJmpBackAddr: %s hook not found!\n", label);
		return false;
	}

	hooks[index].jmpBackAddr = newJmpBackAddr;
	hooks[index].startAddress = newJmpBackAddr - hooks[index].length;

	int startAddress = hooks[index].startAddress;
	int len = hooks[index].length;

	if (!SaveOriginalBytes(index, (void*)startAddress, len))
	{
		LOG(2, "Saving original bytes failed.\n");
		return false;
	}

	return true;
}

DWORD HookManager::GetStartAddress(const char* label)
{
	int index = GetHookStructIndex(label);
	if (index == -1)
	{
		LOG(2, "SetJmpBackAddr: %s hook not found!\n", label);
		return 0;
	}

	return hooks[index].startAddress;
}

//registering a new hook struct without hooking
JMPBACKADDR HookManager::RegisterHook(const char* label, const char* pattern, const char* mask, const int len)
{
	int index = GetHookStructIndex(label);
	if (index != -1)
	{
		LOG(2, "%s hook already present!\n", label);
		return hooks[index].jmpBackAddr;
	}

	hooks.push_back(functionhook_t());
	index = hooks.size() - 1;
	hooks[index].label = label;
	hooks[index].pattern = pattern;
	hooks[index].mask = mask;
	hooks[index].length = len;
	hooks[index].newFunc = 0;

	DWORD startAddress = FindPattern(pattern, mask);
	hooks[index].startAddress = startAddress;

	if (!startAddress)
	{
		LOG(2, "%s signature scanning returned 0\n", label);
		return 0;
	}

	LOG(2, "%s found at: 0x%p\n", label, startAddress);

	return startAddress;
}

//startIndexOfWildCard = set to the wildcard's starting index
//bytesToReturn = 1/2/4
int HookManager::GetOriginalBytes(const char* label, int startIndex, int bytesToReturn)
{
	int index = GetHookStructIndex(label);
	if (index == -1)
	{
		LOG(2, "%s hook not found!\n", label);
		return 0;
	}

	void* needle = (char*)hooks[index].originalBytes + (char)startIndex;

	switch (bytesToReturn)
	{
	case 1:
		return *(unsigned char*)needle;
	case 2:
		return *(unsigned short*)needle;
	case 4:
		return *(unsigned int*)needle;
	default:
		return 0;
	}
}

//startIndexOfWildCard = set to the wildcard's starting index
//bytesToReturn = 1/2/4
int HookManager::GetBytesFromAddr(const char* label, int startIndex, int bytesToReturn)
{
	int index = GetHookStructIndex(label);
	if (index == -1)
	{
		LOG(2, "%s hook not found!\n", label);
		return 0;
	}

	void* needle = (char*)hooks[index].startAddress + (char)startIndex;

	switch (bytesToReturn)
	{
	case 1:
		return *(unsigned char*)needle;
	case 2:
		return *(unsigned short*)needle;
	case 4:
		return *(unsigned int*)needle;
	default:
		return 0;
	}
}

void HookManager::Cleanup()
{
	//
}

int HookManager::GetHookStructIndex(const char* label)
{
	for (unsigned int i = 0; i < hooks.size(); i++)
	{
		if (strcmp(hooks[i].label.c_str(), label) == 0)
			return i;
	}
	return -1;
}

bool HookManager::RestoreOriginalBytes(int index)
{
	DWORD curProtection;
	if (!VirtualProtect((void*)hooks[index].startAddress, hooks[index].length, PAGE_EXECUTE_READWRITE, &curProtection))
		return false;

	for (int i = 0; i < hooks[index].length; i++)
	{
		*((BYTE*)(hooks[index].startAddress + i)) = hooks[index].originalBytes[i];
	}

	DWORD temp;
	if (!VirtualProtect((void*)hooks[index].startAddress, hooks[index].length, curProtection, &temp))
		return false;

	return true;
}

bool HookManager::PlaceHook(void* toHook, void* ourFunc, int len)
{
	if (len < 5 || len > MAX_LENGTH)
	{
		return false;
	}

	DWORD curProtection;
	if (!VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection))
		return false;

	memset(toHook, 0x90, len);

	DWORD relativeAddress = ((DWORD)ourFunc - (DWORD)toHook) - 5;

	*(BYTE*)toHook = 0xE9;
	*(DWORD*)((DWORD)toHook + 1) = relativeAddress;

	DWORD temp;
	if (!VirtualProtect(toHook, len, curProtection, &temp))
		return false;

	return true;
}

int HookManager::OverWriteBytes(void* startAddress, void* endAddress, const char* pattern,
	const char* mask, const char* newBytes)
{
	int overwrittenCount = 0;

	DWORD base = (DWORD)startAddress;
	DWORD size = (DWORD)endAddress - (DWORD)startAddress;

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
			char* toOverwrite = (char*)base + i;

			DWORD curProtection;
			if (!VirtualProtect(toOverwrite, patternLength, PAGE_EXECUTE_READWRITE, &curProtection))
				return overwrittenCount;

			for (int k = 0; k < patternLength; k++)
			{
				*((BYTE*)(toOverwrite + k)) = newBytes[k];
			}

			overwrittenCount++;

			DWORD temp;
			if (!VirtualProtect(toOverwrite, patternLength, curProtection, &temp))
				return overwrittenCount;
		}
	}
	return overwrittenCount;
}

DWORD HookManager::FindPattern(const char* pattern, const char* mask)
{
	////////
	//Get all module related information
	//Get process name
	TCHAR szFileName[MAX_PATH + 1];
	GetModuleFileName(NULL, szFileName, MAX_PATH + 1);

	MODULEINFO modinfo = { 0 };
	HMODULE hModule = GetModuleHandle(szFileName);
	if (hModule == 0)
		return 0;
	GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));
	////////

	//Assign our base and module size
	//Having the values right is ESSENTIAL, this makes sure
	//that we don't scan unwanted memory and leading our game to crash
	DWORD base = (DWORD)modinfo.lpBaseOfDll;
	DWORD size = (DWORD)modinfo.SizeOfImage;

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