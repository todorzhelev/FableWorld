#include <stdafx.h>
#include "GameObjectManager.h"
#include "DirectInput.h"
#include "SkinnedModel.h"

GameObjectManager* m_pGameObjManager = nullptr;

/////////////////////////////////////////////////////////////////////////

GameObjectManager::GameObjectManager(bool bShouldRenderTitles, bool bShouldHighlightPickedObjects, bool bShouldRenderAxis, bool bAreObjectsGrounded, bool bShouldRenderBoundingBoxes, bool bShouldPickOnlySkinnedModels)
{
	m_pPickedObject	= nullptr;
	m_bShouldHighlightPickedObjects = bShouldHighlightPickedObjects;
	m_bShouldRenderTitles			= bShouldRenderTitles;
	m_bShouldRenderAxis				= bShouldRenderAxis;
	m_bAreObjectsGrounded			= bAreObjectsGrounded;
	m_bShouldRenderBoundingBoxes	= bShouldRenderBoundingBoxes;
	m_bShouldPickOnlySkinnedModels	= bShouldPickOnlySkinnedModels;
}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::AddGameObject(GameObject* pGameObject)
{
	m_gameObjects.push_back(pGameObject);

	if( pGameObject->GetObjectType() == EGameObjectType_Skinned )
	{
		m_skinnedModels.push_back(static_cast<SkinnedModel*>(pGameObject));
	}
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

}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::UpdatePicking()
{
	//TODO: fix this to work with normal objects too
	for (auto& gameObject : m_gameObjects)
	{
		float dist = gameObject->GetDistanceToPickedObject();

		if (dist != -1)
		{
			m_mapPickedObjects[dist] = gameObject;
		}
	}

	//the closest object is picked
	//in the game skinned models only should be picked. fix this
	if (!m_mapPickedObjects.empty())
	{
		auto pClosestPickedObject = (m_mapPickedObjects.begin()->second);
		if (pClosestPickedObject)
		{
			pClosestPickedObject->SetPicked(true);
		}

		m_mapPickedObjects.clear();
	}
}
/////////////////////////////////////////////////////////////////////////

std::vector<GameObject*>& GameObjectManager::GetGameObjects()
{
	return m_gameObjects;
}

/////////////////////////////////////////////////////////////////////////

std::vector<SkinnedModel*>& GameObjectManager::GetSkinnedModels()
{
	return m_skinnedModels;
}

/////////////////////////////////////////////////////////////////////////

GameObject* GameObjectManager::GetObjectByName(std::string name)
{
	for(auto& object : m_gameObjects )
	{
		if( !object->GetName().compare(name) )
		{
			return object;
		}
	}

	return nullptr;
}

/////////////////////////////////////////////////////////////////////////

SkinnedModel* GameObjectManager::GetSkinnedModelByName(std::string name)
{
	for (auto& object : m_skinnedModels)
	{
		if ( !object->GetName().compare(name) )
		{
			return static_cast<SkinnedModel*>(object);
		}
	}

	return nullptr;
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

void GameObjectManager::SetShouldRenderBoundingBoxes(bool bShouldRenderBoundingBoxes)
{
	m_bShouldRenderBoundingBoxes = bShouldRenderBoundingBoxes;
}

/////////////////////////////////////////////////////////////////////////

bool GameObjectManager::ShouldRenderBoundingBoxes()
{
	return m_bShouldRenderBoundingBoxes;
}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::SetShouldPickOnlySkinnedModels(bool bShouldPickOnlySkinnedModels)
{
	m_bShouldPickOnlySkinnedModels = bShouldPickOnlySkinnedModels;
}

/////////////////////////////////////////////////////////////////////////

bool GameObjectManager::ShouldPickOnlySkinnedModels()
{
	return m_bShouldPickOnlySkinnedModels;
}

/////////////////////////////////////////////////////////////////////////
