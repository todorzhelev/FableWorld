#include"Game.h"
#include<math.h>

ofstream fout; //for logs.
lua_State* L;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//TODO: refactor the whole code 

Game::Game()
{
	//checks if pixel and vertex shader 2.0 is supported
	if(!checkDeviceCaps())
	{
		MessageBox(0, "checkDeviceCaps() Failed", 0, 0);
		PostQuitMessage(0);
	}

	float fWidth  = (float)pApp->GetPresentParameters().BackBufferWidth;
	float fHeight = (float)pApp->GetPresentParameters().BackBufferHeight;

	//init and setup camera
	camera = new Camera(D3DX_PI * 0.25f, fWidth/fHeight, 1.0f, 2000.0f,true);
	camera->SetCameraMode(ECameraMode::ECameraMode_MoveWithPressedMouse);

	D3DXVECTOR3 vCameraPosition = D3DXVECTOR3(0,100,0);
	camera->SetPosition(vCameraPosition);

	//Initialize the vertex declarations. They are needed for creating the terrain, models and etc.
	InitVertexDeclarations();

	// user isnt typing anything in textbox yet
	typingMode = false;


	//init sky
	sky = new Sky("../../Resources/textures/Sky/grassenvmap1024.dds", 10000.0f);

	//init terrain
	pTerrain = new Terrain("../../Resources/heightmaps/HeightmapFinal.raw",0.85f,513,513,1.0f,1.0f,D3DXVECTOR3(0.0f,0.0f,0.0f));;
	D3DXVECTOR3 toSun(-20.0f, 300.0f, 50.0f);
	D3DXVec3Normalize(&toSun, &toSun);
	pTerrain->SetLightVector(toSun);

	soundsyst = new SoundSyst(100);

	pTextManager->CreateFontFor3DText();
	
	LoadUIComponents();

		//init lua
	L = lua_open();
	//open lua libs
	luaL_openlibs(L);
	
	//with lua_register we bind the functions in the cpp file with the invoked functions in the script file
	//here addStaticModel is function in the script.When it is invoked there it actually invokes l_addStaticModel(lua_State* L)
	lua_register(L,"addStaticModel", l_addStaticModel );
	lua_register(L,"addAnimatedModel",l_addAnimatedModel);
	lua_register(L,"setUpMainHero",l_setUpMainHero);

	OnResetDevice();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Game::LoadUIComponents()
{
	//TODO: this must be moved to some script
	float width  = (float)pApp->GetPresentParameters().BackBufferWidth;
	float height = (float)pApp->GetPresentParameters().BackBufferHeight;

	
	//load static model button
	button_LoadStaticModel = new Button(D3DXVECTOR2(width - 630, 15.0),128,32,"","Button_LSM_final1.dds","Button_LSM_final2_hover.dds");
	pApp->AddUIObject(button_LoadStaticModel);
	
	//load animated model button
	button_LoadAnimatedModel = new Button(D3DXVECTOR2(width - 490, 15.0),128,32,"","Button_LAM_final1.dds","Button_LAM_final2_hover.dds");
	pApp->AddUIObject(button_LoadAnimatedModel);

	//import button
	button_Import = new Button(D3DXVECTOR2(width - 280, 15.0),128,32,"","Button_Import.dds","Button_Import_hover.dds");
	pApp->AddUIObject(button_Import);

	//export button
	button_Export = new Button(D3DXVECTOR2(width - 140, 15.0),128,32,"","Button_Export.dds","Button_Export_hover.dds");
	pApp->AddUIObject(button_Export);

	//translation label
	button_Translation = new Button(D3DXVECTOR2(6.0, 13.0),128,32,"","Button_Translation.dds","Button_Translation.dds");
	pApp->AddUIObject(button_Translation);

	//translation x checkbox
	checkbox_TranslationX = new Checkbox(D3DXVECTOR2(20.0, 45.0),32,32,"x","Checkbox_checked.dds","Checkbox_unchecked.dds","checkbox_TranslationX");
	pApp->AddUIObject(checkbox_TranslationX);

	//translation y checkbox
	checkbox_TranslationY = new Checkbox(D3DXVECTOR2(70.0, 45.0),32,32,"y","Checkbox_checked.dds","Checkbox_unchecked.dds","checkbox_TranslationY");
	pApp->AddUIObject(checkbox_TranslationY);

	//translation z checkbox
	checkbox_TranslationZ = new Checkbox(D3DXVECTOR2(120.0, 45.0),32,32,"z","Checkbox_checked.dds","Checkbox_unchecked.dds","checkbox_TranslationZ");;
	pApp->AddUIObject(checkbox_TranslationZ);

	//export button
	button_Rotation = new Button(D3DXVECTOR2(-10.0, 68.0),128,32,"","Button_Rotation.dds","Button_Rotation.dds");
	pApp->AddUIObject(button_Rotation);

	//rotation x checkbox 
	checkbox_RotationX = new Checkbox(D3DXVECTOR2(20.0, 100.0),32,32,"x","Checkbox_checked.dds","Checkbox_unchecked.dds","checkbox_RotationX");;
	pApp->AddUIObject(checkbox_RotationX);

	//rotation y checkbox
	checkbox_RotationY = new Checkbox(D3DXVECTOR2(70.0, 100.0),32,32,"y","Checkbox_checked.dds","Checkbox_unchecked.dds","checkbox_RotationY");;
	pApp->AddUIObject(checkbox_RotationY);

	//rotation z checkbox 
	checkbox_RotationZ = new Checkbox(D3DXVECTOR2(120.0, 100.0),32,32,"z","Checkbox_checked.dds","Checkbox_unchecked.dds","checkbox_RotationZ");;
	pApp->AddUIObject(checkbox_RotationZ);

	//export button
	button_Scaling = new Button(D3DXVECTOR2(-13.0, 122.0),128,32,"","Button_Scaling.dds","Button_Scaling.dds");
	pApp->AddUIObject(button_Scaling);

	//scaling checkbox
	checkbox_Scaling = new Checkbox(D3DXVECTOR2(20.0, 155.0),32,32,"scale","Checkbox_checked.dds","Checkbox_unchecked.dds","checkbox_Scaling");;
	pApp->AddUIObject(checkbox_Scaling);

	//model name label
	label_modelName = new Label(D3DXVECTOR2(width-300,150),"model name");
	label_modelName->SetVisible(false);
	pApp->AddUIObject(label_modelName);


	//model name textbox
	textbox_modelName = new Textbox(D3DXVECTOR2(width-300, 170.0f),256,32,"","Textbox.dds","Textbox_selected.dds","textbox_modelName");
	textbox_modelName->SetVisible(false);
	textbox_modelName->m_bIsSelected = false;
	pApp->AddUIObject(textbox_modelName);
	
	//type in game label
	label_typeInGame = new Label(D3DXVECTOR2(width-300,220),"type in game");
	label_typeInGame->SetVisible(false);
	pApp->AddUIObject(label_typeInGame);

	//type in game textbox
	textbox_typeInGame = new Textbox(D3DXVECTOR2(width-300, 240.0f),256,32,"","Textbox.dds","Textbox_selected.dds","textbox_typeInGame");
	textbox_typeInGame->SetVisible(false);
	textbox_typeInGame->m_bIsSelected = false;
	pApp->AddUIObject(textbox_typeInGame);

	//title for quest label
	label_titleForQuest = new Label(D3DXVECTOR2(width-300,290),"title for quest");
	label_titleForQuest->SetVisible(false);
	pApp->AddUIObject(label_titleForQuest);

	//title for quest textbox
	textbox_titleForQuest = new Textbox(D3DXVECTOR2(width-300, 310.0f),256,32,"","Textbox.dds","Textbox_selected.dds","textbox_titleForQuest");
	textbox_titleForQuest->SetVisible(false);
	textbox_titleForQuest->m_bIsSelected = false;
	pApp->AddUIObject(textbox_titleForQuest);

	//title for quest label
	label_bindToAnModel = new Label(D3DXVECTOR2(width-300,220),"bind to animated model");
	label_bindToAnModel->SetVisible(false);
	pApp->AddUIObject(label_bindToAnModel);

	//bind to animated model textbox
	textbox_bindToAnModel = new Textbox(D3DXVECTOR2(width-300, 240),256,32,"","Textbox.dds","Textbox_selected.dds","textbox_bindToAnModel");
	textbox_bindToAnModel->SetVisible(false);
	textbox_bindToAnModel->m_bIsSelected = false;
	pApp->AddUIObject(textbox_bindToAnModel);

	//title for quest label
	label_bindToAnModelBone = new Label(D3DXVECTOR2(width-300,290),"bind to animated model bone");
	label_bindToAnModelBone->SetVisible(false);
	pApp->AddUIObject(label_bindToAnModelBone);

	//bind to animated model's bone textbox
	textbox_bindToAnModelBone = new Textbox(D3DXVECTOR2(width-300, 310),256,32,"","Textbox.dds","Textbox_selected.dds","textbox_bindToAnModelBone");
	textbox_bindToAnModelBone->SetVisible(false);
	textbox_bindToAnModelBone->m_bIsSelected = false;
	pApp->AddUIObject(textbox_bindToAnModelBone);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Game::~Game()
{
	/*delete text;
	delete xmesh;
	delete smesh;
	delete terrain;
	delete camera;
	delete Dinput;
	delete but;
	delete wind;
	delete label;
	delete textbox;
	delete soundsyst;
	delete sky;
	delete engine;*/
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Game::checkDeviceCaps()
{
	D3DCAPS9 caps;
	pDxDevice->GetDeviceCaps(&caps); 
	if( caps.VertexShaderVersion < D3DVS_VERSION(2, 0) )
		return false;
	if( caps.PixelShaderVersion < D3DPS_VERSION(2, 0) )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Game::OnLostDevice()
{
	
	sky->OnLostDevice();

	for (auto& obj : pApp->m_vUIObjects)
	{
		obj->OnLostDevice();
	}

	pTextManager->OnLostDevice();
	pTerrain->OnLostDevice();

	for(auto& obj : m_pGameObjManager->GetGameObjects() )
	{
		obj->OnLostDevice();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Game::OnResetDevice()
{
	sky->OnResetDevice();

	for (auto& obj : pApp->m_vUIObjects)
	{
		obj->OnResetDevice();
	}

	pTextManager->OnResetDevice();
	pTerrain->OnResetDevice();

	for (auto& obj : m_pGameObjManager->GetGameObjects())
	{
		obj->OnResetDevice();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Game::OnUpdate(float dt)
{
	//poll starts to listen if any key on the keyboard is pressed
	pDinput->Poll();

		//if escape is pressed in game it switches to another scene
	if(pDinput->IsKeyDown(DIK_ESCAPE))
	{
		IBaseScene* pMenuInGameScene = pApp->GetScene("menuInGame");
		pApp->SetCurrentScene(pMenuInGameScene);
	}

	//update all the game objects
	for (auto& obj : m_pGameObjManager->GetGameObjects())
	{
		obj->OnUpdate(dt);
	}

	//update all the UI objects
	for (auto& obj : pApp->m_vUIObjects)
	{
		obj->OnUpdate();
	}

	pTextManager->OnUpdate(dt);

	//we store the currently selected textbox in string. If its empty nothing is selected
	if( pApp->m_strSelectedTextbox == "" )
	{
		typingMode = false;
	}
	else 
	{
		typingMode = true;
	}


	//dont move the camera while we are typing in textbox
	if( !typingMode )
	{
		camera->OnUpdate(dt);
	}

	
	if( button_LoadStaticModel->IsMouseDown() )
	{
		ImportStaticModel();
	}

	if( button_LoadAnimatedModel->IsMouseDown() )
	{
		LoadAnimatedModel();
	}

	if( button_Export->IsMouseDown() )
	{
		ExportLevel();
	}

	if( button_Import->IsMouseDown() )
	{
		ImportLevel();
	}

	
	//TODO this must not be made like this. When something is picked it should do the job automatically, 
	//even if a callback is needed.
	//iterates through all animated models and checks if any of them is picked
	for (auto& obj : m_pGameObjManager->GetGameObjects())
	{
		if( obj->GetObjectType() == EGameObjectType_Skinned )
		{		
			SkinnedModel* skinnedModel = static_cast<SkinnedModel*>(obj);

			//it it is picked reveals animated model's related textboxes and labels
			if(obj->IsPicked() && !label_typeInGame->IsVisible())
			{
				label_modelName->SetVisible(true);
				textbox_modelName->SetVisible(true);

				label_bindToAnModel->SetVisible(false);
				textbox_bindToAnModel->SetVisible(false);

				label_bindToAnModelBone->SetVisible(false);
				textbox_bindToAnModelBone->SetVisible(false);


				label_titleForQuest->SetVisible(true);
				textbox_titleForQuest->SetVisible(true);

				label_typeInGame->SetVisible(true);
				textbox_typeInGame->SetVisible(true);

			}

			//with switchedPicked we control if we picked new model and unpicked old one
			if( m_pGameObjManager->GetPickedObject() == NULL && obj->IsPicked() ||
				m_pGameObjManager->GetPickedObject() != NULL && 
				obj->IsPicked() &&
				m_pGameObjManager->GetPickedObject()->GetName().compare(obj->GetName()) )
			{
				textbox_typeInGame->SetText(skinnedModel->GetActorType()); //TODO: fix this
				textbox_modelName->SetText(obj->GetName());
				textbox_titleForQuest->SetText(skinnedModel->GetTitleForQuest());
				
				if( m_pGameObjManager->GetPickedObject() != NULL &&  
					m_pGameObjManager->GetPickedObject()->GetName().compare(obj->GetName()) )
				{
					m_pGameObjManager->GetPickedObject()->SetPicked(false);
				}

				m_pGameObjManager->SetPickedObject(obj);
			}

			pickedModelControl(*obj, dt);
		}
		else if( obj->GetObjectType() == EGameObjectType_Static )
		{
			if( obj->IsPicked() && !label_bindToAnModel->IsVisible() )
			{
				label_modelName->SetVisible(true);
				textbox_modelName->SetVisible(true);

				label_bindToAnModel->SetVisible(true);
				textbox_bindToAnModel->SetVisible(true);

				label_bindToAnModelBone->SetVisible(true);
				textbox_bindToAnModelBone->SetVisible(true);


				label_titleForQuest->SetVisible(false);
				textbox_titleForQuest->SetVisible(false);

				label_typeInGame->SetVisible(false);
				textbox_typeInGame->SetVisible(false);
			}

			//with switchedPicked we control if we picked new model and unpicked old one
			if( m_pGameObjManager->GetPickedObject() == NULL && obj->IsPicked() || 
				m_pGameObjManager->GetPickedObject() != NULL && 
				obj->IsPicked() && 
				m_pGameObjManager->GetPickedObject()->GetName().compare(obj->GetName()) )
			{
				//textbox_typeInGame->SetText((*it).second->m_strActorType);
				textbox_modelName->SetText(obj->GetName());
				//textbox_titleForQuest->SetText((*it).second->m_strTitleForQuest);
				
				if( m_pGameObjManager->GetPickedObject() != NULL &&  
					m_pGameObjManager->GetPickedObject()->GetName().compare(obj->GetName()) )
				{
					m_pGameObjManager->GetPickedObject()->SetPicked(false);
				}

				m_pGameObjManager->SetPickedObject(obj);
			}

			pickedModelControl(*obj, dt);
		}
	}

	//unpick model that its picked with K
	//TODO: two panels should be made, which will be used to hide/show all of their children elements
	if( pDinput->IsKeyDown(DIK_K) )
	{
		if( m_pGameObjManager->GetPickedObject() != nullptr )
		{	
			//TODO : fix this
			//m_pGameObjManager->GetGameObjects().erase(m_pGameObjManager->GetPickedObject()->GetName());

			m_pGameObjManager->SetPickedObject(nullptr);

			label_modelName->SetVisible(false);
			textbox_modelName->SetVisible(false);

			label_bindToAnModel->SetVisible(false);
			textbox_bindToAnModel->SetVisible(false);

			label_bindToAnModelBone->SetVisible(false);
			textbox_bindToAnModelBone->SetVisible(false);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Game::pickedModelControl(GameObject& obj, float dt)
{
	if( obj.IsPicked() )
	{
		if (static_cast<Checkbox*>(checkbox_TranslationX)->m_bIsChecked)
		{
			if (pDinput->IsMouseButtonDown(0))
			{
				auto vec = obj.GetLookVector() *40.0*dt;
				auto pos = obj.GetPosition();
				obj.SetPosition(pos + vec);
			}
			else if (pDinput->IsMouseButtonDown(1))
			{
				auto vec = obj.GetLookVector() *40.0*dt;
				auto pos = obj.GetPosition();
				obj.SetPosition(pos - vec);
			}
		}

		if (static_cast<Checkbox*>(checkbox_TranslationY)->m_bIsChecked)
		{
			if (pDinput->IsMouseButtonDown(0))
			{
				auto vec = obj.GetUpVector() *40.0*dt;
				auto pos = obj.GetPosition();
				obj.SetPosition(pos + vec);
			}
			else if (pDinput->IsMouseButtonDown(1))
			{
				auto vec = obj.GetUpVector() *40.0*dt;
				auto pos = obj.GetPosition();
				obj.SetPosition(pos - vec);
			}
		}

		if (static_cast<Checkbox*>(checkbox_TranslationZ)->m_bIsChecked)
		{
			if (pDinput->IsMouseButtonDown(0))
			{
				auto vec = obj.GetRightVector() *40.0*dt;
				auto pos = obj.GetPosition();
				obj.SetPosition(pos + vec);
			}
			else if (pDinput->IsMouseButtonDown(1))
			{
				auto vec = obj.GetRightVector() *40.0*dt;
				auto pos = obj.GetPosition();
				obj.SetPosition(pos - vec);
			}
		}

		if (static_cast<Checkbox*>(checkbox_RotationY)->m_bIsChecked)
		{
			if (pDinput->IsMouseButtonDown(0))
			{
				float yAngle = 0.1;
				obj.ModifyRotationAngleByY(yAngle);

				D3DXMATRIX R;
				D3DXMatrixRotationY(&R, yAngle);
				D3DXVec3TransformCoord(&obj.GetRightVector(), &obj.GetRightVector(), &R);
				D3DXVec3TransformCoord(&obj.GetUpVector(), &obj.GetUpVector(), &R);
				D3DXVec3TransformCoord(&obj.GetLookVector(), &obj.GetLookVector(), &R);
			}
			else if (pDinput->IsMouseButtonDown(1))
			{
				float yAngle = 0.1;
				obj.ModifyRotationAngleByY(-yAngle);

				D3DXMATRIX R;
				//it is -yAngle, because we rotate on the other side
				D3DXMatrixRotationY(&R, -yAngle);
				D3DXVec3TransformCoord(&obj.GetRightVector(), &obj.GetRightVector(), &R);
				D3DXVec3TransformCoord(&obj.GetUpVector(), &obj.GetUpVector(), &R);
				D3DXVec3TransformCoord(&obj.GetLookVector(), &obj.GetLookVector(), &R);
			}
		}

		if (static_cast<Checkbox*>(checkbox_RotationX)->m_bIsChecked)
		{
			if (pDinput->IsMouseButtonDown(0))
			{
				float yAngle = 0.1;
				obj.ModifyRotationAngleByX(yAngle);

				D3DXMATRIX R;
				D3DXMatrixRotationX(&R, yAngle);
				D3DXVec3TransformCoord(&obj.GetRightVector(), &obj.GetRightVector(), &R);
				D3DXVec3TransformCoord(&obj.GetUpVector(), &obj.GetUpVector(), &R);
				D3DXVec3TransformCoord(&obj.GetLookVector(), &obj.GetLookVector(), &R);
			}
			else if (pDinput->IsMouseButtonDown(1))
			{
				float yAngle = 0.1;
				obj.ModifyRotationAngleByX(-yAngle);

				D3DXMATRIX R;
				D3DXMatrixRotationX(&R, -yAngle);
				D3DXVec3TransformCoord(&obj.GetRightVector(), &obj.GetRightVector(), &R);
				D3DXVec3TransformCoord(&obj.GetUpVector(), &obj.GetUpVector(), &R);
				D3DXVec3TransformCoord(&obj.GetLookVector(), &obj.GetLookVector(), &R);
			}
		}

		if (static_cast<Checkbox*>(checkbox_RotationZ)->m_bIsChecked)
		{
			if (pDinput->IsMouseButtonDown(0))
			{
				float yAngle = 0.1;
				obj.ModifyRotationAngleByZ(yAngle);

				D3DXMATRIX R;
				D3DXMatrixRotationZ(&R, yAngle);
				D3DXVec3TransformCoord(&obj.GetRightVector(), &obj.GetRightVector(), &R);
				D3DXVec3TransformCoord(&obj.GetUpVector(), &obj.GetUpVector(), &R);
				D3DXVec3TransformCoord(&obj.GetLookVector(), &obj.GetLookVector(), &R);
			}
			else if (pDinput->IsMouseButtonDown(1))
			{
				float yAngle = 0.1;
				obj.ModifyRotationAngleByZ(-yAngle);

				D3DXMATRIX R;
				D3DXMatrixRotationZ(&R, -yAngle);
				D3DXVec3TransformCoord(&obj.GetRightVector(), &obj.GetRightVector(), &R);
				D3DXVec3TransformCoord(&obj.GetUpVector(), &obj.GetUpVector(), &R);
				D3DXVec3TransformCoord(&obj.GetLookVector(), &obj.GetLookVector(), &R);
			}
		}

		if (static_cast<Checkbox*>(checkbox_Scaling)->m_bIsChecked)
		{
			if (pDinput->IsMouseButtonDown(0))
			{
				obj.ModifyScale(0.01);
			}
			else if (pDinput->IsMouseButtonDown(1))
			{
				obj.ModifyScale(-0.01);
			}
		}
	}
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Game::OnRender()
{
    pDxDevice->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);

    pDxDevice->BeginScene();
	
	sky->OnRender();

	pTerrain->OnRender();

	//render all the game objects
	for(auto& obj : m_pGameObjManager->GetGameObjects() )
	{
		obj->OnRender();
	}
			
	if (m_pGameObjManager->GetPickedObject() != nullptr)
	{
		auto BB = m_pGameObjManager->GetPickedObject()->GetBB();
		BB = BB.TransformByMatrix(BB.m_transformationMatrix);
		for (auto& obj : m_pGameObjManager->GetGameObjects())
		{
			auto BB1 = obj->GetBB();
			BB1 = BB1.TransformByMatrix(BB1.m_transformationMatrix);

			if (m_pGameObjManager->GetPickedObject()->GetName().compare(obj->GetName()))
			{
				if (BB.Collide(BB1))
				{
					cout << "COLLIDING" << obj->GetName() << endl;
				}
			}
		}
	}
			
	for(auto& obj: pApp->m_vUIObjects)
	{
		obj->OnRender(255, 255, 255, 255);
	}

	pDxDevice->EndScene();

	pDxDevice->Present(0, 0, 0, 0);
}
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Game::isNear(GameObject& obj1,GameObject& obj2)
{
	if((obj1.GetPosition().x > obj2.GetPosition().x-30) && (obj1.GetPosition().x < obj2.GetPosition().x+30) &&
	   (obj1.GetPosition().z > obj2.GetPosition().z-30) && (obj1.GetPosition().z < obj2.GetPosition().z+30))
	{
	   return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT Game::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	IBaseMenuObject* pSelectedTextbox = pApp->FindMenuObject(pApp->m_strSelectedTextbox);

	switch( msg )
	{
		//Sent when the window is activated or deactivated.(pressed alt+tab)
		//Game is paused when the window is inactive, and unpaused when become active again
		case WM_ACTIVATE:
			if( LOWORD(wParam) == WA_INACTIVE )
			{
				pApp->SetPaused(true);
			}
			else
			{
				pApp->SetPaused(false);
			}
			return 0;


		//Sent when the user closes the window
		case WM_CLOSE:
			DestroyWindow( pApp->GetMainWindow() );
			return 0;

		//Sent when the window is destroyed
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;



		//Sent when the user type something on the keyboard
		case WM_CHAR:
			

			switch(wParam)
			{
				//this is the code for enter
				//pressing enter in textbox finishes the typing and assigns the new values
				//TODO: create map code, name to be more clear...
				case 0x0D:
				{
					//IBaseMenuObject* pSelectedTextbox = pApp->FindMenuObject(selectedTextbox);


					if( !pApp->m_strSelectedTextbox.empty() )
					{
					    if( pApp->m_strSelectedTextbox == "textbox_typeInGame" )
						{
							if(m_pGameObjManager->GetPickedObject()->GetObjectType() == EGameObjectType_Skinned)
							{
								SkinnedModel* pickedObject = static_cast<SkinnedModel*>(m_pGameObjManager->GetPickedObject());

								pickedObject->SetActorType(pSelectedTextbox->GetText());
								pSelectedTextbox->m_bIsSelected = false;
								pApp->m_strSelectedTextbox = "";
							}
						}
						else if( pApp->m_strSelectedTextbox == "textbox_modelName")
						{
							if( m_pGameObjManager->GetPickedObject()->GetObjectType() == EGameObjectType_Skinned )
							{
								std::string name =  m_pGameObjManager->GetPickedObject()->GetName();
								SkinnedModel *obj = m_pGameObjManager->GetSkinnedModelByName(name);

								obj->SetName(pSelectedTextbox->GetText());
								pSelectedTextbox->m_bIsSelected = false;

								pApp->m_strSelectedTextbox = "";
							}
						}
						else if( pApp->m_strSelectedTextbox == "textbox_titleForQuest" && 
								 m_pGameObjManager->GetPickedObject()->GetObjectType() == EGameObjectType_Skinned )
						{
							std::string name = m_pGameObjManager->GetPickedObject()->GetName();
							SkinnedModel *obj = m_pGameObjManager->GetSkinnedModelByName(name);
							obj->SetTitleForQuest(pSelectedTextbox->GetText());
							pSelectedTextbox->m_bIsSelected = false;

							pApp->m_strSelectedTextbox = "";
						}
						else if( pApp->m_strSelectedTextbox == "textbox_bindToAnModel" && 
								 m_pGameObjManager->GetPickedObject()->GetObjectType() == EGameObjectType_Static )
						{
							m_pGameObjManager->GetPickedObject()->SetBindedToAnimatedModelName(pSelectedTextbox->GetText());

							if( !m_pGameObjManager->GetPickedObject()->GetBindedToBoneName().empty() )
							{
								std::string name = m_pGameObjManager->GetPickedObject()->GetBindedToAnimatedModelName();
								SkinnedModel *obj = m_pGameObjManager->GetSkinnedModelByName(name);

								obj->BindWeaponToModel(m_pGameObjManager->GetPickedObject()->GetName(),m_pGameObjManager->GetPickedObject()->GetBindedToBoneName());

								m_pGameObjManager->GetPickedObject()->SetIsBindable(true);
							}

							pSelectedTextbox->m_bIsSelected = false;
							pApp->m_strSelectedTextbox = "";
						}
						else if( pApp->m_strSelectedTextbox == "textbox_bindToAnModelBone" && 
								 m_pGameObjManager->GetPickedObject()->GetObjectType() == EGameObjectType_Static  )
						{
							m_pGameObjManager->GetPickedObject()->SetBindedToBoneName(pSelectedTextbox->GetText());

							if( !m_pGameObjManager->GetPickedObject()->GetBindedToAnimatedModelName().empty() )
							{
								std::string name = m_pGameObjManager->GetPickedObject()->GetBindedToAnimatedModelName();
								SkinnedModel *obj = m_pGameObjManager->GetSkinnedModelByName(name);

								obj->BindWeaponToModel(m_pGameObjManager->GetPickedObject()->GetName(),m_pGameObjManager->GetPickedObject()->GetBindedToBoneName());

								m_pGameObjManager->GetPickedObject()->SetIsBindable(true);
								
							}
							pSelectedTextbox->m_bIsSelected = false;
							pApp->m_strSelectedTextbox = "";
						}						
					}
				}
				break;

				//here we detect any key pressed and adds it to the selected textbox
				default:
				{
					if( !pApp->m_strSelectedTextbox.empty() )
					{
						
						//if model name textbox is selected
						if( pApp->m_strSelectedTextbox == "textbox_modelName" ||
							pApp->m_strSelectedTextbox == "textbox_typeInGame" ||
							pApp->m_strSelectedTextbox == "textbox_titleForQuest" ||
							pApp->m_strSelectedTextbox == "textbox_bindToAnModel" ||
							pApp->m_strSelectedTextbox == "textbox_bindToAnModelBone" )
						{
								//if we press return delete one char
								if(wParam == VK_BACK && pSelectedTextbox->GetText().size()!= 0)
								{
									string strNewText = pSelectedTextbox->GetText();
									strNewText.erase(strNewText.end() - 1);

									pSelectedTextbox->SetText(strNewText);
								}
								else if((GetStringWidth(pSelectedTextbox->GetText()) < pSelectedTextbox->GetWidth()-30) && wParam != VK_BACK)
								{
									//initially we assign name to the model based on the models filename. Then in the level editor we can change it, 
									//but here limit the size so it doesnt exceed the textbox's dimensions

									if( m_pGameObjManager->GetPickedObject()->GetName().size() > 15 )
									{
										m_pGameObjManager->GetPickedObject()->GetName().erase();
									}

									string strNewText = pSelectedTextbox->GetText() + (TCHAR) wParam;

									pSelectedTextbox->SetText(strNewText);
								}

						}
						else if( pApp->m_strSelectedTextbox.empty() )
						{

						}
				}
				break;

			}
			}
			return 0;

		case WM_KEYDOWN:
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
				}
				break;

				case 'Y':
				{
					if( !typingMode )
					{
						Checkbox* pCheckbox = static_cast<Checkbox*>(checkbox_RotationY);

						if( pCheckbox->m_bIsChecked )
						{
							pCheckbox->m_bIsChecked = false;
						}
						else
						{
							pCheckbox->m_bIsChecked = true;
						}
					}
					
				}
				break;

				case 'X':
				{
					if( !typingMode )
					{
						Checkbox* pCheckbox = static_cast<Checkbox*>(checkbox_TranslationX);

						if( pCheckbox->m_bIsChecked )
						{
							pCheckbox->m_bIsChecked = false;
						}
						else
						{
							pCheckbox->m_bIsChecked = true;
						}
					}
					
				}
				break;

				case 'Z':
				{
					if( !typingMode )
					{
						Checkbox* pCheckbox = static_cast<Checkbox*>(checkbox_TranslationZ);

						if( pCheckbox->m_bIsChecked )
						{
							pCheckbox->m_bIsChecked = false;
						}
						else
						{
							pCheckbox->m_bIsChecked = true;
						}
					}
					
				}
				break;

				case 'C':
				{
					if( !typingMode )
					{
						Checkbox* pCheckbox = static_cast<Checkbox*>(checkbox_Scaling);

						if( pCheckbox->m_bIsChecked )
						{
							pCheckbox->m_bIsChecked = false;
						}
						else
						{
							pCheckbox->m_bIsChecked = true;
						}
					}
					
				}
				break;

				case 'E':
				{
					if( !typingMode )
					{
						ExportLevel();
					}
				}
				break;

				case 'T':
				{
					if( !typingMode )
					{
						ImportStaticModel();
					}
				}
				break;

				case 'N':
				{
					if( !typingMode )
					{
						LoadAnimatedModel();
					}
				}
				break;



			}
			return 0;

		case WM_LBUTTONDOWN:
			switch(wParam)
			{
				case MK_LBUTTON:
				{
					bool bShouldCheck3D = true;
					for(int i = 0; i < pApp->m_vUIObjects.size(); i++)
					{
						if( pApp->m_vUIObjects[i]->IsMouseOver() )
						{
							bShouldCheck3D = false;
						}

						pApp->m_vUIObjects[i]->OnClicked();
					}

					if( bShouldCheck3D )
					{
						m_pGameObjManager->OnUpdate();
					}
				}
			}
			return 0;

  }
  return DefWindowProc(pApp->GetMainWindow(), msg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Game::ImportStaticModel()
{
    static char filename[4096];

    OPENFILENAME ofn;
    memset( &ofn, 0, sizeof(ofn) );

    ofn.lStructSize       = sizeof(ofn);
	ofn.hwndOwner         = pApp->GetMainWindow();
    ofn.hInstance         = 0;
	ofn.lpstrFilter		  = "X Files (*.x)\0*.x\0All Files (*.*)\0*.*\0\0";
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0L;
    ofn.nFilterIndex      = 0L;
    ofn.lpstrFile         = filename;
    ofn.nMaxFile          = sizeof(filename);
    ofn.lpstrFileTitle    = 0;
    ofn.nMaxFileTitle     = 0;
	ofn.lpstrInitialDir   = 0;
    ofn.nFileOffset       = 0;
    ofn.nFileExtension    = 0;
    ofn.lpstrDefExt       = "*.x";
    ofn.lCustData         = 0;
	ofn.lpstrTitle		  = "Select a static .X file to open";
    ofn.Flags			  = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;

	if (GetOpenFileName(&ofn))
	{

		//when we load file windows uses different slashes, which we cant use later in the game, when we load the files for the models in the scripts
		//so we search the string for these slashes and we replace them with the ones we need.
		string str(filename);
		string str2("models");
		string slash = "\\";
		string slash2 = "\/";
		string relativeModelFile;
		size_t found;
		size_t found2;

		found=str.find(str2);
		if (found!=string::npos)
		{
			relativeModelFile = str.substr(found);
		}

		found2=relativeModelFile.find(slash);
		while( found2!=string::npos )
		{
			relativeModelFile.replace(found2,slash.length(),"\/");
			found2=relativeModelFile.find(slash,found2+1);
		}

		string strModelName = GetModelNameFromFilePath(relativeModelFile);
		relativeModelFile  = "../../Resources/"+relativeModelFile;

		GameObject* obj = new StaticModel(strModelName,relativeModelFile,"");

		obj->SetName(strModelName);
		textbox_modelName->SetText(strModelName);

		obj->LoadGameObject();

		m_pGameObjManager->AddGameObject(obj);

		m_pGameObjManager->SetPickedObject(obj);
	}

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Game::LoadAnimatedModel()
{
	static char filename[4096];

	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof(ofn) );

	ofn.lStructSize       = sizeof(ofn);
	ofn.hwndOwner         = pApp->GetMainWindow();
	ofn.hInstance         = 0;
	ofn.lpstrFilter		  = "X Files (*.x)\0*.x\0All Files (*.*)\0*.*\0\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter    = 0L;
	ofn.nFilterIndex      = 0L;
	ofn.lpstrFile         = filename;
	ofn.nMaxFile          = sizeof(filename);
	ofn.lpstrFileTitle    = 0;
	ofn.nMaxFileTitle     = 0;
	ofn.lpstrInitialDir   = 0;
	ofn.nFileOffset       = 0;
	ofn.nFileExtension    = 0;
	ofn.lpstrDefExt       = "*.x";
	ofn.lCustData         = 0;
	ofn.lpstrTitle		  = "Select an animated .X file to open";
	ofn.Flags			  = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;

	if (GetOpenFileName(&ofn))
	{
		string str(filename);
		string str2("models");
		string slash = "\\";
		string slash2 = "\/";
		string relativeModelFile;
		size_t found;
		size_t found2;
		int i = 1;

		//get the path of the models
		found=str.find(str2);
		if (found!=string::npos)
		{
			relativeModelFile = str.substr(found);
		}

		//remove the unneccessary second slash which is added for unknown reasons
		found2=relativeModelFile.find(slash);
		while( found2!=string::npos )
		{
			relativeModelFile.replace(found2,slash.length(),"\/");
			found2=relativeModelFile.find(slash,found2+1);

		}

		string strModelName = GetModelNameFromFilePath(relativeModelFile);
		relativeModelFile  = "../../Resources/"+relativeModelFile;

		GameObject* obj = new SkinnedModel(strModelName,relativeModelFile,"");

		textbox_modelName->SetText(strModelName);

		obj->LoadGameObject();

		m_pGameObjManager->AddGameObject(obj);

		if( m_pGameObjManager->GetPickedObject() != NULL )
		{
			m_pGameObjManager->GetPickedObject()->SetPicked(false);	
		}

		m_pGameObjManager->SetPickedObject(obj);
	}

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//this functions returns default model name, based on the name of the file
//for instance if the file path to the model is models/cho/cho.x, the model name will be cho
string Game::GetModelNameFromFilePath(string strFilePath)
{
	string strModelName = strFilePath;
	string strSlash2 = "\/";

	//get the name of the model, based on the file name
	size_t nFoundPos = strModelName.find(strSlash2);

	int nCurrentPos = 0;
	while( nFoundPos != string::npos )
	{
		strModelName.erase(0,nFoundPos+1);
		nFoundPos = strModelName.find(strSlash2);
	}

	//remove the extension from the file
	strModelName = strModelName.substr(0,strModelName.find("."));

	//it may cause problem if i delete objects. can be fixed later
	int nId = m_pGameObjManager->GetGameObjects().size()+1;

	string strCurrentAmountOfModels = to_string(nId);
	
	//add a number at the end of the model, so it can be unique
	strModelName+= strCurrentAmountOfModels;

	return strModelName;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Game::ExportLevel()
{
	ofstream staticModelsFile("export/staticModels.lua");
	ofstream animatedModelsFile("export/animatedModels.lua");
	ofstream mainHeroFile("export/init.lua");
	ofstream level("export/level.lua");

	//exports static models
	if( level.is_open() )
	{
		//export skinned models first, because of the binded objects
		for(auto& obj : m_pGameObjManager->GetSkinnedModels())
		{
			level << "x = " << obj->GetPosition().x << "\n";
			level << "y = " << obj->GetPosition().y << "\n";
			level << "z = " << obj->GetPosition().z << "\n";
			level << "scale = " << obj->GetScale() << "\n";
			level << "rotX = " << obj->GetRotationAngleByX() << "\n";
			level << "rotY = " << obj->GetRotationAngleByY() << "\n";
			level << "rotZ = " << obj->GetRotationAngleByZ() << "\n";
			level << "titleRotY = " << obj->GetTitleRotationAnglyByY() << "\n";
			level << "modelName = " << "\"" << obj->GetName() << "\"" << "\n";
			level << "titleForQuest = " << "\"" << obj->GetTitleForQuest() << "\"" << "\n";
			level << "modelFileName = " << "\"" << obj->GetModelFileName() << "\"" << "\n";
			level << "textureFileName = " << "\"" << obj->GetTextureFilename() << "\"" << "\n";
			level << "typeInGame = " << "\"" << obj->GetActorType() << "\"" << "\n";

			level << "addAnimatedModel();" << "\n" << "\n";
		}

		for (auto& obj : m_pGameObjManager->GetGameObjects())
		{
			if( obj->GetObjectType() == EGameObjectType_Static )
			{
				level << "x = " << obj->GetPosition().x << "\n";
				level << "y = " << obj->GetPosition().y << "\n";
				level << "z = " << obj->GetPosition().z << "\n";
				level << "scale = " << obj->GetScale() << "\n";
				level << "rotX = " << obj->GetRotationAngleByX() << "\n";
				level << "rotY = " << obj->GetRotationAngleByY() << "\n";
				level << "rotZ = " << obj->GetRotationAngleByZ() << "\n";
				level<<"bindable = "<<obj->IsBindable()<<"\n";
				level<<"bindToAnModel = "<<"\""<<obj->GetBindedToAnimatedModelName()<<"\""<<"\n";
				level<<"bindToAnModelBone = "<<"\""<<obj->GetBindedToBoneName()<<"\""<<"\n";
				level << "modelName = " << "\"" << obj->GetName() << "\"" << "\n";
				level << "modelFileName = " << "\"" << obj->GetModelFileName() << "\"" << "\n";
				level<<"addStaticModel();"<<"\n"<<"\n";
			}
		}

		//TODO : why are we iterating the skinned models again for the main hero??
		for (auto& obj : m_pGameObjManager->GetSkinnedModels())
		{
			if(	obj->GetActorType() == "mainHero" )
			{
				level<<"mainHero = "<<"\""<<obj->GetName() <<"\""<<"\n";
				level<<"setUpMainHero();"<<"\n"<<"\n";
			}
		}

		level.close();
	}
	else cout << "Unable to open file";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Game::ImportLevel()
{
	static char filename[4096];

    OPENFILENAME ofn;
    memset( &ofn, 0, sizeof(ofn) );

    ofn.lStructSize       = sizeof(ofn);
	ofn.hwndOwner         = pApp->GetMainWindow();
    ofn.hInstance         = 0;
	ofn.lpstrFilter		  = "Lua Files (*.lua)\0*.lua\0All Files (*.*)\0*.*\0\0";
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0L;
    ofn.nFilterIndex      = 0L;
    ofn.lpstrFile         = filename;
    ofn.nMaxFile          = sizeof(filename);
    ofn.lpstrFileTitle    = 0;
    ofn.nMaxFileTitle     = 0;
	ofn.lpstrInitialDir   = 0;
    ofn.nFileOffset       = 0;
    ofn.nFileExtension    = 0;
    ofn.lpstrDefExt       = "*.lua";
    ofn.lCustData         = 0;
	ofn.lpstrTitle		  = "Select a lua file containing the level information";
    ofn.Flags			  = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;

	if (GetOpenFileName(&ofn))
	{
		luaL_dofile(L, filename);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
