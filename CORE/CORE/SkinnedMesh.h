#pragma once
#include "Camera.h"
#include "Terrain.h"
#include "Misc.h"
#include "DialogueManager.h"
#include <stdlib.h>
#include <time.h>
#include "StaticMesh.h"

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

	bool IsAttacked() const;

	void SetAttacked(bool attacked);

	bool IsAttacking() const;

	void SetAttacking(bool attacking);

	bool IsDead() const;

	void SetDead(bool dead);

	bool HasDialogue() const;

	void SetHasDialogue(bool hasDialogue);

	string GetAttackerName() const;

	void SetAttackerName(const string& attackerName);

	string GetActorType() const;

	void SetActorType(const string& actorType);

	//name above the object in the game
	ID3DXMesh**		GetTitleMesh();
	void			SetTitleMesh(ID3DXMesh* titleMesh);
	D3DXVECTOR3&	GetTitleLookVector();
	D3DXVECTOR3&	GetTitleRightVector();
	D3DXVECTOR3&	GetTitleUpVector();
	D3DXMATRIX		GetTitleRotationMatrix() const;
	void			SetTitleRotationMatrix(D3DXMATRIX titleRotationMatrix);
	float			GetTitleRotationAnglyByY() const;
	void			SetTitleRotationAnglyByY(float angle);
	void			ModifyTitleRotationAnglyByY(float delta);

	//terrible code duplication fix this .....
	//text for quest above the object
	string			GetTitleForQuest() const;
	void			SetTitleForQuest(const string& titleForQuest);
	ID3DXMesh**		GetTitleForQuestMesh();
	void			SetTitleForQuestMesh(ID3DXMesh* titleForQuestMesh);
	D3DXVECTOR3&	GetTitleForQuestLookVector();
	D3DXVECTOR3&	GetTitleForQuestRightVector();
	D3DXVECTOR3&	GetTitleForQuestUpVector();
	D3DXMATRIX		GetTitleForQuestRotationMatrix() const;
	void			SetTitleForQuestRotationMatrix(D3DXMATRIX titleForQuestRotationMatrix);
	float			GetTitleForQuestRotationAnglyByY() const;
	void			SetTitleForQuestRotationAnglyByY(float angle);
	void			ModifyTitleForQuestRotationAnglyByY(float delta);

	vector<D3DXMATRIX>& GetFinalBonesMatrices();

	void SetFinalBonesMatrices(vector<D3DXMATRIX> bonesMatrices);

	vector<D3DXMATRIX*>& GetToRootMatrices();

	void SetToRootMatrices(vector<D3DXMATRIX*> toRootMatrices);

	D3DXFRAME* GetRootFrame() const;

	void SetRootFrame(D3DXFRAME* rootFrame);

	DWORD GetMaxVertexInfluences() const;

	void SetMaxVertexInfluences(DWORD maxVertexInfluences);

	DWORD GetAmountOfBones() const;

	void SetAmountOfBones(DWORD amountOfBones);

	ID3DXSkinInfo* GetSKinInfo() const;

	void SetSkinInfo(ID3DXSkinInfo* skinInfo);

	ID3DXAnimationController* GetAnimationController() const;

	void SetAnimationController(ID3DXAnimationController* animController);

	LPD3DXANIMATIONSET GetCurrentAnimationSet() const;

	void SetCurrentAnimationSet(LPD3DXANIMATIONSET currentAnimationSet);

	LPD3DXANIMATIONSET GetSecondAnimationSet() const;

	void SetSecondAnimationSet(LPD3DXANIMATIONSET secondAnimationSet);

	DWORD GetCurrentAnimationTrack() const;

	void SetCurrentAnimationTrack(DWORD currentAnimationTrack);

	DWORD GetNewAnimationTrack() const;

	void SetNewAnimationTrack(DWORD newAnimationTrack);

	bool IsSwitched() const;

	void SetIsSwitched(bool isSwitched);

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

	bool m_bIsAttacked;
	bool m_bIsAttacking;
	bool m_bIsDead;
	bool m_bHasDialogue;

	//which enemy attacked this gameobject
	string m_strAttackerName;

	//the type of the model in the game: mainHero, neutral, enemy
	string m_strActorType;


	//title above the model
	ID3DXMesh*  m_pTitleMesh;
	D3DXVECTOR3 m_vTitleLook;
	D3DXVECTOR3 m_vTitleRight;
	D3DXVECTOR3 m_vTitleUp;

	D3DXMATRIX  m_TitleRotationMatrix;
	float       m_fTitleRotationAngleByY;

	//title above the model for a quest
	string      m_strTitleForQuest;
	ID3DXMesh*  m_pTitleForQuestMesh;
	D3DXVECTOR3 m_vTitleForQuestLook;
	D3DXVECTOR3 m_vTitleForQuestRight;
	D3DXVECTOR3 m_vTitleForQuestUp;

	D3DXMATRIX  m_TitleForQuestRotationMatrix;
	float       m_fTitleForQuestRotationAngleByY;

	//animated model related variables
	vector<D3DXMATRIX>  m_vFinalBonesMatrices;
	vector<D3DXMATRIX*> m_vToRootMatrices;
	D3DXFRAME*          m_pRoot;
	DWORD               m_nMaxVertInfluences;
	DWORD               m_nNumBones;
	ID3DXSkinInfo*      m_pSkinInfo;
	ID3DXAnimationController* m_pAnimController;
	LPD3DXANIMATIONSET  m_pCurrentAnimSet;
	LPD3DXANIMATIONSET  m_pSecondAnimSet;
	DWORD               m_nCurrentAnimTrack;
	DWORD               m_nNewAnimTrack;

	//for animation switching when attacking
	bool  m_bIsSwitched;
};

//////////////////////////////////////////////////////////////////////////////

