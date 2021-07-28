#include "ID3D9Wrapper_Sprite.h"

#include "Core/logger.h"

ID3DXSpriteWrapper::ID3DXSpriteWrapper(LPD3DXSPRITE** ppSprite)
{
	m_D3DXSprite = **ppSprite;
	**ppSprite = this;
	LOG(7, "ID3DXSpriteWrapper Created: (0x%p)\n", ppSprite);
}

// IUnknown
HRESULT ID3DXSpriteWrapper::QueryInterface(REFIID iid, LPVOID *ppv)
{
	LOG(7, "Sprite QueryInterface\n");
	return m_D3DXSprite->QueryInterface(iid, ppv);
}

ULONG ID3DXSpriteWrapper::AddRef()
{
	LOG(7, "Sprite AddRef\n");
	return m_D3DXSprite->AddRef();
}

ULONG ID3DXSpriteWrapper::Release()
{
	LOG(7, "Sprite Release\n");
	return m_D3DXSprite->Release();
}

// ID3DXSprite
HRESULT ID3DXSpriteWrapper::GetDevice(LPDIRECT3DDEVICE9* ppDevice)
{
	LOG(7, "Sprite GetDevice\n");
	return m_D3DXSprite->GetDevice(ppDevice);
}

HRESULT ID3DXSpriteWrapper::GetTransform(D3DXMATRIX *pTransform)
{
	LOG(7, "Sprite GetTransform\n");
	return m_D3DXSprite->GetTransform(pTransform);
}

HRESULT ID3DXSpriteWrapper::SetTransform(CONST D3DXMATRIX *pTransform)
{
	LOG(7, "Sprite SetTransform\n");
	return m_D3DXSprite->SetTransform(pTransform);
}

HRESULT ID3DXSpriteWrapper::SetWorldViewRH(CONST D3DXMATRIX *pWorld, CONST D3DXMATRIX *pView)
{
	LOG(7, "Sprite SetWorldViewRH\n");
	return m_D3DXSprite->SetWorldViewRH(pWorld, pView);
}

HRESULT ID3DXSpriteWrapper::SetWorldViewLH(CONST D3DXMATRIX *pWorld, CONST D3DXMATRIX *pView)
{
	LOG(7, "Sprite SetWorldViewLH\n");
	return m_D3DXSprite->SetWorldViewLH(pWorld, pView);
}

HRESULT ID3DXSpriteWrapper::Begin(DWORD Flags)
{
	LOG(7, "Sprite Begin\n");
	return m_D3DXSprite->Begin(Flags);
}

HRESULT ID3DXSpriteWrapper::Draw(LPDIRECT3DTEXTURE9 pTexture, CONST RECT *pSrcRect, CONST D3DXVECTOR3 *pCenter, CONST D3DXVECTOR3 *pPosition, D3DCOLOR Color)
{
	LOG(7, "Sprite Draw (pos:) %.2f %.2f %.2f (pcent:) %.2f %.2f\n", pPosition->x, pPosition->y, pPosition->z, pCenter->x, pCenter->y);

	if (!Settings::savedSettings.isDuelFieldSprite && Settings::settingsIni.viewport != 1)
	{
		D3DXVECTOR2 scale(Settings::savedSettings.newViewport.Width / Settings::savedSettings.origViewportRes.x, 
			Settings::savedSettings.newViewport.Height / Settings::savedSettings.origViewportRes.y); // 1280/1920=0.66667
		D3DXVECTOR2 center(pCenter->x, pCenter->y);
		//D3DXVECTOR2 scalecenter(pPosition->x, pPosition->y);
		D3DXMATRIX m_transform;
		D3DXMatrixTransformation2D(&m_transform, NULL, 0.0f, &scale, &center, 0.0f, NULL);
		m_D3DXSprite->SetTransform(&m_transform);
	}
	return m_D3DXSprite->Draw(pTexture, pSrcRect, pCenter, pPosition, Color);
}

HRESULT ID3DXSpriteWrapper::Flush()
{
	LOG(7, "Sprite Flush\n");
	return m_D3DXSprite->Flush();
}

HRESULT ID3DXSpriteWrapper::End()
{
	LOG(7, "Sprite End\n");
	return m_D3DXSprite->End();
}

HRESULT ID3DXSpriteWrapper::OnLostDevice()
{
	LOG(7, "Sprite OnLostDevice\n");
	return m_D3DXSprite->OnLostDevice();
}
HRESULT ID3DXSpriteWrapper::OnResetDevice()
{
	LOG(7, "Sprite OnResetDevice\n");
	return m_D3DXSprite->OnResetDevice();
}