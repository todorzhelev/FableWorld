#include"GameObject.h"

void GameObject::LoadGameObject()
{
	
}

void GameObject::OnUpdate(float dt)
{
	
}

void GameObject::OnRender()
{
	
}

void GameObject::OnLostDevice()
{
	
}

void GameObject::OnResetDevice()
{
	
}

void GameObject::RenderBindedWeapon(GameObject* pSkMesh, string bone)
{
	
}

string GameObject::GetModelFileName() const
{
	return m_strModelFileName;
}

void GameObject::SetModelFilename(const string& strModelFileName)
{
	m_strModelFileName = strModelFileName;
}

string GameObject::GetName() const
{
	return m_strModelName;
}

void GameObject::SetName(const string& strName)
{
	m_strModelName = strName;
}

D3DXVECTOR3 GameObject::GetPosition() const
{
	return m_vPos;
}

void GameObject::SetPosition(const D3DXVECTOR3& pos)
{
	m_vPos = pos;
}

D3DXVECTOR3& GameObject::GetLookVector()
{
	return m_vLook;
}

D3DXVECTOR3& GameObject::GetRightVector()
{
	return m_vRight;
}

D3DXVECTOR3& GameObject::GetUpVector()
{
	return m_vUp;
}

float GameObject::GetScale() const
{
	return m_fScale;
}

void GameObject::SetScale(float scale)
{
	m_fScale = scale;
}

ID3DXMesh* GameObject::GetMesh() const
{
	return m_pMesh;
}

void GameObject::SetMesh(ID3DXMesh* mesh)
{
	m_pMesh = mesh;
}

vector<Material>& GameObject::GetMaterials()
{
	return m_vMaterials;
}

void GameObject::SetMaterials(vector<Material> materials)
{
	m_vMaterials = materials;
}

vector<IDirect3DTexture9*>& GameObject::GetTextures()
{
	return m_vTextures;
}

void GameObject::SetTextures(vector<IDirect3DTexture9*> textures)
{
	m_vTextures = textures;
}

IDirect3DTexture9* GameObject::GetTexture() const
{
	return m_pTexture;
}

void GameObject::SetTexture(IDirect3DTexture9* texture)
{
	m_pTexture = texture;
}

string GameObject::GetTextureFilename() const
{
	return m_strTextureFileName;
}

void GameObject::SetTextureFilename(const string& filename)
{
	m_strTextureFileName = filename;
}

int GameObject::IsBindable() const
{
	return m_bIsBindable;
}

void GameObject::SetIsBindable(int bindable)
{
	m_bIsBindable = bindable;
}

string GameObject::GetBindedToAnimatedModelName() const
{
	return m_strBindedToAnimatedModelName;
}

void GameObject::SetBindedToAnimatedModelName(const string& bindedToAnimatedModelName)
{
	m_strBindedToAnimatedModelName = bindedToAnimatedModelName;
}

string GameObject::GetBindedToBoneName() const
{
	return m_strBindedToBoneName;
}

void GameObject::SetBindedToBoneName(const string& bindedToBoneName)
{
	m_strBindedToBoneName = bindedToBoneName;
}

unordered_map<GameObject*, string>& GameObject::GetBindedObjects()
{
	return m_mapBindedObjects;
}

void GameObject::SetBindedObjects(unordered_map<GameObject*, string> bindedObjects)
{
	m_mapBindedObjects = bindedObjects;
}

bool GameObject::IsAttacked() const
{
	return m_bIsAttacked;
}

void GameObject::SetAttacked(bool attacked)
{
	m_bIsAttacked = attacked;
}

bool GameObject::IsAttacking() const
{
	return m_bIsAttacking;
}

void GameObject::SetAttacking(bool attacking)
{
	m_bIsAttacking = attacking;
}

bool GameObject::IsDead() const
{
	return m_bIsDead;
}

void GameObject::SetDead(bool dead)
{
	m_bIsDead = dead;
}

bool GameObject::HasDialogue() const
{
	return m_bHasDialogue;
}

void GameObject::SetHasDialogue(bool hasDialogue)
{
	m_bHasDialogue = hasDialogue;
}

string GameObject::GetAttackerName() const
{
	return m_strAttackerName;
}

void GameObject::SetAttackerName(const string& attackerName)
{
	m_strAttackerName = attackerName;
}

string GameObject::GetActorType() const
{
	return m_strActorType;
}

void GameObject::SetActorType(const string& actorType)
{
	m_strActorType = actorType;
}

ID3DXMesh** GameObject::GetTitleMesh()
{
	return& m_pTitleMesh;
}

void GameObject::SetTitleMesh(ID3DXMesh* titleMesh)
{
	m_pTitleMesh = titleMesh;
}

D3DXVECTOR3& GameObject::GetTitleLookVector()
{
	return m_vTitleLook;
}

D3DXVECTOR3& GameObject::GetTitleRightVector()
{
	return m_vTitleRight;
}

D3DXVECTOR3& GameObject::GetTitleUpVector()
{
	return m_vTitleUp;
}

D3DXMATRIX GameObject::GetTitleRotationMatrix() const
{
	return m_TitleRotationMatrix;
}

void GameObject::SetTitleRotationMatrix(D3DXMATRIX titleRotationMatrix)
{
	m_TitleRotationMatrix = titleRotationMatrix;
}

float GameObject::GetTitleRotationAnglyByY() const
{
	return m_fTitleRotationAngleByY;
}

void GameObject::SetTitleRotationAnglyByY(float angle)
{
	m_fTitleRotationAngleByY = angle;
}

void GameObject::ModifyTitleRotationAnglyByY(float delta)
{
	m_fTitleRotationAngleByY += delta;
}

string GameObject::GetTitleForQuest() const
{
	return m_strTitleForQuest;
}

void GameObject::SetTitleForQuest(const string& titleForQuest)
{
	m_strTitleForQuest = titleForQuest;
}

ID3DXMesh** GameObject::GetTitleForQuestMesh()
{
	return &m_pTitleForQuestMesh;
}

void GameObject::SetTitleForQuestMesh(ID3DXMesh* titleForQuestMesh)
{
	m_pTitleForQuestMesh = titleForQuestMesh;
}

D3DXVECTOR3& GameObject::GetTitleForQuestLookVector()
{
	return m_vTitleForQuestLook;
}

D3DXVECTOR3& GameObject::GetTitleForQuestRightVector()
{
	return m_vTitleForQuestRight;
}

D3DXVECTOR3& GameObject::GetTitleForQuestUpVector()
{
	return m_vTitleForQuestUp;
}

D3DXMATRIX GameObject::GetTitleForQuestRotationMatrix() const
{
	return m_TitleForQuestRotationMatrix;
}

void GameObject::SetTitleForQuestRotationMatrix(D3DXMATRIX titleForQuestRotationMatrix)
{
	m_TitleForQuestRotationMatrix = titleForQuestRotationMatrix;
}

float GameObject::GetTitleForQuestRotationAnglyByY() const
{
	return m_fTitleForQuestRotationAngleByY;
}

void GameObject::SetTitleForQuestRotationAnglyByY(float angle)
{
	m_fTitleForQuestRotationAngleByY = angle;
}

void GameObject::ModifyTitleForQuestRotationAnglyByY(float delta)
{
	m_fTitleForQuestRotationAngleByY += delta;
}

vector<D3DXMATRIX>& GameObject::GetFinalBonesMatrices()
{
	return m_vFinalBonesMatrices;
}

void GameObject::SetFinalBonesMatrices(vector<D3DXMATRIX> bonesMatrices)
{
	m_vFinalBonesMatrices = bonesMatrices;
}

vector<D3DXMATRIX*>& GameObject::GetToRootMatrices()
{
	return m_vToRootMatrices;
}

void GameObject::SetToRootMatrices(vector<D3DXMATRIX*> toRootMatrices)
{
	m_vToRootMatrices = toRootMatrices;
}

D3DXFRAME* GameObject::GetRootFrame() const
{
	return m_pRoot;
}

void GameObject::SetRootFrame(D3DXFRAME* rootFrame)
{
	m_pRoot = rootFrame;
}

DWORD GameObject::GetMaxVertexInfluences() const
{
	return m_nMaxVertInfluences;
}

void GameObject::SetMaxVertexInfluences(DWORD maxVertexInfluences)
{
	m_nMaxVertInfluences = maxVertexInfluences;
}

DWORD GameObject::GetAmountOfBones() const
{
	return m_nNumBones;
}

void GameObject::SetAmountOfBones(DWORD amountOfBones)
{
	m_nNumBones = amountOfBones;
}

ID3DXSkinInfo* GameObject::GetSKinInfo() const
{
	return m_pSkinInfo;
}

void GameObject::SetSkinInfo(ID3DXSkinInfo* skinInfo)
{
	m_pSkinInfo = skinInfo;
}

ID3DXAnimationController* GameObject::GetAnimationController() const
{
	return m_pAnimController;
}

void GameObject::SetAnimationController(ID3DXAnimationController* animController)
{
	m_pAnimController = animController;
}

LPD3DXANIMATIONSET GameObject::GetCurrentAnimationSet() const
{
	return m_pCurrentAnimSet;
}

void GameObject::SetCurrentAnimationSet(LPD3DXANIMATIONSET currentAnimationSet)
{
	m_pCurrentAnimSet = currentAnimationSet;
}

LPD3DXANIMATIONSET GameObject::GetSecondAnimationSet() const
{
	return m_pSecondAnimSet;
}

void GameObject::SetSecondAnimationSet(LPD3DXANIMATIONSET secondAnimationSet)
{
	m_pSecondAnimSet = secondAnimationSet;
}

DWORD GameObject::GetCurrentAnimationTrack() const
{
	return m_nCurrentAnimTrack;
}

void GameObject::SetCurrentAnimationTrack(DWORD currentAnimationTrack)
{
	m_nCurrentAnimTrack = currentAnimationTrack;
}

DWORD GameObject::GetNewAnimationTrack() const
{
	return m_nNewAnimTrack;
}

void GameObject::SetNewAnimationTrack(DWORD newAnimationTrack)
{
	m_nNewAnimTrack = newAnimationTrack;
}

bool GameObject::IsSwitched() const
{
	return m_bIsSwitched;
}

void GameObject::SetIsSwitched(bool isSwitched)
{
	m_bIsSwitched = isSwitched;
}

float GameObject::GetRotationAngleByX() const
{
	return m_fRotAngleX;
}

void GameObject::SetRotationAngleByX(float rotationAngleByX)
{
	m_fRotAngleX = rotationAngleByX;
}

void GameObject::ModifyRotationAngleByX(float delta)
{
	m_fRotAngleX += delta;
}

float GameObject::GetRotationAngleByY() const
{
	return m_fRotAngleY;
}

void GameObject::SetRotationAngleByY(float rotationAngleByY)
{
	m_fRotAngleY = rotationAngleByY;
}


void GameObject::ModifyRotationAngleByY(float delta)
{
	m_fRotAngleY += delta;
}

float GameObject::GetRotationAngleByZ() const
{
	return m_fRotAngleZ;
}

void GameObject::SetRotationAngleByZ(float rotationAngleByZ)
{
	m_fRotAngleZ = rotationAngleByZ;
}

void GameObject::ModifyRotationAngleByZ(float delta)
{
	m_fRotAngleZ += delta;
}

bool GameObject::IsPicked() const
{
	return m_bIsPicked;
}

void GameObject::SetPicked(bool picked)
{
	m_bIsPicked = picked;
}

ID3DXMesh* GameObject::GetBBMesh() const
{
	return m_pBoundingBoxMesh;
}

void GameObject::SetBBMesh(ID3DXMesh* BBMesh)
{
	m_pBoundingBoxMesh = BBMesh;
}

Material GameObject::GetBBMaterial() const
{
	return m_BoundingBoxMaterial;
}

void GameObject::SetBBMaterial(Material material)
{
	m_BoundingBoxMaterial = material;
}

AABB GameObject::GetBB() const
{
	return m_BoundingBox;
}

void GameObject::SetBB(AABB boundingBox)
{
	m_BoundingBox = boundingBox;
}

D3DXMATRIX GameObject::GetBBOffsetMatrix() const
{
	return m_BoundingBoxOffset;
}

void GameObject::SetBBOffsetMatrix(D3DXMATRIX offsetMatrix)
{
	m_BoundingBoxOffset = offsetMatrix;
}

EGameObjectType GameObject::GetObjectType() const
{
	return m_eGameObjectType;
}

void GameObject::SetObjectType(EGameObjectType type)
{
	m_eGameObjectType = type;
}

D3DXMATRIX GameObject::GetCombinedTransfMatrix() const
{
	return m_CombinedTransformationMatrix;
}

void GameObject::SetCombinedTransfMatrix(D3DXMATRIX mat)
{
	m_CombinedTransformationMatrix = mat;
}