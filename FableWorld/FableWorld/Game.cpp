#include"Game.h"
#include<math.h>
/*
SourceFile:Game.cpp
Purpose:loads the game models, terrain,skybox, sounds,dialogues for the game, updates and renders them.
*/

ofstream fout; //for logs.
lua_State* L;

/////////////////////////////////////////////////////////////////////////
/*
Function:Game
Purpose:constructor. It loads all the functionality in the game
*/
Game::Game()
{
	//checks if it is supported pixel and vertex shader 2.0
	if(!pEngine->IsShaderVersionSupported())
	{
		MessageBox(0, "Shader version is not supported", 0, 0);
		PostQuitMessage(0); 
	}

	//sprite for the interface in the game
	D3DXCreateSprite(pDxDevice,&m_pInterfaceSprite);
	//textures for the interface
	D3DXCreateTextureFromFile(pDxDevice,"../../Resources/textures/healthbar.dds",&m_pHealthBarTexture);
	D3DXCreateTextureFromFile(pDxDevice,"../../Resources/textures/healthbar_filled.dds",&m_pHealthBarFilledTexture);
	D3DXCreateTextureFromFile(pDxDevice,"../../Resources/textures/healthbar_filled_enemy.dds",&m_phealthBarFilledEnemyTexture);
	m_rHealthBarRectangle.left = 0;  
	m_rHealthBarRectangle.top = 0;  
	m_rHealthBarRectangle.right  = 270;  
	m_rHealthBarRectangle.bottom = 17;

	m_rEnemyHealthBarRectangle.left = 0;  
	m_rEnemyHealthBarRectangle.top = 0;  
	m_rEnemyHealthBarRectangle.right  = 270;  
	m_rEnemyHealthBarRectangle.bottom = 17;

	m_vHealthBarPosition = D3DXVECTOR3(100.0,5.0,0.0);
	m_vEnemyHealthBarPosition = D3DXVECTOR3(370,5.0,0.0);

	//init lua
	L = lua_open();
	//open lua libs
	luaL_openlibs(L);
	
	//with lua_register we bind the functions in the cpp file with the invoked functions in the script file
	//here addStaticModel is function in the script.When it is invoked there it actually invokes l_addStaticModel(lua_State* L)
	lua_register(L,"addStaticModel", l_addStaticModel );
	lua_register(L,"addAnimatedModel",l_addAnimatedModel);
	lua_register(L,"add2DSound",l_add2DSound);
	lua_register(L,"add3DSound",l_add3DSound);
	lua_register(L,"addQuest",l_addQuest);
	lua_register(L,"setUpMainHero",l_setUpMainHero);

	float fWidth  = (float)pEngine->GetPresentParameters().BackBufferWidth;
	float fHeight = (float)pEngine->GetPresentParameters().BackBufferHeight;

	camera = new Camera(D3DX_PI * 0.25f, fWidth/fHeight, 1.0f, 2000.0f,false);
	camera->SetCameraMode(ECameraMode_MoveWithPressedMouse);

	//Initialize the vertex declarations. They are needed for creating the terrain, models and etc.
	InitVertexDeclarations();

	pSky = new Sky("../../Resources/textures/grassenvmap1024.dds", 10000.0f);

	pTerrain = new Terrain("../../Resources/heightmaps/heightmap_full copy.raw",1.0f,513,513,1.0f,1.0f,D3DXVECTOR3(0.0f,0.0f,0.0f));
	//the direction to the sun
	D3DXVECTOR3 lightVector(20.0f, 300.0f, 50.0f);
	D3DXVec3Normalize(&lightVector, &lightVector);
	pTerrain->SetLightVector(lightVector);

	soundsyst = new SoundSyst(100);

	pTextManager->CreateFontFor3DText();

	//loads the models, sounds and quests from the scripts
	//luaL_dofile(L, "scripts/animatedModels.lua");
	//luaL_dofile(L, "scripts/staticModels.lua");
	luaL_dofile(L, "scripts/level.lua");
	luaL_dofile(L, "scripts/sounds.lua");
	luaL_dofile(L, "scripts/quests.lua");
	//luaL_dofile(L, "scripts/init.lua");

	pDialogueManager = new DialogueManager;
	pDialogueManager->LoadDialogues("dialogues/dialogue.xml");

	//creates 3d titles for the models and check for dialogues
	for(auto it=m_pGameObjManager->GetGameObjects().begin();it!=m_pGameObjManager->GetGameObjects().end();it++)
	{
		if( (*it).second->m_eGameObjectType == EGameObjectType_Skinned )
		{
			//create mesh for 3d text above the models
			pTextManager->CreateMeshFor3DText((*it).second);
			
			//check if certain skinned mesh has dialog attached to it
			for(auto it1 = pDialogueManager->m_vGameObjectsWithDialogues.begin();it1!=pDialogueManager->m_vGameObjectsWithDialogues.end();it1++)
			{
				if( (*it).second->m_strModelName == (*it1) )
				{
					(*it).second->m_bHasDialogue = true;
					pTextManager->CreateMeshFor3DTextQuest((*it).second);
				}
			}
			
		}
	}
	
	// hides the enemy health bar before we attack enemy.
	m_bIsEnemyHealthBarVisible = false;

	pMainHero = static_cast<SkinnedMesh*>(m_pGameObjManager->GetGameObjects().find(mainHero)->second);
	
	D3DXVECTOR2 spellPosition(0,600);

	m_pHealSpell = new Button(spellPosition,64,64,"","blueTear.dds","blueTear.dds");

	InitDebugGraphicsShader();

	OnResetDevice();
}

/////////////////////////////////////////////////////////////////////////
/*
Function:InitDebugGraphicsShader
Purpose:initializes the shader for debug graphics
*/

void Game::InitDebugGraphicsShader()
{
	D3DXCreateEffectFromFile(pDxDevice, "../../CORE/CORE/shaders/DebugGraphicsShader.fx", 0, 0, D3DXSHADER_DEBUG, 0, &m_pDebugGraphicsEffect, 0);
	m_hDebugGraphicsTechnique  = m_pDebugGraphicsEffect->GetTechniqueByName("DebugGraphics3DTech");
	m_hDebugGraphicsWVPMatrix  = m_pDebugGraphicsEffect->GetParameterByName(0, "WVP");
}

/////////////////////////////////////////////////////////////////////////
/*
Function:~Game
Purpose:destructor
*/
Game::~Game()
{
	lua_close(L);
	delete pTextManager;
	delete pTerrain;
	delete camera;
	delete pDinput;
	delete pEngine;
	delete pDialogueManager;
	delete soundsyst;
	delete pSky;
}

/////////////////////////////////////////////////////////////////////////
/*
Function:onLostDevice
Purpose:invokes the corresponding OnLostDevice functions
*/
void Game::OnLostDevice()
{
	pSky->OnLostDevice();
	pTextManager->OnLostDevice();
	pTerrain->OnLostDevice();

	for(auto it = m_pGameObjManager->GetGameObjects().begin();it!=m_pGameObjManager->GetGameObjects().end();it++)
	{
		(*it).second->OnLostDevice();
	}

	m_pInterfaceSprite->OnLostDevice();
}


/////////////////////////////////////////////////////////////////////////
/*
Function:onResetDevice
Purpose:invokes the corresponding onResetDevice functions
*/
void Game::OnResetDevice()
{
	pSky->OnResetDevice();
	pTextManager->OnResetDevice();
	pTerrain->OnResetDevice();
	
	for(auto it = m_pGameObjManager->GetGameObjects().begin();it!=m_pGameObjManager->GetGameObjects().end();it++)
	{
		(*it).second->OnResetDevice();
	}
	m_pInterfaceSprite->OnResetDevice();
	
	//after the onResetDevice there might be change in the size of the screen so set
	//the new dimensions to the camera
	//float w = (float)pEngine->GetPresentParameters().BackBufferWidth;
	//float h = (float)pEngine->GetPresentParameters().BackBufferHeight;
	//camera->BuildProjectionMatrix(D3DX_PI * 0.25f, w/h, 1.0f, 2000.0f);
}


/////////////////////////////////////////////////////////////////////////
/*
Function:onUpdate
Purpose:updates all the functionallity in game
*/
void Game::OnUpdate(float dt)
{
	//poll starts to listen if any key on the keyboard is pressed
	pDinput->Poll();
	if( pDinput->IsKeyDown(DIK_F))
	{
		pEngine->SwitchToFullscreen(true);
	}
	//if escape is pressed in game it switches to another scene
	if(pDinput->IsKeyDown(DIK_ESCAPE))
	{
		IBaseScene* pMenuInGameScene = pEngine->GetScene("menuInGame");
		pEngine->SetCurrentScene(pMenuInGameScene);
	}

	//update all the game objects
	for(auto it = m_pGameObjManager->GetGameObjects().begin();it!=m_pGameObjManager->GetGameObjects().end();it++)
	{
		(*it).second->OnUpdate(dt);
	}

	soundsyst->OnUpdate();
	pTextManager->OnUpdate(dt);
	camera->OnUpdate(dt);
	
	soundsyst->PlayAllSounds();

	//updates for the dialogues
	for(map<string,DialogueObject>::iterator it=pDialogueManager->m_mapModelDialogue.begin();it!=pDialogueManager->m_mapModelDialogue.end();it++)
	{
		if( !(*it).second.m_bIsEnded )
		{
			pDialogueManager->UpdateLabelTree((*it).second.m_pTree->GetRoot());
			pDialogueManager->UpdateLabelTreeRoot((*it).second.m_pTree->GetRoot());
			pDialogueManager->StartDialogue((*it).second.m_pTree->GetRoot(),(*it).second);
			pDialogueManager->ChangeDialogue((*it).second.m_pCurrentDialogueNode,(*it).second);
			pDialogueManager->LabelClicked((*it).second,m_mapActiveQuests,availableQuests);
		}
	}
	

	//binding the camera to mainHero in the game and moving it. mainHero is set in the scripts in init.lua
	if( !pMainHero->m_bIsDead )
	{
		MoveObject(mainHero,dt);
	}

	//checking if there is active quest
	if(!m_mapActiveQuests.empty())
	{
		for(map<string,QuestObject>::iterator it=m_mapActiveQuests.begin();it!=m_mapActiveQuests.end();it++)
		{
			//if mainHero is close to the required from the quest object and the required object is dead the quest is completed.
			if(IsObjectNear(pMainHero,m_pGameObjManager->GetGameObjects().find((*it).second.requiredObject)->second) && 
				m_pGameObjManager->GetGameObjects().find((*it).second.requiredObject)->second->m_bIsDead )
			{
				(*it).second.completed = true;
			}
		}
	}
	
	//revealing dialogue if model is picked
	for(auto it=pDialogueManager->m_mapModelDialogue.begin();it!=pDialogueManager->m_mapModelDialogue.end();it++)
	{
		if( m_pGameObjManager->GetGameObjects().find((*it).second.m_strModel)->second->m_bIsPicked && !(*it).second.m_bIsEnded )
		{
			(*it).second.m_pTree->GetRoot()->m_pLabel->SetVisible(true);
			GameObject* pGameObject = m_pGameObjManager->GetGameObjects().find((*it).second.m_strModel)->second;
			pGameObject->m_bIsPicked = false;
		}
	}

	//updating the models's titles positions
	for(auto it=m_pGameObjManager->GetGameObjects().begin();it!=m_pGameObjManager->GetGameObjects().end();it++)
	{
		if( (*it).second->m_eGameObjectType == EGameObjectType_Skinned )
		{
			float angle = D3DXVec3Dot(&(*it).second->m_vTitleRight,&camera->GetLookVector());
			(*it).second->m_fTitleRotationAngleByY += angle;

			D3DXMATRIX R;
			D3DXMatrixRotationY(&R, angle);
			D3DXVec3TransformCoord(&(*it).second->m_vTitleLook, &(*it).second->m_vTitleLook, &R);
			D3DXVec3TransformCoord(&(*it).second->m_vTitleRight, &(*it).second->m_vTitleRight, &R);
			D3DXVec3TransformCoord(&(*it).second->m_vTitleUp, &(*it).second->m_vTitleUp, &R);
		}
	
	}

	//updating the models's title's quest positions. 
	//At the moment these titles are ? signs above the head of the model if he got dialogue attached.
	for(map<string,GameObject*>::iterator it=m_pGameObjManager->GetGameObjects().begin();it!=m_pGameObjManager->GetGameObjects().end();it++)
	{
		if( (*it).second->m_eGameObjectType == EGameObjectType_Skinned )
		{
			float angle = D3DXVec3Dot(&(*it).second->m_vTitleForQuestRight,&camera->GetLookVector());
			(*it).second->m_fTitleForQuestRotationAngleByY += angle;

			D3DXMATRIX R;
			D3DXMatrixRotationY(&R, angle);
			D3DXVec3TransformCoord(&(*it).second->m_vTitleForQuestLook, &(*it).second->m_vTitleForQuestLook, &R);
			D3DXVec3TransformCoord(&(*it).second->m_vTitleForQuestRight, &(*it).second->m_vTitleForQuestRight, &R);
			D3DXVec3TransformCoord(&(*it).second->m_vTitleForQuestUp, &(*it).second->m_vTitleForQuestUp, &R);
		}
	}

	/*for(map<string,GameObject*>::iterator it=m_pGameObjManager->GetGameObjects().begin();it!=m_pGameObjManager->GetGameObjects().end();it++)
	{
		DrawLine(pMainHero->m_vPos,(*it).second->m_vPos);
	}*/

	//if mainHero is attacking
	for(map<string,GameObject*>::iterator it=m_pGameObjManager->GetGameObjects().begin();it!=m_pGameObjManager->GetGameObjects().end();it++)
	{
		if( (*it).second->m_eGameObjectType == EGameObjectType_Skinned )
		{
			if( pDinput->IsMouseButtonDown(0) && 
			   IsObjectNear(pMainHero,(*it).second) && 
			   pMainHero->m_strModelName != (*it).second->m_strModelName &&
			   (*it).second->m_strActorType == "enemy"
			   )
			{           
				m_bIsEnemyHealthBarVisible = true;
				(*it).second->m_bIsAttacked = true;
				(*it).second->m_strAttackerName = mainHero;
				pMainHero->PlayAnimationOnce("attack_1");
				if( (pMainHero->m_bIsAttacking) )
				{
					m_rEnemyHealthBarRectangle.right-=70;
				}
			}
		
			// enemy AI
			if( (*it).second->m_bIsAttacked && IsObjectNear(pMainHero,(*it).second))
			{
				if( m_rHealthBarRectangle.right > 0.0 )
				{
					pMainHero->m_strAttackerName = (*it).second->m_strModelName;
					SkinnedMesh* pSkinnedMesh = static_cast<SkinnedMesh*>((*it).second);

					pSkinnedMesh->PlayAnimationOnce("attack_1");
				
					if( ((*it).second->m_bIsAttacking) )
					{
						m_rHealthBarRectangle.right-=70;
					}
				}
				else
				{
					(*it).second->m_bIsAttacked = false;
				}
			}

			//when the enemy is attacked it updates his rotation so it can face the mainHero
			if( (*it).second->m_bIsAttacked && 
				!(*it).second->m_bIsDead &&
			   IsObjectNear(pMainHero,(*it).second)
			  )
			{
				D3DXVECTOR3 vActorPosition = (*it).second->m_vPos;
				D3DXVECTOR3 vMainHeroPosition = pMainHero->m_vPos;

				D3DXVECTOR3 vDistanceVector = vActorPosition - vMainHeroPosition;
				D3DXVec3Normalize(&vDistanceVector,&vDistanceVector);

				//for some reason the look vector is the right vector must be fixed
				float angle = D3DXVec3Dot(&(*it).second->m_vRight,&vDistanceVector);
				(*it).second->m_fRotAngleY += angle;

				D3DXMATRIX R;
				D3DXMatrixRotationY(&R, angle);
				D3DXVec3TransformCoord(&(*it).second->m_vLook, &(*it).second->m_vLook, &R);
				D3DXVec3TransformCoord(&(*it).second->m_vRight, &(*it).second->m_vRight, &R);
				D3DXVec3TransformCoord(&(*it).second->m_vUp, &(*it).second->m_vUp, &R);
			}

			//if mainHero is fighting with enemy, but started to run and is no longer close to the enemy, 
			//the enemy updates his vectors so he can face the mainHero and run in his direction.
			//When he is close enough he start to attack again.
			if( (*it).second->m_bIsAttacked && 
				!(*it).second->m_bIsDead &&
			   !IsObjectNear(pMainHero,(*it).second)
			  )
			{
				SkinnedMesh* pSkinnedMesh = static_cast<SkinnedMesh*>((*it).second);
				pSkinnedMesh->PlayAnimation("run");
			
				D3DXVECTOR3 dir(0.0f, 0.0f, 0.0f);

				dir -= (*it).second->m_vLook;

				D3DXVECTOR3 newPos = (*it).second->m_vPos+ dir*40.0*dt;
				(*it).second->m_vPos = newPos;

				//put it in a function. the code is duplicated.
				D3DXVECTOR3 vActorPosition = (*it).second->m_vPos;
				D3DXVECTOR3 vMainHeroPosition = pMainHero->m_vPos;

				D3DXVECTOR3 vDistanceVector = vActorPosition - vMainHeroPosition;
				D3DXVec3Normalize(&vDistanceVector,&vDistanceVector);

				float angle = D3DXVec3Dot(&(*it).second->m_vRight,&vDistanceVector);
				(*it).second->m_fRotAngleY += angle;

				D3DXMATRIX R;
				D3DXMatrixRotationY(&R, angle);
				D3DXVec3TransformCoord(&(*it).second->m_vLook, &(*it).second->m_vLook, &R);
				D3DXVec3TransformCoord(&(*it).second->m_vRight, &(*it).second->m_vRight, &R);
				D3DXVec3TransformCoord(&(*it).second->m_vUp, &(*it).second->m_vUp, &R);
			}
		}
	}

	if( pDinput->IsMouseButtonUp(0) && m_pHealSpell->IsClicked() )
	{
		m_rHealthBarRectangle.right += 20;
	}

	m_pHealSpell->OnUpdate();

	//controls the health bars of the enemy and the mainHero.
	ManageHealthBars();
}
	

/////////////////////////////////////////////////////////////////////////
/*
Function:MoveObject
Purpose:makes the camera to follow model in game and moving it with WASD from keyboard and mouse
*/
void Game::MoveObject(string objectTitle, float dt)
{
	//this vector holds the new direction to move
	D3DXVECTOR3 dir(0.0f, 0.0f, 0.0f);

	SkinnedMesh* pSkinnedMesh = static_cast<SkinnedMesh*>(m_pGameObjManager->GetGameObjects().find(objectTitle)->second);
	
	if( pDinput->IsKeyDown(DIK_W) )
	{
		pSkinnedMesh->PlayAnimation("run");
		dir += pSkinnedMesh->m_vLook;
	}
	if( pDinput->IsKeyDown(DIK_S) )
	{
		pSkinnedMesh->PlayAnimation("run");
		dir -= pSkinnedMesh->m_vLook;
	}
	if( pDinput->IsKeyDown(DIK_A) )
	{
		pSkinnedMesh->PlayAnimation("run");
		dir -= pSkinnedMesh->m_vRight;
	}
	if( pDinput->IsKeyDown(DIK_D) )
	{
		pSkinnedMesh->PlayAnimation("run");
		dir += pSkinnedMesh->m_vRight;
	}

	if( !pDinput->IsKeyDown(DIK_W) && !pDinput->IsKeyDown(DIK_S) && !pDinput->IsKeyDown(DIK_A) && !pDinput->IsKeyDown(DIK_D) )
	{
		pSkinnedMesh->PlayAnimation("idle");
	}
	
	D3DXVECTOR3 newPos = pSkinnedMesh->m_vPos+ dir*40.0*dt;
	pSkinnedMesh->m_vPos = newPos;
	
	if( camera->GetCameraMode() == ECameraMode_MoveWithoutPressedMouse )
	{
			//if we just move the mouse move the camera
			RotateObject(objectTitle,dt);

	}
	else if( camera->GetCameraMode() == ECameraMode_MoveWithPressedMouse )
	{
		//if we hold the left mouse button move the camera
		if(pDinput->IsMouseButtonDown(0))
		{
			RotateObject(objectTitle,dt);
		}
	}

	//updates the camera position based on the new position of the model.
	//via cameraOffset we can control how far the camera is from the model and at what position.
	camera->GetPosition() = pSkinnedMesh->m_vPos + camera->GetOffset();
}

/////////////////////////////////////////////////////////////////////////
/*
Function:objectRotation
Purpose:rotates to model and the camera if the mouse is moved
*/
void Game::RotateObject(string objectTitle, float dt)
{
	SkinnedMesh* pSkinnedMesh = static_cast<SkinnedMesh*>(m_pGameObjManager->GetGameObjects().find(objectTitle)->second);
	
	float yAngle = pDinput->GetMouseDX() / (19000*dt);
	pSkinnedMesh->m_fRotAngleY +=yAngle;
	
	D3DXMATRIX R;
	D3DXMatrixRotationY(&R, yAngle);
	D3DXVec3TransformCoord(&camera->GetRightVector(), &camera->GetRightVector(), &R);
	D3DXVec3TransformCoord(&camera->GetUpVector(), &camera->GetUpVector(), &R);
	D3DXVec3TransformCoord(&camera->GetLookVector(), &camera->GetLookVector(), &R);

	D3DXVec3TransformCoord(&camera->GetOffset(), &camera->GetOffset(), &R);

	pSkinnedMesh->m_vLook  = camera->GetLookVector();
	pSkinnedMesh->m_vRight = camera->GetRightVector();
	pSkinnedMesh->m_vUp    = camera->GetUpVector();
}


/////////////////////////////////////////////////////////////////////////
/*
Function:healthControl
Purpose:controls the health of the hero and enemy and make them play dead animation if they are at zero health
*/
void Game::ManageHealthBars()
{
	if( m_rHealthBarRectangle.right <= 0.0 )
	{
		pMainHero->PlayAnimationOnceAndStop("dead");
		pMainHero->m_bIsDead = true;
		
		//set the animation of the attacked to idle, because sometimes got bugged and plays run animaiton
		string strEnemy = pMainHero->m_strAttackerName;
		SkinnedMesh* pEnemy = static_cast<SkinnedMesh*>(m_pGameObjManager->GetGameObjects().find(strEnemy)->second);
		pEnemy->PlayAnimation("idle");
	}
	if( m_rEnemyHealthBarRectangle.right <= 0.0 )
	{
		string strEnemy = pMainHero->m_strAttackerName;
		SkinnedMesh* pEnemy = static_cast<SkinnedMesh*>(m_pGameObjManager->GetGameObjects().find(strEnemy)->second);
		pEnemy->PlayAnimationOnceAndStop("dead");
		pEnemy->m_bIsDead = true;
	}
}


/////////////////////////////////////////////////////////////////////////
/*
Function:OnRender
Purpose:render all
*/
void Game::OnRender()
{

    pDxDevice->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);

    pDxDevice->BeginScene();
	
			pSky->OnRender();
			pTerrain->OnRender();

			for(map<string,GameObject*>::iterator it = m_pGameObjManager->GetGameObjects().begin();it!=m_pGameObjManager->GetGameObjects().end();it++)
			{
				(*it).second->OnRender();
			}
			
			pTextManager->DrawFPS();
	
			//draws dialogues
			for(map<string,DialogueObject>::iterator it=pDialogueManager->m_mapModelDialogue.begin();it!=pDialogueManager->m_mapModelDialogue.end();it++)
			{
				pDialogueManager->RenderLabelTreeRoot((*it).second.m_pTree->GetRoot());
				pDialogueManager->RenderLabelTree((*it).second.m_pTree->GetRoot());
			}

			//text->drawText("Press L to switch between the two camera modes",400,40,0,0,255,0,0,0);

			//draws quest stuff
			for(map<string,QuestObject>::iterator it=m_mapActiveQuests.begin();it!=m_mapActiveQuests.end();it++)
			{
				if((*it).second.completed)
				{
					pTextManager->RenderText((*it).second.title.c_str(),pEngine->GetPresentParameters().BackBufferWidth-420,70,0,0,255,255,255,0);
					pTextManager->RenderText("Completed",pEngine->GetPresentParameters().BackBufferWidth-590,70,0,0,255,255,255,0);
				}
				else if( !pMainHero->m_bIsDead )
				{
					pTextManager->RenderText((*it).second.title.c_str(),pEngine->GetPresentParameters().BackBufferWidth-420,70,0,0,255,255,255,0);
				}
				else if( !(*it).second.completed && pMainHero->m_bIsDead )
				{
					pTextManager->RenderText((*it).second.title.c_str(),pEngine->GetPresentParameters().BackBufferWidth-420,70,0,0,255,255,255,0);
					pTextManager->RenderText("Epic fail!",pEngine->GetPresentParameters().BackBufferWidth-570,70,0,0,255,255,255,0);
				}
			}

			

			//draws the healthbars
			m_pInterfaceSprite->Begin(D3DXSPRITE_ALPHABLEND);

				m_pInterfaceSprite->Draw(m_pHealthBarTexture,NULL,NULL,&m_vHealthBarPosition,D3DXCOLOR(255,255,255,255));
				m_pInterfaceSprite->Draw(m_pHealthBarFilledTexture,&m_rHealthBarRectangle,NULL,&m_vHealthBarPosition,D3DXCOLOR(255,255,255,255));

				if( m_bIsEnemyHealthBarVisible )
				{
					m_pInterfaceSprite->Draw(m_pHealthBarTexture,NULL,NULL,&m_vEnemyHealthBarPosition,D3DXCOLOR(255,255,255,255));
					m_pInterfaceSprite->Draw(m_phealthBarFilledEnemyTexture,&m_rEnemyHealthBarRectangle,NULL,&m_vEnemyHealthBarPosition,D3DXCOLOR(255,255,255,255));
				}

			m_pInterfaceSprite->End();
			
			m_pHealSpell->OnRender();

	pDxDevice->EndScene();

	pDxDevice->Present(0, 0, 0, 0);
}
	
/////////////////////////////////////////////////////////////////////////
/*
Function:DrawLine
Purpose:draws a line
*/
void Game::DrawLine(const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd)
{
	pDxDevice->BeginScene();

	m_pDebugGraphicsEffect->SetTechnique(m_hDebugGraphicsTechnique);

	m_pDebugGraphicsEffect->SetMatrix(m_hDebugGraphicsWVPMatrix, &(camera->GetViewProjMatrix()));
	UINT numPasses = 0;
	m_pDebugGraphicsEffect->Begin(&numPasses, 0);
	m_pDebugGraphicsEffect->BeginPass(0);

		IDirect3DVertexBuffer9* pVertexBuffer;

		pDxDevice->CreateVertexBuffer(2 * sizeof (VertexPositionColor), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &pVertexBuffer, 0);

		VertexPositionColor* v = 0;
		pVertexBuffer->Lock(0, 0,  (void**)&v, 0);

		v[0].m_vPos = vStart;
		//v[0].m_color = D3DXCOLOR(1.0f,1.0,1.0f,1.0f);

		v[1].m_vPos = vEnd;
		//v[1].m_color = D3DXCOLOR(1.0f,1.0,1.0f,1.0f);

		pVertexBuffer->Unlock();

		pDxDevice->SetStreamSource(0, pVertexBuffer, 0, sizeof(VertexPositionColor));

		pDxDevice->SetVertexDeclaration(pEngine->GetPosColDeclaration());

		pDxDevice->DrawPrimitive(D3DPT_LINELIST,0,1);

	m_pDebugGraphicsEffect->EndPass();
	m_pDebugGraphicsEffect->End();


	pDxDevice->EndScene();

	pDxDevice->Present(0, 0, 0, 0);
}


/////////////////////////////////////////////////////////////////////////
/*
Function:IsObjectNear
Purpose:checks if two models are close
*/
bool Game::IsObjectNear(GameObject* obj1,GameObject* obj2)
{
	if((obj1->m_vPos.x > obj2->m_vPos.x-30) && (obj1->m_vPos.x < obj2->m_vPos.x+30) &&
	   (obj1->m_vPos.z > obj2->m_vPos.z-30) && (obj1->m_vPos.z < obj2->m_vPos.z+30))
	{
	   return true;
	}
	else 
	{
		return false;
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:MsgProc
Purpose:this function detects various messages sent to the window like WM_CLOSE, WM_ACTIVATE and etc.
		Used mainly for the textbox and camera mode switching
*/
LRESULT Game::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
		//Sent when the window is activated or deactivated.(pressed alt+tab)
		//Game is paused when the window is inactive, and unpaused when become active again
		case WM_ACTIVATE:
		{
			if( LOWORD(wParam) == WA_INACTIVE )
			{
				pEngine->SetPaused(true);
			}
			else
			{
				pEngine->SetPaused(false);
			}

			return 0;
		}


		//Sent when the user closes the window
		case WM_CLOSE:
		{
			DestroyWindow( pEngine->GetMainWindow() );
		
			return 0;
		}

		//Sent when the window is destroyed
		case WM_DESTROY:
		{
			PostQuitMessage(0);

			return 0;
		}

		case WM_KEYDOWN:
		{
			switch(wParam)
			{
				case 'L':
				{
					if( camera->GetCameraMode() == ECameraMode_MoveWithoutPressedMouse )
					{
						camera->SetCameraMode(ECameraMode_MoveWithPressedMouse);
					}
					else if( camera->GetCameraMode() == ECameraMode_MoveWithPressedMouse )
					{
						camera->SetCameraMode(ECameraMode_MoveWithoutPressedMouse);
					}
				}
			}

			return 0;
		}

  }
	return DefWindowProc(pEngine->GetMainWindow(), msg, wParam, lParam);
}


/////////////////////////////////////////////////////////////////////////
