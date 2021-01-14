#include "D3DXMath.h"

#include "d3d9.h"

#include "Core/logger.h"

#include <detours.h>

#pragma comment(lib, "detours.lib")

typedef D3DXMATRIX* (WINAPI *D3DXMatrixLookAtLH_t)
(D3DXMATRIX *pOut, CONST D3DXVECTOR3 *pEye, CONST D3DXVECTOR3 *pAt, CONST D3DXVECTOR3 *pUp);

typedef D3DXMATRIX* (WINAPI *D3DXMatrixPerspectiveFovLH_t)
(D3DXMATRIX *pOut, FLOAT fovy, FLOAT Aspect, FLOAT zn, FLOAT zf);

typedef D3DXMATRIX* (WINAPI *D3DXMatrixMultiply_t)
(D3DXMATRIX *pOut, CONST D3DXMATRIX *pM1, CONST D3DXMATRIX *pM2);

typedef D3DXMATRIX* (WINAPI *D3DXMatrixScaling_t)
(D3DXMATRIX *pOut, FLOAT sx, FLOAT sy, FLOAT sz);

typedef D3DXMATRIX* (WINAPI *D3DXMatrixTranslation_t)
(D3DXMATRIX *pOut, FLOAT x, FLOAT y, FLOAT z);

typedef D3DXVECTOR3* (WINAPI *D3DXVec3TransformCoord_t)
(D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM);

typedef D3DXVECTOR4* (WINAPI *D3DXVec4Transform_t)
(D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV, CONST D3DXMATRIX *pM);

typedef D3DXMATRIX* (WINAPI *D3DXMatrixTransformation2D_t)
(D3DXMATRIX *pOut, CONST D3DXVECTOR2* pScalingCenter, FLOAT ScalingRotation, CONST D3DXVECTOR2* pScaling,
CONST D3DXVECTOR2* pRotationCenter, FLOAT Rotation, CONST D3DXVECTOR2* pTranslation);

D3DXMatrixLookAtLH_t orig_D3DXMatrixLookAtLH;
D3DXMatrixPerspectiveFovLH_t orig_D3DXMatrixPerspectiveFovLH;
D3DXMatrixMultiply_t orig_D3DXMatrixMultiply;
D3DXMatrixScaling_t orig_D3DXMatrixScaling;
D3DXMatrixTranslation_t orig_D3DXMatrixTranslation;
D3DXVec3TransformCoord_t orig_D3DXVec3TransformCoord;
D3DXVec4Transform_t orig_D3DXVec4Transform;
D3DXMatrixTransformation2D_t orig_D3DXMatrixTransformation2D;

D3DXMATRIX* WINAPI hook_D3DXMatrixLookAtLH(D3DXMATRIX *pOut, CONST D3DXVECTOR3 *pEye, CONST D3DXVECTOR3 *pAt, CONST D3DXVECTOR3 *pUp)
{
	LOG(7, "D3DXMatrixLookAtLH 0x%p (pEye:) %.2f %.2f %.2f (pAt:) %.2f %.2f %.2f (pUP:) %.2f %.2f %.2f\n", 
		pOut, pEye->x, pEye->y, pEye->z, pAt->x, pAt->y, pAt->z, pUp->x, pUp->y, pUp->z);
	//LOG(7, "BEFORE:\n");
	//LOG(7, "%.2f %.2f %,2 %,2\n", pOut->_11, pOut->_12, pOut->_13, pOut->_14);
	//LOG(7, "%.2f %.2f %,2 %,2\n", pOut->_21, pOut->_22, pOut->_23, pOut->_24);
	//LOG(7, "%.2f %.2f %,2 %,2\n", pOut->_31, pOut->_32, pOut->_33, pOut->_34);
	//LOG(7, "%.2f %.2f %,2 %,2\n", pOut->_41, pOut->_42, pOut->_43, pOut->_44);

	D3DXMATRIX* ret = orig_D3DXMatrixLookAtLH(pOut, pEye, pAt, pUp);

	//LOG(7, "AFTER:\n");
	//LOG(7, "%.2f %.2f %,2 %,2\n", pOut->_11, pOut->_12, pOut->_13, pOut->_14);
	//LOG(7, "%.2f %.2f %,2 %,2\n", pOut->_21, pOut->_22, pOut->_23, pOut->_24);
	//LOG(7, "%.2f %.2f %,2 %,2\n", pOut->_31, pOut->_32, pOut->_33, pOut->_34);
	//LOG(7, "%.2f %.2f %,2 %,2\n", pOut->_41, pOut->_42, pOut->_43, pOut->_44);

	return ret;
}

D3DXMATRIX* WINAPI hook_D3DXMatrixPerspectiveFovLH(D3DXMATRIX *pOut, FLOAT fovy, FLOAT Aspect, FLOAT zn, FLOAT zf)
{
	LOG(7, "D3DXMatrixPerspectiveFovLH 0x%p %.2f %.2f %.2f %.2f\n", pOut, fovy, Aspect, zn, zf);
	//LOG(7, "BEFORE:\n");
	//LOG(7, "%.2f %.2f %,2 %,2\n", pOut->_11, pOut->_12, pOut->_13, pOut->_14);
	//LOG(7, "%.2f %.2f %,2 %,2\n", pOut->_21, pOut->_22, pOut->_23, pOut->_24);
	//LOG(7, "%.2f %.2f %,2 %,2\n", pOut->_31, pOut->_32, pOut->_33, pOut->_34);
	//LOG(7, "%.2f %.2f %,2 %,2\n", pOut->_41, pOut->_42, pOut->_43, pOut->_44);

	D3DXMATRIX* ret = orig_D3DXMatrixPerspectiveFovLH(pOut, fovy, Aspect, zn, zf);

	//LOG(7, "After:\n");
	//LOG(7, "%.2f %.2f %,2 %,2\n", pOut->_11, pOut->_12, pOut->_13, pOut->_14);
	//LOG(7, "%.2f %.2f %,2 %,2\n", pOut->_21, pOut->_22, pOut->_23, pOut->_24);
	//LOG(7, "%.2f %.2f %,2 %,2\n", pOut->_31, pOut->_32, pOut->_33, pOut->_34);
	//LOG(7, "%.2f %.2f %,2 %,2\n", pOut->_41, pOut->_42, pOut->_43, pOut->_44);

	return ret;
}

D3DXMATRIX* WINAPI hook_D3DXMatrixMultiply(D3DXMATRIX *pOut, CONST D3DXMATRIX *pM1, CONST D3DXMATRIX *pM2)
{
	D3DXMATRIX* ret = orig_D3DXMatrixMultiply(pOut, pM1, pM2);
	LOG(7, "D3DXMatrixMultiply\n");
	return ret;
}

D3DXMATRIX* WINAPI hook_D3DXMatrixScaling(D3DXMATRIX *pOut, FLOAT sx, FLOAT sy, FLOAT sz)
{
	D3DXMATRIX* ret = orig_D3DXMatrixScaling(pOut, sx, sy, sz);
	LOG(7, "D3DXMatrixScaling\n");
	return ret;
}

D3DXMATRIX* WINAPI hook_D3DXMatrixTranslation(D3DXMATRIX *pOut, FLOAT x, FLOAT y, FLOAT z)
{
	D3DXMATRIX* ret = orig_D3DXMatrixTranslation(pOut, x, y, z);
	LOG(7, "D3DXMatrixTranslation\n");
	return ret;
}

D3DXVECTOR3* WINAPI hook_D3DXVec3TransformCoord(D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM)
{
	D3DXVECTOR3* ret = orig_D3DXVec3TransformCoord(pOut, pV, pM);
	LOG(7, "D3DXVec3TransformCoord\n");
	return ret;
}

D3DXVECTOR4* WINAPI hook_D3DXVec4Transform(D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV, CONST D3DXMATRIX *pM)
{
	LOG(7, "D3DXVec4Transform\n");
	return orig_D3DXVec4Transform(pOut, pV, pM);
}

D3DXMATRIX* WINAPI hook_D3DXMatrixTransformation2D(D3DXMATRIX *pOut, CONST D3DXVECTOR2* pScalingCenter,
	FLOAT ScalingRotation, CONST D3DXVECTOR2* pScaling, CONST D3DXVECTOR2* pRotationCenter, FLOAT Rotation, CONST D3DXVECTOR2* pTranslation)
{
	LOG(7, "D3DXMatrixTransformation2D\n");
	return orig_D3DXMatrixTransformation2D(pOut, pScalingCenter, ScalingRotation, pScaling, pRotationCenter, Rotation, pTranslation);
}

void hookD3DMaths()
{
	HMODULE hM = GetModuleHandleA("d3dx9_43.dll");
	PBYTE pD3DXMatrixLookAtLH = (PBYTE)GetProcAddress(hM, "D3DXMatrixLookAtLH");
	PBYTE pD3DXMatrixPerspectiveFovLH = (PBYTE)GetProcAddress(hM, "D3DXMatrixPerspectiveFovLH");
	PBYTE pD3DXMatrixMultiply = (PBYTE)GetProcAddress(hM, "D3DXMatrixMultiply");
	PBYTE pD3DXMatrixScaling = (PBYTE)GetProcAddress(hM, "D3DXMatrixScaling");
	PBYTE pD3DXMatrixTranslation = (PBYTE)GetProcAddress(hM, "D3DXMatrixTranslation");
	PBYTE pD3DXVec3TransformCoord = (PBYTE)GetProcAddress(hM, "D3DXVec3TransformCoord");
	PBYTE pD3DXVec4Transform = (PBYTE)GetProcAddress(hM, "D3DXVec4Transform");
	PBYTE pD3DXMatrixTransformation2D = (PBYTE)GetProcAddress(hM, "D3DXMatrixTransformation2D");

	hookSucceeded((PBYTE)pD3DXMatrixLookAtLH, "D3DXMatrixLookAtLH");
	hookSucceeded((PBYTE)pD3DXMatrixPerspectiveFovLH, "D3DXMatrixPerspectiveFovLH");
	hookSucceeded((PBYTE)pD3DXMatrixMultiply, "D3DXMatrixMultiply");
	hookSucceeded((PBYTE)pD3DXMatrixScaling, "D3DXMatrixScaling");
	hookSucceeded((PBYTE)pD3DXMatrixTranslation, "D3DXMatrixTranslation");
	hookSucceeded((PBYTE)pD3DXVec3TransformCoord, "D3DXVec3TransformCoord");
	hookSucceeded((PBYTE)pD3DXVec4Transform, "D3DXVec4Transform");
	hookSucceeded((PBYTE)pD3DXMatrixTransformation2D, "D3DXMatrixTransformation2D");

	orig_D3DXMatrixLookAtLH = (D3DXMatrixLookAtLH_t)DetourFunction(pD3DXMatrixLookAtLH, (LPBYTE)hook_D3DXMatrixLookAtLH);
	orig_D3DXMatrixPerspectiveFovLH = (D3DXMatrixPerspectiveFovLH_t)DetourFunction(pD3DXMatrixPerspectiveFovLH, (LPBYTE)hook_D3DXMatrixPerspectiveFovLH);
	orig_D3DXMatrixMultiply = (D3DXMatrixMultiply_t)DetourFunction(pD3DXMatrixMultiply, (LPBYTE)hook_D3DXMatrixMultiply);
	orig_D3DXMatrixScaling = (D3DXMatrixScaling_t)DetourFunction(pD3DXMatrixScaling, (LPBYTE)hook_D3DXMatrixScaling);
	orig_D3DXMatrixTranslation = (D3DXMatrixTranslation_t)DetourFunction(pD3DXMatrixTranslation, (LPBYTE)hook_D3DXMatrixTranslation);
	orig_D3DXVec3TransformCoord = (D3DXVec3TransformCoord_t)DetourFunction(pD3DXVec3TransformCoord, (LPBYTE)hook_D3DXVec3TransformCoord);
	orig_D3DXVec4Transform = (D3DXVec4Transform_t)DetourFunction(pD3DXVec4Transform, (LPBYTE)hook_D3DXVec4Transform);
	orig_D3DXMatrixTransformation2D = (D3DXMatrixTransformation2D_t)DetourFunction(pD3DXMatrixTransformation2D, (LPBYTE)hook_D3DXMatrixTransformation2D);
}