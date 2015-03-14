#pragma once
#include "../../CORE/CORE/IBaseScene.h"
#include "../../CORE/CORE/DirectInput.h"
#include "../../CORE/CORE/Misc.h"
#include "Menu.h"
#include "../../CORE/CORE/Label.h"
using namespace std;

//this is the menu scene and contains all the thing needed in the menu
class Menu: public IBaseScene
{
public:

	Menu();

	virtual void OnLostDevice();

	virtual void OnResetDevice();

	virtual void OnUpdate(float dt);

	virtual void OnRender();

	LRESULT		 MsgProc(UINT msg, WPARAM wParam, LPARAM lParam);

private:

	Label* m_pLabelStartGame;
	Label* m_pLabelQuit;

	ID3DXSprite*	   m_pSpriteForLogo;
	IDirect3DTexture9* m_pLogoInFullscreenTexture;
	IDirect3DTexture9* m_pLogoInWindowTexture;
};