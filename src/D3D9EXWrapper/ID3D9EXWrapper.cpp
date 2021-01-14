#include "ID3D9EXWrapper.h"

#include "ID3D9EXWrapper_Device.h"

#include "Core/logger.h"

Direct3D9ExWrapper::Direct3D9ExWrapper(IDirect3D9Ex **ppIDirect3D9Ex)
{
	LOG(1, "cDirect3D9ExWrapper\n");

	Direct3D9Ex = *ppIDirect3D9Ex;
	*ppIDirect3D9Ex = this;
}

Direct3D9ExWrapper::~Direct3D9ExWrapper() {}

HRESULT APIENTRY Direct3D9ExWrapper::RegisterSoftwareDevice(void* pInitializeFunction)
{
	return Direct3D9Ex->RegisterSoftwareDevice(pInitializeFunction);
}

HRESULT APIENTRY Direct3D9ExWrapper::QueryInterface(const IID &riid, void **ppvObj)
{
	HRESULT hRes = Direct3D9Ex->QueryInterface(riid, ppvObj);

	if (hRes == S_OK)
		*ppvObj = this;
	else
		*ppvObj = NULL;

	return hRes;
}

ULONG APIENTRY Direct3D9ExWrapper::AddRef()
{
	return Direct3D9Ex->AddRef();
}

ULONG APIENTRY Direct3D9ExWrapper::Release()
{
	ULONG res = Direct3D9Ex->Release();

	if (res == 0)
		delete this;

	return res;
}

UINT APIENTRY Direct3D9ExWrapper::GetAdapterCount()
{
	return Direct3D9Ex->GetAdapterCount();
}

HRESULT APIENTRY Direct3D9ExWrapper::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier)
{
	return Direct3D9Ex->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}

UINT APIENTRY Direct3D9ExWrapper::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format)
{
	return Direct3D9Ex->GetAdapterModeCount(Adapter, Format);
}

HRESULT APIENTRY Direct3D9ExWrapper::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode)
{
	//LOG(1, "EnumAdapterModes\n");
	HRESULT ret = Direct3D9Ex->EnumAdapterModes(Adapter, Format, Mode, pMode);
	//LOG(1, "\t- Adapter: %u\n", Adapter);
	//LOG(1, "\t- Format: %d\n", Format);
	//LOG(1, "\t- Mode: %u\n", Mode);
	//LOG(1, "\t- ReturnFormat: %d\n", pMode->Format);
	//LOG(1, "\t- ReturnWidth: %u\n", pMode->Width);
	//LOG(1, "\t- ReturnHeight: %d\n", pMode->Height);
	//LOG(1, "\t- ReturnRefreshRate: %u\n", pMode->RefreshRate);
	//Settings::savedSettings.adapterRefreshRate = pMode->RefreshRate;
	return ret;
}

HRESULT APIENTRY Direct3D9ExWrapper::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode)
{
	return Direct3D9Ex->GetAdapterDisplayMode(Adapter, pMode);
}

HRESULT APIENTRY Direct3D9ExWrapper::CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed)
{
	return Direct3D9Ex->CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed);
}

HRESULT APIENTRY Direct3D9ExWrapper::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
{
	return Direct3D9Ex->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
}

HRESULT APIENTRY Direct3D9ExWrapper::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels)
{
	return Direct3D9Ex->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
}

HRESULT APIENTRY Direct3D9ExWrapper::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat)
{
	return Direct3D9Ex->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
}

HRESULT APIENTRY Direct3D9ExWrapper::CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat)
{
	return Direct3D9Ex->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
}

HRESULT APIENTRY Direct3D9ExWrapper::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps)
{
	return Direct3D9Ex->GetDeviceCaps(Adapter, DeviceType, pCaps);
}

HMONITOR APIENTRY Direct3D9ExWrapper::GetAdapterMonitor(UINT Adapter)
{
	return Direct3D9Ex->GetAdapterMonitor(Adapter);
}

HRESULT APIENTRY Direct3D9ExWrapper::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DDevice9 **ppReturnedDeviceInterface)
{
	LOG(1, "CreateDevice\n")

	HRESULT hRet = Direct3D9Ex->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

	if (SUCCEEDED(hRet))
	{
		Settings::applySettingsIni(pPresentationParameters);
	}

	return hRet;
}

UINT APIENTRY Direct3D9ExWrapper::GetAdapterModeCountEx(UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter)
{
	return Direct3D9Ex->GetAdapterModeCountEx(Adapter, pFilter);
}

HRESULT APIENTRY Direct3D9ExWrapper::EnumAdapterModesEx(UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode)
{
	return Direct3D9Ex->EnumAdapterModesEx(Adapter, pFilter, Mode, pMode);
}

HRESULT APIENTRY Direct3D9ExWrapper::GetAdapterDisplayModeEx(UINT Adapter, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation)
{
	return Direct3D9Ex->GetAdapterDisplayModeEx(Adapter, pMode, pRotation);
}

HRESULT APIENTRY Direct3D9ExWrapper::CreateDeviceEx(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode, IDirect3DDevice9Ex** ppReturnedDeviceInterface)
{
	LOG(1, "CreateDeviceEx:\n");
	LOG(1, "\tAdapter: %d\n\tDeviceType: %d\n\thFocusWindow: 0x%p\n\tBehaviorFlags: 0x%p\n",
		Adapter, DeviceType, hFocusWindow, BehaviorFlags);

	logD3DPParams(pPresentationParameters, true);

	HRESULT hRet = Direct3D9Ex->CreateDeviceEx(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface);

	if (SUCCEEDED(hRet))
	{
		LOG(1, "CreateDeviceEx created with original PresentationParameters\n");
		Settings::applySettingsIni(pPresentationParameters);
		logD3DPParams(pPresentationParameters, false);
		Direct3DDevice9ExWrapper *ret = new Direct3DDevice9ExWrapper(ppReturnedDeviceInterface, pPresentationParameters, this);
	}
	else
	{
		LOG(1, "CreateDeviceEx failed to be created with original PresentationParameters\n");
	}

	return hRet;
}

HRESULT APIENTRY Direct3D9ExWrapper::GetAdapterLUID(UINT Adapter, LUID * pLUID)
{
	return Direct3D9Ex->GetAdapterLUID(Adapter, pLUID);
}