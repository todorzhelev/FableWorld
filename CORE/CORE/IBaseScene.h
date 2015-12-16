#pragma once
#include<string>
#include<windows.h>
using namespace std;

class IBaseScene
{
public:
	virtual void OnLostDevice() {}

	virtual void OnResetDevice() {}

	virtual void OnUpdate(float dt) {}

	virtual void OnRender() {}

	//this function detects various messages sent to the window like WM_CLOSE, WM_ACTIVATE key pressed and etc.
	virtual LRESULT MsgProc(UINT msg, WPARAM wParam, LPARAM lParam) { return 1;}
};

