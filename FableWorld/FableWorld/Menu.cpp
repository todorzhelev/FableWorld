#include <stdafx.h>
#include "Menu.h"
#include "Game.h"
#include "../../CORE/CORE/TextManager.h"

/////////////////////////////////////////////////////////////////////////

Menu::Menu()
{
	//Dinput is responsible for mouse move detection, key pressed detection and etc.
	pDinput = new DirectInput(DISCL_NONEXCLUSIVE|DISCL_FOREGROUND, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND);

	pTextManager = new TextManager;

	D3DXCreateSprite(pDxDevice,&m_pSpriteForLogo);
	D3DXCreateTextureFromFile(pDxDevice,"../../Resources/textures/Logo/logo_FableWorld.jpg",&m_pLogoInWindowTexture);
	D3DXCreateTextureFromFile(pDxDevice,"../../Resources/textures/Logo/logo_FableWorldFullscreen.jpg",&m_pLogoInFullscreenTexture);

	float posx = static_cast<float>(pApp->GetPresentParameters().BackBufferWidth/2);
	float posy = static_cast<float>(pApp->GetPresentParameters().BackBufferHeight/2);

	m_pLabelStartGame = new Label(D3DXVECTOR2(posx-40.0f,posy-20.0f),"Start game");
	m_pLabelStartGame->SetVisible(true);

	m_pLabelQuit = new Label(D3DXVECTOR2(posx-40.0f,posy+20.0f),"Quit");
	m_pLabelQuit->SetVisible(true);

	OnResetDevice();
}

/////////////////////////////////////////////////////////////////////////

void Menu::OnLostDevice()
{
	pTextManager->OnLostDevice();
	m_pSpriteForLogo->OnLostDevice();
}

/////////////////////////////////////////////////////////////////////////

void Menu::OnResetDevice()
{
	pTextManager->OnResetDevice();
	m_pSpriteForLogo->OnResetDevice();
	float w = (float)pApp->GetPresentParameters().BackBufferWidth;
	float h = (float)pApp->GetPresentParameters().BackBufferHeight;

	m_pLabelStartGame->SetPosition(D3DXVECTOR2(w/2-40.0f,h/2-20.0f));
	m_pLabelQuit->SetPosition(D3DXVECTOR2(w/2-40.0f,h/2+20.0f));
}

/////////////////////////////////////////////////////////////////////////

void Menu::OnUpdate(float dt)
{
	pDinput->Poll();
	if( pDinput->IsKeyDown(DIK_F))
	{
		pApp->SwitchToFullscreen(true);
	}

	m_pLabelStartGame->OnUpdate();
	m_pLabelQuit->OnUpdate();

	if(m_pLabelStartGame->IsMouseDown())
	{
		pApp->GetScene("menu")->OnLostDevice();

		IBaseScene* pGameScene = pApp->GetScene("heroSelect");
		pApp->SetCurrentScene(pGameScene);
	}

	if(m_pLabelQuit->IsMouseDown())
	{
		PostQuitMessage(0);
	}

}


/////////////////////////////////////////////////////////////////////////

void Menu::OnRender()
{
	pDxDevice->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
	pDxDevice->BeginScene();
	D3DXVECTOR3 posForLogo(0.0,0.0,0.0);
	m_pSpriteForLogo->Begin(D3DXSPRITE_ALPHABLEND);
		
	if(pApp->GetPresentParameters().Windowed)
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

LRESULT Menu::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	
	return DefWindowProc(pApp->GetMainWindow(), msg, wParam, lParam);
}


/////////////////////////////////////////////////////////////////////////


	
