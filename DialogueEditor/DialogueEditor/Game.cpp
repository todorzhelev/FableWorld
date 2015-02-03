#include"Game.h"
#include "Button3D.h"
#include "Button.h"
#include "Tree.h"
//#include "IBase3DMenuObject.h"
#include "Textbox.h"
#include<math.h>

#include "StaticMesh.h"
/*
SourceFile:Game.cpp
Purpose:loads the game models, terrain,skybox, sounds,dialogues for the game, updates and renders them.
*/

ofstream fout; //for logs.

/////////////////////////////////////////////////////////////////////////
/*
Function:Game
Purpose:constructor. It loads all the functionality in the game
*/
Game::Game()
{
	pDinput = new DirectInput(DISCL_NONEXCLUSIVE|DISCL_FOREGROUND, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND);

	pTextManager = new TextManager;

	float fWidth  = (float)pApp->GetPresentParameters().BackBufferWidth;
	float fHeight = (float)pApp->GetPresentParameters().BackBufferHeight;

	camera = new Camera(D3DX_PI * 0.25f, fWidth/fHeight, 1.0f, 2000.0f);
	camera->SetCameraMode(ECameraMode_MoveWithPressedMouse);

		
	float posx = static_cast<float>(pApp->GetPresentParameters().BackBufferWidth/2);
	float posy = static_cast<float>(pApp->GetPresentParameters().BackBufferHeight/2);

	string strLabelString = "StartGame";
	m_pLabelStartGame = new Label(strLabelString, true,
								  static_cast<int>(GetStringHeight(strLabelString)),
								  static_cast<int>(GetStringWidth(strLabelString)),
								  D3DXVECTOR2(posx-40.0f,posy-20.0f),false,false);
	

	//Initialize the vertex declarations. They are needed for creating the terrain, models and etc.
	InitVertexDeclarations();


	pTextManager->CreateFontFor3DText();

	m_pTestButton = new Button3D;
	string strText = "omgfffffffffffffffffffffffffffffffffffffffhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh";

	pTextManager->CreateMeshFor3DText(strText,m_pTestButton->m_pTextMesh);
	m_pTestButton->SetText(strText);
	m_pTestButton->InitShader();
	m_pTestButton->m_pBackgroundMesh = new StaticMesh("land","models/background.x","grass-texture-02.dds");
	m_pTestButton->m_pBackgroundMesh->LoadGameObject();
	pApp->AddUIObject(m_pTestButton);

	m_pTree = new Tree;

	node* pNode = new node;
	pNode->m_pButton = m_pTestButton;
	pNode->m_nKey = 1;
	pNode->m_strText = "root";

	m_pTree->SetRoot(pNode);


	float width  = (float)pApp->GetPresentParameters().BackBufferWidth;
	float height = (float)pApp->GetPresentParameters().BackBufferHeight;

	//bind to animated model's bone textbox
	m_pTestTextbox = new Textbox;

	m_pTestTextbox->SetIdleStateTextureFileName("textures/GUI/Textbox.dds");
	m_pTestTextbox->SetMouseOverStateTextureFileName("textures/GUI/Textbox_selected.dds");
	m_pTestTextbox->SetID("testTextbox");
	m_pTestTextbox->SetVisible(true);
	m_pTestTextbox->SetHeight(32);
	m_pTestTextbox->SetWidth(256);
	m_pTestTextbox->SetPosition(D3DXVECTOR2(width-300, 310));
	m_pTestTextbox->Init();
	m_pTestTextbox->SetSelected(false);
	pApp->AddUIObject(m_pTestTextbox);

	OnResetDevice();
}



/////////////////////////////////////////////////////////////////////////
/*
Function:~Game
Purpose:destructor
*/
Game::~Game()
{
}

/////////////////////////////////////////////////////////////////////////
/*
Function:onLostDevice
Purpose:invokes the corresponding OnLostDevice functions
*/
void Game::OnLostDevice()
{
	pTextManager->OnLostDevice();
	for(auto it = pApp->m_vUIObjects.begin();it!=pApp->m_vUIObjects.end();it++)
	{
		(*it)->OnLostDevice();
	}
}


/////////////////////////////////////////////////////////////////////////
/*
Function:onResetDevice
Purpose:invokes the corresponding onResetDevice functions
*/
void Game::OnResetDevice()
{
	pTextManager->OnResetDevice();
	for(auto it = pApp->m_vUIObjects.begin();it!=pApp->m_vUIObjects.end();it++)
	{
		(*it)->OnResetDevice();
	}
}


/////////////////////////////////////////////////////////////////////////
/*
Function:onUpdate
Purpose:updates all the functionallity in game
*/
void Game::OnUpdate(float dt)
{
	pDinput->Poll();

	//m_pTestObject->OnUpdate(dt);

	//update all the UI objects
	for(auto it = pApp->m_vUIObjects.begin();it!=pApp->m_vUIObjects.end();it++)
	{
		(*it)->OnUpdate();
	}

	pTextManager->OnUpdate(dt);
	camera->OnUpdate(dt);
	camera->MoveCamera(dt);

	//if( m_pTestButton->IsClicked() )
	{
		string str = m_pTestButton->GetText();
		m_pTestTextbox->SetText(m_pTestButton->GetText() );
	}
	m_pLabelStartGame->OnUpdate();

	//m_pTestButton->OnUpdate();
}
	

/////////////////////////////////////////////////////////////////////////
/*
Function:OnRender
Purpose:render all
*/
void Game::OnRender()
{

    pDxDevice->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);

    pDxDevice->BeginScene();
	
	for(auto it = pApp->m_vUIObjects.begin();it!=pApp->m_vUIObjects.end();it++)
	{
		(*it)->OnRender();
	}

	//m_pTestButton->OnRender();

	pDxDevice->EndScene();

	pDxDevice->Present(0, 0, 0, 0);
}


/////////////////////////////////////////////////////////////////////////
/*
Function:MsgProc
Purpose:this function detects various messages sent to the window like WM_CLOSE, WM_ACTIVATE and etc.
		Used mainly for the textbox and camera mode switching
*/
LRESULT Game::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
		//Sent when the window is activated or deactivated.(pressed alt+tab)
		//Game is paused when the window is inactive, and unpaused when become active again
		case WM_ACTIVATE:
		{
			if( LOWORD(wParam) == WA_INACTIVE )
			{
				pApp->SetPaused(true);
			}
			else
			{
				pApp->SetPaused(false);
			}

			return 0;
		}


		//Sent when the user closes the window
		case WM_CLOSE:
		{
			DestroyWindow( pApp->GetMainWindow() );
		
			return 0;
		}

		//Sent when the window is destroyed
		case WM_DESTROY:
		{
			PostQuitMessage(0);

			return 0;
		}

		case WM_KEYDOWN:
		{
			switch(wParam)
			{
				case 'L':
				{
					if( camera->GetCameraMode() == ECameraMode_MoveWithoutPressedMouse )
					{
						camera->SetCameraMode(ECameraMode_MoveWithPressedMouse);
					}
					else if( camera->GetCameraMode() == ECameraMode_MoveWithPressedMouse )
					{
						camera->SetCameraMode(ECameraMode_MoveWithoutPressedMouse);
					}
				}
			}

			return 0;
		}

		case WM_LBUTTONDOWN:
			switch(wParam)
			{
				case MK_LBUTTON:
				{
					for(int i = 0; i < pApp->m_vUIObjects.size(); i++)
					{
						pApp->m_vUIObjects[i]->OnClicked();
					}
				}
			}

  }
	return DefWindowProc(pApp->GetMainWindow(), msg, wParam, lParam);
}


/////////////////////////////////////////////////////////////////////////
