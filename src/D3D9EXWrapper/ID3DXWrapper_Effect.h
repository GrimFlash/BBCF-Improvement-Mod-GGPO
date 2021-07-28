#pragma once
#include "d3d9.h"

interface ID3DXEffectWrapper : public ID3DXEffect
{
public:
	ID3DXEffectWrapper(LPD3DXEFFECT** ppEffect);
	ID3DXEffect* m_D3DXEffect;

	// ID3DXBaseEffect
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID *ppv);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// Descs
	STDMETHOD(GetDesc)(D3DXEFFECT_DESC* pDesc);
	STDMETHOD(GetParameterDesc)(D3DXHANDLE hParameter, D3DXPARAMETER_DESC* pDesc);
	STDMETHOD(GetTechniqueDesc)(D3DXHANDLE hTechnique, D3DXTECHNIQUE_DESC* pDesc);
	STDMETHOD(GetPassDesc)(D3DXHANDLE hPass, D3DXPASS_DESC* pDesc);
	STDMETHOD(GetFunctionDesc)(D3DXHANDLE hShader, D3DXFUNCTION_DESC* pDesc);

	// Handle operations
	STDMETHOD_(D3DXHANDLE, GetParameter)(D3DXHANDLE hParameter, UINT Index);
	STDMETHOD_(D3DXHANDLE, GetParameterByName)(D3DXHANDLE hParameter, LPCSTR pName);
	STDMETHOD_(D3DXHANDLE, GetParameterBySemantic)(D3DXHANDLE hParameter, LPCSTR pSemantic);
	STDMETHOD_(D3DXHANDLE, GetParameterElement)(D3DXHANDLE hParameter, UINT Index);
	STDMETHOD_(D3DXHANDLE, GetTechnique)(UINT Index);
	STDMETHOD_(D3DXHANDLE, GetTechniqueByName)(LPCSTR pName);
	STDMETHOD_(D3DXHANDLE, GetPass)(D3DXHANDLE hTechnique, UINT Index);
	STDMETHOD_(D3DXHANDLE, GetPassByName)(D3DXHANDLE hTechnique, LPCSTR pName);
	STDMETHOD_(D3DXHANDLE, GetFunction)(UINT Index);
	STDMETHOD_(D3DXHANDLE, GetFunctionByName)(LPCSTR pName);
	STDMETHOD_(D3DXHANDLE, GetAnnotation)(D3DXHANDLE hObject, UINT Index);
	STDMETHOD_(D3DXHANDLE, GetAnnotationByName)(D3DXHANDLE hObject, LPCSTR pName);

	// Get/Set Parameters
	STDMETHOD(SetValue)(D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes);
	STDMETHOD(GetValue)(D3DXHANDLE hParameter, LPVOID pData, UINT Bytes);
	STDMETHOD(SetBool)(D3DXHANDLE hParameter, BOOL b);
	STDMETHOD(GetBool)(D3DXHANDLE hParameter, BOOL* pb);
	STDMETHOD(SetBoolArray)(D3DXHANDLE hParameter, CONST BOOL* pb, UINT Count);
	STDMETHOD(GetBoolArray)(D3DXHANDLE hParameter, BOOL* pb, UINT Count);
	STDMETHOD(SetInt)(D3DXHANDLE hParameter, INT n);
	STDMETHOD(GetInt)(D3DXHANDLE hParameter, INT* pn);
	STDMETHOD(SetIntArray)(D3DXHANDLE hParameter, CONST INT* pn, UINT Count);
	STDMETHOD(GetIntArray)(D3DXHANDLE hParameter, INT* pn, UINT Count);
	STDMETHOD(SetFloat)(D3DXHANDLE hParameter, FLOAT f);
	STDMETHOD(GetFloat)(D3DXHANDLE hParameter, FLOAT* pf);
	STDMETHOD(SetFloatArray)(D3DXHANDLE hParameter, CONST FLOAT* pf, UINT Count);
	STDMETHOD(GetFloatArray)(D3DXHANDLE hParameter, FLOAT* pf, UINT Count);
	STDMETHOD(SetVector)(D3DXHANDLE hParameter, CONST D3DXVECTOR4* pVector);
	STDMETHOD(GetVector)(D3DXHANDLE hParameter, D3DXVECTOR4* pVector);
	STDMETHOD(SetVectorArray)(D3DXHANDLE hParameter, CONST D3DXVECTOR4* pVector, UINT Count);
	STDMETHOD(GetVectorArray)(D3DXHANDLE hParameter, D3DXVECTOR4* pVector, UINT Count);
	STDMETHOD(SetMatrix)(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix);
	STDMETHOD(GetMatrix)(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix);
	STDMETHOD(SetMatrixArray)(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count);
	STDMETHOD(GetMatrixArray)(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count);
	STDMETHOD(SetMatrixPointerArray)(D3DXHANDLE hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count);
	STDMETHOD(GetMatrixPointerArray)(D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count);
	STDMETHOD(SetMatrixTranspose)(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix);
	STDMETHOD(GetMatrixTranspose)(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix);
	STDMETHOD(SetMatrixTransposeArray)(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count);
	STDMETHOD(GetMatrixTransposeArray)(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count);
	STDMETHOD(SetMatrixTransposePointerArray)(D3DXHANDLE hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count);
	STDMETHOD(GetMatrixTransposePointerArray)(D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count);
	STDMETHOD(SetString)(D3DXHANDLE hParameter, LPCSTR pString);
	STDMETHOD(GetString)(D3DXHANDLE hParameter, LPCSTR* ppString);
	STDMETHOD(SetTexture)(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture);
	STDMETHOD(GetTexture)(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 *ppTexture);
	STDMETHOD(GetPixelShader)(D3DXHANDLE hParameter, LPDIRECT3DPIXELSHADER9 *ppPShader);
	STDMETHOD(GetVertexShader)(D3DXHANDLE hParameter, LPDIRECT3DVERTEXSHADER9 *ppVShader);

	//Set Range of an Array to pass to device
	//Usefull for sending only a subrange of an array down to the device
	STDMETHOD(SetArrayRange)(D3DXHANDLE hParameter, UINT uStart, UINT uEnd);
	// ID3DXBaseEffect


	// Pool
	STDMETHOD(GetPool)(LPD3DXEFFECTPOOL* ppPool);

	// Selecting and setting a technique
	STDMETHOD(SetTechnique)(D3DXHANDLE hTechnique);
	STDMETHOD_(D3DXHANDLE, GetCurrentTechnique)();
	STDMETHOD(ValidateTechnique)(D3DXHANDLE hTechnique);
	STDMETHOD(FindNextValidTechnique)(D3DXHANDLE hTechnique, D3DXHANDLE *pTechnique);
	STDMETHOD_(BOOL, IsParameterUsed)(D3DXHANDLE hParameter, D3DXHANDLE hTechnique);

	// Using current technique
	// Begin           starts active technique
	// BeginPass       begins a pass
	// CommitChanges   updates changes to any set calls in the pass.   should be called before
	//                 any DrawPrimitive call to d3d
	// EndPass         ends a pass
	// End             ends active technique
	STDMETHOD(Begin)(UINT *pPasses, DWORD Flags);
	STDMETHOD(BeginPass)(UINT Pass);
	STDMETHOD(CommitChanges)();
	STDMETHOD(EndPass)();
	STDMETHOD(End)();

	// Managing D3D Device
	STDMETHOD(GetDevice)(LPDIRECT3DDEVICE9* ppDevice);
	STDMETHOD(OnLostDevice)();
	STDMETHOD(OnResetDevice)();

	// Logging device calls
	STDMETHOD(SetStateManager)(LPD3DXEFFECTSTATEMANAGER pManager);
	STDMETHOD(GetStateManager)(LPD3DXEFFECTSTATEMANAGER *ppManager);

	// Parameter blocks
	STDMETHOD(BeginParameterBlock)();
	STDMETHOD_(D3DXHANDLE, EndParameterBlock)();
	STDMETHOD(ApplyParameterBlock)(D3DXHANDLE hParameterBlock);
	STDMETHOD(DeleteParameterBlock)(D3DXHANDLE hParameterBlock);

	// Cloning
	STDMETHOD(CloneEffect)(LPDIRECT3DDEVICE9 pDevice, LPD3DXEFFECT* ppEffect);

	// Fast path for setting variables directly in ID3DXEffect
	STDMETHOD(SetRawValue)(D3DXHANDLE hParameter, LPCVOID pData, UINT ByteOffset, UINT Bytes);
};