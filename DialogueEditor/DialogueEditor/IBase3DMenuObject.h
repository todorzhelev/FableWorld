#pragma once
#include"Misc.h"
#include"IBaseMenuObject.h"
#include"Camera.h"

class IBase3DMenuObject : public IBaseMenuObject
{
public:
	
	IBase3DMenuObject() {}
	IBase3DMenuObject(string strText, bool bIsVisible, int nWidth, int nHeight, D3DXVECTOR2 vPos, bool bIsMouseDown = false, bool bIsMouseOver = false);

	virtual void OnLostDevice() = 0;

	virtual void OnResetDevice() = 0;
	
	virtual bool IsMouseOver() = 0;

	virtual bool IsMouseDown() = 0;

	virtual void OnUpdate() = 0;

	virtual void OnRender() = 0;

	virtual bool IsClicked() { return false; }

	virtual void InitShader();

public:

	D3DXVECTOR3 m_vLook;
	D3DXVECTOR3 m_vRight;
	D3DXVECTOR3 m_vUp;

	ID3DXEffect* m_pShaderEffect;
	D3DXHANDLE 	 m_hShaderTechnique;
	D3DXHANDLE 	 m_hShaderWVPMatrix;

	bool		 m_bIsPicked;

};

