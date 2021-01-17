#include "ID3D9EXWrapper_Device.h"

#include "Core/interfaces.h"
#include "Core/logger.h"
#include "Game/MatchState.h"
#include "Hooks/hooks_bbcf.h"
#include "Hooks/hooks_customGameModes.h"
#include "Hooks/hooks_palette.h"
#include "Overlay/WindowManager.h"

#include <steam_api.h>

#pragma comment(lib, "steam_api.lib")

Direct3DDevice9ExWrapper::Direct3DDevice9ExWrapper(IDirect3DDevice9Ex **ppReturnedDeviceInterface, D3DPRESENT_PARAMETERS *pPresentParam, IDirect3D9Ex *pIDirect3D9Ex)
{
	LOG(1, "cDirect3DDevice9ExWrapper with modified PresentationParameters\n");

	m_Direct3DDevice9Ex = *ppReturnedDeviceInterface;
	*ppReturnedDeviceInterface = this;
	m_Direct3D9Ex = pIDirect3D9Ex;

	g_interfaces.pD3D9ExWrapper = *ppReturnedDeviceInterface;

	//place all other hooks that can only be placed after steamDRM unpacks the .exe in memory!!!
	placeHooks_bbcf();
	placeHooks_palette();
	placeHooks_CustomGameModes();
}

Direct3DDevice9ExWrapper::~Direct3DDevice9ExWrapper() {}

HRESULT APIENTRY Direct3DDevice9ExWrapper::QueryInterface(const IID &riid, void **ppvObj)
{
	LOG(7, "QueryInterface\n");

	HRESULT hRes = m_Direct3DDevice9Ex->QueryInterface(riid, ppvObj);

	if (hRes == S_OK)
		*ppvObj = this;
	else
		*ppvObj = NULL;

	return hRes;
}

ULONG APIENTRY Direct3DDevice9ExWrapper::AddRef()
{
	LOG(7, "AddRef\n");
	return m_Direct3DDevice9Ex->AddRef();
}

ULONG APIENTRY Direct3DDevice9ExWrapper::Release()
{
	LOG(7, "Release\n");

	ULONG res = m_Direct3DDevice9Ex->Release();

	if (res == 0)
		delete this;

	return res;
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::TestCooperativeLevel()
{
	LOG(7, "TestCooperativeLevel\n");
	return m_Direct3DDevice9Ex->TestCooperativeLevel();
}

UINT APIENTRY Direct3DDevice9ExWrapper::GetAvailableTextureMem()
{
	LOG(7, "GetAvailableTextureMem\n");
	return m_Direct3DDevice9Ex->GetAvailableTextureMem();
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::EvictManagedResources()
{
	LOG(7, "EvictManagedResources\n");
	return m_Direct3DDevice9Ex->EvictManagedResources();
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetDirect3D(IDirect3D9** ppD3D9)
{
	LOG(7, "GetDirect3D\n");

	HRESULT hRet = m_Direct3DDevice9Ex->GetDirect3D(ppD3D9);

	if (SUCCEEDED(hRet))
		*ppD3D9 = m_Direct3D9Ex;

	return hRet;
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetDeviceCaps(D3DCAPS9* pCaps)
{
	LOG(7, "GetDeviceCaps\n");
	return m_Direct3DDevice9Ex->GetDeviceCaps(pCaps);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode)
{
	LOG(7, "GetDisplayMode\n");
	return m_Direct3DDevice9Ex->GetDisplayMode(iSwapChain, pMode);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
	LOG(7, "GetCreationParameters\n");
	return m_Direct3DDevice9Ex->GetCreationParameters(pParameters);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap)
{
	LOG(7, "SetCursorProperties\n");
	return m_Direct3DDevice9Ex->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}

void APIENTRY Direct3DDevice9ExWrapper::SetCursorPosition(int X, int Y, DWORD Flags)
{
	LOG(7, "SetCursorPosition\n");
	return m_Direct3DDevice9Ex->SetCursorPosition(X, Y, Flags);
}

BOOL APIENTRY Direct3DDevice9ExWrapper::ShowCursor(BOOL bShow)
{
	LOG(7, "ShowCursor\n");
	return m_Direct3DDevice9Ex->ShowCursor(bShow);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain)
{
	LOG(7, "CreateAdditionalSwapChain\n");
	return m_Direct3DDevice9Ex->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain)
{
	LOG(7, "GetSwapChain\n");
	return m_Direct3DDevice9Ex->GetSwapChain(iSwapChain, pSwapChain);
}

UINT APIENTRY Direct3DDevice9ExWrapper::GetNumberOfSwapChains()
{
	LOG(7, "GetNumberOfSwapChains\n");
	return m_Direct3DDevice9Ex->GetNumberOfSwapChains();
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	LOG(7, "Reset\n");
	Settings::applySettingsIni(pPresentationParameters);
	return m_Direct3DDevice9Ex->Reset(pPresentationParameters);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
	LOG(7, "Present\n");
	return m_Direct3DDevice9Ex->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer)
{
	LOG(7, "GetBackBuffer\n");
	return m_Direct3DDevice9Ex->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)
{
	LOG(7, "GetRasterStatus\n");
	return m_Direct3DDevice9Ex->GetRasterStatus(iSwapChain, pRasterStatus);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetDialogBoxMode(BOOL bEnableDialogs)
{
	LOG(7, "SetDialogBoxMode\n");
	return m_Direct3DDevice9Ex->SetDialogBoxMode(bEnableDialogs);
}

void APIENTRY Direct3DDevice9ExWrapper::SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp)
{
	LOG(7, "SetGammaRamp\n");
	return m_Direct3DDevice9Ex->SetGammaRamp(iSwapChain, Flags, pRamp);
}

void APIENTRY Direct3DDevice9ExWrapper::GetGammaRamp(UINT iSwapChaiTn, D3DGAMMARAMP* pRamp)
{
	LOG(7, "GetGammaRamp\n");
	return m_Direct3DDevice9Ex->GetGammaRamp(iSwapChaiTn, pRamp);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
{
	LOG(7, "CreateTexture %u %u %u\n", Width, Height, Levels);
	return m_Direct3DDevice9Ex->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)
{
	LOG(7, "CreateVolumeTexture\n");
	return m_Direct3DDevice9Ex->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
{
	LOG(7, "CreateCubeTexture\n");
	return m_Direct3DDevice9Ex->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
{
	LOG(7, "CreateVertexBuffer\n");
	return m_Direct3DDevice9Ex->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle)
{
	LOG(7, "CreateIndexBuffer\n");
	return m_Direct3DDevice9Ex->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	LOG(1, "CreateRenderTarget\n");
	return m_Direct3DDevice9Ex->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	LOG(7, "CreateDepthStencilSurface\n");
	return m_Direct3DDevice9Ex->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint)
{
	LOG(7, "UpdateSurface\n");
	return m_Direct3DDevice9Ex->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
	LOG(7, "UpdateTexture\n");
	return m_Direct3DDevice9Ex->UpdateTexture(pSourceTexture, pDestinationTexture);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface)
{
	LOG(7, "GetRenderTargetData\n");
	return m_Direct3DDevice9Ex->GetRenderTargetData(pRenderTarget, pDestSurface);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface)
{
	LOG(7, "GetFrontBufferData\n");
	return m_Direct3DDevice9Ex->GetFrontBufferData(iSwapChain, pDestSurface);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::StretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
	LOG(7, "StretchRect\n");

	if (pSourceRect)
	{
		LOG(7, "StretchRect 0x%p : %ld %ld -- 0x%p\n", pSourceRect, pSourceRect->right, pSourceRect->bottom, pDestRect);
	}

	if (Settings::settingsIni.viewport != 1)
	{
		LOG(7, "/t- modifying to %ld %ld\n", Settings::savedSettings.newSourceRect.right, Settings::savedSettings.newSourceRect.bottom);

		return m_Direct3DDevice9Ex->StretchRect(pSourceSurface, &Settings::savedSettings.newSourceRect, pDestSurface, pDestRect, Filter);
	}

	return m_Direct3DDevice9Ex->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color)
{
	LOG(7, "ColorFill\n");
	return m_Direct3DDevice9Ex->ColorFill(pSurface, pRect, color);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	LOG(7, "CreateOffscreenPlainSurface\n");
	return m_Direct3DDevice9Ex->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
	LOG(7, "SetRenderTarget %d 0x%p\n", RenderTargetIndex, pRenderTarget);
	//void *pContainer = NULL;
	//IDirect3DTexture9 *pTexture = NULL;
	//HRESULT hr = pRenderTarget->GetContainer(IID_IDirect3DTexture9, &pContainer);
	//if (SUCCEEDED(hr) && pContainer)
	//{
	//	LOG(7, "TEXTURE ACQUIRED SUCCESSFULLY\n");
	//	pTexture = (IDirect3DTexture9 *)pContainer;
	//}
	if (Settings::settingsIni.viewport != 1)
	{
		D3DSURFACE_DESC desc;
		pRenderTarget->GetDesc(&desc);
		if (desc.Width == 1024 && desc.Height == 192)
		{
			Settings::savedSettings.isDuelFieldSprite = true;
		}
		else
		{
			Settings::savedSettings.isDuelFieldSprite = false;
		}
	}

	return m_Direct3DDevice9Ex->SetRenderTarget(RenderTargetIndex, pRenderTarget);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget)
{
	LOG(7, "GetRenderTarget\n");
	return m_Direct3DDevice9Ex->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil)
{
	LOG(7, "SetDepthStencilSurface\n");
	return m_Direct3DDevice9Ex->SetDepthStencilSurface(pNewZStencil);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface)
{
	LOG(7, "GetDepthStencilSurface\n");
	return m_Direct3DDevice9Ex->GetDepthStencilSurface(ppZStencilSurface);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::BeginScene()
{
	LOG(7, "BeginScene\n");
	return m_Direct3DDevice9Ex->BeginScene();
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::EndScene()
{
	LOG(7, "EndScene\n");

	MatchState::OnUpdate();
	WindowManager::GetInstance().Render();

	return m_Direct3DDevice9Ex->EndScene();
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	LOG(7, "Clear\n");
	return m_Direct3DDevice9Ex->Clear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
	LOG(7, "SetTransform %ld &pMatrix: 0x%p &*pMatrix: 0x%p pMatrix: 0x%p\n", State, &pMatrix, &*pMatrix, pMatrix);
	return m_Direct3DDevice9Ex->SetTransform(State, pMatrix);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
	LOG(7, "GetTransform\n");
	return m_Direct3DDevice9Ex->GetTransform(State, pMatrix);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::MultiplyTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
	LOG(7, "MultiplyTransform\n");
	return m_Direct3DDevice9Ex->MultiplyTransform(State, pMatrix);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
	LOG(7, "SetViewport 0x%p : %d %d\n", pViewport, pViewport->Width, pViewport->Height);

	if (pViewport->Width >= 1280 && Settings::settingsIni.viewport != 1) //only change the ones above native resolution. there are stuffs like shadows that use 512x512 that must not be changed
	{
		LOG(7, "MODIFYING VIEWPORT TO : %d %d\n", Settings::savedSettings.newViewport.Width, Settings::savedSettings.newViewport.Height);
		return m_Direct3DDevice9Ex->SetViewport(&Settings::savedSettings.newViewport);
	}

	return m_Direct3DDevice9Ex->SetViewport(pViewport);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetViewport(D3DVIEWPORT9* pViewport)
{
	HRESULT ret = m_Direct3DDevice9Ex->GetViewport(pViewport);
	LOG(7, "GetViewport %ld %ld %ld %ld 0x%p\n", pViewport->X, pViewport->Y, pViewport->Width, pViewport->Height, pViewport);

	return ret;
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetMaterial(CONST D3DMATERIAL9* pMaterial)
{
	LOG(7, "SetMaterial\n");
	return m_Direct3DDevice9Ex->SetMaterial(pMaterial);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetMaterial(D3DMATERIAL9* pMaterial)
{
	LOG(7, "GetMaterial\n");
	return m_Direct3DDevice9Ex->GetMaterial(pMaterial);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetLight(DWORD Index, CONST D3DLIGHT9* pLight)
{
	LOG(7, "SetLight\n");
	return m_Direct3DDevice9Ex->SetLight(Index, pLight);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetLight(DWORD Index, D3DLIGHT9* pLight)
{
	LOG(7, "GetLight\n");
	return m_Direct3DDevice9Ex->GetLight(Index, pLight);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::LightEnable(DWORD Index, BOOL Enable)
{
	LOG(7, "LightEnable\n");
	return m_Direct3DDevice9Ex->LightEnable(Index, Enable);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetLightEnable(DWORD Index, BOOL* pEnable)
{
	LOG(7, "GetLightEnable\n");
	return m_Direct3DDevice9Ex->GetLightEnable(Index, pEnable);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetClipPlane(DWORD Index, CONST float* pPlane)
{
	LOG(7, "SetClipPlane\n");
	return m_Direct3DDevice9Ex->SetClipPlane(Index, pPlane);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetClipPlane(DWORD Index, float* pPlane)
{
	LOG(7, "GetClipPlane\n");
	return m_Direct3DDevice9Ex->GetClipPlane(Index, pPlane);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	LOG(7, "SetRenderState\n");
	return m_Direct3DDevice9Ex->SetRenderState(State, Value);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue)
{
	LOG(7, "GetRenderState\n");
	return m_Direct3DDevice9Ex->GetRenderState(State, pValue);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB)
{
	LOG(7, "CreateStateBlock\n");
	return m_Direct3DDevice9Ex->CreateStateBlock(Type, ppSB);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::BeginStateBlock()
{
	LOG(7, "BeginStateBlock\n");
	return m_Direct3DDevice9Ex->BeginStateBlock();
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::EndStateBlock(IDirect3DStateBlock9** ppSB)
{
	LOG(7, "EndStateBlock\n");
	return m_Direct3DDevice9Ex->EndStateBlock(ppSB);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus)
{
	LOG(7, "SetClipStatus\n");
	return m_Direct3DDevice9Ex->SetClipStatus(pClipStatus);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetClipStatus(D3DCLIPSTATUS9* pClipStatus)
{
	LOG(7, "GetClipStatus\n");
	return m_Direct3DDevice9Ex->GetClipStatus(pClipStatus);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture)
{
	LOG(7, "GetTexture\n");
	return m_Direct3DDevice9Ex->GetTexture(Stage, ppTexture);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
	LOG(7, "SetTexture 0x%p\n", pTexture);
	return m_Direct3DDevice9Ex->SetTexture(Stage, pTexture);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)
{
	LOG(7, "GetTextureStageState\n");
	return m_Direct3DDevice9Ex->GetTextureStageState(Stage, Type, pValue);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	LOG(7, "SetTextureStageState\n");
	return m_Direct3DDevice9Ex->SetTextureStageState(Stage, Type, Value);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue)
{
	LOG(7, "GetSamplerState\n");
	return m_Direct3DDevice9Ex->GetSamplerState(Sampler, Type, pValue);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
	LOG(7, "SetSamplerState\n");

	if (Settings::settingsIni.viewport == 3)
	{
		if (Type == D3DSAMP_MINFILTER && Settings::savedSettings.isFiltering)
		{
			//Sampler = 2;
			Value = D3DTEXF_LINEAR; //D3DTEXF_LINEAR;
		}
	}

	return m_Direct3DDevice9Ex->SetSamplerState(Sampler, Type, Value);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::ValidateDevice(DWORD* pNumPasses)
{
	LOG(7, "ValidateDevice\n");
	return m_Direct3DDevice9Ex->ValidateDevice(pNumPasses);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY* pEntries)
{
	LOG(7, "SetPaletteEntries\n");
	return m_Direct3DDevice9Ex->SetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries)
{
	LOG(7, "GetPaletteEntries\n");
	return m_Direct3DDevice9Ex->GetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetCurrentTexturePalette(UINT PaletteNumber)
{
	LOG(7, "SetCurrentTexturePalette\n");
	return m_Direct3DDevice9Ex->SetCurrentTexturePalette(PaletteNumber);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetCurrentTexturePalette(UINT *PaletteNumber)
{
	LOG(7, "GetCurrentTexturePalette\n");
	return m_Direct3DDevice9Ex->GetCurrentTexturePalette(PaletteNumber);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetScissorRect(CONST RECT* pRect)
{
	LOG(7, "SetScissorRect\n");
	return m_Direct3DDevice9Ex->SetScissorRect(pRect);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetScissorRect(RECT* pRect)
{
	LOG(7, "GetScissorRect\n");
	return m_Direct3DDevice9Ex->GetScissorRect(pRect);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetSoftwareVertexProcessing(BOOL bSoftware)
{
	LOG(7, "SetSoftwareVertexProcessing\n");
	return m_Direct3DDevice9Ex->SetSoftwareVertexProcessing(bSoftware);
}

BOOL APIENTRY Direct3DDevice9ExWrapper::GetSoftwareVertexProcessing()
{
	LOG(7, "GetSoftwareVertexProcessing\n");
	return m_Direct3DDevice9Ex->GetSoftwareVertexProcessing();
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetNPatchMode(float nSegments)
{
	LOG(7, "SetNPatchMode\n");
	return m_Direct3DDevice9Ex->SetNPatchMode(nSegments);
}

float APIENTRY Direct3DDevice9ExWrapper::GetNPatchMode()
{
	LOG(7, "GetNPatchMode\n");
	return m_Direct3DDevice9Ex->GetNPatchMode();
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	LOG(7, "DrawPrimitive\n");
	return m_Direct3DDevice9Ex->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	LOG(7, "DrawIndexedPrimitive\n");
	Settings::savedSettings.isFiltering = true;
	return m_Direct3DDevice9Ex->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	LOG(7, "DrawPrimitiveUP\n");
	return m_Direct3DDevice9Ex->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	LOG(7, "DrawIndexedPrimitiveUP\n");
	return m_Direct3DDevice9Ex->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags)
{
	LOG(7, "ProcessVertices\n");
	return m_Direct3DDevice9Ex->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl)
{
	LOG(7, "CreateVertexDeclaration\n");
	return m_Direct3DDevice9Ex->CreateVertexDeclaration(pVertexElements, ppDecl);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl)
{
	LOG(7, "SetVertexDeclaration\n");
	return m_Direct3DDevice9Ex->SetVertexDeclaration(pDecl);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl)
{
	LOG(7, "GetVertexDeclaration\n");
	return m_Direct3DDevice9Ex->GetVertexDeclaration(ppDecl);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetFVF(DWORD FVF)
{
	LOG(7, "SetFVF\n");
	return m_Direct3DDevice9Ex->SetFVF(FVF);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetFVF(DWORD* pFVF)
{
	LOG(7, "GetFVF\n");
	return m_Direct3DDevice9Ex->GetFVF(pFVF);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::CreateVertexShader(CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader)
{
	LOG(7, "CreateVertexShader\n");
	return m_Direct3DDevice9Ex->CreateVertexShader(pFunction, ppShader);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetVertexShader(IDirect3DVertexShader9* pShader)
{
	LOG(7, "SetVertexShader: 0x%p\n", pShader);
	return m_Direct3DDevice9Ex->SetVertexShader(pShader);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetVertexShader(IDirect3DVertexShader9** ppShader)
{
	LOG(7, "GetVertexShader\n");
	return m_Direct3DDevice9Ex->GetVertexShader(ppShader);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
	LOG(7, "SetVertexShaderConstantF 0x%p %u\n", pConstantData, Vector4fCount);
	//for (int i = 0; i < Vector4fCount; i++)
	//{
	//	LOG(7, "%.2f ", pConstantData[i]);
	//}
	//LOG(7, "\n");
	return m_Direct3DDevice9Ex->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
	LOG(7, "GetVertexShaderConstantF\n");
	return m_Direct3DDevice9Ex->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
	LOG(7, "SetVertexShaderConstantI\n");
	return m_Direct3DDevice9Ex->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
	LOG(7, "GetVertexShaderConstantI\n");
	return m_Direct3DDevice9Ex->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount)
{
	LOG(7, "SetVertexShaderConstantB\n");
	return m_Direct3DDevice9Ex->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
	LOG(7, "GetVertexShaderConstantB\n");
	return m_Direct3DDevice9Ex->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{
	LOG(7, "SetStreamSource\n");
	if (StreamNumber == 0)
		m_Stride = Stride;

	return m_Direct3DDevice9Ex->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride)
{
	LOG(7, "GetStreamSource\n");
	return m_Direct3DDevice9Ex->GetStreamSource(StreamNumber, ppStreamData, pOffsetInBytes, pStride);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetStreamSourceFreq(UINT StreamNumber, UINT Setting)
{
	LOG(7, "SetStreamSourceFreq\n");
	return m_Direct3DDevice9Ex->SetStreamSourceFreq(StreamNumber, Setting);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting)
{
	LOG(7, "GetStreamSourceFreq\n");
	return m_Direct3DDevice9Ex->GetStreamSourceFreq(StreamNumber, pSetting);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetIndices(IDirect3DIndexBuffer9* pIndexData)
{
	LOG(7, "SetIndices\n");
	return m_Direct3DDevice9Ex->SetIndices(pIndexData);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetIndices(IDirect3DIndexBuffer9** ppIndexData)
{
	LOG(7, "GetIndices\n");
	return m_Direct3DDevice9Ex->GetIndices(ppIndexData);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::CreatePixelShader(CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader)
{
	LOG(7, "CreatePixelShader\n");
	return m_Direct3DDevice9Ex->CreatePixelShader(pFunction, ppShader);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetPixelShader(IDirect3DPixelShader9* pShader)
{
	LOG(7, "SetPixelShader: 0x%p\n", pShader);
	return m_Direct3DDevice9Ex->SetPixelShader(pShader);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetPixelShader(IDirect3DPixelShader9** ppShader)
{
	LOG(7, "GetPixelShader\n");
	return m_Direct3DDevice9Ex->GetPixelShader(ppShader);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
{
	LOG(7, "SetPixelShaderConstantF\n");
	return m_Direct3DDevice9Ex->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
	LOG(7, "GetPixelShaderConstantF\n");
	return m_Direct3DDevice9Ex->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
{
	LOG(7, "SetPixelShaderConstantI\n");
	return m_Direct3DDevice9Ex->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
	LOG(7, "GetPixelShaderConstantI\n");
	return m_Direct3DDevice9Ex->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT  BoolCount)
{
	LOG(7, "SetPixelShaderConstantB\n");
	return m_Direct3DDevice9Ex->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
	LOG(7, "GetPixelShaderConstantB\n");
	return m_Direct3DDevice9Ex->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::DrawRectPatch(UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
	LOG(7, "DrawRectPatch\n");
	return m_Direct3DDevice9Ex->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::DrawTriPatch(UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
	LOG(7, "DrawTriPatch\n");
	return m_Direct3DDevice9Ex->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::DeletePatch(UINT Handle)
{
	LOG(7, "DeletePatch\n");
	return m_Direct3DDevice9Ex->DeletePatch(Handle);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
{
	LOG(7, "CreateQuery\n");
	return m_Direct3DDevice9Ex->CreateQuery(Type, ppQuery);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetConvolutionMonoKernel(UINT width, UINT height, float* rows, float* columns)
{
	LOG(7, "SetConvolutionMonoKernel\n");
	return m_Direct3DDevice9Ex->SetConvolutionMonoKernel(width, height, rows, columns);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::ComposeRects(IDirect3DSurface9* pSrc, IDirect3DSurface9* pDst, IDirect3DVertexBuffer9* pSrcRectDescs, UINT NumRects, IDirect3DVertexBuffer9* pDstRectDescs, D3DCOMPOSERECTSOP Operation, int Xoffset, int Yoffset)
{
	LOG(7, "ComposeRects\n");
	return m_Direct3DDevice9Ex->ComposeRects(pSrc, pDst, pSrcRectDescs, NumRects, pDstRectDescs, Operation, Xoffset, Yoffset);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::PresentEx(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags)
{
	LOG(7, "PresentEx 0x%p 0x%p\n", pSourceRect, pDestRect);
	return m_Direct3DDevice9Ex->PresentEx(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetGPUThreadPriority(INT* pPriority)
{
	LOG(7, "GetGPUThreadPriority\n");
	return m_Direct3DDevice9Ex->GetGPUThreadPriority(pPriority);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetGPUThreadPriority(INT Priority)
{
	LOG(7, "SetGPUThreadPriority\n");
	return m_Direct3DDevice9Ex->SetGPUThreadPriority(Priority);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::WaitForVBlank(UINT iSwapChain)
{
	LOG(7, "WaitForVBlank\n");
	return m_Direct3DDevice9Ex->WaitForVBlank(iSwapChain);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::CheckResourceResidency(IDirect3DResource9** pResourceArray, UINT32 NumResources)
{
	LOG(7, "CheckResourceResidency\n");
	return m_Direct3DDevice9Ex->CheckResourceResidency(pResourceArray, NumResources);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::SetMaximumFrameLatency(UINT MaxLatency)
{
	LOG(7, "SetMaximumFrameLatency\n");
	return m_Direct3DDevice9Ex->SetMaximumFrameLatency(MaxLatency);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetMaximumFrameLatency(UINT* pMaxLatency)
{
	LOG(7, "GetMaximumFrameLatency\n");
	return m_Direct3DDevice9Ex->GetMaximumFrameLatency(pMaxLatency);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::CheckDeviceState(HWND hDestinationWindow)
{
	LOG(7, "CheckDeviceState\n");
	return m_Direct3DDevice9Ex->CheckDeviceState(hDestinationWindow);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::CreateRenderTargetEx(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage)
{
	LOG(3, "CreateRenderTargetEx\n");
	LOG(3, "\t- Width: %u\n", Width);
	LOG(3, "\t- Height: %u\n", Height);
	LOG(3, "\t- Multisample: %d\n", MultiSample);
	LOG(3, "\t- ppSurface: 0x%p\n", ppSurface);

	if (Settings::settingsIni.viewport != 1)
	{
		LOG(3, "\t- modifying to %u %u\n", Settings::settingsIni.renderwidth, Settings::settingsIni.renderheight);

		Settings::savedSettings.origViewportRes.x = (float)Width;
		Settings::savedSettings.origViewportRes.y = (float)Height;
		Settings::savedSettings.newViewport.MinZ = 0.0;
		Settings::savedSettings.newViewport.MaxZ = 1.0;

		return m_Direct3DDevice9Ex->CreateRenderTargetEx(Settings::settingsIni.renderwidth, Settings::settingsIni.renderheight, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle, Usage);
	}

	LOG(7, "SETTINGS\n - viewport %d\n - renderheight %d\n - renderheight %d\n",
		Settings::settingsIni.viewport, Settings::settingsIni.renderheight, Settings::settingsIni.renderwidth);

	LOG(7, " - SourceRect.right %u\n - SourceRect.bottom %u\n - Viewport.Width %d\n - Viewport.Height %d\n",
		Settings::savedSettings.newSourceRect.right,
		Settings::savedSettings.newSourceRect.bottom,
		Settings::savedSettings.newViewport.Width,
		Settings::savedSettings.newViewport.Height);

	return m_Direct3DDevice9Ex->CreateRenderTargetEx(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle, Usage);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::CreateOffscreenPlainSurfaceEx(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage)
{
	LOG(3, "CreateOffscreenPlainSurfaceEx\n");
	return m_Direct3DDevice9Ex->CreateOffscreenPlainSurfaceEx(Width, Height, Format, Pool, ppSurface, pSharedHandle, Usage);
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::CreateDepthStencilSurfaceEx(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage)
{
	LOG(3, "CreateDepthStencilSurfaceEx\n");
	LOG(3, "\t- Width: %u\n", Width);
	LOG(3, "\t- Height: %u\n", Height);
	LOG(3, "\t- Multisample: %d\n", MultiSample);
	LOG(3, "\t- ppSurface: 0x%p\n", ppSurface);

	if (Settings::settingsIni.viewport != 1)
	{
		LOG(3, "\t- modifying to %u %u\n", Settings::settingsIni.renderwidth, Settings::settingsIni.renderheight);
		Width = Settings::settingsIni.renderwidth;
		Height = Settings::settingsIni.renderheight;
	}

	HRESULT ret = m_Direct3DDevice9Ex->CreateDepthStencilSurfaceEx(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle, Usage);

	LOG(2, "\t- HRESULT: %ld\n", ret);

	return ret;
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::ResetEx(D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode)
{
	LOG(3, "ResetEx\n");
	logD3DPParams(pPresentationParameters, true);
	Settings::applySettingsIni(pPresentationParameters);
	logD3DPParams(pPresentationParameters, false);

	WindowManager::GetInstance().InvalidateDeviceObjects();

	HRESULT ret = m_Direct3DDevice9Ex->ResetEx(pPresentationParameters, pFullscreenDisplayMode);

	WindowManager::GetInstance().CreateDeviceObjects();

	return ret;
}

HRESULT APIENTRY Direct3DDevice9ExWrapper::GetDisplayModeEx(UINT iSwapChain, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation)
{
	LOG(7, "GetDisplayModeEx\n");
	return m_Direct3DDevice9Ex->GetDisplayModeEx(iSwapChain, pMode, pRotation);
}