#include "../include/bbcf_im.h"
#include <detours.h>
#include "../include/D3D9ExWrapper/ID3D9EXWrapper.h"
#include "../include/additional_hooks.h"
#include "../include/ImGui/ImGuiSystem.h"
#include "../include/containers.h"
#include "../include/custom_palette.h"
#include "../include/crashdump.h"

// The original to call
typedef HRESULT(__stdcall *Direct3DCreate9Ex_t)(UINT SDKVersion, IDirect3D9Ex**);
Direct3DCreate9Ex_t orig_Direct3DCreate9Ex;

HRESULT __stdcall hook_Direct3DCreate9Ex(UINT sdkVers, IDirect3D9Ex** pD3DEx)
{
	LOG(1, "Direct3DCreate9EX pD3DEx: 0x%x\n", pD3DEx);
	HRESULT retval = orig_Direct3DCreate9Ex(sdkVers, pD3DEx); // real one

	Direct3D9ExWrapper *ret = new Direct3D9ExWrapper(&*pD3DEx);
	return retval;
}

void BBCF_IM_Start()
{
	if (Settings::loadSettingsIni())
	{
		logSettingsIni();
		Settings::initSavedSettings();
		Containers::Init();

		HMODULE hM = GetModuleHandleA("d3d9.dll");
		PBYTE pDirect3DCreate9Ex = (PBYTE)GetProcAddress(hM, "Direct3DCreate9Ex");
		checkHookSuccess((PBYTE)pDirect3DCreate9Ex, "Direct3DCreate9Ex");
		orig_Direct3DCreate9Ex = (Direct3DCreate9Ex_t)DetourFunction(pDirect3DCreate9Ex, (LPBYTE)hook_Direct3DCreate9Ex);

		if (!additional_hooks_detours())
		{
			MessageBoxA(NULL, "FAILED HOOKS", "BBCFIM", MB_OK);
			ExitProcess(0);
		}

		CreateCustomFolders();
		SetUnhandledExceptionFilter(UnhandledExFilter);
	}
}

void BBCF_IM_Shutdown()
{
	ImGuiSystem::WriteLogToFile();

	ImGuiSystem::Shutdown();

	Containers::Cleanup();

	closeLogger();
}