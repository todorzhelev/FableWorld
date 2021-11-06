#pragma once
#include "IBaseScene.h"
#include "Label.h"
#include "TextManager.h"
#include "DirectInput.h"
#include "Misc.h"


//this scenes contains all the things for the menu in game.This menu will appear if in game the games presses escape
class MenuInGame: public IBaseScene {
public:

	MenuInGame();

	virtual void OnLostDevice() override;
	virtual void OnResetDevice() override;

	virtual void OnUpdate(float dt) override;
	virtual void OnRender() override;

	virtual LRESULT MsgProc(UINT msg, WPARAM wParam, LPARAM lParam) override;

private:

	Label* m_pLabelContinueGame;
	Label* m_pLabelQuit;
};