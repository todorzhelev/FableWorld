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

	//todo: should init everything in the constructor
	GameObject() {};
	virtual ~GameObject() = 0;
	virtual void OnUpdate(float dt) = 0;
	virtual void OnRender() = 0;
	virtual void OnLostDevice() = 0;
	virtual void OnResetDevice() = 0;

	virtual void LoadGameObject();
	virtual void RenderBindedWeapon(GameObject* pSkMesh, std::string bone);

	virtual float GetDistanceToPickedObject();

	std::string GetModelFileName() const;
	void SetModelFilename(const std::string& strModelFileName);

	std::string& GetName();

	void SetName(const std::string& strName);

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

	std::vector<Material>& GetMaterials();

	void SetMaterials(std::vector<Material> materials);

	std::vector<IDirect3DTexture9*>& GetTextures();

	void SetTextures(std::vector<IDirect3DTexture9*> textures);

	IDirect3DTexture9* GetTexture() const;

	void SetTexture(IDirect3DTexture9* texture);

	std::string GetTextureFilename() const;

	void SetTextureFilename(const std::string& filename);

	int IsBindable() const;

	void SetIsBindable(int bindable);

	std::string GetBindedToAnimatedModelName() const;

	void SetBindedToAnimatedModelName(const std::string& bindedToAnimatedModelName);

	std::string GetBindedToBoneName() const;

	void SetBindedToBoneName(const std::string& bindedToBoneName);

	std::unordered_map<GameObject*, std::string>& GetBindedObjects();

	void SetBindedObjects(std::unordered_map<GameObject*, std::string> bindedObjects);

	void TransformByMatrix(D3DXMATRIX matrix);

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
	std::string m_strModelFileName;

	//the id of the model
	std::string m_strModelName;

	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vLook;
	D3DXVECTOR3 m_vRight;
	D3DXVECTOR3 m_vUp;

	float m_fScale;

	//the mesh
	ID3DXMesh* m_pMesh;	
	
	std::vector<Material> m_vMaterials;
	
	//used if we have more than one texture assigned to the model
	std::vector<IDirect3DTexture9*> m_vTextures;

	//the texture for the model
	IDirect3DTexture9* m_pTexture;
	//the file name of the texture
	std::string m_strTextureFileName;

	//can this model be binded
	int m_bIsBindable;

	std::string m_strBindedToAnimatedModelName;
	std::string m_strBindedToBoneName;
	
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
