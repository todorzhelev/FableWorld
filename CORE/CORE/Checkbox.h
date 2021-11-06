#pragma once
#include "DirectInput.h"
#include "Misc.h"
#include "TextManager.h"
#include "IBaseMenuObject.h"

/*
this class contains the functions for creating checkbox, checking if it is pressed and rendering
*/
class Checkbox: public IBaseMenuObject {
public:
	Checkbox(D3DXVECTOR2 vPosition, int nWidth, int nHeight, std::string strText, std::string strIdleStateTexFileName, std::string strMouseOverStateTexFileName,std::string strId);

	void Init();

	virtual void OnLostDevice();
	virtual void OnResetDevice();
	
	virtual bool IsMouseOver();
	virtual bool IsMouseDown();

	virtual void OnClicked();

	virtual void OnUpdate();
	virtual void OnRender(int a,int r,int g,int b);


	bool m_bIsChecked;

	private:
};

	
