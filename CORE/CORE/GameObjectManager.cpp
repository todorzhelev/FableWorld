#include "GameObjectManager.h"
#include "DirectInput.h"
#include "SkinnedMesh.h"

GameObjectManager* m_pGameObjManager = nullptr;

/////////////////////////////////////////////////////////////////////////

GameObjectManager::GameObjectManager(bool bShouldRenderTitles, bool bShouldHighlightPickedObjects, bool bShouldRenderAxis, bool bAreObjectsGrounded)
{
	m_pPickedObject	= nullptr;
	m_bShouldHighlightPickedObjects = bShouldHighlightPickedObjects;
	m_bShouldRenderTitles  = bShouldRenderTitles;
	m_bShouldRenderAxis    = bShouldRenderAxis;
	m_bAreObjectsGrounded = bAreObjectsGrounded;
}

/////////////////////////////////////////////////////////////////////////

/*
Function:addGameObject
Purpose:puts new GameObject in the map
*/
void GameObjectManager::AddGameObject(GameObject* pGameObject)
{
	m_mapGameObjects[pGameObject->m_strModelName] = pGameObject;
}

/////////////////////////////////////////////////////////////////////////

GameObject* GameObjectManager::GetPickedObject()
{
	return m_pPickedObject;
}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::SetPickedObject(GameObject* pPickedObject)
{
	m_pPickedObject = pPickedObject;
}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::OnUpdate()
{
	for(auto it=m_pGameObjManager->GetGameObjects().begin();it!=m_pGameObjManager->GetGameObjects().end();it++)
	{
		if( (*it).second->m_eGameObjectType == EGameObjectType_Skinned )
		{	
			if( pDinput->IsMouseButtonDown(0) )
			{
				D3DXVECTOR3 vOrigin(0.0f, 0.0f, 0.0f);
				D3DXVECTOR3 vDir(0.0f, 0.0f, 0.0f);

				GetWorldPickingRay(vOrigin, vDir);
				AABB box;
				it->second->m_BoundingBox.transformByMatrix(it->second->m_CombinedTransformationMatrix, box);
				if( D3DXBoxBoundProbe(&box.minPt, &box.maxPt, &vOrigin, &vDir) )
				{
					SkinnedMesh* pMesh = static_cast<SkinnedMesh*>(it->second);

					D3DXFRAME* pFrame = pMesh->FindFrameWithMesh(it->second->m_pRoot);

					float nDistance = 0.0f;

					//m_mapPickedObjects[nDistance] = it->second;

					if( IsPickedSkinnedObject(pFrame,it->second->m_CombinedTransformationMatrix,vOrigin,vDir,nDistance) )
					{
						m_mapPickedObjects[nDistance] = it->second;
					}
				}
			}
		}
		else if( (*it).second->m_eGameObjectType == EGameObjectType_Static )
		{
			//if( pDinput->IsMouseButtonDown(0) )
			{
				float nDistance = 0.0f;
				if( IsPickedStaticObject(it->second,nDistance) )
				{
					m_mapPickedObjects[nDistance] = it->second;
				}
			}
		}

	}

	//the closest object is picked
	if( !m_mapPickedObjects.empty() )
	{
		auto pClosestPickedObject = (m_mapPickedObjects.begin()->second);
		if( pClosestPickedObject )
		{
			pClosestPickedObject->m_bIsPicked = true;
		}

		m_mapPickedObjects.clear();
	}
}

/////////////////////////////////////////////////////////////////////////

bool GameObjectManager::IsPickedStaticObject(GameObject* pObj, float& nDistance)
{
	D3DXVECTOR3 vOrigin(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vDir(0.0f, 0.0f, 0.0f);

	GetWorldPickingRay(vOrigin, vDir);

	AABB box;
	bool bIsPicked = false;

	pObj->m_BoundingBox.transformByMatrix(pObj->m_CombinedTransformationMatrix, box);
	bIsPicked = D3DXBoxBoundProbe(&box.minPt, &box.maxPt, &vOrigin, &vDir);

	if( bIsPicked )
	{
		//by finding the inverse of mesh CombinedMatrix we transform from World space to the mesh local space
		//this is needed, because we have to transform the picking vectors to the mesh local space
		//which is required by the D3DXIntersect function
		D3DXMATRIX InverseWorldMatrix;
		D3DXMatrixInverse(&InverseWorldMatrix, 0, &pObj->m_CombinedTransformationMatrix);

		//transform the Ray using the inverse matrix
		D3DXVec3TransformCoord(&vOrigin, &vOrigin, &InverseWorldMatrix);
		D3DXVec3TransformNormal(&vDir, &vDir, &InverseWorldMatrix);

		BOOL hit = false;
		DWORD faceIndex = -1;
		float u = 0.0f;
		float v = 0.0f;
		float dist = 0.0f;
		ID3DXBuffer* allhits = 0;
		DWORD numHits = 0;
		D3DXIntersect(pObj->m_pMesh, &vOrigin, &vDir, &hit,&faceIndex, &u, &v, &dist, &allhits, &numHits);
		releaseX(allhits);

		nDistance = dist;
		return hit;
	}
}

/////////////////////////////////////////////////////////////////////////

bool GameObjectManager::IsPickedSkinnedObject(D3DXFRAME* pFrame,D3DXMATRIX combinedMatrix,D3DXVECTOR3 vOrigin,D3DXVECTOR3 vDir, float& nDistance )
{
	D3DXMESHCONTAINER* pMeshContainer = pFrame->pMeshContainer;

	D3DXFRAME* pSibling	 = pFrame->pFrameSibling;
    D3DXFRAME* pFirstChild = pFrame->pFrameFirstChild;

	if( pMeshContainer != NULL )
	{
		//fout<<pFrame->Name << endl;

		D3DXMATRIX InverseWorldMatrix;
		D3DXMatrixInverse(&InverseWorldMatrix, 0, &combinedMatrix);

		//transform the Ray using the inverse matrix
		D3DXVec3TransformCoord(&vOrigin, &vOrigin, &InverseWorldMatrix);
		D3DXVec3TransformNormal(&vDir, &vDir, &InverseWorldMatrix);

		BOOL hit = false;
		DWORD faceIndex = -1;
		float u = 0.0f;
		float v = 0.0f;
		float dist = 0.0f;
		ID3DXBuffer* allhits = 0;
		DWORD numHits = 0;
	
		D3DXIntersect(pMeshContainer->MeshData.pMesh, &vOrigin, &vDir, &hit,&faceIndex, &u, &v, &dist, &allhits, &numHits);
		releaseX(allhits);

		nDistance = dist;

		return hit;
	}

	if( pSibling )
	{
		IsPickedSkinnedObject(pSibling, combinedMatrix,vOrigin,vDir,nDistance);
	}

	if( pFirstChild )
	{
		IsPickedSkinnedObject(pFirstChild, combinedMatrix,vOrigin,vDir,nDistance);
	}

	return false;
	
}

/////////////////////////////////////////////////////////////////////////

map<string,GameObject*>& GameObjectManager::GetGameObjects()
{
	return m_mapGameObjects;
}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::SetShouldRenderTitles(bool bShouldRenderTitles)
{
	m_bShouldRenderTitles = bShouldRenderTitles;
}

/////////////////////////////////////////////////////////////////////////

bool GameObjectManager::ShouldRenderTitles()
{
	return m_bShouldRenderTitles;
}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::SetShouldHighlightPickedObjects(bool bShouldHighlightPickedObjects)
{
	m_bShouldHighlightPickedObjects = bShouldHighlightPickedObjects;
}

/////////////////////////////////////////////////////////////////////////

bool GameObjectManager::ShouldHighlightPickedObjects()
{
	return m_bShouldHighlightPickedObjects;
}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::SetShouldRenderAxis(bool bShouldRenderAxis)
{
	m_bShouldRenderAxis = bShouldRenderAxis;
}

/////////////////////////////////////////////////////////////////////////

bool GameObjectManager::ShouldRenderAxis()
{
	return m_bShouldRenderAxis;
}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::SetAreObjectsGrounded(bool bAreObjectsGrounded)
{
	m_bAreObjectsGrounded = bAreObjectsGrounded;
}

/////////////////////////////////////////////////////////////////////////

bool GameObjectManager::AreObjectsGrounded()
{
	return m_bAreObjectsGrounded;
}

/////////////////////////////////////////////////////////////////////////