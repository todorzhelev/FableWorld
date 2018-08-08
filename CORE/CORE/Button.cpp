#include <stdafx.h>
#include "Button.h"

Button::Button(D3DXVECTOR2 vPosition, int nWidth, int nHeight, std::string strText,std::string strIdleStateTexFileName, std::string strMouseOverStateTexFileName)
:IBaseMenuObject(vPosition,nWidth,nHeight,strText,strIdleStateTexFileName,strMouseOverStateTexFileName)
{
	D3DXCreateSprite(pDxDevice,&m_pSprite);

	HRESULT err = D3DXCreateTextureFromFile(pDxDevice,m_strIdleStateTextureFileName.c_str(),&m_pIdleStateTexture);
	CheckHR(err);

	err = D3DXCreateTextureFromFile(pDxDevice,m_strMouseOverStateTextureFileName.c_str(),&m_pMouseOverStateTexture);
	CheckHR(err);

	m_bIsMouseOver = false;
	m_bIsMouseDown = false;
}

/////////////////////////////////////////////////////////////////////////

void Button::Init()
{

}

/////////////////////////////////////////////////////////////////////////

void Button::OnLostDevice()
{
	m_pSprite->OnLostDevice();
}

/////////////////////////////////////////////////////////////////////////

void Button::OnResetDevice()
{
	m_pSprite->OnResetDevice();
}

/////////////////////////////////////////////////////////////////////////

bool Button::IsMouseOver()
{
	POINT s;
	GetCursorPos(&s);

	//ScreenToClient makes the mouse coordinates relative to area of the program not to the screen 
	//i.e. the coordinates will start from top-left position of the window
	ScreenToClient(pApp->GetMainWindow(), &s);

	//if the mouse is inside return true
	if((s.x > m_vPosition.x) && 
	   (s.y > m_vPosition.y) &&
	   (s.x< (m_vPosition.x + m_nWidth)) &&
	   (s.y < (m_vPosition.y + m_nHeight)))
	{
			return true;
	}
	else 
	{
		return false;
	}
		
}


/////////////////////////////////////////////////////////////////////////

bool Button::IsMouseDown()
{
	//if the mouse is over the area and the left button is pressed then return true
	if(IsMouseOver() && pDinput->IsMouseButtonDown(0))
	{
		return true;
	}
	else
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////

bool Button::IsClicked()
{
	return m_bIsMouseDown;
}

/////////////////////////////////////////////////////////////////////////

void Button::OnUpdate()
{
	if(IsMouseOver())
	{
		m_bIsMouseOver = true;
	}
	else
	{
		m_bIsMouseOver = false;
	}

	if(IsMouseDown())
	{
		m_bIsMouseDown = true;
	}
	else 
	{   
		m_bIsMouseDown = false;
	}

}

/////////////////////////////////////////////////////////////////////////

void Button::OnRender()
{
	D3DXVECTOR3 vPos = D3DXVECTOR3(m_vPosition.x, m_vPosition.y, 0.f);

	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	//if the mouse is over the button draw the mouseOverText
	if(m_bIsMouseOver)
	{
		m_pSprite->Draw(m_pMouseOverStateTexture,NULL,NULL,&vPos,D3DXCOLOR(255,255,255,255));
	}
	else
	{
		m_pSprite->Draw(m_pIdleStateTexture,NULL,NULL,&vPos,D3DXCOLOR(255,255,255,255));
	}
	m_pSprite->End();
}


/////////////////////////////////////////////////////////////////////////

void Button::OnRender(int a,int r,int g,int b)
{
	D3DXVECTOR3 vPos = D3DXVECTOR3(m_vPosition.x, m_vPosition.y, 0.f);

	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	//if the mouse is over the button draw the mouseOverText
	if(m_bIsMouseOver)
	{
		m_pSprite->Draw(m_pMouseOverStateTexture,NULL,NULL,&vPos,D3DXCOLOR(255,255,255,255));
	}
	else
	{
		m_pSprite->Draw(m_pIdleStateTexture,NULL,NULL,&vPos,D3DXCOLOR(255,255,255,255));
	}
	m_pSprite->End();
}

/////////////////////////////////////////////////////////////////////////