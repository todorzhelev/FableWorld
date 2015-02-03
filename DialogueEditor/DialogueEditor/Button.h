#pragma once
#include "DirectInput.h"
#include "Misc.h"
#include "IBase2DMenuObject.h"

/*
this class contains the functions for creating button, checking if it is pressed and rendering
Button has the functionality to load image and can check if the mouse is over that image. Then the image will be switched with another
simulating mouseover also will trigger some events if the button is selected( for instance start ability of hero)
*/
class Button: public IBase2DMenuObject
{
public:
	virtual void OnLostDevice();
	virtual void OnResetDevice();
	
	virtual bool IsMouseOver();
	virtual bool IsMouseDown();

	virtual void OnUpdate();
	virtual void OnRender();

	virtual bool IsClicked();

	void		 Init();
};

	
