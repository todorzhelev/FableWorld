#pragma once

#include"GameObject.h"
#include"SkinnedModel.h"

class SkinnedModel;

class GameObjectManager {
public:
	GameObjectManager(bool bShouldRenderTitles, bool bShouldHighlightPickedObjects, bool bShouldRenderAxis, bool bAreObjectsGrounded, bool bShouldRenderBoundingBoxes, bool bShouldPickOnlySkinnedModels);
	void AddGameObject(GameObject* pGameObject);
	GameObject* GetPickedObject();
	void SetPickedObject(GameObject* pPickedObject);
	std::vector<GameObject*>&  GetGameObjects();
	std::vector<SkinnedModel*>& GetSkinnedModels();
	GameObject* GetObjectByName(std::string name);
	SkinnedModel* GetSkinnedModelByName(std::string name);
	void SetShouldRenderTitles(bool bShouldRenderTitles);
	bool ShouldRenderTitles();
	void SetShouldHighlightPickedObjects(bool bShouldHighlightPickedObjects);
	bool ShouldHighlightPickedObjects();
	void SetShouldRenderAxis(bool bShouldRenderAxis);
	bool ShouldRenderAxis();
	void SetAreObjectsGrounded(bool bAreObjectsGrounded);
	bool AreObjectsGrounded();
	void SetShouldRenderBoundingBoxes(bool bShouldRenderAxis);
	bool ShouldRenderBoundingBoxes();
	void SetShouldPickOnlySkinnedModels(bool bShouldPickOnlySkinnedModels);
	bool ShouldPickOnlySkinnedModels();
	void RemoveObject(std::string objId);
	void OnUpdate();
	void UpdatePicking(const std::unique_ptr<Camera>& camera);
private:
	//in the level editor the titles above the skinned meshes are not needed
	bool	    m_bShouldRenderTitles;
	//used in the level editor so it can be visually seen if object is picked
	bool	    m_bShouldHighlightPickedObjects;
	//used in the level editor to visually show the look, up and right vector
	bool		m_bShouldRenderAxis;
	bool		m_bAreObjectsGrounded;
	bool		m_bShouldRenderBoundingBoxes;
	bool		m_bShouldPickOnlySkinnedModels;
	GameObject*	m_pPickedObject;
	//must be unique pointers so I dont need some retarded destroy function for the meshes
	std::vector<GameObject*> m_gameObjects;
	std::vector<SkinnedModel*> m_skinnedModels;
	//stores all the hit objects by the mouse from single click
	std::map<int,GameObject*> m_mapPickedObjects;
	static int m_lastObjectId;
};