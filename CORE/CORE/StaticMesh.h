#pragma once

#include "SkinnedMesh.h"
#include "TextManager.h"
#include "Misc.h"
#include "Terrain.h"
#include "GameObject.h"
#include "GameObjectManager.h"

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

	virtual void RenderBindedWeapon(GameObject* pSkMesh, string bone);

	void		 BuildBoundingBox();

	void		 RenderBoundingBox();

	void		 BuildEffect();
	
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

	D3DXHANDLE 	 m_hIsPicked;

};

//////////////////////////////////////////////////////////////////////////////