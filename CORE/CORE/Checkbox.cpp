#include"Checkbox.h"
#include"TextManager.h"

/////////////////////////////////////////////////////////////////////////

Checkbox::Checkbox(D3DXVECTOR2 vPosition, int nWidth, int nHeight, std::string strText, std::string strIdleStateTexFileName, std::string strMouseOverStateTexFileName, std::string strId)
:IBaseMenuObject(vPosition,nWidth,nHeight,strText,strIdleStateTexFileName,strMouseOverStateTexFileName,strId)
{
	D3DXCreateSprite(pDxDevice,&m_pSprite);
	D3DXCreateTextureFromFile(pDxDevice,m_strIdleStateTextureFileName.c_str(),&m_pIdleStateTexture);
	D3DXCreateTextureFromFile(pDxDevice,m_strMouseOverStateTextureFileName.c_str(),&m_pMouseOverStateTexture);
	m_bIsMouseDown = false;
	m_bIsMouseOver = false;
	m_bIsChecked   = false;
}

/////////////////////////////////////////////////////////////////////////

void Checkbox::Init()
{
}

/////////////////////////////////////////////////////////////////////////

void Checkbox::OnLostDevice()
{
	m_pSprite->OnLostDevice();
}

/////////////////////////////////////////////////////////////////////////

void Checkbox::OnResetDevice()
{
	m_pSprite->OnResetDevice();
}

/////////////////////////////////////////////////////////////////////////

bool Checkbox::IsMouseOver()
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

void Checkbox::OnClicked()
{
	if( IsMouseOver() )
	{
		if( m_bIsChecked )
		{
			m_bIsChecked = false;
		}
		else
		{
			m_bIsChecked = true;
		}
	}

}

/////////////////////////////////////////////////////////////////////////

bool Checkbox::IsMouseDown()
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

void Checkbox::OnUpdate()
{
	if( IsMouseOver())
	{
		m_bIsMouseOver = true;
	}
	else
	{
		m_bIsMouseOver = false;
	}

	if( IsMouseDown())
	{
		m_bIsMouseDown = true;
	}
	else if( !IsMouseDown() )
	{
		m_bIsMouseDown = false;
	}
}

/////////////////////////////////////////////////////////////////////////

void Checkbox::OnRender(int a,int r,int g,int b)
{
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	D3DXVECTOR3 vPos = D3DXVECTOR3(m_vPosition.x, m_vPosition.y, 0.f);

	//if the mouse is over the button draw the mouseOverText
	if( !m_bIsChecked )
	{
		m_pSprite->Draw(m_pMouseOverStateTexture,NULL,NULL,&vPos,D3DXCOLOR(255,255,255,255));
			
		m_pSprite->Flush();
	}
	else
	{
		m_pSprite->Draw(m_pIdleStateTexture,NULL,NULL,&vPos,D3DXCOLOR(255,255,255,255));
		m_pSprite->Flush();
	}
	m_pSprite->End();

	pTextManager->RenderText(m_strText.c_str(),
					m_vPosition.x+20,
					m_vPosition.y,
					m_vPosition.x+20,
					m_vPosition.y+32,255,255,255,255);
}

/////////////////////////////////////////////////////////////////////////
