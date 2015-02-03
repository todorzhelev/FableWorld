#pragma once
#include"Misc.h"
#include"IBaseMenuObject.h"

class IBase2DMenuObject : public IBaseMenuObject
{
public:
	
	IBase2DMenuObject() {}

	IBase2DMenuObject(string strText, bool bIsVisible, int nWidth, int nHeight, D3DXVECTOR2 vPos, bool bIsMouseDown = false, bool bIsMouseOver = false);

	virtual void OnLostDevice() = 0;

	virtual void OnResetDevice() = 0;
	
	virtual bool IsMouseOver() = 0;

	virtual bool IsMouseDown() = 0;

	virtual void OnUpdate() = 0;

	virtual void OnRender() {};

	virtual bool IsClicked() { return false; }


protected:

	ID3DXSprite* m_pSprite;

};

