#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <dinputd.h>
#include "Misc.h"

//////////////////////////////////////////////////////////////////////////////

class DirectInput {
public:
	DirectInput(DWORD keyboardCoopFlags, DWORD mouseCoopFlags);
	~DirectInput();
	void  Poll();
	bool  IsKeyDown(char key);
	bool  IsMouseButtonDown(int button);
	bool  IsMouseButtonUp(int button);
	float GetMouseDX();
	float GetMouseDY();

private:
	IDirectInput8* m_pDirectInput;
	//keyboard
	IDirectInputDevice8* m_pKeyboad;
	//this array stores the states of every key on the keyboard
	char m_keyboardState[256]; 
	//mouse
	IDirectInputDevice8* m_pMouse;
	DIMOUSESTATE2 m_mouseState;	
};

//////////////////////////////////////////////////////////////////////////////





