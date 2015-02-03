#pragma once

#include"IBase3DMenuObject.h"
#include"Misc.h"
#include"StaticMesh.h"

class Button3D : public IBase3DMenuObject
{
public:

	Button3D() {}

	virtual void OnLostDevice();
	virtual void OnResetDevice();
	
	virtual bool IsMouseOver();
	virtual bool IsMouseDown();

	virtual void OnUpdate();

	virtual void OnRender();

	virtual bool IsClicked();

	void		 Init();

	void		BuildBoundingBox();

	bool		IsPicked(D3DXMATRIX TransformationMatrix,D3DXVECTOR3 origin,D3DXVECTOR3 dir);

	AABB m_BoundingBox;
public:

	ID3DXMesh*  m_pTextMesh;
	StaticMesh*  m_pBackgroundMesh;
};

