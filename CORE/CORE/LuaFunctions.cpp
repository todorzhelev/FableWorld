#include <stdafx.h>
#include"LuaFunctions.h"

std::string mainHero = "";

/////////////////////////////////////////////////////////////////////////
/*
Function:l_addStaticModel
Purpose:load static models, i.e. models whithout animations
*/
int l_addStaticModel(lua_State* L)
{
	GameObject* pMesh = new StaticModel;

	//GameObject obj;
	pMesh->GetLookVector()	 = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	pMesh->GetRightVector()  = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	pMesh->GetUpVector()	 = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	D3DXVECTOR3 pos;
	lua_getglobal(L, "x");
	pos.x = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	lua_getglobal(L, "y");
	pos.y = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	lua_getglobal(L, "z");
	pos.z = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	pMesh->SetPosition(pos);

	lua_getglobal(L, "scale");
	pMesh->SetScale(static_cast<float>(lua_tonumber(L,lua_gettop(L))));

	lua_getglobal(L, "rotX");
	pMesh->SetRotationAngleByX(static_cast<float>(lua_tonumber(L,lua_gettop(L))));

	lua_getglobal(L, "rotY");
	pMesh->SetRotationAngleByY(static_cast<float>(lua_tonumber(L,lua_gettop(L))));

	lua_getglobal(L, "rotZ");
	pMesh->SetRotationAngleByZ(static_cast<float>(lua_tonumber(L,lua_gettop(L))));

	lua_getglobal(L, "bindable");
	pMesh->SetIsBindable(static_cast<int>(lua_tonumber(L,lua_gettop(L))));

	lua_getglobal(L,"bindToAnModel");
	pMesh->SetBindedToAnimatedModelName(lua_tostring(L,lua_gettop(L)));

	lua_getglobal(L,"bindToAnModelBone");
	pMesh->SetBindedToBoneName(lua_tostring(L,lua_gettop(L)));

	lua_getglobal(L,"modelName");
	pMesh->SetName(lua_tostring(L,lua_gettop(L)));

	lua_getglobal(L, "modelFileName");
	pMesh->SetModelFilename(lua_tostring(L,lua_gettop(L)));

	pMesh->LoadGameObject();

	pMesh->SetObjectType(EGameObjectType_Static);

	m_pGameObjManager->AddGameObject(pMesh);
	
	if (pMesh->IsBindable() && !pMesh->GetBindedToAnimatedModelName().empty() && !pMesh->GetBindedToBoneName().empty())
	{
		GameObject* obj = m_pGameObjManager->GetObjectByName(pMesh->GetBindedToAnimatedModelName());
		SkinnedModel* pSkinnedModel = nullptr;
		if (obj != nullptr)
		{
			pSkinnedModel = static_cast<SkinnedModel*>(obj);
		}

		pSkinnedModel->BindWeaponToModel(pMesh->GetName(), pMesh->GetBindedToBoneName());
	}

	return 1;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:l_addAnimatedModel
Purpose:load animated model
*/
int l_addAnimatedModel(lua_State* L)
{
	SkinnedModel* pMesh = new SkinnedModel;

	pMesh->GetLookVector()  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	pMesh->GetRightVector() = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	pMesh->GetUpVector()    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	pMesh->GetTitleLookVector()  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	pMesh->GetTitleRightVector() = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	pMesh->GetTitleUpVector()    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	pMesh->GetTitleForQuestLookVector()  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	pMesh->GetTitleForQuestRightVector() = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	pMesh->GetTitleForQuestUpVector()    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pMesh->SetTitleForQuestRotationAnglyByY(0.0);

	D3DXVECTOR3 pos;

	lua_getglobal(L, "x");
	pos.x = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	lua_getglobal(L, "y");
	pos.y = static_cast<float>(lua_tonumber(L,lua_gettop(L))); 

	lua_getglobal(L, "z");
	pos.z = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	pMesh->SetPosition(pos);

	lua_getglobal(L, "scale");
	pMesh->SetScale(static_cast<float>(lua_tonumber(L,lua_gettop(L))));

	lua_getglobal(L, "rotX");
	pMesh->SetRotationAngleByX(static_cast<float>(lua_tonumber(L,lua_gettop(L))));

	lua_getglobal(L, "rotY");
	pMesh->SetRotationAngleByY(static_cast<float>(lua_tonumber(L,lua_gettop(L))));

	pMesh->SetRotationAngleByZ(0);

	lua_getglobal(L, "titleRotY");
	pMesh->SetTitleRotationAnglyByY(static_cast<float>(lua_tonumber(L,lua_gettop(L))));

	lua_getglobal(L,"modelName");
	pMesh->SetName(lua_tostring(L,lua_gettop(L)));

	lua_getglobal(L,"titleForQuest");
	pMesh->SetTitleForQuest(lua_tostring(L,lua_gettop(L)));

	lua_getglobal(L, "modelFileName");
	pMesh->SetModelFilename(lua_tostring(L,lua_gettop(L)));

	lua_getglobal(L, "textureFileName");
	pMesh->SetTextureFilename(lua_tostring(L,lua_gettop(L)));

	lua_getglobal(L, "typeInGame");
	pMesh->SetActorType(lua_tostring(L,lua_gettop(L)));

	pMesh->SetAttacked(false);
	pMesh->SetAttacking(false);
	pMesh->SetDead(false);
	pMesh->SetPicked(false);
	pMesh->SetHasDialogue(false);
	pMesh->SetAttackerName("");

	pMesh->LoadGameObject();

	pMesh->SetObjectType(EGameObjectType_Skinned);

	m_pGameObjManager->AddGameObject(pMesh);
	
	return 1;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:l_addQuest
Purpose:load quest
*/
int l_addQuest(lua_State* L)
{
	Quest* quest = new Quest;
	lua_getglobal(L, "questTitle");
	quest->m_strTitle = lua_tostring(L,lua_gettop(L));

	lua_getglobal(L, "questContent");
	quest->m_strContent = lua_tostring(L,lua_gettop(L));

	lua_getglobal(L, "requiredObject");
	quest->m_strRequiredObject = lua_tostring(L,lua_gettop(L));

	quest->m_bIsCompleted = false;
	quest->m_bIsStarted = false;

	auto& quests = GetQuestManager()->GetQuests();
	quests.push_back(quest);

	return 1;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:l_setUpMainHero
Purpose:set ups the main hero in the game
*/
int l_setUpMainHero(lua_State* L)
{
	lua_getglobal(L, "mainHero");
	mainHero = lua_tostring(L,lua_gettop(L));

	return 1;
}


/////////////////////////////////////////////////////////////////////////



