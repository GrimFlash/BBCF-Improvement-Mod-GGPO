#include "../include/bbcf_im.h"

typedef HRESULT(WINAPI *DirectInput8Create_t)(HINSTANCE inst_handle, DWORD version, const IID& r_iid, LPVOID* out_wrapper, LPUNKNOWN p_unk);
DirectInput8Create_t orig_DirectInput8Create;

HRESULT WINAPI DirectInput8Create(HINSTANCE inst_handle, DWORD version, const IID& r_iid, LPVOID* out_wrapper, LPUNKNOWN p_unk)
{
	LOG(1, "DirectInput8Create\n");
	return orig_DirectInput8Create(inst_handle, version, r_iid, out_wrapper, p_unk);
}

void Init(HANDLE hModule)
{
	openLogger();
	LOG(1, "Init\n");
	HMODULE hMod;
	char dllPath[MAX_PATH];

	GetSystemDirectoryA(dllPath, MAX_PATH);
	strcat_s(dllPath, "\\dinput8.dll");
	hMod = LoadLibraryA(dllPath);
	orig_DirectInput8Create = (DirectInput8Create_t)GetProcAddress(hMod, "DirectInput8Create");

	if (hMod != INVALID_HANDLE_VALUE && orig_DirectInput8Create)
	{
		LOG(1, "orig_DirectInput8Create: 0x%p\n", orig_DirectInput8Create);
		LOG(1, "Starting D3DHook_Start thread\n");
		HANDLE thread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)BBCF_IM_Start, hModule, NULL, NULL);
		CloseHandle(thread);
	}
	else
	{
		MessageBoxA(NULL, "Could not load original dinput8.dll!", "BBCFIM", MB_OK);
		ExitProcess(0);
	}
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		Init(hModule);
		break;

	case DLL_PROCESS_DETACH:
		BBCF_IM_Shutdown();
		break;
	}
	return TRUE;
}