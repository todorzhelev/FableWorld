#include <stdafx.h>
#include "GameObjectManager.h"
#include "DirectInput.h"
#include "SkinnedModel.h"

int GameObjectManager::m_lastObjectId = 0;

/////////////////////////////////////////////////////////////////////////

GameObjectManager::GameObjectManager(bool bShouldRenderTitles, bool bShouldHighlightPickedObjects, bool bShouldRenderAxis, bool bAreObjectsGrounded, bool bShouldRenderBoundingBoxes, bool bShouldPickOnlySkinnedModels) {
	m_pPickedObject	= nullptr;
	m_bShouldHighlightPickedObjects = bShouldHighlightPickedObjects;
	m_bShouldRenderTitles			= bShouldRenderTitles;
	m_bShouldRenderAxis				= bShouldRenderAxis;
	m_bAreObjectsGrounded			= bAreObjectsGrounded;
	m_bShouldRenderBoundingBoxes	= bShouldRenderBoundingBoxes;
	m_bShouldPickOnlySkinnedModels	= bShouldPickOnlySkinnedModels;

	printf("GameObjectManager created\n");
}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::AddGameObject(std::shared_ptr<GameObject> pGameObject) {
	m_lastObjectId++;
	pGameObject->SetId(m_lastObjectId);

	m_gameObjects.push_back(pGameObject);

	if (pGameObject->GetObjectType() == EGameObjectType_Skinned) {
		auto skinnedModel = std::static_pointer_cast<SkinnedModel>(pGameObject);
		m_skinnedModels.push_back(skinnedModel);
	}
}

/////////////////////////////////////////////////////////////////////////

std::shared_ptr<GameObject> GameObjectManager::GetPickedObject() {
	return m_pPickedObject;
}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::SetPickedObject(std::shared_ptr<GameObject> pPickedObject) {
	m_pPickedObject = pPickedObject;
}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::OnUpdate() {}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::UpdatePicking(const std::unique_ptr<Camera>& camera) {
	auto& mapPickedObjects = m_mapPickedObjects;

	//dont want to capture everything
	auto lambda = [&mapPickedObjects, &camera](std::shared_ptr<GameObject> obj) {
		float dist = obj->GetDistanceToPickedObject(camera);

		if (dist != -1) {
			mapPickedObjects[dist] = obj;
		}
	};

	if (!m_bShouldPickOnlySkinnedModels) {
		for (auto& obj : m_gameObjects) {
			lambda(obj);
		}
	}
	else {
		for (auto& obj : m_skinnedModels) {
			lambda(obj);
		}
	}

	//the closest object is picked
	//in the game skinned models only should be picked. fix this
	if (!m_mapPickedObjects.empty()) {
		auto pClosestPickedObject = (m_mapPickedObjects.begin()->second);
		if (pClosestPickedObject) {
			//the old picked object is no longer picked
			if (m_pPickedObject) {
				m_pPickedObject->SetPicked(false);
			}

			pClosestPickedObject->SetPicked(true);
			m_pPickedObject = pClosestPickedObject;
		}

		m_mapPickedObjects.clear();
	}
}
/////////////////////////////////////////////////////////////////////////

std::vector<std::shared_ptr<GameObject>>& GameObjectManager::GetGameObjects() {
	return m_gameObjects;
}

/////////////////////////////////////////////////////////////////////////

std::vector<std::shared_ptr<SkinnedModel>>& GameObjectManager::GetSkinnedModels() {
	return m_skinnedModels;
}

/////////////////////////////////////////////////////////////////////////

std::shared_ptr<GameObject> GameObjectManager::GetObjectByName(std::string name) {
	for(auto& object : m_gameObjects ) {
		if (!object->GetName().compare(name)) {
			return object;
		}
	}
	return nullptr;
}

/////////////////////////////////////////////////////////////////////////

std::shared_ptr<SkinnedModel> GameObjectManager::GetSkinnedModelByName(std::string name) {
	for (auto& object : m_skinnedModels) {
		if (!object->GetName().compare(name)) {
			return object;
		}
	}
	return nullptr;
}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::SetShouldRenderTitles(bool bShouldRenderTitles) {
	m_bShouldRenderTitles = bShouldRenderTitles;
}

/////////////////////////////////////////////////////////////////////////

bool GameObjectManager::ShouldRenderTitles() {
	return m_bShouldRenderTitles;
}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::SetShouldHighlightPickedObjects(bool bShouldHighlightPickedObjects) {
	m_bShouldHighlightPickedObjects = bShouldHighlightPickedObjects;
}

/////////////////////////////////////////////////////////////////////////

bool GameObjectManager::ShouldHighlightPickedObjects() {
	return m_bShouldHighlightPickedObjects;
}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::SetShouldRenderAxis(bool bShouldRenderAxis) {
	m_bShouldRenderAxis = bShouldRenderAxis;
}

/////////////////////////////////////////////////////////////////////////

bool GameObjectManager::ShouldRenderAxis() {
	return m_bShouldRenderAxis;
}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::SetAreObjectsGrounded(bool bAreObjectsGrounded) {
	m_bAreObjectsGrounded = bAreObjectsGrounded;
}

/////////////////////////////////////////////////////////////////////////

bool GameObjectManager::AreObjectsGrounded() {
	return m_bAreObjectsGrounded;
}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::SetShouldRenderBoundingBoxes(bool bShouldRenderBoundingBoxes) {
	m_bShouldRenderBoundingBoxes = bShouldRenderBoundingBoxes;
}

/////////////////////////////////////////////////////////////////////////

bool GameObjectManager::ShouldRenderBoundingBoxes() {
	return m_bShouldRenderBoundingBoxes;
}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::SetShouldPickOnlySkinnedModels(bool bShouldPickOnlySkinnedModels) {
	m_bShouldPickOnlySkinnedModels = bShouldPickOnlySkinnedModels;
}

/////////////////////////////////////////////////////////////////////////

bool GameObjectManager::ShouldPickOnlySkinnedModels() {
	return m_bShouldPickOnlySkinnedModels;
}

/////////////////////////////////////////////////////////////////////////

void GameObjectManager::RemoveObject(std::string objId) {
	auto it = std::find_if(m_gameObjects.begin(), m_gameObjects.end(),
							[objId](std::shared_ptr<GameObject> obj) { return !obj->GetName().compare(objId); });

	if (it != m_gameObjects.end()) {
		if ((*it)->GetObjectType() == EGameObjectType_Skinned) {
			auto skinnedIt = std::find_if(m_skinnedModels.begin(), m_skinnedModels.end(),
											[objId](std::shared_ptr<SkinnedModel> obj) { return !obj->GetName().compare(objId); });

			if (skinnedIt != m_skinnedModels.end()){
				m_skinnedModels.erase(skinnedIt);
			}
		}

		m_gameObjects.erase(it);
	}
}

/////////////////////////////////////////////////////////////////////////