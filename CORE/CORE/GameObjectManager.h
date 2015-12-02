#include<map>
#include<string>
#include"GameObject.h"

#pragma once

class GameObjectManager
{
public:

	GameObjectManager(bool bShouldRenderTitles, bool bShouldHighlightPickedObjects, bool bShouldRenderAxis, bool bAreObjectsGrounded, bool bShouldRenderBoundingBoxes);

	void AddGameObject(GameObject* pGameObject);

	GameObject* GetPickedObject();

	void SetPickedObject(GameObject* pPickedObject);

	std::vector<GameObject*>&  GetGameObjects();

	GameObject* GetObjectByName(std::string name);

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

	void OnUpdate();

	bool IsPickedStaticObject(GameObject* pObj, float& nDistance);

	bool IsPickedSkinnedObject(D3DXFRAME* pFrame,D3DXMATRIX combinedMatrix,D3DXVECTOR3 vOrigin,D3DXVECTOR3 vDir, float& nDistance );

private:

	//in the level editor the titles above the skinned meshes are not needed
	bool	    m_bShouldRenderTitles;

	//used in the level editor so it can be visually seen if object is picked
	bool	    m_bShouldHighlightPickedObjects;

	//used in the level editor to visually show the look, up and right vector
	bool		m_bShouldRenderAxis;

	bool		m_bAreObjectsGrounded;

	bool		m_bShouldRenderBoundingBoxes;

	GameObject*	m_pPickedObject;

	std::vector<GameObject*> m_gameObjects;
	//map<string,GameObject*> m_mapGameObjects;

	//stores all the hit objects by the mouse from single click
	map<int,GameObject*> m_mapPickedObjects;
};

extern GameObjectManager* m_pGameObjManager;

