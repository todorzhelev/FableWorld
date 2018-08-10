#pragma once

class IBaseScene
{
public:
	virtual void OnLostDevice() = 0;

	virtual void OnResetDevice() = 0;

	virtual void OnUpdate(float dt) = 0;

	virtual void OnRender() = 0;

	//this function detects various messages sent to the window like WM_CLOSE, WM_ACTIVATE key pressed and etc.
	virtual LRESULT MsgProc(UINT msg, WPARAM wParam, LPARAM lParam) { return 1;}
};

