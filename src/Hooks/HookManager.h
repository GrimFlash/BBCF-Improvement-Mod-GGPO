#pragma once
#include <string>
#include <vector>
#include <Windows.h>

#define MAX_LENGTH 32

typedef DWORD JMPBACKADDR;

struct functionhook_t
{
	std::string label;
	std::string pattern;
	std::string mask;
	int length; //number of bytes to replace
	DWORD startAddress; //address is 0 if signature scanning was unsuccessful
	DWORD jmpBackAddr;
	void* newFunc;
	char originalBytes[MAX_LENGTH];
	bool activated; //is the hook in effect
};

class HookManager
{
public:
	static JMPBACKADDR SetHook(const char* label, const char* pattern, const char* mask, const int length, void* newFunc, bool activate = true); //returns 0 if hook failed
	static bool SetHook(const char* label, void* newFunc, bool activate = true);
	static bool IsHookActivated(const char* label);
	static bool ActivateHook(const char* label); //0 hook not found, 1 success
	static bool DeactivateHook(const char* label); // 0 hook not found, 1 success
	static JMPBACKADDR GetJmpBackAddr(const char* label); /* do not call this whenever you want to jump back
														  searching through the array each time is bad for performance,
														  use this func ONCE to store the address in a variable*/
	static bool SetJmpBackAddr(const char* label, DWORD newJmpBackAddr);
	static DWORD GetStartAddress(const char* label);
	static JMPBACKADDR RegisterHook(const char* label, const char* pattern, const char* mask, const int len);
	static int GetOriginalBytes(const char* label, int startIndex, int bytesToReturn);
	static int GetBytesFromAddr(const char* label, int startIndex, int bytesToReturn);
	static int OverWriteBytes(void* startAddress, void* endAddress, const char* pattern, const char* mask, const char* newBytes);
	static void Cleanup(); //empty atm
private:
	static std::vector<functionhook_t> hooks; //stores hook structs
	static int GetHookStructIndex(const char* label); //returns the index of hook struct
	static bool SaveOriginalBytes(int hookIndex, void* startAddress, int len);
	static bool PlaceHook(void* toHook, void* ourFunc, int len);
	static bool RestoreOriginalBytes(int functionhook_index);
	static DWORD FindPattern(const char* pattern, const char* mask);
};