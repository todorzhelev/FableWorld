#include "DirectInput.h"

//////////////////////////////////////////////////////////////////////////////

DirectInput* pDinput = NULL;

/////////////////////////////////////////////////////////////////////////
/*
Function:DirectInput
Purpose:constructor. acquires keyboard and mouse devices
*/
DirectInput::DirectInput(DWORD keyboardCoopFlags, DWORD mouseCoopFlags)
{
	ZeroMemory(m_keyboardState, sizeof(m_keyboardState));
	ZeroMemory(&m_mouseState, sizeof(m_mouseState));

	if( FAILED(DirectInput8Create(pEngine->GetAppInstance(), DIRECTINPUT_VERSION,IID_IDirectInput8, (void**)&m_pDirectInput, 0)) )
	{
		MessageBox(0,"Failed Creating Dinput8",0,0);
	}

	//Keyboard
	m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboad, 0);
	m_pKeyboad->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyboad->SetCooperativeLevel(pEngine->GetMainWindow(), keyboardCoopFlags);
	m_pKeyboad->Acquire();

	//Mouse
	m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse, 0);
	m_pMouse->SetDataFormat(&c_dfDIMouse2);
	m_pMouse->SetCooperativeLevel(pEngine->GetMainWindow(), mouseCoopFlags);
	m_pMouse->Acquire();

}


/////////////////////////////////////////////////////////////////////////
/*
Function:~DirectInput
Purpose:destructor
*/
DirectInput::~DirectInput()
{
	releaseX(m_pDirectInput);
	m_pKeyboad->Unacquire();
	releaseX(m_pKeyboad);
	m_pMouse->Unacquire();
	releaseX(m_pMouse);
}



/////////////////////////////////////////////////////////////////////////
/*
Function:Poll
Purpose:starting to poll for user input if the devices are accessible. if not it tries to acquire them
*/
void DirectInput::Poll()
{
	//Keyboard
	HRESULT hResult = m_pKeyboad->GetDeviceState(sizeof(m_keyboardState), (void**)&m_keyboardState); 

	if( FAILED(hResult) )
	{
		m_pKeyboad->Acquire();
	}

	// Poll mouse.
	hResult = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE2), (void**)&m_mouseState); 

	if( FAILED(hResult) )
	{
		m_pMouse->Acquire(); 
	}
}


/////////////////////////////////////////////////////////////////////////
/*
Function:IsKeyDown
Purpose:test if key is pressed
		we make bitwise and with 0x80, because according to microsoft the high-order bit in 
		keyboardstate[key] must be set(i.e. be 1) if the key is down.
		by making bitwise AND with 0x80, we are clearing the other bits and leaving only the high-order bit to be set if 
		in the previous value is again set.
		example: keyboardstate[key] = 1001 1100 & 1000 0000(0x80)  = 1000 0000 -> the high order bit is set, therefore the key is down
*/
bool DirectInput::IsKeyDown(char key)
{
	return (m_keyboardState[key] & 0x80) != 0;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:IsMouseButtonDown
Purpose:test if mouse button is down
*/
bool DirectInput::IsMouseButtonDown(int button)
{
	return (m_mouseState.rgbButtons[button] & 0x80) != 0;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:IsMouseButtonUp
Purpose:test if mouse button is up
*/
bool DirectInput::IsMouseButtonUp(int button)
{
	return (m_mouseState.rgbButtons[button] & 0x80) == 0;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:GetMouseDX
Purpose:returns lX for the mouse used in rotation.
*/
float DirectInput::GetMouseDX()
{
	return (float)m_mouseState.lX;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:mouseDY
Purpose:returns lY for the mouse used in rotation.
*/
float DirectInput::GetMouseDY()
{
	return (float)m_mouseState.lY;
}


/////////////////////////////////////////////////////////////////////////