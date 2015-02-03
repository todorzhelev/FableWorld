#include "Menu.h"
#include "Game.h"
#include "../../../TheCore/TheCore/TheCore/TextManager.h"

/*
SourceFile:Menu.cpp
Purpose:loads the logo and labels for the menu and renders them
*/


/////////////////////////////////////////////////////////////////////////
/*
Function:Menu
Purpose:constructor. It loads all the functionality in the menu
*/
Menu::Menu()
{
	
	//Dinput is responsibly for mouse move detection, key pressed detection and etc.
	pDinput = new DirectInput(DISCL_NONEXCLUSIVE|DISCL_FOREGROUND, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND);

	pTextManager = new TextManager;

	D3DXCreateSprite(pDxDevice,&m_pSpriteForLogo);
	D3DXCreateTextureFromFile(pDxDevice,"../../../Resources/textures/logo_FableWorld.jpg",&m_pLogoInWindowTexture);
	D3DXCreateTextureFromFile(pDxDevice,"../../../Resources/textures/logo_FableWorldFullscreen.jpg",&m_pLogoInFullscreenTexture);

	float posx = static_cast<float>(pEngine->GetPresentParameters().BackBufferWidth/2);
	float posy = static_cast<float>(pEngine->GetPresentParameters().BackBufferHeight/2);

	m_pLabelStartGame = new Label(D3DXVECTOR2(posx-40.0f,posy-20.0f),"Start game");
	m_pLabelStartGame->SetVisible(true);

	m_pLabelQuit = new Label(D3DXVECTOR2(posx-40.0f,posy+20.0f),"Quit");
	m_pLabelQuit->SetVisible(true);

	OnResetDevice();
}

/////////////////////////////////////////////////////////////////////////
/*
Function:onLostDevice
Purpose:invokes the corresponding OnLostDevice functions
*/
void Menu::OnLostDevice()
{
	pTextManager->OnLostDevice();
	m_pSpriteForLogo->OnLostDevice();
}


/////////////////////////////////////////////////////////////////////////
/*
Function:onResetDevice
Purpose:invokes the corresponding OnResetDevice functions
*/
void Menu::OnResetDevice()
{
	pTextManager->OnResetDevice();
	m_pSpriteForLogo->OnResetDevice();
	float w = (float)pEngine->GetPresentParameters().BackBufferWidth;
	float h = (float)pEngine->GetPresentParameters().BackBufferHeight;

	m_pLabelStartGame->SetPosition(D3DXVECTOR2(w/2-40.0f,h/2-20.0f));
	m_pLabelQuit->SetPosition(D3DXVECTOR2(w/2-40.0f,h/2+20.0f));
}


/////////////////////////////////////////////////////////////////////////
/*
Function:onUpdate
Purpose:updates all the functionallity in menu
*/
void Menu::OnUpdate(float dt)
{
	pDinput->Poll();
	if( pDinput->IsKeyDown(DIK_F))
	{
		pEngine->SwitchToFullscreen(true);
	}

	m_pLabelStartGame->OnUpdate();
	m_pLabelQuit->OnUpdate();

	if(m_pLabelStartGame->IsMouseDown())
	{
		pEngine->GetScene("menu")->OnLostDevice();

		IBaseScene* pGameScene = pEngine->GetScene("heroSelect");
		pEngine->SetCurrentScene(pGameScene);
	}

	if(m_pLabelQuit->IsMouseDown())
	{
		PostQuitMessage(0);
	}

}


/////////////////////////////////////////////////////////////////////////
/*
Function:onRender
Purpose:render all
*/
void Menu::OnRender()
{
	pDxDevice->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
	pDxDevice->BeginScene();
	D3DXVECTOR3 posForLogo(0.0,0.0,0.0);
	m_pSpriteForLogo->Begin(D3DXSPRITE_ALPHABLEND);
		
	if(pEngine->GetPresentParameters().Windowed)
	{
		m_pSpriteForLogo->Draw(m_pLogoInWindowTexture,NULL,NULL,&posForLogo,D3DXCOLOR(255,255,255,255));
	}
	else
	{
		m_pSpriteForLogo->Draw(m_pLogoInFullscreenTexture,NULL,NULL,&posForLogo,D3DXCOLOR(255,255,255,255));
	}
	m_pSpriteForLogo->End();

		///text->drawText("Completed",500,40,0,0,255,255,255,0);
	if(!m_pLabelStartGame->IsMouseOver())
	{
		m_pLabelStartGame->OnRender(255,255,255,255);
	}
	else
	{
		m_pLabelStartGame->OnRender(150,255,255,255);
	}

	if(!m_pLabelQuit->IsMouseOver())
	{
		m_pLabelQuit->OnRender(255,255,255,255);
	}
	else
	{
		m_pLabelQuit->OnRender(150,255,255,255);
	}

	pDxDevice->EndScene();
	pDxDevice->Present(0, 0, 0, 0);
}


/////////////////////////////////////////////////////////////////////////
/*
Function:msgProc
Purpose:this function detects various messages sent to the window like WM_CLOSE, WM_ACTIVATE and etc.
		Used mainly for the textbox and camera mode switching.Isnt used here, because DirectInput works fine for the things we need in menu
*/
LRESULT Menu::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	
	return DefWindowProc(pEngine->GetMainWindow(), msg, wParam, lParam);
}


/////////////////////////////////////////////////////////////////////////


	
