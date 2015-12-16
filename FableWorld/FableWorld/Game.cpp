#include"Game.h"
#include<math.h>

ofstream fout; //for logs.
lua_State* L;

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
	D3DXCreateTextureFromFile(pDxDevice,"../../Resources/textures/GUI/healthbar.dds",&m_pHealthBarTexture);
	D3DXCreateTextureFromFile(pDxDevice,"../../Resources/textures/GUI/healthbar_filled.dds",&m_pHealthBarFilledTexture);
	D3DXCreateTextureFromFile(pDxDevice,"../../Resources/textures/GUI/healthbar_filled_enemy.dds",&m_phealthBarFilledEnemyTexture);
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

	float fWidth  = (float)pApp->GetPresentParameters().BackBufferWidth;
	float fHeight = (float)pApp->GetPresentParameters().BackBufferHeight;

	camera = new Camera(D3DX_PI * 0.25f, fWidth/fHeight, 1.0f, 2000.0f,false);
	camera->SetCameraMode(ECameraMode::ECameraMode_MoveWithPressedMouse);

	//Initialize the vertex declarations. They are needed for creating the terrain, models and etc.
	InitVertexDeclarations();

	pSky = new Sky("../../Resources/textures/Sky/grassenvmap1024.dds", 10000.0f);

	//pTerrain = new Terrain("../../Resources/heightmaps/HeightmapFinal.raw",1.0f,513,513,1.0f,1.0f,D3DXVECTOR3(0.0f,0.0f,0.0f));
	pTerrain = new Terrain("../../Resources/heightmaps/coastMountain1025.raw", 1.0f, 1025, 1025, 1.0f, 1.0f, D3DXVECTOR3(0.0f, 0.0f, 0.0f));

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
	auto& gameObjects = m_pGameObjManager->GetSkinnedModels();
	for(auto& gameObject : gameObjects)
	{
		//create mesh for 3d text above the models
		pTextManager->CreateMeshFor3DText(gameObject);
			
		//check if certain skinned mesh has dialog attached to it
		for(auto it1 = pDialogueManager->m_vGameObjectsWithDialogues.begin();it1!=pDialogueManager->m_vGameObjectsWithDialogues.end();it1++)
		{
			if(gameObject->GetName() == (*it1) )
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
	
	D3DXVECTOR2 spellPosition(0,600);

	m_pHealSpell = new Button(spellPosition,64,64,"","blueTear.dds","blueTear.dds");

	InitDebugGraphicsShader();
}

/////////////////////////////////////////////////////////////////////////

//initializes the shader for debug graphics
void Game::InitDebugGraphicsShader()
{
	D3DXCreateEffectFromFile(pDxDevice, "../../CORE/CORE/shaders/DebugGraphicsShader.fx", 0, 0, D3DXSHADER_DEBUG, 0, &m_pDebugGraphicsEffect, 0);
	m_hDebugGraphicsTechnique  = m_pDebugGraphicsEffect->GetTechniqueByName("DebugGraphics3DTech");
	m_hDebugGraphicsWVPMatrix  = m_pDebugGraphicsEffect->GetParameterByName(0, "WVP");
}

/////////////////////////////////////////////////////////////////////////

Game::~Game()
{
	lua_close(L);
	delete pTextManager;
	delete pTerrain;
	delete camera;
	delete pDinput;
	delete pApp;
	delete pDialogueManager;
	delete soundsyst;
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

	m_pGameObjManager->OnUpdate();

	soundsyst->OnUpdate();
	pTextManager->OnUpdate(dt);
	camera->OnUpdate(dt);
	
	soundsyst->PlayAllSounds();

	//updates for the dialogues
	for(auto& dialogue: pDialogueManager->m_mapModelDialogue)
	{
		if( !dialogue.second.m_bIsEnded )
		{
			pDialogueManager->UpdateLabelTree(dialogue.second.m_pTree->GetRoot());
			pDialogueManager->UpdateLabelTreeRoot(dialogue.second.m_pTree->GetRoot());
			pDialogueManager->StartDialogue(dialogue.second.m_pTree->GetRoot(), dialogue.second);
			pDialogueManager->ChangeDialogue(dialogue.second.m_pCurrentDialogueNode, dialogue.second);
			pDialogueManager->LabelClicked(dialogue.second,m_mapActiveQuests,availableQuests);
		}
	}
	

	//binding the camera to mainHero in the game and moving it. mainHero is set in the scripts in init.lua
	if( !pMainHero->IsDead() && !camera->IsCameraFree() )
	{
		MoveObject(mainHero,dt);
	}

	//checking if there is active quest
	if(!m_mapActiveQuests.empty())
	{
		for(auto& quest : m_mapActiveQuests)
		{
			SkinnedModel* reqObject = m_pGameObjManager->GetSkinnedModelByName(quest.second.requiredObject);

			//if mainHero is close to the required from the quest object and the required object is dead the quest is completed.
			if(IsObjectNear(pMainHero, reqObject) && reqObject->IsDead())
			{
				quest.second.completed = true;
			}
		}
	}
	
	//revealing dialogue if model is picked
	for (auto& dialogue : pDialogueManager->m_mapModelDialogue)
	{
		GameObject* obj = m_pGameObjManager->GetObjectByName(dialogue.second.m_strModel);

		if(obj->IsPicked() && !dialogue.second.m_bIsEnded )
		{
			dialogue.second.m_pTree->GetRoot()->m_pLabel->SetVisible(true);
			obj->SetPicked(false);
		}
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

	/*for(map<string,GameObject*>::iterator it=m_pGameObjManager->GetGameObjects().begin();it!=m_pGameObjManager->GetGameObjects().end();it++)
	{
		DrawLine(pMainHero->m_vPos,gameObject->m_vPos);
	}*/

	//if mainHero is attacking
	for (auto& gameObject : m_pGameObjManager->GetSkinnedModels())
	{
		if( pDinput->IsMouseButtonDown(0) && 
			IsObjectNear(pMainHero,gameObject) && 
			pMainHero->GetName() != gameObject->GetName() &&
			gameObject->GetActorType() == "enemy"
			)
		{           
			m_bIsEnemyHealthBarVisible = true;
			gameObject->SetAttacked(true);
			gameObject->SetAttackerName(mainHero);
			pMainHero->PlayAnimationOnce("attack_1");
			if( (pMainHero->IsAttacking()) )
			{
				m_rEnemyHealthBarRectangle.right-=70;
			}
		}
		
		// enemy AI
		if( gameObject->IsAttacked() && IsObjectNear(pMainHero,gameObject))
		{
			if( m_rHealthBarRectangle.right > 0.0 )
			{
				pMainHero->SetAttackerName(gameObject->GetName());
				SkinnedModel* pSkinnedModel = static_cast<SkinnedModel*>(gameObject);

				pSkinnedModel->PlayAnimationOnce("attack_1");
				
				if( (gameObject->IsAttacking()) )
				{
					m_rHealthBarRectangle.right-=70;
				}
			}
			else
			{
				gameObject->SetAttacked(false);
			}
		}

		//when the enemy is attacked it updates his rotation so it can face the mainHero
		if( gameObject->IsAttacked() && !gameObject->IsDead() && IsObjectNear(pMainHero,gameObject) )
		{
			D3DXVECTOR3 vActorPosition = gameObject->GetPosition();
			D3DXVECTOR3 vMainHeroPosition = pMainHero->GetPosition();

			D3DXVECTOR3 vDistanceVector = vActorPosition - vMainHeroPosition;
			D3DXVec3Normalize(&vDistanceVector,&vDistanceVector);

			//for some reason the look vector is the right vector must be fixed
			float angle = D3DXVec3Dot(&gameObject->GetRightVector(),&vDistanceVector);
			gameObject->ModifyRotationAngleByY(angle);

			D3DXMATRIX R;
			D3DXMatrixRotationY(&R, angle);
			D3DXVec3TransformCoord(&gameObject->GetLookVector(), &gameObject->GetLookVector(), &R);
			D3DXVec3TransformCoord(&gameObject->GetRightVector(), &gameObject->GetRightVector(), &R);
			D3DXVec3TransformCoord(&gameObject->GetUpVector(), &gameObject->GetUpVector(), &R);
		}

		//if mainHero is fighting with enemy, but started to run and is no longer close to the enemy, 
		//the enemy updates his vectors so he can face the mainHero and run in his direction.
		//When he is close enough he start to attack again.
		if( gameObject->IsAttacked() && !gameObject->IsDead() && !IsObjectNear(pMainHero,gameObject)
			)
		{
			SkinnedModel* pSkinnedModel = static_cast<SkinnedModel*>(gameObject);
			pSkinnedModel->PlayAnimation("run");
			
			D3DXVECTOR3 dir(0.0f, 0.0f, 0.0f);

			dir -= gameObject->GetLookVector();

			D3DXVECTOR3 newPos = gameObject->GetPosition()+ dir*40.0*dt;
			gameObject->SetPosition(newPos);

			//put it in a function. the code is duplicated.
			D3DXVECTOR3 vActorPosition = gameObject->GetPosition();
			D3DXVECTOR3 vMainHeroPosition = pMainHero->GetPosition();

			D3DXVECTOR3 vDistanceVector = vActorPosition - vMainHeroPosition;
			D3DXVec3Normalize(&vDistanceVector,&vDistanceVector);

			float angle = D3DXVec3Dot(&gameObject->GetRightVector(),&vDistanceVector);
			gameObject->ModifyRotationAngleByY(angle);

			D3DXMATRIX R;
			D3DXMatrixRotationY(&R, angle);
			D3DXVec3TransformCoord(&gameObject->GetLookVector(), &gameObject->GetLookVector(), &R);
			D3DXVec3TransformCoord(&gameObject->GetRightVector(), &gameObject->GetRightVector(), &R);
			D3DXVec3TransformCoord(&gameObject->GetUpVector(), &gameObject->GetUpVector(), &R);
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

//makes the camera to follow model in game and moving it with WASD from keyboard and mouse
void Game::MoveObject(string objectTitle, float dt)
{
	//this vector holds the new direction to move
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
		dir += pSkinnedModel->GetLookVector();
	}
	if( pDinput->IsKeyDown(DIK_S) )
	{
		pSkinnedModel->PlayAnimation("run");
		dir -= pSkinnedModel->GetLookVector();
	}
	if( pDinput->IsKeyDown(DIK_A) )
	{
		pSkinnedModel->PlayAnimation("run");
		dir -= pSkinnedModel->GetRightVector();
	}
	if( pDinput->IsKeyDown(DIK_D) )
	{
		pSkinnedModel->PlayAnimation("run");
		dir += pSkinnedModel->GetRightVector();
	}

	if( !pDinput->IsKeyDown(DIK_W) && !pDinput->IsKeyDown(DIK_S) && !pDinput->IsKeyDown(DIK_A) && !pDinput->IsKeyDown(DIK_D) )
	{
		pSkinnedModel->PlayAnimation("idle");
	}
	
	if (camera->GetCameraMode() == ECameraMode::ECameraMode_MoveWithoutPressedMouse)
	{
		//if we just move the mouse move the camera
		RotateObject(objectTitle, dt);

	}
	else if (camera->GetCameraMode() == ECameraMode::ECameraMode_MoveWithPressedMouse)
	{
		//if we hold the left mouse button move the camera
		if (pDinput->IsMouseButtonDown(0))
		{
			RotateObject(objectTitle, dt);
		}
	}

	D3DXVECTOR3 newPos = pSkinnedModel->GetPosition() + dir*150.0*dt;
	if( pTerrain->IsValidPosition(newPos.x,newPos.z))
	{
		pSkinnedModel->SetPosition(newPos);
	}

	//updates the camera position based on the new position of the model and the zoom
	//we zoom in the direction of the look vector. If the zoom is negative it will go in the opposite direction
	D3DXVECTOR3 upOffset = D3DXVECTOR3(0, 25, 0);
	D3DXVECTOR3 cameraPos = pMainHero->GetPosition() + camera->GetLookVector()*camera->GetZoom() + upOffset;

	camera->SetPosition(cameraPos);
}

/////////////////////////////////////////////////////////////////////////

//rotates to model and the camera if the mouse is moved
void Game::RotateObject(string objectTitle, float dt)
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
	D3DXVec3TransformCoord(&camera->GetRightVector(), &camera->GetRightVector(), &R);
	D3DXVec3TransformCoord(&camera->GetUpVector(), &camera->GetUpVector(), &R);
	D3DXVec3TransformCoord(&camera->GetLookVector(), &camera->GetLookVector(), &R);

	pSkinnedModel->GetLookVector()  = camera->GetLookVector();
	pSkinnedModel->GetRightVector() = camera->GetRightVector();
	pSkinnedModel->GetUpVector()    = camera->GetUpVector();
}


/////////////////////////////////////////////////////////////////////////

//controls the health of the hero and enemy and make them play dead animation if they are at zero health
void Game::ManageHealthBars()
{
	string strEnemy = pMainHero->GetAttackerName();
	GameObject* obj = m_pGameObjManager->GetObjectByName(strEnemy);
	SkinnedModel* pEnemy = nullptr;
	if (obj != nullptr)
	{
		pEnemy = static_cast<SkinnedModel*>(obj);
	}

	if( m_rHealthBarRectangle.right <= 0.0 )
	{
		pMainHero->PlayAnimationOnceAndStop("dead");
		pMainHero->SetDead(true);
		
		//set the animation of the attacked to idle, because sometimes got bugged and plays run animaiton
		pEnemy->PlayAnimation("idle");
	}
	if( m_rEnemyHealthBarRectangle.right <= 0.0 )
	{
		pEnemy->PlayAnimationOnceAndStop("dead");
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
			}
			
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
						cout << "COLLIDING" << object->GetName() << endl;
					}
				}
			}

			pTextManager->DrawFPS();
	
			//draws dialogues
			for (auto& dialogue : pDialogueManager->m_mapModelDialogue)
			{
				pDialogueManager->RenderLabelTreeRoot(dialogue.second.m_pTree->GetRoot());
				pDialogueManager->RenderLabelTree(dialogue.second.m_pTree->GetRoot());
			}

			//text->drawText("Press L to switch between the two camera modes",400,40,0,0,255,0,0,0);

			//draws quest stuff
			for(auto& quest: m_mapActiveQuests)
			{
				if(quest.second.completed)
				{
					pTextManager->RenderText(quest.second.title.c_str(),pApp->GetPresentParameters().BackBufferWidth-420,70,0,0,255,255,255,0);
					pTextManager->RenderText("Completed",pApp->GetPresentParameters().BackBufferWidth-590,70,0,0,255,255,255,0);
				}
				else if( !pMainHero->IsDead() )
				{
					pTextManager->RenderText(quest.second.title.c_str(),pApp->GetPresentParameters().BackBufferWidth-420,70,0,0,255,255,255,0);
				}
				else if( !quest.second.completed && pMainHero->IsDead() )
				{
					pTextManager->RenderText(quest.second.title.c_str(),pApp->GetPresentParameters().BackBufferWidth-420,70,0,0,255,255,255,0);
					pTextManager->RenderText("Epic fail!",pApp->GetPresentParameters().BackBufferWidth-570,70,0,0,255,255,255,0);
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

		pDxDevice->SetVertexDeclaration(pApp->GetPosColDeclaration());

		pDxDevice->DrawPrimitive(D3DPT_LINELIST,0,1);

	m_pDebugGraphicsEffect->EndPass();
	m_pDebugGraphicsEffect->End();

	pDxDevice->EndScene();

	pDxDevice->Present(0, 0, 0, 0);
}


/////////////////////////////////////////////////////////////////////////

//checks if two models are close
bool Game::IsObjectNear(GameObject* obj1,GameObject* obj2)
{
	if((obj1->GetPosition().x > obj2->GetPosition().x-30) && (obj1->GetPosition().x < obj2->GetPosition().x+30) &&
	   (obj1->GetPosition().z > obj2->GetPosition().z-30) && (obj1->GetPosition().z < obj2->GetPosition().z+30))
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
					if( camera->GetCameraMode() == ECameraMode::ECameraMode_MoveWithoutPressedMouse )
					{
						camera->SetCameraMode(ECameraMode::ECameraMode_MoveWithPressedMouse);
					}
					else if( camera->GetCameraMode() == ECameraMode::ECameraMode_MoveWithPressedMouse )
					{
						camera->SetCameraMode(ECameraMode::ECameraMode_MoveWithoutPressedMouse);
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
			}

			return 0;
		}

		case WM_MOUSEWHEEL:
		{
			auto delta = GET_WHEEL_DELTA_WPARAM(wParam);

			camera->ModifyZoom(delta / 10);
		}

  }
	return DefWindowProc(pApp->GetMainWindow(), msg, wParam, lParam);
}


/////////////////////////////////////////////////////////////////////////
