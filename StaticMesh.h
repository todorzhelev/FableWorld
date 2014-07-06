#pragma once

#include "SkinnedMesh.h"
#include "TextManager.h"
#include "Misc.h"
#include "Terrain.h"
#include "GameObject.h"

//////////////////////////////////////////////////////////////////////////////

class StaticMesh : public GameObject
{
public:
	StaticMesh();
	StaticMesh(string strModelName, string ModelFileName, string strTextureFileName);
	
	virtual void LoadGameObject();

	virtual void OnUpdate(float fDeltaTime);

	virtual void OnRender();

	virtual void OnResetDevice();

	virtual void OnLostDevice();

	void		 BuildBoundingBox();

	void		 RenderBoundingBox();

	bool		 IsPicked(D3DXMATRIX T,D3DXVECTOR3 origin,D3DXVECTOR3 dir);

	void		 BuildEffect();
	
	void		 RenderBindedWeapon();

private:

	//texture used for models that dont have any
	IDirect3DTexture9* m_pWhiteTexture;

	Light		 m_light;

	ID3DXEffect* m_pEffect;
	D3DXHANDLE	 m_hEffectTechnique;
	D3DXHANDLE	 m_hWVPMatrix;
	D3DXHANDLE	 m_hTexture;
	D3DXHANDLE	 m_hMaterial;
	D3DXHANDLE	 m_hLight;
};

//////////////////////////////////////////////////////////////////////////////