#include"LuaFunctions.h"

//map of the available quests, loaded from the scripts
map<string,QuestObject> availableQuests;

//the main hero in the game. The camera is attached to him.
string mainHero;

/////////////////////////////////////////////////////////////////////////
/*
Function:l_addStaticModel
Purpose:load static models, i.e. models whithout animations
*/
int l_addStaticModel(lua_State* L)
{
	GameObject* pMesh = new StaticMesh;

	//GameObject obj;
	pMesh->m_vLook	 = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	pMesh->m_vRight  = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	pMesh->m_vUp	 = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	lua_getglobal(L, "x");
	pMesh->m_vPos.x = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	lua_getglobal(L, "y");
	pMesh->m_vPos.y = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	lua_getglobal(L, "z");
	pMesh->m_vPos.z = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	lua_getglobal(L, "scale");
	pMesh->m_fScale = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	lua_getglobal(L, "rotX");
	pMesh->m_fRotAngleX = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	lua_getglobal(L, "rotY");
	pMesh->m_fRotAngleY = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	lua_getglobal(L, "rotZ");
	pMesh->m_fRotAngleZ = static_cast<float>(lua_tonumber(L,lua_gettop(L)));
	
	lua_getglobal(L, "titleRotY");
	pMesh->m_fTitleRotationAngleByY = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	lua_getglobal(L, "bindable");
	pMesh->m_bIsBindable = static_cast<int>(lua_tonumber(L,lua_gettop(L)));

	lua_getglobal(L,"bindToAnModel");
	pMesh->m_strBindedToAnimatedModelName = lua_tostring(L,lua_gettop(L));

	lua_getglobal(L,"bindToAnModelBone");
	pMesh->m_strBindedToBoneName = lua_tostring(L,lua_gettop(L));

	lua_getglobal(L,"modelName");
	pMesh->m_strModelName = lua_tostring(L,lua_gettop(L));

	lua_getglobal(L, "modelFileName");
	pMesh->m_strModelFileName = lua_tostring(L,lua_gettop(L));

	pMesh->LoadGameObject();

	pMesh->m_eGameObjectType = EGameObjectType_Static;

	m_pGameObjManager->AddGameObject(pMesh);
	
	if (pMesh->m_bIsBindable && !pMesh->m_strBindedToAnimatedModelName.empty() && !pMesh->m_strBindedToBoneName.empty())
	{
		SkinnedMesh* pSkinnedMesh = static_cast<SkinnedMesh*>(m_pGameObjManager->GetGameObjects().find(pMesh->m_strBindedToAnimatedModelName)->second);

		pSkinnedMesh->BindWeaponToModel(pMesh->m_strModelName, pMesh->m_strBindedToBoneName);
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
	GameObject* pMesh = new SkinnedMesh;

	pMesh->m_vLook		= D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	pMesh->m_vRight		= D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	pMesh->m_vUp		= D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	pMesh->m_vTitleLook		= D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	pMesh->m_vTitleRight	= D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	pMesh->m_vTitleUp	 	= D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	pMesh->m_vTitleForQuestLook		= D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	pMesh->m_vTitleForQuestRight	= D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	pMesh->m_vTitleForQuestUp		= D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pMesh->m_fTitleForQuestRotationAngleByY	= 0.0;

	lua_getglobal(L, "x");
	pMesh->m_vPos.x = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	lua_getglobal(L, "y");
	pMesh->m_vPos.y = static_cast<float>(lua_tonumber(L,lua_gettop(L))); 

	lua_getglobal(L, "z");
	pMesh->m_vPos.z = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	lua_getglobal(L, "scale");
	pMesh->m_fScale = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	lua_getglobal(L, "rotX");
	pMesh->m_fRotAngleX = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	lua_getglobal(L, "rotY");
	pMesh->m_fRotAngleY = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	pMesh->m_fRotAngleZ = 0;

	lua_getglobal(L, "titleRotY");
	pMesh->m_fTitleRotationAngleByY = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	lua_getglobal(L,"modelName");
	pMesh->m_strModelName = lua_tostring(L,lua_gettop(L));

	lua_getglobal(L,"titleForQuest");
	pMesh->m_strTitleForQuest = lua_tostring(L,lua_gettop(L));

	lua_getglobal(L, "modelFileName");
	pMesh->m_strModelFileName = lua_tostring(L,lua_gettop(L));

	lua_getglobal(L, "textureFileName");
	pMesh->m_strTextureFileName = lua_tostring(L,lua_gettop(L));

	lua_getglobal(L, "typeInGame");
	pMesh->m_strActorType = lua_tostring(L,lua_gettop(L));

	pMesh->m_bIsAttacked = false;
	pMesh->m_bIsAttacking = false;
	pMesh->m_nCurrentAnimTrack = 0;
	pMesh->m_nNewAnimTrack = 1;
	pMesh->m_bIsSwitched = false;
	pMesh->m_bIsDead = false;
	pMesh->m_bIsPicked = false;
	pMesh->m_bHasDialogue = false;
	pMesh->m_strAttackerName = "";

	pMesh->LoadGameObject();

	pMesh->m_eGameObjectType = EGameObjectType_Skinned;

	m_pGameObjManager->AddGameObject(pMesh);
	
	return 1;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:l_add2DSound
Purpose:load 2d sound
*/
int l_add2DSound(lua_State* L)
{
	SoundObject SoundObj;
	lua_getglobal(L, "soundFileName");
	SoundObj.m_strSoundFileName = lua_tostring(L,lua_gettop(L));

	lua_getglobal(L, "soundVolume");
	SoundObj.m_fVolume = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	SoundObj.m_bIsPlaying = false;
	SoundObj.m_bIs2D = true;
	SoundObj.m_bIs3D = false;
	soundsyst->CreateBackgroundSound(SoundObj);
	soundsyst->AddSoundObject(SoundObj);

	return 1;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:l_add3DSound
Purpose:load 3d sound
*/
int l_add3DSound(lua_State* L)
{
	SoundObject SoundObj;
	lua_getglobal(L, "soundFileName");
	SoundObj.m_strSoundFileName = lua_tostring(L,lua_gettop(L));

	lua_getglobal(L, "soundVolume");
	SoundObj.m_fVolume = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	lua_getglobal(L, "x");
	SoundObj.m_vSoundPosition.x = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	lua_getglobal(L, "y");
	SoundObj.m_vSoundPosition.y = static_cast<float>(lua_tonumber(L,lua_gettop(L))); 

	lua_getglobal(L, "z");
	SoundObj.m_vSoundPosition.z = static_cast<float>(lua_tonumber(L,lua_gettop(L)));

	lua_getglobal(L, "soundMinDist");
	SoundObj.m_fMinDistance = static_cast<float>(lua_tonumber(L,lua_gettop(L)));
	lua_getglobal(L, "soundMaxDist");
	SoundObj.m_fMaxDistance = static_cast<float>(lua_tonumber(L,lua_gettop(L)));
	SoundObj.m_bIsPlaying = false;
	SoundObj.m_bIs2D = false;
	SoundObj.m_bIs3D = true;
	soundsyst->Create3DSound(SoundObj);
	soundsyst->AddSoundObject(SoundObj);
	return 1;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:l_addQuest
Purpose:load quest
*/
int l_addQuest(lua_State* L)
{
	QuestObject obj;
	lua_getglobal(L, "questTitle");
	obj.title = lua_tostring(L,lua_gettop(L));

	lua_getglobal(L, "questContent");
	obj.content = lua_tostring(L,lua_gettop(L));

	lua_getglobal(L, "requiredObject");
	obj.requiredObject = lua_tostring(L,lua_gettop(L));

	obj.completed = false;
	availableQuests[obj.title] = obj;
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



