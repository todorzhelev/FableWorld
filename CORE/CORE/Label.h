#pragma once
#include"DirectInput.h"
#include"Misc.h"
#include"IBaseMenuObject.h"


class Label: public IBaseMenuObject
{
public:

	Label(D3DXVECTOR2 vPosition, std::string strText);

	void Init();

	virtual void OnLostDevice();
	virtual void OnResetDevice();

	virtual bool IsMouseOver();
	virtual bool IsMouseDown();

	virtual void OnUpdate();
	virtual void OnRender(int a,int r,int g,int b);
};