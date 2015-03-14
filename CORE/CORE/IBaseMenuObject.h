#pragma once
#include"Misc.h"

const string strResourcesRelativePath = "../../Resources/textures/GUI/";

class IBaseMenuObject
{
public:

	IBaseMenuObject() {};

	IBaseMenuObject(D3DXVECTOR2 vPosition, int nWidth, int nHeight, string strText="",string strIdleStateTexFileName="", string strMouseOverStateTexFileName="", string strId="");

	virtual void Init() {};

	virtual void OnLostDevice() = 0;

	virtual void OnResetDevice() = 0;
	
	virtual bool IsMouseOver() = 0;

	virtual bool IsMouseDown() = 0;

	virtual void OnUpdate() = 0;

	//virtual void OnRender() {};

	virtual void OnRender(int a,int r,int g,int b) {};

	virtual bool IsClicked() { return false; }

	virtual void OnClicked();

	void		 SetPosition(D3DXVECTOR2 vPosition);

	D3DXVECTOR2  GetPosition();

	void		 SetMouseOver(bool bIsMouseOver);

	bool		 GetIsMouseOver();

	void		 SetMouseDown(bool bIsMouseDown);

	bool		 GetIsMouseDown();

	void		 SetVisible(bool bVisible);

	bool		 IsVisible();

	void		 SetIdleStateTextureFileName(string strFileName);

	void		 SetMouseOverStateTextureFileName(string strFileName);

	void		 SetIdleStateTexture(IDirect3DTexture9* pTexture);

	void		 SetMouseOverStateTexture(IDirect3DTexture9* pTexture);

	void		 SetText(string strText);

	string		 GetText();

	void		 SetWidth(int nWidth);

	int			 GetWidth();

	void		 SetHeight(int nHeight);

	int			 GetHeight();

	string		 m_strId;

	bool		 m_bIsSelected;

protected:

	D3DXVECTOR2 m_vPosition;

	ID3DXSprite* m_pSprite;

	int m_nWidth;
	int m_nHeight;

	bool m_bIsMouseOver;
	bool m_bIsMouseDown;

	bool m_bIsVisible;
	

	string m_strText;

	string m_strIdleStateTextureFileName;
	string m_strMouseOverStateTextureFileName;

	IDirect3DTexture9* m_pIdleStateTexture;
	IDirect3DTexture9* m_pMouseOverStateTexture;

};

