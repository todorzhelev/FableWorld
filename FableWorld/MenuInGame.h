#pragma once
#include "../../../TheCore/TheCore/TheCore/IBaseScene.h"
#include "../../../TheCore/TheCore/TheCore/Label.h"
#include "../../../TheCore/TheCore/TheCore/TextManager.h"
#include "../../../TheCore/TheCore/TheCore/DirectInput.h"
#include "../../../TheCore/TheCore/TheCore/Misc.h"
using namespace std;

//this scenes contains all the things for the menu in game.This menu will appear if in game the games presses escape
class MenuInGame: public IBaseScene
{
public:

	MenuInGame();

	void	OnLostDevice();
	void	OnResetDevice();

	void	OnUpdate(float dt);
	void	OnRender();

	LRESULT MsgProc(UINT msg, WPARAM wParam, LPARAM lParam);

private:

	Label* m_pLabelContinueGame;
	Label* m_pLabelQuit;
};