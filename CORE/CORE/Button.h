#pragma once
#include "DirectInput.h"
#include "Misc.h"
#include "IBaseMenuObject.h"

class Button: public IBaseMenuObject
{
public:

	Button(D3DXVECTOR2 vPosition, int nWidth, int nHeight, std::string strText, std::string strIdleStateTexFileName, std::string strMouseOverStateTexFileName);

	virtual void OnLostDevice();
	virtual void OnResetDevice();
	
	virtual bool IsMouseOver();
	virtual bool IsMouseDown();

	virtual void OnUpdate();
	virtual void OnRender();

	void	OnRender(int a,int r,int g,int b);

	virtual bool IsClicked();

	void		 Init();
};

	
