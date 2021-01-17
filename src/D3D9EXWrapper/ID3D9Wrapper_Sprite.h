#pragma once
#include "d3d9.h"

interface ID3DXSpriteWrapper : public ID3DXSprite
{
public:
	ID3DXSpriteWrapper(LPD3DXSPRITE** ppSprite);
	ID3DXSprite* m_D3DXSprite;

	// IUnknown
	STDMETHOD(QueryInterface)( REFIID iid, LPVOID *ppv) ;
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// ID3DXSprite
	STDMETHOD(GetDevice)( LPDIRECT3DDEVICE9* ppDevice) ;

	STDMETHOD(GetTransform)( D3DXMATRIX *pTransform) ;
	STDMETHOD(SetTransform)( CONST D3DXMATRIX *pTransform) ;

	STDMETHOD(SetWorldViewRH)( CONST D3DXMATRIX *pWorld, CONST D3DXMATRIX *pView) ;
	STDMETHOD(SetWorldViewLH)( CONST D3DXMATRIX *pWorld, CONST D3DXMATRIX *pView) ;

	STDMETHOD(Begin)( DWORD Flags) ;
	STDMETHOD(Draw)( LPDIRECT3DTEXTURE9 pTexture, CONST RECT *pSrcRect, CONST D3DXVECTOR3 *pCenter, CONST D3DXVECTOR3 *pPosition, D3DCOLOR Color) ;
	STDMETHOD(Flush)();
	STDMETHOD(End)();

	STDMETHOD(OnLostDevice)();
	STDMETHOD(OnResetDevice)();
};