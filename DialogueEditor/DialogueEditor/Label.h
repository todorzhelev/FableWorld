#pragma once
#include"DirectInput.h"
#include"Misc.h"
#include"IBase2DMenuObject.h"

/*
this class contains functions for creating, checking if mouse is over or down and rendering a label
Label is text which can be tested for mouse over and mouse down. Used in the dialogue system and menu
*/
class Label: public IBase2DMenuObject
{
public:
	void Init();

	Label(string strText, bool bIsVisible, int nWidth, int nHeight, D3DXVECTOR2 vPos, bool bIsMouseDown = false, bool bIsMouseOver = false);

	virtual void OnLostDevice();
	virtual void OnResetDevice();

	virtual bool IsMouseOver();
	virtual bool IsMouseDown();

	virtual void OnUpdate();
	void OnRender(int a,int r,int g,int b);
};