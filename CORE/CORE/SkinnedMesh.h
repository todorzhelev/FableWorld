#pragma once
#include "Camera.h"
#include "Terrain.h"
#include "Misc.h"
#include "DialogueManager.h"
#include <stdlib.h>
#include <time.h>
#include "GameObject.h"

//////////////////////////////////////////////////////////////////////////////

//we extend the D3DXFRAME structure so we can have matrix which will store
//all the the transformations that bone's parent has 
struct FrameEx : public D3DXFRAME
{
	D3DXMATRIX ToRootMatrix;
};

//////////////////////////////////////////////////////////////////////////////

//we need the implement this class because DirectX doesnt include functionality for creating frames, mesh containers and for destroying them
class AllocateHierarchy : public ID3DXAllocateHierarchy 
{
public:
	HRESULT STDMETHODCALLTYPE CreateFrame(THIS_ PCSTR Name, D3DXFRAME** NewFrame);                     

	HRESULT STDMETHODCALLTYPE CreateMeshContainer(PCSTR Name, const D3DXMESHDATA* MeshData,const D3DXMATERIAL* Materials, const D3DXEFFECTINSTANCE* EffectInstances, 
												  DWORD NumMaterials, const DWORD *Adjacency, ID3DXSkinInfo* SkinInfo, D3DXMESHCONTAINER** NewMeshContainer);     

	HRESULT STDMETHODCALLTYPE DestroyFrame(THIS_ D3DXFRAME* frame);              
	HRESULT STDMETHODCALLTYPE DestroyMeshContainer(THIS_ D3DXMESHCONTAINER* MeshContainer);
};

//////////////////////////////////////////////////////////////////////////////

//this class contains all the functions for loading animated model, updating his bones, and rendering the model
class SkinnedMesh : public GameObject
{
public:
	SkinnedMesh();
	SkinnedMesh(string strModelName, string ModelFileName, string strTextureFileName, bool bShouldRenderTitles = true);
	~SkinnedMesh();

	virtual void 	LoadGameObject();

	virtual void 	OnUpdate(float fDeltaTime);

	virtual void 	OnRender();

	virtual void 	OnResetDevice();

	virtual void 	OnLostDevice();

	virtual void	RenderBindedWeapon(GameObject* pSkMesh, string bone);

	void			PlayAnimation(LPCSTR strAnimationName);
	
	void 			PlayAnimationOnce(LPCSTR strAnimationName);
	
	void 			PlayAnimationOnceAndStop(LPCSTR strAnimationName);
	
	void 			BindWeaponToModel(string weapon,string frameToBind);

	D3DXFRAME* 		FindFrameWithMesh(D3DXFRAME* frame);

private:

	void 			RenderTitles();
	
	void 			RenderTitlesForQuest();
	
	void 			RenderBoundingBox();
		
	void 			BuildBoundingBox();
	
	void 			BuildSkinnedMesh(ID3DXMesh* pMesh);
	
	void 			BuildToRootMatricesPtrArray();
	
	void 			BuildToRootMatrices(FrameEx* bone, D3DXMATRIX& parentsBoneToRootMatrix);
	
    D3DXMATRIX* 	GetFinalBonesMatricesArray();
	
	void 			BuildEffect();
	
	void 			BuildEffectForTitles();	

	bool			IsPicked(D3DXFRAME* pFrame,D3DXMATRIX combinedMatrix,D3DXVECTOR3 vOrigin,D3DXVECTOR3 vDir);

private:

	IDirect3DTexture9* m_pWhiteTexture;

	int m_nMaxBonesSupported; 

	const double m_fTransTimeForAttack;
	const double m_fTransTimeForIdle;

	//effect parameters for the mesh
	ID3DXEffect* m_pEffect;
	D3DXHANDLE 	 m_hEffectTechnique;
	D3DXHANDLE 	 m_hWVPMatrix;
	D3DXHANDLE 	 m_hTexture;
	D3DXHANDLE 	 m_hMaterial;
	D3DXHANDLE 	 m_hLight;
	D3DXHANDLE 	 m_hFinalBonesMatrices;
	D3DXHANDLE 	 m_hIsPicked;

	Light m_light;
	Material  m_whiteMaterial;

	//effect parameters for the titles above the mesh
	ID3DXEffect* m_pTitlesEffect;
	D3DXHANDLE 	 m_hTitlesEffectTechnique;
	D3DXHANDLE 	 m_hTitlesWVPMatrix;

	bool		 m_bShouldRenderTitles;
};

//////////////////////////////////////////////////////////////////////////////

