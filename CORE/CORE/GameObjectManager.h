#pragma once

#include"GameObject.h"
#include"SkinnedModel.h"

class SkinnedModel;

class GameObjectManager {
public:
	GameObjectManager(bool bShouldRenderTitles, bool bShouldHighlightPickedObjects, bool bShouldRenderAxis, bool bAreObjectsGrounded, bool bShouldRenderBoundingBoxes, bool bShouldPickOnlySkinnedModels);
	void AddGameObject(std::shared_ptr<GameObject> pGameObject);
	auto GetPickedObject() -> std::shared_ptr<GameObject>;
	void SetPickedObject(std::shared_ptr<GameObject> pPickedObject);
	auto GetGameObjects() -> std::vector<std::shared_ptr<GameObject>>&;
	auto GetSkinnedModels() -> std::vector<std::shared_ptr<SkinnedModel>>&;
	auto GetObjectByName(std::string name) -> std::shared_ptr<GameObject>;
	auto GetSkinnedModelByName(std::string name) -> std::shared_ptr<SkinnedModel>;
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
	bool m_bShouldRenderTitles;
	//used in the level editor so it can be visually seen if object is picked
	bool m_bShouldHighlightPickedObjects;
	//used in the level editor to visually show the look, up and right vector
	bool m_bShouldRenderAxis;
	bool m_bAreObjectsGrounded;
	bool m_bShouldRenderBoundingBoxes;
	bool m_bShouldPickOnlySkinnedModels;

	std::shared_ptr<GameObject>	m_pPickedObject;
	std::vector<std::shared_ptr<GameObject>>   m_gameObjects; //contains all game objects, including skinned models and binded static models
	std::vector<std::shared_ptr<SkinnedModel>> m_skinnedModels; //a separate vector of skinned models for easier access
	std::map<int, std::shared_ptr<GameObject>> m_mapPickedObjects; //stores all the hit objects by the mouse from single click
	static int m_lastObjectId;
};