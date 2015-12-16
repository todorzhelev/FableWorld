#include"Textbox.h"
#include"TextManager.h"

Textbox::Textbox(D3DXVECTOR2 vPosition, int nWidth, int nHeight, string strText,string strIdleStateTexFileName, string strMouseOverStateTexFileName,string strId)
:IBaseMenuObject(vPosition,nWidth,nHeight,strText,strIdleStateTexFileName,strMouseOverStateTexFileName,strId)
{
	D3DXCreateSprite(pDxDevice,&m_pSprite);
	D3DXCreateTextureFromFile(pDxDevice,m_strIdleStateTextureFileName.c_str(),&m_pIdleStateTexture);
	D3DXCreateTextureFromFile(pDxDevice,m_strMouseOverStateTextureFileName.c_str(),&m_pMouseOverStateTexture);

	m_bIsMouseDown = false;
	m_bIsMouseOver = false;
}

/////////////////////////////////////////////////////////////////////////

void Textbox::Init()
{
	D3DXCreateSprite(pDxDevice,&m_pSprite);
	D3DXCreateTextureFromFile(pDxDevice,m_strIdleStateTextureFileName.c_str(),&m_pIdleStateTexture);
	D3DXCreateTextureFromFile(pDxDevice,m_strMouseOverStateTextureFileName.c_str(),&m_pMouseOverStateTexture);

	m_bIsMouseDown = false;
	m_bIsMouseOver = false;
}


/////////////////////////////////////////////////////////////////////////

void Textbox::OnLostDevice()
{
	m_pSprite->OnLostDevice();
}


/////////////////////////////////////////////////////////////////////////

void Textbox::OnResetDevice()
{
	m_pSprite->OnResetDevice();
}


/////////////////////////////////////////////////////////////////////////

bool Textbox::IsMouseOver()
{
	POINT s;
	GetCursorPos(&s);

	ScreenToClient(pApp->GetMainWindow(), &s);

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

bool Textbox::IsMouseDown()
{
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

void Textbox::OnClicked()
{
	if( IsMouseOver() && m_bIsVisible)
	{
		m_bIsSelected = true;
	}

	if( m_bIsSelected )
	{
		if( pApp->m_strSelectedTextbox.empty() )
		{
			pApp->m_strSelectedTextbox = m_strId;
		}
		else if( m_strId != pApp->m_strSelectedTextbox )
		{
			IBaseMenuObject* pSelectedTextbox = pApp->FindMenuObject(pApp->m_strSelectedTextbox);

			pSelectedTextbox->m_bIsSelected = false;
			pApp->m_strSelectedTextbox = m_strId;
		}
	}
}

/////////////////////////////////////////////////////////////////////////

void Textbox::OnUpdate()
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

void Textbox::OnRender()
{
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	if(m_bIsVisible)
	{
		D3DXVECTOR3 vPos = D3DXVECTOR3(m_vPosition.x, m_vPosition.y, 0.f);

		m_pSprite->Draw(m_pIdleStateTexture,NULL,NULL,&vPos,D3DXCOLOR(255,255,255,255));
	}
	m_pSprite->End();
		
	if(m_bIsVisible)
	{
		pTextManager->RenderText(m_strText.c_str(),
							     static_cast<int>(m_vPosition.x)+10,
							     static_cast<int>(m_vPosition.y)+7,
							     static_cast<int>(m_vPosition.x)+20,
							     static_cast<int>(m_vPosition.y)+32,
							     255,255,255,255);
	}
}

/////////////////////////////////////////////////////////////////////////

void Textbox::OnRender(int a,int r,int g,int b)
{
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	if( m_bIsVisible)
	{
		D3DXVECTOR3 vPos = D3DXVECTOR3(m_vPosition.x, m_vPosition.y, 0.f);

		if( !m_bIsSelected )
		{
			m_pSprite->Draw(m_pIdleStateTexture,NULL,NULL,&vPos,D3DXCOLOR(255,255,255,255));
		}
		else
		{
			m_pSprite->Draw(m_pMouseOverStateTexture,NULL,NULL,&vPos,D3DXCOLOR(255,255,255,255));
		}
	}
	m_pSprite->End();

	if( m_bIsVisible )
	{
		pTextManager->RenderText(m_strText.c_str(),m_vPosition.x+10,
			m_vPosition.y+7,
			m_vPosition.x+20,
			m_vPosition.y+32,255,255,255,255);
	}
}

/////////////////////////////////////////////////////////////////////////

