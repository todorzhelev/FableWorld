#include <stdafx.h>
#include "DirectInput.h"

//////////////////////////////////////////////////////////////////////////////
//TODO: should get rid of all this when migrating to DX12

/////////////////////////////////////////////////////////////////////////

//acquires keyboard and mouse devices
DirectInput::DirectInput(DWORD keyboardCoopFlags, DWORD mouseCoopFlags) {
	ZeroMemory(m_keyboardState, sizeof(m_keyboardState));
	ZeroMemory(&m_mouseState, sizeof(m_mouseState));

	CheckSuccess(DirectInput8Create(pApp->GetAppInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, 0));

	//Keyboard
	m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboad, 0);
	m_pKeyboad->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyboad->SetCooperativeLevel(pApp->GetMainWindow(), keyboardCoopFlags);
	m_pKeyboad->Acquire();

	//Mouse
	m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse, 0);
	m_pMouse->SetDataFormat(&c_dfDIMouse2);
	m_pMouse->SetCooperativeLevel(pApp->GetMainWindow(), mouseCoopFlags);
	m_pMouse->Acquire();

	printf("DirectInput created\n");
}

/////////////////////////////////////////////////////////////////////////

DirectInput::~DirectInput() {
	ReleaseX(m_pDirectInput);
	m_pKeyboad->Unacquire();
	ReleaseX(m_pKeyboad);
	m_pMouse->Unacquire();
	ReleaseX(m_pMouse);
}

/////////////////////////////////////////////////////////////////////////

//starting to poll for user input if the devices are accessible. if not it tries to acquire them
void DirectInput::Poll() {
	//Keyboard
	HRESULT hResult = m_pKeyboad->GetDeviceState(sizeof(m_keyboardState), (void**)&m_keyboardState); 

	if (FAILED(hResult)) {
		m_pKeyboad->Acquire();
	}

	// Poll mouse.
	hResult = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE2), (void**)&m_mouseState); 

	if (FAILED(hResult)) {
		m_pMouse->Acquire(); 
	}
}

/////////////////////////////////////////////////////////////////////////

//test if key is pressed
//we make bitwise and with 0x80, because according to microsoft the high-order bit in 
//keyboardstate[key] must be set(i.e. be 1) if the key is down.
//by making bitwise AND with 0x80, we are clearing the other bits and leaving only the high-order bit to be set if 
//in the previous value is again set.
//example: keyboardstate[key] = 1001 1100 & 1000 0000(0x80)  = 1000 0000 -> the high order bit is set, therefore the key is down
bool DirectInput::IsKeyDown(char key) {
	return (m_keyboardState[key] & 0x80) != 0;
}

/////////////////////////////////////////////////////////////////////////

bool DirectInput::IsMouseButtonDown(int button) {
	return (m_mouseState.rgbButtons[button] & 0x80) != 0;
}

/////////////////////////////////////////////////////////////////////////

bool DirectInput::IsMouseButtonUp(int button) {
	return (m_mouseState.rgbButtons[button] & 0x80) == 0;
}

/////////////////////////////////////////////////////////////////////////

//returns lX for the mouse used in rotation.
float DirectInput::GetMouseDX() {
	return (float)m_mouseState.lX;
}

/////////////////////////////////////////////////////////////////////////

//returns lY for the mouse used in rotation.
float DirectInput::GetMouseDY() {
	return (float)m_mouseState.lY;
}

/////////////////////////////////////////////////////////////////////////