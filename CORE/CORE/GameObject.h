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

	virtual float GetDistanceToPickedObject();

	string GetModelFileName() const;
	void SetModelFilename(const string& strModelFileName);

	string& GetName();

	void SetName(const string& strName);

	D3DXVECTOR3 GetPosition() const;

	void SetPosition(const D3DXVECTOR3& pos);

	D3DXVECTOR3& GetLookVector();
	D3DXVECTOR3& GetRightVector();
	D3DXVECTOR3& GetUpVector();

	float GetScale() const;

	void SetScale(float scale);

	void ModifyScale(float delta);

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

	

	float GetRotationAngleByX() const;
	void SetRotationAngleByX(float rotationAngleByX);
	void ModifyRotationAngleByX(float delta);

	float GetRotationAngleByY() const;
	void SetRotationAngleByY(float rotationAngleByY);
	void ModifyRotationAngleByY(float delta);

	float GetRotationAngleByZ() const;
	void SetRotationAngleByZ(float rotationAngleByZ);
	void ModifyRotationAngleByZ(float delta);

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

protected:

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
