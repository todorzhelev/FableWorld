#pragma once
#include"DirectInput.h"
#include"TextManager.h"
#include"Misc.h"
#include "IBase2DMenuObject.h"

using namespace std;

/*
This class contains all the functions for rendering textbox and maintaining his content
Textbox consist of image which is the area of the textbox and text inside
*/
class Textbox: public IBase2DMenuObject
{
public:

	Textbox() {}
	void Init();

	void OnLostDevice();
	void OnResetDevice();
	
	bool IsMouseOver();
	bool IsMouseDown();

	
	void OnClicked();

	virtual void OnUpdate();
	//virtual void OnRender(int a,int r,int g,int b);
	virtual void OnRender();

};