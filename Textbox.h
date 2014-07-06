#pragma once
#include"DirectInput.h"
#include"Misc.h"
#include "IBaseMenuObject.h"

using namespace std;

/*
This class contains all the functions for rendering textbox and maintaining his content
Textbox consist of image which is the area of the textbox and text inside
*/
class Textbox:public IBaseMenuObject
{
public:
	
	void Init();

	void OnLostDevice();
	void OnResetDevice();
	
	bool IsMouseOver();
	bool IsMouseDown();

	void OnUpdate();
	void OnRender();

};