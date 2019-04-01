#include <stdafx.h>
#include "Game.h"
#include "QuestManager.h"

/////////////////////////////////////////////////////////////////////////

Game::Game()
{
	//checks if it is supported pixel and vertex shader 2.0
	if(!pApp->IsShaderVersionSupported())
	{
		MessageBox(0, "Shader version is not supported", 0, 0);
		PostQuitMessage(0); 
	}

	//sprite for the interface in the game
	D3DXCreateSprite(pDxDevice,&m_pInterfaceSprite);
	//textures for the interface
	CheckSuccess(D3DXCreateTextureFromFile(pDxDevice,"../../Resources/textures/GUI/healthbar.dds",&m_pHealthBarTexture));
	CheckSuccess(D3DXCreateTextureFromFile(pDxDevice,"../../Resources/textures/GUI/healthbar_filled.dds",&m_pHealthBarFilledTexture));
	CheckSuccess(D3DXCreateTextureFromFile(pDxDevice,"../../Resources/textures/GUI/healthbar_filled_enemy.dds",&m_phealthBarFilledEnemyTexture));
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
	g_luaState = lua_open();
	//open lua libs
	luaL_openlibs(g_luaState);
	
	//with lua_register we bind the functions in the cpp file with the invoked functions in the script file
	//here addStaticModel is function in the script.When it is invoked there it actually invokes l_addStaticModel(lua_State* L)
	lua_register(g_luaState,"addStaticModel", l_addStaticModel );
	lua_register(g_luaState,"addAnimatedModel",l_addAnimatedModel);
	lua_register(g_luaState,"addQuest",l_addQuest);
	lua_register(g_luaState,"setUpMainHero",l_setUpMainHero);

	float fWidth  = (float)pApp->GetPresentParameters().BackBufferWidth;
	float fHeight = (float)pApp->GetPresentParameters().BackBufferHeight;

	camera = new Camera(D3DX_PI * 0.25f, fWidth/fHeight, 1.0f, 2000.0f,true);
	camera->SetCameraMode(ECameraMode::MoveWithPressedMouse);
	camera->SetPosition(D3DXVECTOR3(0,200,100));
	//camera->RotateUp(-300);

	//Initialize the vertex declarations. They are needed for creating the terrain, models and etc.
	InitVertexDeclarations();

	pSky = new Sky("../../Resources/textures/Sky/grassenvmap1024.dds", 10000.0f);

	//TODO: this should be specified in the level file
	pTerrain = new Terrain("../../Resources/heightmaps/HeightmapFinal.raw",1.0f,513,513,10.0f,10.0f,D3DXVECTOR3(0.0f,0.0f,0.0f));
	//pTerrain = new Terrain("../../Resources/heightmaps/coastMountain1025.raw", 1.0f, 1025, 1025, 10.0f, 10.0f, D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//the direction to the sun
	D3DXVECTOR3 lightVector(20.0f, 300.0f, 50.0f);
	D3DXVec3Normalize(&lightVector, &lightVector);
	pTerrain->SetLightVector(lightVector);

	pTextManager->CreateFontFor3DText();

	//loads the models, sounds and quests from the scripts
	luaL_dofile(g_luaState, "../../Resources/scripts/levelInGame.lua");
	luaL_dofile(g_luaState, "../../Resources/scripts/quests.lua");

	pDialogueManager = new DialogueManager;
	//TODO: probably specify this in the level file?
	pDialogueManager->LoadDialogues("../../Resources/dialogues/dialogue.xml");

	//creates 3d titles for the models and check for dialogues
	auto& gameObjects = m_pGameObjManager->GetSkinnedModels();
	for(auto& gameObject : gameObjects)
	{
		//create mesh for 3d text above the models
		pTextManager->CreateMeshFor3DText(gameObject);
	}
	
	for(auto& gameObject : gameObjects )
	{
		for( auto& dialogue : pDialogueManager->GetDialogues() )
		{
			if( !gameObject->GetName().compare(dialogue->m_strModel) )
			{
				gameObject->SetHasDialogue(true);
				pTextManager->CreateMeshFor3DTextQuest(gameObject);
			}
		}
	}
	// hides the enemy health bar before we attack enemy.
	m_bIsEnemyHealthBarVisible = false;

	GameObject* obj = m_pGameObjManager->GetObjectByName(mainHero);

	if(obj != nullptr )
	{
		pMainHero = static_cast<SkinnedModel*>(obj);
	}
	
	auto height = pApp->GetPresentParameters().BackBufferHeight;
	D3DXVECTOR2 spellPosition(100, height-80);

	m_pHealSpell = new Button(spellPosition,64,64,"","heal1.dds","heal1.dds");

	InitDebugGraphicsShader();

	//TODO: this does not work for all cases...
	//it should be more generic - it should align with the vectors of the main hero.
	D3DXMATRIX R;
	D3DXMatrixRotationY(&R, D3DX_PI/2); //rotate this bad boy
	camera->TransformByMatrix(R);

	m_pGunEffect = std::unique_ptr<GunEffect>(new GunEffect("../../Resources/shaders/Effects/GunShader.fx","GunEffectTech","../../Resources/textures/Effects/bolt.dds",100, D3DXVECTOR4(0, -9.8f, 0.0f,0.0f)));
	
	m_isAIRunningToTarget = false;
	m_AIIntersectPoint = D3DXVECTOR3(0, 0, 0);

	GameObject* objCho = m_pGameObjManager->GetObjectByName("cho");
	m_pGameObjManager->SpawnAnimatedClone(objCho); //~7.5MB per one cho. Did not expect that wow
	m_pGameObjManager->SpawnAnimatedClone(objCho);
	m_pGameObjManager->SpawnAnimatedClone(objCho);
	m_pGameObjManager->SpawnAnimatedClone(objCho);
	m_pGameObjManager->SpawnAnimatedClone(objCho);
	m_pGameObjManager->SpawnAnimatedClone(objCho);
}

/////////////////////////////////////////////////////////////////////////

//initializes the shader for debug graphics
void Game::InitDebugGraphicsShader()
{
	D3DXCreateEffectFromFile(pDxDevice, "../../Resources/shaders/DebugGraphicsShader.fx", 0, 0, D3DXSHADER_DEBUG, 0, &m_pDebugGraphicsEffect, 0);
	m_hDebugGraphicsTechnique  = m_pDebugGraphicsEffect->GetTechniqueByName("DebugGraphics3DTech");
	m_hDebugGraphicsWVPMatrix  = m_pDebugGraphicsEffect->GetParameterByName(0, "WVP");
}

/////////////////////////////////////////////////////////////////////////

Game::~Game()
{
	lua_close(g_luaState);
	delete pTextManager;
	delete pTerrain;
	delete camera;
	delete pDinput;
	delete pApp;
	delete pDialogueManager;
	delete pSky;
}

/////////////////////////////////////////////////////////////////////////

void Game::OnLostDevice()
{
	pSky->OnLostDevice();
	pTextManager->OnLostDevice();
	pTerrain->OnLostDevice();

	for(auto& gameObject : m_pGameObjManager->GetGameObjects())
	{
		gameObject->OnLostDevice();
	}

	m_pInterfaceSprite->OnLostDevice();
	m_pHealSpell->OnLostDevice();
}


/////////////////////////////////////////////////////////////////////////

void Game::OnResetDevice()
{
	pSky->OnResetDevice();
	pTextManager->OnResetDevice();
	pTerrain->OnResetDevice();
	
	for (auto& gameObject : m_pGameObjManager->GetGameObjects())
	{
		gameObject->OnResetDevice();
	}
	m_pInterfaceSprite->OnResetDevice();

	m_pHealSpell->OnResetDevice();
	
	//after the onResetDevice there might be change in the size of the screen so set
	//the new dimensions to the camera
	//float w = (float)pApp->GetPresentParameters().BackBufferWidth;
	//float h = (float)pApp->GetPresentParameters().BackBufferHeight;
	//camera->BuildProjectionMatrix(D3DX_PI * 0.25f, w/h, 1.0f, 2000.0f);
}


/////////////////////////////////////////////////////////////////////////

void Game::OnUpdate(float dt)
{
	//poll starts to listen if any key on the keyboard is pressed
	pDinput->Poll();
	if( pDinput->IsKeyDown(DIK_F))
	{
		pApp->SwitchToFullscreen(true);
	}
	//if escape is pressed in game it switches to another scene
	if(pDinput->IsKeyDown(DIK_ESCAPE))
	{
		IBaseScene* pMenuInGameScene = pApp->GetScene("menuInGame");
		pApp->SetCurrentScene(pMenuInGameScene);
	}

	//update all the game objects
	for (auto& gameObject : m_pGameObjManager->GetGameObjects())
	{
		gameObject->OnUpdate(dt);
	}

	pTextManager->OnUpdate(dt);
	camera->OnUpdate(dt);
	

	pDialogueManager->OnUpdate();

	//checking if there is active quest
	for (auto& quest : GetQuestManager()->GetQuests())
	{
		SkinnedModel* reqObject = m_pGameObjManager->GetSkinnedModelByName(quest->m_strRequiredObject);

		//if mainHero is close to the required from the quest object and the required object is dead the quest is completed.
		if (IsObjectNear(pMainHero->GetPosition(), reqObject->GetPosition()) && reqObject->IsDead())
		{
			quest->m_bIsCompleted = true;
		}
	}

	//binding the camera to mainHero in the game and moving it. mainHero is set in the scripts in init.lua
	if( !pMainHero->IsDead() && !camera->IsCameraFree() )
	{
		MoveObject(mainHero,dt);
	}

	//updating the models's titles positions
	for(auto& gameObject : m_pGameObjManager->GetSkinnedModels())
	{
		float angle = D3DXVec3Dot(&gameObject->GetTitleRightVector(),&camera->GetLookVector());
		gameObject->ModifyTitleRotationAnglyByY(angle);

		D3DXMATRIX R;
		D3DXMatrixRotationY(&R, angle);
		D3DXVec3TransformCoord(&gameObject->GetTitleLookVector(), &gameObject->GetTitleLookVector(), &R);
		D3DXVec3TransformCoord(&gameObject->GetTitleRightVector(), &gameObject->GetTitleRightVector(), &R);
		D3DXVec3TransformCoord(&gameObject->GetTitleUpVector(), &gameObject->GetTitleUpVector(), &R);
	}

	//updating the models's title's quest positions. 
	//At the moment these titles are ? signs above the head of the model if he got dialogue attached.
	for (auto& gameObject : m_pGameObjManager->GetSkinnedModels())
	{
		float angle = D3DXVec3Dot(&gameObject->GetTitleForQuestRightVector(),&camera->GetLookVector());
		gameObject->ModifyTitleForQuestRotationAnglyByY(angle);

		D3DXMATRIX R;
		D3DXMatrixRotationY(&R, angle);
		D3DXVec3TransformCoord(&gameObject->GetTitleForQuestLookVector(), &gameObject->GetTitleForQuestLookVector(), &R);
		D3DXVec3TransformCoord(&gameObject->GetTitleForQuestRightVector(), &gameObject->GetTitleForQuestRightVector(), &R);
		D3DXVec3TransformCoord(&gameObject->GetTitleForQuestUpVector(), &gameObject->GetTitleForQuestUpVector(), &R);
	}

	//for (auto it : m_pGameObjManager->GetGameObjects())
	//{
	//	DrawLine(pMainHero->GetPosition(), it->GetPosition());
	//}

	ManageHealthBars();
	UpdateAI(dt);

	if( pDinput->IsMouseButtonUp(0) && m_pHealSpell->IsClicked() )
	{
		m_rHealthBarRectangle.right += 20;
	}

	m_pHealSpell->OnUpdate();

	m_pGunEffect->OnUpdate(dt);

	static float delay = 0.0f;
	if (pDinput->IsKeyDown(DIK_SPACE) && delay <= 0.0f)
	{
		auto pickedObj = m_pGameObjManager->GetPickedObject();
		if (pickedObj)
		{
			delay = 0.1f;
			m_pGunEffect->AddParticle(pickedObj);
		}
	}

	auto pickedObj = m_pGameObjManager->GetPickedObject();

	if (pickedObj)
	{
		if (pDinput->IsMouseButtonDown(1))
		{
			D3DXVECTOR3 vOrigin(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 vDir(0.0f, 0.0f, 0.0f);

			GetWorldPickingRay(vOrigin, vDir);

			D3DXPLANE plane;
			D3DXPlaneFromPointNormal(&plane, &D3DXVECTOR3(0,0,0),&D3DXVECTOR3(0,1,0));

			//I though that only origin and direction is needed and since
			//line is thought to be endless I am not going to need end point.
			//looks like we need it. I should really check the API next time, before assuming things.
			D3DXVECTOR3 lineEndPoint = vOrigin + INT_MAX * vDir;
			D3DXPlaneIntersectLine(&m_AIIntersectPoint, &plane, &vOrigin, &lineEndPoint);

			m_isAIRunningToTarget = true;
		}
	}

	if (m_isAIRunningToTarget && pickedObj)
	{
		if (!IsObjectNear(pickedObj->GetPosition(), m_AIIntersectPoint,30))
		{
			RunToTarget(pickedObj, m_AIIntersectPoint, dt);
		}
		else
		{
			SkinnedModel* pSkinnedModel = static_cast<SkinnedModel*>(pickedObj);
			pSkinnedModel->PlayAnimation("idle");
			m_isAIRunningToTarget = false;
			m_AIIntersectPoint = D3DXVECTOR3(0, 0, 0);
		}
	}

	delay -= dt;
}

/////////////////////////////////////////////////////////////////////////

void Game::UpdateAI(float dt)
{
	for (auto& gameObject : m_pGameObjManager->GetSkinnedModels())
	{
		//main hero attacking enemy
		if (//pDinput->IsMouseButtonDown(0) &&
			IsObjectNear(pMainHero->GetPosition(), gameObject->GetPosition()) &&
			pMainHero->GetName() != gameObject->GetName() &&
			gameObject->GetActorType() == "enemy" &&
			!pMainHero->IsDead()
			)
		{
			m_bIsEnemyHealthBarVisible = true;
			gameObject->SetAttacked(true);
			gameObject->SetAttackerName(mainHero);

			int num = rand() % 2;
			LPCSTR animName = num % 2 == 0 ? "attack_1" : "attack_2";
			pMainHero->PlayAnimationOnce(animName);
			if (pMainHero->JustStartedPlayingAnimationOnce())
			{
				m_rEnemyHealthBarRectangle.right -= 70;
			}
		}

		// main hero attacked by enemy
		if (gameObject->IsAttacked() && 
			IsObjectNear(pMainHero->GetPosition(), gameObject->GetPosition()) &&
			!gameObject->IsDead() && 
			!pMainHero->IsDead())
		{
			if (m_rHealthBarRectangle.right > 0.0)
			{
				pMainHero->SetAttackerName(gameObject->GetName());
				SkinnedModel* pSkinnedModel = static_cast<SkinnedModel*>(gameObject);

				int num = rand() % 2;
				LPCSTR animName = num % 2 == 0 ? "attack_1" : "attack_2";
				pSkinnedModel->PlayAnimationOnce(animName);

				if (gameObject->JustStartedPlayingAnimationOnce())
				{
					m_rHealthBarRectangle.right -= 70;
				}
			}
			else
			{
				gameObject->SetAttacked(false);
			}
		}

		//when the enemy is attacked it updates his rotation so it can face the mainHero
		if (gameObject->IsAttacked() && 
			!gameObject->IsDead() && 
			IsObjectNear(pMainHero->GetPosition(), gameObject->GetPosition()))
		{
			D3DXVECTOR3 vActorPosition = gameObject->GetPosition();
			D3DXVECTOR3 vMainHeroPosition = pMainHero->GetPosition();

			D3DXVECTOR3 vDistanceVector = vActorPosition - vMainHeroPosition;
			D3DXVec3Normalize(&vDistanceVector, &vDistanceVector);

			//for some reason the look std::vector is the right std::vector must be fixed
			float angle = D3DXVec3Dot(&gameObject->GetRightVector(), &vDistanceVector);
			gameObject->ModifyRotationAngleByY(angle);

			D3DXMATRIX R;
			D3DXMatrixRotationY(&R, angle);
			gameObject->TransformByMatrix(R);
		}


		//if mainHero is fighting with enemy, but started to run and is no longer close to the enemy, 
		//the enemy updates his vectors so he can face the mainHero and run in his direction.
		//When he is close enough he start to attack again.
		if (gameObject->IsAttacked() && 
			!gameObject->IsDead() && 
			!IsObjectNear(pMainHero->GetPosition(), gameObject->GetPosition()) && 
			!pMainHero->IsDead()
			)
		{
			RunToTarget(gameObject, pMainHero->GetPosition(), dt);
		}
	}
}

/////////////////////////////////////////////////////////////////////////

void Game::RunToTarget(GameObject* runner, D3DXVECTOR3 targetPos, float dt)
{
	float speed = 200.f;
	SkinnedModel* pSkinnedModel = static_cast<SkinnedModel*>(runner);
	pSkinnedModel->SetMovementSpeed(speed / 100);
	pSkinnedModel->PlayAnimation("run");

	D3DXVECTOR3 dir(0.0f, 0.0f, 0.0f);

	dir -= runner->GetLookVector();

	D3DXVECTOR3 newPos = runner->GetPosition() + dir * speed*dt;
	runner->SetPosition(newPos);

	D3DXVECTOR3 vActorPosition = runner->GetPosition();

	D3DXVECTOR3 vDistanceVector = vActorPosition - targetPos;
	D3DXVec3Normalize(&vDistanceVector, &vDistanceVector);

	float angle = D3DXVec3Dot(&runner->GetRightVector(), &vDistanceVector);
	runner->ModifyRotationAngleByY(angle);

	D3DXMATRIX R;
	D3DXMatrixRotationY(&R, angle);
	runner->TransformByMatrix(R);
}

/////////////////////////////////////////////////////////////////////////

//makes the camera to follow model in game and moving it with WASD from keyboard and mouse
void Game::MoveObject(std::string objectTitle, float dt)
{
	//this std::vector holds the new direction to move
	D3DXVECTOR3 dir(0.0f, 0.0f, 0.0f);

	GameObject* obj = m_pGameObjManager->GetObjectByName(objectTitle);
	SkinnedModel* pSkinnedModel = nullptr;

	if (obj != nullptr)
	{
		pSkinnedModel = static_cast<SkinnedModel*>(obj);
	}
	
	if( pDinput->IsKeyDown(DIK_W) )
	{
		pSkinnedModel->PlayAnimation("run");
		dir += camera->GetLookVector();
	}
	if( pDinput->IsKeyDown(DIK_S) )
	{
		pSkinnedModel->PlayAnimation("run");
		dir -= camera->GetLookVector();
	}
	if( pDinput->IsKeyDown(DIK_A) )
	{
		pSkinnedModel->PlayAnimation("run");
		dir -= camera->GetRightVector();
	}
	if( pDinput->IsKeyDown(DIK_D) )
	{
		pSkinnedModel->PlayAnimation("run");
		dir += camera->GetRightVector();
	}

	if( !pDinput->IsKeyDown(DIK_W) && !pDinput->IsKeyDown(DIK_S) && !pDinput->IsKeyDown(DIK_A) && !pDinput->IsKeyDown(DIK_D) )
	{
		pSkinnedModel->PlayAnimation("idle");
	}
	
	if (camera->GetCameraMode() == ECameraMode::MoveWithoutPressedMouse)
	{
		//if we just move the mouse move the camera
		RotateObject(objectTitle, dt);

	}
	else if (camera->GetCameraMode() == ECameraMode::MoveWithPressedMouse)
	{
		//if we hold the left mouse button move the camera
		if (pDinput->IsMouseButtonDown(0))
		{
			RotateObject(objectTitle, dt);
		}
	}

	D3DXVECTOR3 newPos = pSkinnedModel->GetPosition() + dir*150.0*pSkinnedModel->GetMovementSpeed()*dt;
	if( pTerrain->IsValidPosition(newPos.x,newPos.z))
	{
		pSkinnedModel->SetPosition(newPos);
	}

	//updates the camera position based on the new position of the model and the zoom
	//we zoom in the direction of the look std::vector. If the zoom is negative it will go in the opposite direction
	D3DXVECTOR3 upOffset = D3DXVECTOR3(0, 25, 0);
	D3DXVECTOR3 cameraPos = pMainHero->GetPosition() + camera->GetLookVector()*camera->GetZoom() + upOffset;

	camera->SetPosition(cameraPos);
}

/////////////////////////////////////////////////////////////////////////

//rotates to model and the camera if the mouse is moved
void Game::RotateObject(std::string objectTitle, float dt)
{
	GameObject* obj = m_pGameObjManager->GetObjectByName(objectTitle);
	SkinnedModel* pSkinnedModel = nullptr;

	if (obj != nullptr)
	{
		pSkinnedModel = static_cast<SkinnedModel*>(obj);
	}
	
	float yAngle = pDinput->GetMouseDX() / (19000*dt);
	pSkinnedModel->ModifyRotationAngleByY(yAngle);
	
	D3DXMATRIX R;
	D3DXMatrixRotationY(&R, yAngle);

	camera->TransformByMatrix(R);

	pSkinnedModel->GetLookVector()  = camera->GetLookVector();
	pSkinnedModel->GetRightVector() = camera->GetRightVector();
	pSkinnedModel->GetUpVector()    = camera->GetUpVector();
}


/////////////////////////////////////////////////////////////////////////

//controls the health of the hero and enemy and make them play dead animation if they are at zero health
void Game::ManageHealthBars()
{
	std::string strEnemy = pMainHero->GetAttackerName();
	GameObject* obj = m_pGameObjManager->GetObjectByName(strEnemy);
	SkinnedModel* pEnemy = nullptr;
	if (obj != nullptr)
	{
		pEnemy = static_cast<SkinnedModel*>(obj);
	}

	if( m_rHealthBarRectangle.right <= 0.0 && !pMainHero->IsDead())
	{
		pMainHero->PlayAnimationOnceAndStopTrack("dead");
		pMainHero->SetDead(true);

		pEnemy->SetAnimationOnTrack("idle",0);
	}
	if( m_rEnemyHealthBarRectangle.right <= 0.0 && !pEnemy->IsDead() )
	{
		pEnemy->PlayAnimationOnceAndStopTrack("dead");
		pEnemy->SetDead(true);
	}
}


/////////////////////////////////////////////////////////////////////////

void Game::OnRender()
{

    pDxDevice->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);

    pDxDevice->BeginScene();
	
			pSky->OnRender();
			pTerrain->OnRender();

			for (auto& gameObject : m_pGameObjManager->GetGameObjects())
			{
				gameObject->OnRender();
				//DrawLine(gameObject->GetPosition(), gameObject->GetLookVector());
				//DrawLine(gameObject->GetPosition(), gameObject->GetUpVector());
				//DrawLine(gameObject->GetPosition(), gameObject->GetRightVector());
			}
			
			//DrawLine(camera->GetPosition(), camera->GetLookVector());

			auto BB = pMainHero->GetBB();
			BB = BB.TransformByMatrix(BB.m_transformationMatrix);
			for (auto& object:m_pGameObjManager->GetGameObjects())
			{
				auto BB1 = object->GetBB();
				BB1 = BB1.TransformByMatrix(BB1.m_transformationMatrix);

				if (pMainHero->GetName().compare(object->GetName()))
				{
					if (BB.Collide(BB1))
					{
						//cout << "COLLIDING" << object->GetName() << endl;
					}
				}
			}

			//pTextManager->DrawFPS();
	
			//draws dialogues
			for (auto& dialogue : pDialogueManager->GetDialogues())
			{
				pDialogueManager->RenderDialogueTree(dialogue->m_pTree->m_pRoot);
			}

			//text->drawText("Press L to switch between the two camera modes",400,40,0,0,255,0,0,0);

			//draws quest stuff
			for(auto& quest: GetQuestManager()->GetQuests())
			{
				if( quest->m_bIsStarted )
				{
					if (quest->m_bIsCompleted)
					{
						pTextManager->RenderText(quest->m_strTitle.c_str(), pApp->GetPresentParameters().BackBufferWidth - 420, 70, 0, 0, 255, 255, 255, 0);
						pTextManager->RenderText("Completed", pApp->GetPresentParameters().BackBufferWidth - 590, 70, 0, 0, 255, 255, 255, 0);
					}
					else if (!pMainHero->IsDead())
					{
						pTextManager->RenderText(quest->m_strTitle.c_str(), pApp->GetPresentParameters().BackBufferWidth - 420, 70, 0, 0, 255, 255, 255, 0);
					}
					else if (!quest->m_bIsCompleted && pMainHero->IsDead())
					{
						pTextManager->RenderText(quest->m_strTitle.c_str(), pApp->GetPresentParameters().BackBufferWidth - 420, 70, 0, 0, 255, 255, 255, 0);
						pTextManager->RenderText("Epic fail!", pApp->GetPresentParameters().BackBufferWidth - 570, 70, 0, 0, 255, 255, 255, 0);
					}
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

			m_pGunEffect->OnRender();

	pDxDevice->EndScene();

	pDxDevice->Present(0, 0, 0, 0);
}
	
/////////////////////////////////////////////////////////////////////////

void Game::DrawLine(const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd)
{
	//pDxDevice->BeginScene();

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

		pDxDevice->SetVertexDeclaration(pApp->GetPCDecl());

		pDxDevice->DrawPrimitive(D3DPT_LINELIST,0,1);

	m_pDebugGraphicsEffect->EndPass();
	m_pDebugGraphicsEffect->End();

	//pDxDevice->EndScene();

	//pDxDevice->Present(0, 0, 0, 0);
}


/////////////////////////////////////////////////////////////////////////

//checks if two models are close
//i think this should be changed in the future with circles
bool Game::IsObjectNear(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, float t)
{
	if((pos1.x > pos2.x-t) && (pos1.x < pos2.x+t) &&
	   (pos1.z > pos2.z-t) && (pos1.z < pos2.z+t))
	{
	   return true;
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////

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
				pApp->SetPaused(true);
			}
			else
			{
				pApp->SetPaused(false);
			}

			return 0;
		}


		//Sent when the user closes the window
		case WM_CLOSE:
		{
			DestroyWindow( pApp->GetMainWindow() );
		
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
					if( camera->GetCameraMode() == ECameraMode::MoveWithoutPressedMouse )
					{
						camera->SetCameraMode(ECameraMode::MoveWithPressedMouse);
					}
					else if( camera->GetCameraMode() == ECameraMode::MoveWithPressedMouse )
					{
						camera->SetCameraMode(ECameraMode::MoveWithoutPressedMouse);
					}

					break;
				}
				case 'B':
				{
					m_pGameObjManager->SetShouldRenderBoundingBoxes(!m_pGameObjManager->ShouldRenderBoundingBoxes());

					break;
				}
				case 'C':
				{
					//TODO: if the camera becomes attached again align it with the object
					camera->SetCameraFree(!camera->IsCameraFree());

					break;
				}
				case 'Q':
				{
					m_rHealthBarRectangle.right += 20;

					break;
				}

				case VK_SHIFT:
				{
					pMainHero->SetMovementSpeed(2);
					break;
				}
			}

			return 0;
		}

		case WM_MOUSEWHEEL:
		{
			auto delta = GET_WHEEL_DELTA_WPARAM(wParam);

			camera->ModifyZoom(delta / 10);

			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			switch (wParam)
			{
				case MK_LBUTTON:
				{
					m_pGameObjManager->UpdatePicking();
				}
				break;
			}

			return 0;
		}

		case WM_KEYUP:
		{
			switch( wParam )
			{
				case VK_SHIFT:
				{
					pMainHero->SetMovementSpeed(1);
					break;
				}
			}
		}
	}

	return DefWindowProc(pApp->GetMainWindow(), msg, wParam, lParam);
}


/////////////////////////////////////////////////////////////////////////
