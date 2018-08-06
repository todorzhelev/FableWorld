#pragma once
#include"DirectInput.h"
#include"TextManager.h"
#include"Misc.h"
#include "IBaseMenuObject.h"

class Textbox: public IBaseMenuObject
{
public:

	Textbox(D3DXVECTOR2 vPosition, int nWidth, int nHeight, std::string strText,std::string strIdleStateTexFileName, std::string strMouseOverStateTexFileName,std::string strId);

	void Init();

	void OnLostDevice();
	void OnResetDevice();
	
	bool IsMouseOver();
	bool IsMouseDown();

	
	void OnClicked();

	virtual void OnUpdate();
	virtual void OnRender(int a,int r,int g,int b);
	virtual void OnRender();
};