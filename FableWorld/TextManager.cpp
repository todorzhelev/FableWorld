#include "TextManager.h"

/////////////////////////////////////////////////////////////////////////

TextManager* pTextManager = NULL;

/////////////////////////////////////////////////////////////////////////
/*
Function:Text
Purpose:constructor. load the font for drawing on the screen.
*/
TextManager::TextManager():m_fFPS(0.0)
{
	if( AddFontResource("fonts/starcraft.ttf") == 0 )
	{
		MessageBox(0,"cannot load font",0,0);
	}

	D3DXFONT_DESC fontDesc;

	fontDesc.Height				= 18;
	fontDesc.Width				= 0;
	fontDesc.Weight				= 0;
	fontDesc.MipLevels			= 1;
	fontDesc.Italic				= false;
	fontDesc.CharSet			= DEFAULT_CHARSET;
	fontDesc.OutputPrecision	= OUT_DEFAULT_PRECIS;
	fontDesc.Quality			= DEFAULT_QUALITY;
	fontDesc.PitchAndFamily		= DEFAULT_PITCH | FF_DONTCARE;
	_tcscpy_s(fontDesc.FaceName, _T("Starcraft Normal"));

	if( SUCCEEDED(D3DXCreateFontIndirect(pDxDevice, &fontDesc, &m_pFont2D)) )
	{
		fout<<"font created\n";
	}
}


/////////////////////////////////////////////////////////////////////////
/*
Function:GetFont2D
Purpose:returns the used font in the game
*/
ID3DXFont* TextManager::GetFont2D()
{
	return m_pFont2D;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:CreateFontFor3DText
Purpose:create font for the 3d titles above the models in the game
*/
void TextManager::CreateFontFor3DText()
{
	//creates device context. The device context is needed so the 3d text can be rendered.
	m_hDC = CreateCompatibleDC(NULL);

	m_pFont3D = CreateFont( 10,						//Height
							0,						//Width
							0,						//Escapement
							0,						//Orientation
							FW_NORMAL,				//Weight
							false,					//Italic
							false,					//Underline
							false,					//Strikeout
							DEFAULT_CHARSET,
							OUT_DEFAULT_PRECIS,  
							CLIP_DEFAULT_PRECIS, 
							DEFAULT_QUALITY,     
							DEFAULT_PITCH|FF_DONTCARE, 
							"Verdana"
							);




	SelectObject(m_hDC, m_pFont3D);
}


/////////////////////////////////////////////////////////////////////////
/*
Function:CreateMeshFor3DText
Purpose:creates 3D text model for model titles
*/
void TextManager::CreateMeshFor3DText(GameObject* pGameObject)
{
	D3DXCreateText(pDxDevice,  
				   m_hDC,          
				   pGameObject->m_strModelName.c_str(),		//Text 
				   0.001f,            
				   0.04f,          
				   &pGameObject->m_pTitleMesh,			//Mesh
				   NULL,       
				   NULL);
}


/////////////////////////////////////////////////////////////////////////
/*
Function:CreateMeshFor3DTextQuest
Purpose:creates 3D text model for quest titles
*/
void TextManager::CreateMeshFor3DTextQuest(GameObject* pGameObject)
{
	D3DXCreateText( pDxDevice,  
					m_hDC,          
					pGameObject->m_strTitleForQuest.c_str(),	  //Text 
					0.001f,            
					0.04f,          
					&pGameObject->m_pTitleForQuestMesh,       //Mesh
					NULL,       
					NULL);
}


/////////////////////////////////////////////////////////////////////////
/*
Function:RenderText
Purpose:draw text on the screen
*/
void TextManager::RenderText(LPCSTR dtext,int x1,int y1,int x2,int y2,int alpha,int color1,int color2,int color3)
{
	RECT rec = {x1,y1,x2,y2};
	m_pFont2D->DrawTextA(0,dtext,-1,&rec,DT_NOCLIP,D3DCOLOR_ARGB(alpha,color1,color2,color3));
}


/////////////////////////////////////////////////////////////////////////
/*
Function:OnUpdate
Purpose:updates the frames per second
*/
void TextManager::OnUpdate(float dt)
{
	//calculate the fps
	static float fFps = 0.0f;
	static float fTimeElapsed = 0.0f;

	fFps += 1.0f;
	fTimeElapsed += dt;

	if(fTimeElapsed >= 1.0f)
	{
		m_fFPS = fFps;

		fFps = 0.0f;
		fTimeElapsed = 0.0f;	
	}
}

/////////////////////////////////////////////////////////////////////////
/*
Function:DrawFPS
Purpose:draws the fps on the screen
*/
void TextManager::DrawFPS()
{
	static char buffer[256];

	sprintf_s(buffer,"fps: %.2f \n",m_fFPS);

	int w = pEngine->GetPresentParameters().BackBufferWidth;
	int h = pEngine->GetPresentParameters().BackBufferHeight;

	RECT rec = {w-100,5,0,0};
	m_pFont2D->DrawTextA(0,buffer,-1,&rec,DT_NOCLIP,D3DCOLOR_XRGB(0,0,0));
}

/////////////////////////////////////////////////////////////////////////
/*
Function:OnResetDevice
Purpose:invoked when the device is reseted
*/
void TextManager::OnResetDevice()
{
	m_pFont2D->OnResetDevice();	
}

/////////////////////////////////////////////////////////////////////////
/*
Function:OnLostDevice
Purpose:invoked when the device is lost
*/
void TextManager::OnLostDevice()
{
	m_pFont2D->OnLostDevice();
}


/////////////////////////////////////////////////////////////////////////
