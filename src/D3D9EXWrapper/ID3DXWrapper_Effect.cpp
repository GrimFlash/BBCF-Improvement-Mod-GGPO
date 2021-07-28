#include "ID3DXWrapper_Effect.h"

#include "Core/logger.h"

ID3DXEffectWrapper::ID3DXEffectWrapper(LPD3DXEFFECT** ppEffect)
{
	LOG(7, "Effect cID3DXEffectWrapper Created\n");
	m_D3DXEffect = **ppEffect;
	**ppEffect = this;
}

// ID3DXBaseEffect
HRESULT APIENTRY ID3DXEffectWrapper::QueryInterface(REFIID iid, LPVOID *ppv)
{
	LOG(7, "Effect QueryInterface\n");
	return m_D3DXEffect->QueryInterface(iid, ppv);
}

ULONG APIENTRY ID3DXEffectWrapper::AddRef()
{
	LOG(7, "Effect AddRef\n");
	return m_D3DXEffect->AddRef();
}

ULONG APIENTRY ID3DXEffectWrapper::Release()
{
	LOG(7, "Effect Release\n");
	ULONG res = m_D3DXEffect->Release();
	if (res == 0) {
		delete this;
	}

	return res;
}

// Descs
HRESULT APIENTRY ID3DXEffectWrapper::GetDesc(D3DXEFFECT_DESC* pDesc)
{
	LOG(7, "Effect GetDesc\n");
	return m_D3DXEffect->GetDesc(pDesc);
}

HRESULT APIENTRY ID3DXEffectWrapper::GetParameterDesc(D3DXHANDLE hParameter, D3DXPARAMETER_DESC* pDesc)
{
	LOG(7, "Effect GetParameterDesc\n");
	return m_D3DXEffect->GetParameterDesc(hParameter, pDesc);
}

HRESULT APIENTRY ID3DXEffectWrapper::GetTechniqueDesc(D3DXHANDLE hTechnique, D3DXTECHNIQUE_DESC* pDesc)
{
	LOG(7, "Effect GetTechniqueDesc\n");
	return m_D3DXEffect->GetTechniqueDesc(hTechnique, pDesc);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetPassDesc(D3DXHANDLE hPass, D3DXPASS_DESC* pDesc)
{
	LOG(7, "Effect GetPassDesc\n");
	return m_D3DXEffect->GetPassDesc(hPass, pDesc);
}

HRESULT APIENTRY ID3DXEffectWrapper::GetFunctionDesc(D3DXHANDLE hShader, D3DXFUNCTION_DESC* pDesc)
{
	LOG(7, "Effect GetFunctionDesc\n");
	return m_D3DXEffect->GetFunctionDesc(hShader, pDesc);
}

// Handle operations
D3DXHANDLE APIENTRY ID3DXEffectWrapper::GetParameter(D3DXHANDLE hParameter, UINT Index)
{
	LOG(7, "Effect GetParameter\n");
	return m_D3DXEffect->GetParameter(hParameter, Index);
}

D3DXHANDLE APIENTRY ID3DXEffectWrapper::GetParameterByName(D3DXHANDLE hParameter, LPCSTR pName)
{
	LOG(7, "Effect GetParameterByName\n");
	return m_D3DXEffect->GetParameterByName(hParameter, pName);
}

D3DXHANDLE APIENTRY ID3DXEffectWrapper::GetParameterBySemantic(D3DXHANDLE hParameter, LPCSTR pSemantic)
{
	D3DXHANDLE hndl = m_D3DXEffect->GetParameterBySemantic(hParameter, pSemantic);
	LOG(7, "Effect GetParameterBySemantic Handle: 0x%p - Semantic: %s : 0x%p\n", hParameter, pSemantic, hndl);
	return hndl;
}

D3DXHANDLE APIENTRY ID3DXEffectWrapper::GetParameterElement(D3DXHANDLE hParameter, UINT Index)
{
	LOG(7, "Effect GetParameterElement\n");
	return m_D3DXEffect->GetParameterElement(hParameter, Index);
}

D3DXHANDLE APIENTRY ID3DXEffectWrapper::GetTechnique(UINT Index)
{
	LOG(7, "Effect GetTechnique\n");
	return m_D3DXEffect->GetTechnique(Index);
}

D3DXHANDLE APIENTRY ID3DXEffectWrapper::GetTechniqueByName(LPCSTR pName)
{
	D3DXHANDLE hndl = m_D3DXEffect->GetTechniqueByName(pName);
	LOG(7, "Effect GetTechniqueByName - %s : 0x%p\n", pName, hndl);
	return hndl;
}

D3DXHANDLE APIENTRY ID3DXEffectWrapper::GetPass(D3DXHANDLE hTechnique, UINT Index)
{
	LOG(7, "Effect GetPass\n");
	return m_D3DXEffect->GetPass(hTechnique, Index);
}

D3DXHANDLE APIENTRY ID3DXEffectWrapper::GetPassByName(D3DXHANDLE hTechnique, LPCSTR pName)
{
	LOG(7, "Effect GetPassByName\n");
	return m_D3DXEffect->GetPassByName(hTechnique, pName);
}

D3DXHANDLE APIENTRY ID3DXEffectWrapper::GetFunction(UINT Index)
{
	LOG(7, "Effect GetFunction\n");
	return m_D3DXEffect->GetFunction(Index);
}

D3DXHANDLE APIENTRY ID3DXEffectWrapper::GetFunctionByName(LPCSTR pName)
{
	LOG(7, "Effect GetFunctionByName\n");
	return m_D3DXEffect->GetFunctionByName(pName);
}

D3DXHANDLE APIENTRY ID3DXEffectWrapper::GetAnnotation(D3DXHANDLE hObject, UINT Index)
{
	LOG(7, "Effect GetAnnotation\n");
	return m_D3DXEffect->GetAnnotation(hObject, Index);
}

D3DXHANDLE APIENTRY ID3DXEffectWrapper::GetAnnotationByName(D3DXHANDLE hObject, LPCSTR pName)
{
	LOG(7, "Effect GetAnnotationByName\n");
	return m_D3DXEffect->GetAnnotationByName(hObject, pName);
}

// Get/Set Parameters
HRESULT APIENTRY ID3DXEffectWrapper::SetValue(D3DXHANDLE hParameter, LPCVOID pData, UINT Bytes)
{
	LOG(7, "Effect SetValue\n");
	return m_D3DXEffect->SetValue(hParameter, pData, Bytes);
}

HRESULT APIENTRY ID3DXEffectWrapper::GetValue(D3DXHANDLE hParameter, LPVOID pData, UINT Bytes)
{
	LOG(7, "Effect GetValue\n");
	return m_D3DXEffect->GetValue(hParameter, pData, Bytes);
}

HRESULT APIENTRY ID3DXEffectWrapper::SetBool(D3DXHANDLE hParameter, BOOL b)
{
	LOG(7, "Effect SetBool\n");
	return m_D3DXEffect->SetBool(hParameter, b);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetBool(D3DXHANDLE hParameter, BOOL* pb)
{
	LOG(7, "Effect GetBool\n");
	return m_D3DXEffect->GetBool(hParameter, pb);
}
HRESULT APIENTRY ID3DXEffectWrapper::SetBoolArray(D3DXHANDLE hParameter, CONST BOOL* pb, UINT Count)
{
	LOG(7, "Effect SetBoolArray\n");
	return m_D3DXEffect->SetBoolArray(hParameter, pb, Count);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetBoolArray(D3DXHANDLE hParameter, BOOL* pb, UINT Count)
{
	LOG(7, "Effect GetBoolArray\n");
	return m_D3DXEffect->GetBoolArray(hParameter, pb, Count);
}
HRESULT APIENTRY ID3DXEffectWrapper::SetInt(D3DXHANDLE hParameter, INT n)
{
	LOG(7, "Effect SetInt\n");
	return m_D3DXEffect->SetInt(hParameter, n);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetInt(D3DXHANDLE hParameter, INT* pn)
{
	LOG(7, "Effect GetInt\n");
	return m_D3DXEffect->GetInt(hParameter, pn);
}
HRESULT APIENTRY ID3DXEffectWrapper::SetIntArray(D3DXHANDLE hParameter, CONST INT* pn, UINT Count)
{
	LOG(7, "Effect SetIntArray\n");
	return m_D3DXEffect->SetIntArray(hParameter, pn, Count);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetIntArray(D3DXHANDLE hParameter, INT* pn, UINT Count)
{
	LOG(7, "Effect GetIntArray\n");
	return m_D3DXEffect->GetIntArray(hParameter, pn, Count);
}
HRESULT APIENTRY ID3DXEffectWrapper::SetFloat(D3DXHANDLE hParameter, FLOAT f)
{
	LOG(7, "Effect SetFloat\n");
	return m_D3DXEffect->SetFloat(hParameter, f);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetFloat(D3DXHANDLE hParameter, FLOAT* pf)
{
	LOG(7, "Effect GetFloat\n");
	return m_D3DXEffect->GetFloat(hParameter, pf);
}
HRESULT APIENTRY ID3DXEffectWrapper::SetFloatArray(D3DXHANDLE hParameter, CONST FLOAT* pf, UINT Count)
{
	LOG(7, "Effect SetFloatArray\n");
	return m_D3DXEffect->SetFloatArray(hParameter, pf, Count);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetFloatArray(D3DXHANDLE hParameter, FLOAT* pf, UINT Count)
{
	LOG(7, "Effect GetFloatArray\n");
	return m_D3DXEffect->GetFloatArray(hParameter, pf, Count);
}
HRESULT APIENTRY ID3DXEffectWrapper::SetVector(D3DXHANDLE hParameter, CONST D3DXVECTOR4* pVector)
{
	LOG(7, "Effect SetVector %.2f %.2f %.2f %.2f\n", pVector->w, pVector->x, pVector->y, pVector->z);
	return m_D3DXEffect->SetVector(hParameter, pVector);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetVector(D3DXHANDLE hParameter, D3DXVECTOR4* pVector)
{
	LOG(7, "Effect GetVector\n");
	return m_D3DXEffect->GetVector(hParameter, pVector);
}
HRESULT APIENTRY ID3DXEffectWrapper::SetVectorArray(D3DXHANDLE hParameter, CONST D3DXVECTOR4* pVector, UINT Count)
{
	LOG(7, "Effect SetVectorArray\n");
	return m_D3DXEffect->SetVectorArray(hParameter, pVector, Count);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetVectorArray(D3DXHANDLE hParameter, D3DXVECTOR4* pVector, UINT Count)
{
	LOG(7, "Effect GetVectorArray\n");
	return m_D3DXEffect->GetVectorArray(hParameter, pVector, Count);
}
HRESULT APIENTRY ID3DXEffectWrapper::SetMatrix(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix)
{
	LOG(7, "Effect SetMatrix 0x%p\n", pMatrix);
	LOG(7, "%.2f %.2f %,2 %,2\n", pMatrix->_11, pMatrix->_12, pMatrix->_13, pMatrix->_14);
	LOG(7, "%.2f %.2f %,2 %,2\n", pMatrix->_21, pMatrix->_22, pMatrix->_23, pMatrix->_24);
	LOG(7, "%.2f %.2f %,2 %,2\n", pMatrix->_31, pMatrix->_32, pMatrix->_33, pMatrix->_34);
	LOG(7, "%.2f %.2f %,2 %,2\n", pMatrix->_41, pMatrix->_42, pMatrix->_43, pMatrix->_44);
	return m_D3DXEffect->SetMatrix(hParameter, pMatrix);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetMatrix(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix)
{
	LOG(7, "Effect GetMatrix\n");
	return m_D3DXEffect->GetMatrix(hParameter, pMatrix);
}
HRESULT APIENTRY ID3DXEffectWrapper::SetMatrixArray(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count)
{
	LOG(7, "Effect SetMatrixArray\n");
	return m_D3DXEffect->SetMatrixArray(hParameter, pMatrix, Count);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetMatrixArray(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count)
{
	LOG(7, "Effect GetMatrixArray\n");
	return m_D3DXEffect->GetMatrixArray(hParameter, pMatrix, Count);
}
HRESULT APIENTRY ID3DXEffectWrapper::SetMatrixPointerArray(D3DXHANDLE hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count)
{
	LOG(7, "Effect SetMatrixPointerArray\n");
	return m_D3DXEffect->SetMatrixPointerArray(hParameter, ppMatrix, Count);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetMatrixPointerArray(D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count)
{
	LOG(7, "Effect GetMatrixPointerArray\n");
	return m_D3DXEffect->GetMatrixPointerArray(hParameter, ppMatrix, Count);
}
HRESULT APIENTRY ID3DXEffectWrapper::SetMatrixTranspose(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix)
{
	LOG(7, "Effect SetMatrixTranspose\n");
	return m_D3DXEffect->SetMatrixTranspose(hParameter, pMatrix);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetMatrixTranspose(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix)
{
	LOG(7, "Effect GetMatrixTranspose\n");
	return m_D3DXEffect->GetMatrixTranspose(hParameter, pMatrix);
}
HRESULT APIENTRY ID3DXEffectWrapper::SetMatrixTransposeArray(D3DXHANDLE hParameter, CONST D3DXMATRIX* pMatrix, UINT Count)
{
	LOG(7, "Effect SetMatrixArray\n");
	return m_D3DXEffect->SetMatrixTransposeArray(hParameter, pMatrix, Count);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetMatrixTransposeArray(D3DXHANDLE hParameter, D3DXMATRIX* pMatrix, UINT Count)
{
	LOG(7, "Effect GetMatrixTransposeArray\n");
	return m_D3DXEffect->GetMatrixTransposeArray(hParameter, pMatrix, Count);
}
HRESULT APIENTRY ID3DXEffectWrapper::SetMatrixTransposePointerArray(D3DXHANDLE hParameter, CONST D3DXMATRIX** ppMatrix, UINT Count)
{
	LOG(7, "Effect SetMatrixTransposePointerArray\n");
	return m_D3DXEffect->SetMatrixTransposePointerArray(hParameter, ppMatrix, Count);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetMatrixTransposePointerArray(D3DXHANDLE hParameter, D3DXMATRIX** ppMatrix, UINT Count)
{
	LOG(7, "Effect GetMatrixTransposePointerArray\n");
	return m_D3DXEffect->GetMatrixTransposePointerArray(hParameter, ppMatrix, Count);
}
HRESULT APIENTRY ID3DXEffectWrapper::SetString(D3DXHANDLE hParameter, LPCSTR pString)
{
	LOG(7, "Effect SetString\n");
	return m_D3DXEffect->SetString(hParameter, pString);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetString(D3DXHANDLE hParameter, LPCSTR* ppString)
{
	LOG(7, "Effect GetString\n");
	return m_D3DXEffect->GetString(hParameter, ppString);
}
HRESULT APIENTRY ID3DXEffectWrapper::SetTexture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 pTexture)
{
	LOG(7, "Effect SetTexture\n");

	if (Settings::settingsIni.viewport == 3)
	{
		IDirect3DTexture9* ptext = NULL;
		if (pTexture)
		{
			if (SUCCEEDED(pTexture->QueryInterface(__uuidof(IDirect3DTexture9), (void**)&ptext)))
			{
				LOG(7, "Inside texturequeryinterface 0x%p\n", ptext);
				D3DSURFACE_DESC desc;
				ptext->GetLevelDesc(0, &desc);
				if (desc.Width == 256 && desc.Height == 1)
				{
					Settings::savedSettings.isFiltering = false;
				}
				ptext->Release();
			}
		}
	}

	return m_D3DXEffect->SetTexture(hParameter, pTexture);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetTexture(D3DXHANDLE hParameter, LPDIRECT3DBASETEXTURE9 *ppTexture)
{
	LOG(7, "Effect GetTexture\n");
	return m_D3DXEffect->GetTexture(hParameter, ppTexture);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetPixelShader(D3DXHANDLE hParameter, LPDIRECT3DPIXELSHADER9 *ppPShader)
{
	LOG(7, "Effect GetPixelShader\n");
	return m_D3DXEffect->GetPixelShader(hParameter, ppPShader);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetVertexShader(D3DXHANDLE hParameter, LPDIRECT3DVERTEXSHADER9 *ppVShader)
{
	LOG(7, "Effect GetVertexShader\n");
	return m_D3DXEffect->GetVertexShader(hParameter, ppVShader);
}

//Set Range of an Array to pass to device
//Usefull for sending only a subrange of an array down to the device
HRESULT APIENTRY ID3DXEffectWrapper::SetArrayRange(D3DXHANDLE hParameter, UINT uStart, UINT uEnd)
{
	LOG(7, "Effect SetArrayRange\n");
	return m_D3DXEffect->SetArrayRange(hParameter, uStart, uEnd);
}

// ID3DXBaseEffect

// Pool
HRESULT APIENTRY ID3DXEffectWrapper::GetPool(LPD3DXEFFECTPOOL* ppPool)
{
	LOG(7, "Effect GetPool\n");
	return m_D3DXEffect->GetPool(ppPool);
}

// Selecting and setting a technique
HRESULT APIENTRY ID3DXEffectWrapper::SetTechnique(D3DXHANDLE hTechnique)
{
	LOG(7, "Effect SetTechnique Handle: 0x%p\n", hTechnique);
	return m_D3DXEffect->SetTechnique(hTechnique);
}
D3DXHANDLE APIENTRY ID3DXEffectWrapper::GetCurrentTechnique()
{
	LOG(7, "Effect GetCurrentTechnique\n");
	return m_D3DXEffect->GetCurrentTechnique();
}
HRESULT APIENTRY ID3DXEffectWrapper::ValidateTechnique(D3DXHANDLE hTechnique)
{
	LOG(7, "Effect ValidateTechnique\n");
	return m_D3DXEffect->ValidateTechnique(hTechnique);
}
HRESULT APIENTRY ID3DXEffectWrapper::FindNextValidTechnique(D3DXHANDLE hTechnique, D3DXHANDLE *pTechnique)
{
	LOG(7, "Effect FindNextValidTechnique\n");
	return m_D3DXEffect->FindNextValidTechnique(hTechnique, pTechnique);
}
BOOL APIENTRY ID3DXEffectWrapper::IsParameterUsed(D3DXHANDLE hParameter, D3DXHANDLE hTechnique)
{
	LOG(7, "Effect IsParameterUsed\n");
	return m_D3DXEffect->IsParameterUsed(hParameter, hTechnique);
}

// Using current technique
// Begin           starts active technique
// BeginPass       begins a pass
// CommitChanges   updates changes to any set calls in the pass.   should be called before
//                 any DrawPrimitive call to d3d
// EndPass         ends a pass
// End             ends active technique
HRESULT APIENTRY ID3DXEffectWrapper::Begin(UINT *pPasses, DWORD Flags)
{
	return m_D3DXEffect->Begin(pPasses, Flags);
}
HRESULT APIENTRY ID3DXEffectWrapper::BeginPass(UINT Pass)
{
	return m_D3DXEffect->BeginPass(Pass);
}
HRESULT APIENTRY ID3DXEffectWrapper::CommitChanges()
{
	return m_D3DXEffect->CommitChanges();
}
HRESULT APIENTRY ID3DXEffectWrapper::EndPass()
{
	return m_D3DXEffect->EndPass();
}
HRESULT APIENTRY ID3DXEffectWrapper::End()
{
	return m_D3DXEffect->End();
}

// Managing D3D Device
HRESULT APIENTRY ID3DXEffectWrapper::GetDevice(LPDIRECT3DDEVICE9* ppDevice)
{
	LOG(7, "Effect GetDevice\n");
	return m_D3DXEffect->GetDevice(ppDevice);
}
HRESULT APIENTRY ID3DXEffectWrapper::OnLostDevice()
{
	LOG(7, "Effect OnLostDevice\n");
	return m_D3DXEffect->OnLostDevice();
}
HRESULT APIENTRY ID3DXEffectWrapper::OnResetDevice()
{
	LOG(7, "Effect OnResetDevice\n");
	return m_D3DXEffect->OnResetDevice();
}

// Logging device calls
HRESULT APIENTRY ID3DXEffectWrapper::SetStateManager(LPD3DXEFFECTSTATEMANAGER pManager)
{
	LOG(7, "Effect SetStateManager\n");
	return m_D3DXEffect->SetStateManager(pManager);
}
HRESULT APIENTRY ID3DXEffectWrapper::GetStateManager(LPD3DXEFFECTSTATEMANAGER *ppManager)
{
	LOG(7, "Effect GetStateManager\n");
	return m_D3DXEffect->GetStateManager(ppManager);
}

// Parameter blocks
HRESULT APIENTRY ID3DXEffectWrapper::BeginParameterBlock()
{
	return m_D3DXEffect->BeginParameterBlock();
}
D3DXHANDLE APIENTRY ID3DXEffectWrapper::EndParameterBlock()
{
	return m_D3DXEffect->EndParameterBlock();
}
HRESULT APIENTRY ID3DXEffectWrapper::ApplyParameterBlock(D3DXHANDLE hParameterBlock)
{
	return m_D3DXEffect->ApplyParameterBlock(hParameterBlock);
}
HRESULT APIENTRY ID3DXEffectWrapper::DeleteParameterBlock(D3DXHANDLE hParameterBlock)
{
	return m_D3DXEffect->DeleteParameterBlock(hParameterBlock);
}

// Cloning
HRESULT APIENTRY ID3DXEffectWrapper::CloneEffect(LPDIRECT3DDEVICE9 pDevice, LPD3DXEFFECT* ppEffect)
{
	return m_D3DXEffect->CloneEffect(pDevice, ppEffect);
}

// Fast path for setting variables directly in ID3DXEffect
HRESULT APIENTRY ID3DXEffectWrapper::SetRawValue(D3DXHANDLE hParameter, LPCVOID pData, UINT ByteOffset, UINT Bytes)
{
	return m_D3DXEffect->SetRawValue(hParameter, pData, ByteOffset, Bytes);
}