#pragma once
#include<string>
#include<windows.h>
using namespace std;

class Scenes
{
public:
	virtual bool checkDeviceCaps() { return true; }
	virtual void onLostDevice() {}
	virtual void onResetDevice() {}
	virtual void onUpdate(float dt) {}
	virtual void onRender() {}
	virtual LRESULT msgProc(UINT msg, WPARAM wParam, LPARAM lParam) { return 1;}

};
extern Scenes* scenes;

