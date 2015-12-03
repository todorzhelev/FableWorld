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

	virtual void LoadGameObject();
	virtual void OnUpdate(float dt);
	virtual void OnRender();
	virtual void OnLostDevice();
	virtual void OnResetDevice();
	virtual void RenderBindedWeapon(GameObject* pSkMesh, string bone);

	string GetModelFileName() const;
	void SetModelFilename(const string& strModelFileName);

	string GetName() const;

	void SetName(const string& strName);

	D3DXVECTOR3 GetPosition() const;

	void SetPosition(const D3DXVECTOR3& pos);

	D3DXVECTOR3 GetLookVector() const;

	void SetLookVector(const D3DXVECTOR3& look);

	D3DXVECTOR3 GetRightVector() const;

	void SetRightVector(const D3DXVECTOR3& right);

	D3DXVECTOR3 GetUpVector() const;

	void SetUpVector(const D3DXVECTOR3& up);

	float GetScale() const;

	void SetScale(float scale);

	ID3DXMesh* GetMesh() const;

	void SetMesh(ID3DXMesh* mesh);

	vector<Material>& GetMaterials();

	void SetMaterials(vector<Material> materials);

	vector<IDirect3DTexture9*>& GetTextures();

	void SetTextures(vector<IDirect3DTexture9*> textures);

	IDirect3DTexture9* GetTexture() const;

	void SetTexture(IDirect3DTexture9* texture);

	string GetTextureFilename() const;

	void SetTextureFilename(const string& filename);

	int IsBindable() const;

	void SetIsBindable(int bindable);

	string GetBindedToAnimatedModelName() const;

	void SetBindedToAnimatedModelName(const string& bindedToAnimatedModelName);

	string GetBindedToBoneName() const;

	void SetBindedToBoneName(const string& bindedToBoneName);

	unordered_map<GameObject*, string>& GetBindedObjects();

	void SetBindedObjects(unordered_map<GameObject*, string> bindedObjects);

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

	ID3DXMesh* GetTitleMesh() const;

	void SetTitleMesh(ID3DXMesh* titleMesh);

	D3DXVECTOR3 GetTitleLookVector() const;

	void SetTitleLookVector(const D3DXVECTOR3& look);

	D3DXVECTOR3 GetTitleRightVector() const;

	void SetTitleRightVector(const D3DXVECTOR3& right);

	D3DXVECTOR3 GetTitleUpVector() const;

	void SetTitleUpVector(const D3DXVECTOR3& up);

	D3DXMATRIX GetTitleRotationMatrix() const;

	void SetTitleRotationMatrix(D3DXMATRIX titleRotationMatrix);

	float GetTitleRotationAnglyByY() const;

	void SetTitleRotationAnglyByY(float titleRotationAnglyByY);

	string GetTitleForQuest() const;

	//terrible code duplication.....
	void SetTitleForQuest(const string& titleForQuest);

	ID3DXMesh* GetTitleForQuestMesh() const;

	void SetTitleForQuestMesh(ID3DXMesh* titleForQuestMesh);

	D3DXVECTOR3 GetTitleForQuestLookVector() const;

	void SetTitleForQuestLookVector(const D3DXVECTOR3& look);

	D3DXVECTOR3 GetTitleForQuestRightVector() const;

	void SetTitleForQuestRightVector(const D3DXVECTOR3& right);

	D3DXVECTOR3 GetTitleForQuestUpVector() const;

	void SetTitleForQuestUpVector(const D3DXVECTOR3& up);

	D3DXMATRIX GetTitleForQuestRotationMatrix() const;

	void SetTitleForQuestRotationMatrix(D3DXMATRIX titleForQuestRotationMatrix);

	float GetTitleForQuestRotationAnglyByY() const;
		
	void SetTitleForQuestRotationAnglyByY(float titleForQuestRotationAnglyByY);

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

	float GetRotationAngleByX() const;

	void SetRotationAngleByX(float rotationAngleByX);

	float GetRotationAngleByY() const;

	void SetRotationAngleByY(float rotationAngleByY);

	float GetRotationAngleByZ() const;

	void SetRotationAngleByZ(float rotationAngleByZ);

	bool IsPicked() const;

	void SetPicked(bool picked);

	ID3DXMesh* GetBBMesh() const;

	void SetBBMesh(ID3DXMesh* BBMesh);

	Material GetBBMaterial() const;

	void SetBBMaterial(Material material);

	AABB GetBB() const;

	void SetBB(AABB boundingBox);

	D3DXMATRIX GetBBOffsetMatrix() const;

	void SetBBOffsetMatrix(D3DXMATRIX offsetMatrix);

	EGameObjectType GetObjectType() const;

	void SetObjectType(EGameObjectType type);

	D3DXMATRIX GetCombinedTransfMatrix() const;

	void SetCombinedTransfMatrix(D3DXMATRIX mat);

//private:

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
