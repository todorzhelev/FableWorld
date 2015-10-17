#pragma once

#include"Misc.h"
#include"AABB.h"

//////////////////////////////////////////////////////////////////////////////

enum EGameObjectType
{
	EGameObjectType_Static,
	EGameObjectType_Skinned
};

//////////////////////////////////////////////////////////////////////////////

class GameObject
{
public:

	virtual void LoadGameObject() {};

	virtual void OnUpdate(float dt){};

	virtual void OnRender(){};

	virtual void OnLostDevice() {};

	virtual void OnResetDevice() {};

	virtual void RenderBindedWeapon(GameObject* pSkMesh, string bone) {};


//make it protected and add getters/setters..
public:

	//the name of the .x file
	string m_strModelFileName;

	//the id of the model
	string m_strModelName;

	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vLook;
	D3DXVECTOR3 m_vRight;
	D3DXVECTOR3 m_vUp;

	float m_fScale;

	//the mesh
	ID3DXMesh* m_pMesh;	
	
	vector<Material> m_vMaterials;
	
	//used if we have more than one texture assigned to the model
	vector<IDirect3DTexture9*> m_vTextures;

	//the texture for the model
	IDirect3DTexture9* m_pTexture;
	//the file name of the texture
	string m_strTextureFileName;

	//can this model be binded
	int m_bIsBindable;

	string m_strBindedToAnimatedModelName;
	string m_strBindedToBoneName;
	
	//this map holds which weapon to which bone is attached
	unordered_map<GameObject*,string> m_mapBindedObjects;

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
	
	float m_fRotAngleX;
	float m_fRotAngleY;
	float m_fRotAngleZ;
	
	//is the model picked by mouse
	bool m_bIsPicked;

	//used for rendering the bounding box
	ID3DXMesh*  m_pBoundingBoxMesh;
	Material    m_BoundingBoxMaterial;
	AABB        m_BoundingBox;
	D3DXMATRIX  m_BoundingBoxOffset;

	EGameObjectType m_eGameObjectType;

	D3DXMATRIX  m_CombinedTransformationMatrix;
};

//////////////////////////////////////////////////////////////////////////////
