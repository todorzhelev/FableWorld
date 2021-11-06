#pragma once 

#include "Camera.h"
#include "Misc.h"

//////////////////////////////////////////////////////////////////////////////

class SkinnedModel;

class TextManager {
public:

	TextManager();
	ID3DXFont*  GetFont2D();
	void		CreateFontFor3DText();
	void		CreateMeshFor3DText(SkinnedModel* pGameObject);
	void		CreateMeshFor3DTextQuest(SkinnedModel* pGameObject);
	void		RenderText(LPCSTR dtext,int x1,int y1,int x2,int y2,int alpha,int color1,int color2,int color3);
	void		DrawFPS();
	void		OnUpdate(float dt);
	void		OnResetDevice();
	void		OnLostDevice();

private:

	//stores the current frames per second
	float m_fFPS;
	HFONT		m_pFont3D;
	HDC			m_hDC;
	ID3DXFont*	m_pFont2D;

};

//////////////////////////////////////////////////////////////////////////////

extern TextManager* pTextManager;

//////////////////////////////////////////////////////////////////////////////