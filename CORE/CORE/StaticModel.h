#pragma once

#include "SkinnedModel.h"
#include "TextManager.h"
#include "Misc.h"
#include "Terrain.h"
#include "GameObject.h"
#include "GameObjectManager.h"

//////////////////////////////////////////////////////////////////////////////

class StaticModel : public GameObject
{
public:
	StaticModel();
	StaticModel(std::string strModelName, std::string ModelFileName, std::string strTextureFileName);
	virtual ~StaticModel();
	virtual void LoadGameObject() override;

	virtual void OnUpdate(float fDeltaTime) override;

	virtual void OnRender() override;

	virtual void OnResetDevice() override;

	virtual void OnLostDevice() override;

	virtual void RenderBindedWeapon(GameObject* pSkMesh, std::string bone) override;

	virtual float GetDistanceToPickedObject() override;

	virtual bool SpawnClone() override;

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