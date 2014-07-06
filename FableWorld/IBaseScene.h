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

	virtual LRESULT MsgProc(UINT msg, WPARAM wParam, LPARAM lParam) { return 1;}
};

