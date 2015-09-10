#include"Game.h"
#include<math.h>

ofstream fout; //for logs.
lua_State* L;


/*
Function:Game
Purpose:constructor. It loads all the functionality in the game
*/
Game::Game()
{
	//checks if pixel and vertex shader 2.0 is supported
	if(!checkDeviceCaps())
	{
		MessageBox(0, "checkDeviceCaps() Failed", 0, 0);
		PostQuitMessage(0);
	}

	float fWidth  = (float)pEngine->GetPresentParameters().BackBufferWidth;
	float fHeight = (float)pEngine->GetPresentParameters().BackBufferHeight;

	//init and setup camera
	camera = new Camera(D3DX_PI * 0.25f, fWidth/fHeight, 1.0f, 2000.0f,true);
	camera->SetCameraMode(ECameraMode_MoveWithPressedMouse);

	D3DXVECTOR3 vCameraPosition = D3DXVECTOR3(0,100,0);
	camera->SetPosition(vCameraPosition);

	//Initialize the vertex declarations. They are needed for creating the terrain, models and etc.
	InitVertexDeclarations();

	// user isnt typing anything in textbox yet
	typingMode = false;


	//init sky
	sky = new Sky("../../Resources/textures/grassenvmap1024.dds", 10000.0f);

	//init terrain
	pTerrain = new Terrain("../../Resources/heightmaps/heightmap_full copy.raw",0.85f,513,513,1.0f,1.0f,D3DXVECTOR3(0.0f,0.0f,0.0f));;
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


void Game::LoadUIComponents()
{
	//TODO: this must be moved to some script
	float width  = (float)pEngine->GetPresentParameters().BackBufferWidth;
	float height = (float)pEngine->GetPresentParameters().BackBufferHeight;

	
	//load static model button
	button_LoadStaticModel = new Button(D3DXVECTOR2(width - 630, 15.0),128,32,"","Button_LSM_final1.dds","Button_LSM_final2_hover.dds");
	pEngine->AddUIObject(button_LoadStaticModel);
	
	//load animated model button
	button_LoadAnimatedModel = new Button(D3DXVECTOR2(width - 490, 15.0),128,32,"","Button_LAM_final1.dds","Button_LAM_final2_hover.dds");
	pEngine->AddUIObject(button_LoadAnimatedModel);

	//import button
	button_Import = new Button(D3DXVECTOR2(width - 280, 15.0),128,32,"","Button_Import.dds","Button_Import_hover.dds");
	pEngine->AddUIObject(button_Import);

	//export button
	button_Export = new Button(D3DXVECTOR2(width - 140, 15.0),128,32,"","Button_Export.dds","Button_Export_hover.dds");
	pEngine->AddUIObject(button_Export);

	//translation label
	button_Translation = new Button(D3DXVECTOR2(6.0, 13.0),128,32,"","Button_Translation.dds","Button_Translation.dds");
	pEngine->AddUIObject(button_Translation);

	//translation x checkbox
	checkbox_TranslationX = new Checkbox(D3DXVECTOR2(20.0, 45.0),32,32,"x","Checkbox_checked.dds","Checkbox_unchecked.dds","checkbox_TranslationX");
	pEngine->AddUIObject(checkbox_TranslationX);

	//translation y checkbox
	checkbox_TranslationY = new Checkbox(D3DXVECTOR2(70.0, 45.0),32,32,"y","Checkbox_checked.dds","Checkbox_unchecked.dds","checkbox_TranslationY");
	pEngine->AddUIObject(checkbox_TranslationY);

	//translation z checkbox
	checkbox_TranslationZ = new Checkbox(D3DXVECTOR2(120.0, 45.0),32,32,"z","Checkbox_checked.dds","Checkbox_unchecked.dds","checkbox_TranslationZ");;
	pEngine->AddUIObject(checkbox_TranslationZ);

	//export button
	button_Rotation = new Button(D3DXVECTOR2(-10.0, 68.0),128,32,"","Button_Rotation.dds","Button_Rotation.dds");
	pEngine->AddUIObject(button_Rotation);

	//rotation x checkbox 
	checkbox_RotationX = new Checkbox(D3DXVECTOR2(20.0, 100.0),32,32,"x","Checkbox_checked.dds","Checkbox_unchecked.dds","checkbox_RotationX");;
	pEngine->AddUIObject(checkbox_RotationX);

	//rotation y checkbox
	checkbox_RotationY = new Checkbox(D3DXVECTOR2(70.0, 100.0),32,32,"y","Checkbox_checked.dds","Checkbox_unchecked.dds","checkbox_RotationY");;
	pEngine->AddUIObject(checkbox_RotationY);

	//rotation z checkbox 
	checkbox_RotationZ = new Checkbox(D3DXVECTOR2(120.0, 100.0),32,32,"z","Checkbox_checked.dds","Checkbox_unchecked.dds","checkbox_RotationZ");;
	pEngine->AddUIObject(checkbox_RotationZ);

	//export button
	button_Scaling = new Button(D3DXVECTOR2(-13.0, 122.0),128,32,"","Button_Scaling.dds","Button_Scaling.dds");
	pEngine->AddUIObject(button_Scaling);

	//scaling checkbox
	checkbox_Scaling = new Checkbox(D3DXVECTOR2(20.0, 155.0),32,32,"scale","Checkbox_checked.dds","Checkbox_unchecked.dds","checkbox_Scaling");;
	pEngine->AddUIObject(checkbox_Scaling);

	//model name label
	label_modelName = new Label(D3DXVECTOR2(width-300,150),"model name");
	label_modelName->SetVisible(false);
	pEngine->AddUIObject(label_modelName);


	//model name textbox
	textbox_modelName = new Textbox(D3DXVECTOR2(width-300, 170.0f),256,32,"","Textbox.dds","Textbox_selected.dds","textbox_modelName");
	textbox_modelName->SetVisible(false);
	textbox_modelName->m_bIsSelected = false;
	pEngine->AddUIObject(textbox_modelName);
	
	//type in game label
	label_typeInGame = new Label(D3DXVECTOR2(width-300,220),"type in game");
	label_typeInGame->SetVisible(false);
	pEngine->AddUIObject(label_typeInGame);

	//type in game textbox
	textbox_typeInGame = new Textbox(D3DXVECTOR2(width-300, 240.0f),256,32,"","Textbox.dds","Textbox_selected.dds","textbox_typeInGame");
	textbox_typeInGame->SetVisible(false);
	textbox_typeInGame->m_bIsSelected = false;
	pEngine->AddUIObject(textbox_typeInGame);

	//title for quest label
	label_titleForQuest = new Label(D3DXVECTOR2(width-300,290),"title for quest");
	label_titleForQuest->SetVisible(false);
	pEngine->AddUIObject(label_titleForQuest);

	//title for quest textbox
	textbox_titleForQuest = new Textbox(D3DXVECTOR2(width-300, 310.0f),256,32,"","Textbox.dds","Textbox_selected.dds","textbox_titleForQuest");
	textbox_titleForQuest->SetVisible(false);
	textbox_titleForQuest->m_bIsSelected = false;
	pEngine->AddUIObject(textbox_titleForQuest);

	//title for quest label
	label_bindToAnModel = new Label(D3DXVECTOR2(width-300,220),"bind to animated model");
	label_bindToAnModel->SetVisible(false);
	pEngine->AddUIObject(label_bindToAnModel);

	//bind to animated model textbox
	textbox_bindToAnModel = new Textbox(D3DXVECTOR2(width-300, 240),256,32,"","Textbox.dds","Textbox_selected.dds","textbox_bindToAnModel");
	textbox_bindToAnModel->SetVisible(false);
	textbox_bindToAnModel->m_bIsSelected = false;
	pEngine->AddUIObject(textbox_bindToAnModel);

	//title for quest label
	label_bindToAnModelBone = new Label(D3DXVECTOR2(width-300,290),"bind to animated model bone");
	label_bindToAnModelBone->SetVisible(false);
	pEngine->AddUIObject(label_bindToAnModelBone);

	//bind to animated model's bone textbox
	textbox_bindToAnModelBone = new Textbox(D3DXVECTOR2(width-300, 310),256,32,"","Textbox.dds","Textbox_selected.dds","textbox_bindToAnModelBone");
	textbox_bindToAnModelBone->SetVisible(false);
	textbox_bindToAnModelBone->m_bIsSelected = false;
	pEngine->AddUIObject(textbox_bindToAnModelBone);
}


/*
Function:~Game
Purpose:destructor
*/
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

/*
Function:checkDeviceCaps
Purpose:checks for shader support
*/
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

/*
Function:onLostDevice
Purpose:invokes the corresponding OnLostDevice functions
*/
void Game::OnLostDevice()
{
	
	sky->OnLostDevice();

	for(vector<IBaseMenuObject*>::iterator it = pEngine->m_vUIObjects.begin();it!=pEngine->m_vUIObjects.end();it++)
	{
		(*it)->OnLostDevice();
	}

	pTextManager->OnLostDevice();
	pTerrain->OnLostDevice();

	for(map<string,GameObject*>::iterator it = m_pGameObjManager->GetGameObjects().begin();it!=m_pGameObjManager->GetGameObjects().end();it++)
	{
		(*it).second->OnLostDevice();
	}

	//spriteForInterface->OnLostDevice();
}

/*
Function:onResetDevice
Purpose:invokes the corresponding onResetDevice functions
*/
void Game::OnResetDevice()
{
	sky->OnResetDevice();

	for(vector<IBaseMenuObject*>::iterator it = pEngine->m_vUIObjects.begin();it!=pEngine->m_vUIObjects.end();it++)
	{
		(*it)->OnResetDevice();
	}

	pTextManager->OnResetDevice();
	pTerrain->OnResetDevice();

	for(map<string,GameObject*>::iterator it = m_pGameObjManager->GetGameObjects().begin();it!=m_pGameObjManager->GetGameObjects().end();it++)
	{
		(*it).second->OnResetDevice();
	}
}

/*
Function ate
Purpose:updates all the functionallity in game
*/
void Game::OnUpdate(float dt)
{
	//poll starts to listen if any key on the keyboard is pressed
	pDinput->Poll();

		//if escape is pressed in game it switches to another scene
	if(pDinput->IsKeyDown(DIK_ESCAPE))
	{
		IBaseScene* pMenuInGameScene = pEngine->GetScene("menuInGame");
		pEngine->SetCurrentScene(pMenuInGameScene);
	}

	//update all the game objects
	for(map<string,GameObject*>::iterator it = m_pGameObjManager->GetGameObjects().begin();it!=m_pGameObjManager->GetGameObjects().end();it++)
	{
		(*it).second->OnUpdate(dt);
	}

	//update all the UI objects
	for(vector<IBaseMenuObject*>::iterator it = pEngine->m_vUIObjects.begin();it!=pEngine->m_vUIObjects.end();it++)
	{
		(*it)->OnUpdate();
	}

	pTextManager->OnUpdate(dt);

	//we store the currently selected textbox in string. If its empty nothing is selected
	if( pEngine->m_strSelectedTextbox == "" )
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
	for(auto it=m_pGameObjManager->GetGameObjects().begin();it!=m_pGameObjManager->GetGameObjects().end();it++)
	{
		if( (*it).second->m_eGameObjectType == EGameObjectType_Skinned )
		{		
			//it it is picked reveals animated model's related textboxes and labels
			if( (*it).second->m_bIsPicked && !label_typeInGame->IsVisible())
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
			if( m_pGameObjManager->GetPickedObject() == NULL && (*it).second->m_bIsPicked || 
				m_pGameObjManager->GetPickedObject() != NULL && 
				(*it).second->m_bIsPicked && 
				m_pGameObjManager->GetPickedObject()->m_strModelName.compare((*it).second->m_strModelName) )
			{
				textbox_typeInGame->SetText((*it).second->m_strActorType);
				textbox_modelName->SetText((*it).second->m_strModelName);
				textbox_titleForQuest->SetText((*it).second->m_strTitleForQuest);
				
				if( m_pGameObjManager->GetPickedObject() != NULL &&  
					m_pGameObjManager->GetPickedObject()->m_strModelName.compare((*it).second->m_strModelName) )
				{
					m_pGameObjManager->GetPickedObject()->m_bIsPicked = false;
				}

				m_pGameObjManager->SetPickedObject((*it).second);
			}

			pickedModelControl(*(*it).second, dt);
		}
		else if( (*it).second->m_eGameObjectType == EGameObjectType_Static )
		{
			if( (*it).second->m_bIsPicked && !label_bindToAnModel->IsVisible() )
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
			if( m_pGameObjManager->GetPickedObject() == NULL && (*it).second->m_bIsPicked || 
				m_pGameObjManager->GetPickedObject() != NULL && 
				(*it).second->m_bIsPicked && 
				m_pGameObjManager->GetPickedObject()->m_strModelName.compare((*it).second->m_strModelName) )
			{
				textbox_typeInGame->SetText((*it).second->m_strActorType);
				textbox_modelName->SetText((*it).second->m_strModelName);
				textbox_titleForQuest->SetText((*it).second->m_strTitleForQuest);
				
				if( m_pGameObjManager->GetPickedObject() != NULL &&  
					m_pGameObjManager->GetPickedObject()->m_strModelName.compare((*it).second->m_strModelName) )
				{
					m_pGameObjManager->GetPickedObject()->m_bIsPicked = false;
				}

				m_pGameObjManager->SetPickedObject((*it).second);
			}

			pickedModelControl(*(*it).second, dt);
		}
	}

	//unpick model that its picked with K
	//TODO: two panels should be made, which will be used to hide/show all of their children elements
	if( pDinput->IsKeyDown(DIK_K) )
	{
		if( m_pGameObjManager->GetPickedObject() != nullptr )
		{	
			m_pGameObjManager->GetGameObjects().erase(m_pGameObjManager->GetPickedObject()->m_strModelName);

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

void Game::pickedModelControl(GameObject& obj, float dt)
{
	if( obj.m_bIsPicked && static_cast<Checkbox*>(checkbox_TranslationX)->m_bIsChecked )
		{
			if( pDinput->IsMouseButtonDown(0) )
			{
				obj.m_vPos += ( obj.m_vLook *40.0*dt);
			}
			else if( pDinput->IsMouseButtonDown(1) )
			{
				obj.m_vPos -= ( obj.m_vLook *40.0*dt);
			}
		}

		if( obj.m_bIsPicked && static_cast<Checkbox*>(checkbox_TranslationY)->m_bIsChecked )
		{
			if( pDinput->IsMouseButtonDown(0) )
			{
				obj.m_vPos += ( obj.m_vUp*40.0*dt);
			}
			else if( pDinput->IsMouseButtonDown(1) )
			{
				obj.m_vPos -= ( obj.m_vUp*40.0*dt);
			}
		}

		

		if( obj.m_bIsPicked && static_cast<Checkbox*>(checkbox_TranslationZ)->m_bIsChecked  )
		{
			if( pDinput->IsMouseButtonDown(0) )
			{
				obj.m_vPos += ( obj.m_vRight *40.0*dt);
			}
			else if( pDinput->IsMouseButtonDown(1) )
			{
				obj.m_vPos -= ( obj.m_vRight *40.0*dt);
			}
		}

		if( obj.m_bIsPicked && static_cast<Checkbox*>(checkbox_RotationY)->m_bIsChecked )
		{
			if( pDinput->IsMouseButtonDown(0) )
			{
				float yAngle = 0.1;
				obj.m_fRotAngleY +=yAngle;

				D3DXMATRIX R;
				D3DXMatrixRotationY(&R, yAngle);
				D3DXVec3TransformCoord(&obj.m_vRight, &obj.m_vRight, &R);
				D3DXVec3TransformCoord(&obj.m_vUp, &obj.m_vUp, &R);
				D3DXVec3TransformCoord(&obj.m_vLook, &obj.m_vLook, &R);
			}
			else if( pDinput->IsMouseButtonDown(1) )
			{
				float yAngle = 0.1;
				obj.m_fRotAngleY -=yAngle;

				D3DXMATRIX R;
				//it is -yAngle, because we rotate on the other side
				D3DXMatrixRotationY(&R, -yAngle);
				D3DXVec3TransformCoord(&obj.m_vRight, &obj.m_vRight, &R);
				D3DXVec3TransformCoord(&obj.m_vUp, &obj.m_vUp, &R);
				D3DXVec3TransformCoord(&obj.m_vLook, &obj.m_vLook, &R);
			}
		}

		if( obj.m_bIsPicked && static_cast<Checkbox*>(checkbox_RotationX)->m_bIsChecked )
		{
			if( pDinput->IsMouseButtonDown(0) )
			{
				float yAngle = 0.1;
				obj.m_fRotAngleX +=yAngle;

				D3DXMATRIX R;
				D3DXMatrixRotationX(&R, yAngle);
				D3DXVec3TransformCoord(&obj.m_vRight, &obj.m_vRight, &R);
				D3DXVec3TransformCoord(&obj.m_vUp, &obj.m_vUp, &R);
				D3DXVec3TransformCoord(&obj.m_vLook, &obj.m_vLook, &R);
			}
			else if( pDinput->IsMouseButtonDown(1) )
			{
				float yAngle = 0.1;
				obj.m_fRotAngleX -=yAngle;

				D3DXMATRIX R;
				D3DXMatrixRotationX(&R, -yAngle);
				D3DXVec3TransformCoord(&obj.m_vRight, &obj.m_vRight, &R);
				D3DXVec3TransformCoord(&obj.m_vUp, &obj.m_vUp, &R);
				D3DXVec3TransformCoord(&obj.m_vLook, &obj.m_vLook, &R);
			}
		}

		if( obj.m_bIsPicked && static_cast<Checkbox*>(checkbox_RotationZ)->m_bIsChecked )
		{
			if( pDinput->IsMouseButtonDown(0) )
			{
				float yAngle = 0.1;
				obj.m_fRotAngleZ +=yAngle;

				D3DXMATRIX R;
				D3DXMatrixRotationZ(&R, yAngle);
				D3DXVec3TransformCoord(&obj.m_vRight, &obj.m_vRight, &R);
				D3DXVec3TransformCoord(&obj.m_vUp, &obj.m_vUp, &R);
				D3DXVec3TransformCoord(&obj.m_vLook, &obj.m_vLook, &R);
			}
			else if( pDinput->IsMouseButtonDown(1) )
			{
				float yAngle = 0.1;
				obj.m_fRotAngleZ -=yAngle;

				D3DXMATRIX R;
				D3DXMatrixRotationZ(&R, -yAngle);
				D3DXVec3TransformCoord(&obj.m_vRight, &obj.m_vRight, &R);
				D3DXVec3TransformCoord(&obj.m_vUp, &obj.m_vUp, &R);
				D3DXVec3TransformCoord(&obj.m_vLook, &obj.m_vLook, &R);
			}
		}

		

		if( obj.m_bIsPicked && static_cast<Checkbox*>(checkbox_Scaling)->m_bIsChecked )
		{
			if( pDinput->IsMouseButtonDown(0) )
			{
				obj.m_fScale +=0.01;
			}
			else if( pDinput->IsMouseButtonDown(1) )
			{
				obj.m_fScale -= 0.01;
			}
		}
}
/*
Function:onRender
Purpose:render all
*/
void Game::OnRender()
{

    pDxDevice->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);

    pDxDevice->BeginScene();
	
			sky->OnRender();

			pTerrain->OnRender();

	
			//render all the game objects
			for(map<string,GameObject*>::iterator it = m_pGameObjManager->GetGameObjects().begin();it!=m_pGameObjManager->GetGameObjects().end();it++)
			{
				(*it).second->OnRender();
			}
					
			for(vector<IBaseMenuObject*>::iterator it = pEngine->m_vUIObjects.begin();it!=pEngine->m_vUIObjects.end();it++)
			{
				(*it)->OnRender(255,255,255,255);
			}
			

 
	pDxDevice->EndScene();

	pDxDevice->Present(0, 0, 0, 0);
}
	
/*
Function:isNear
Purpose:checks if two models are close
*/
bool Game::isNear(GameObject& obj1,GameObject& obj2)
{
	if((obj1.m_vPos.x > obj2.m_vPos.x-30) && (obj1.m_vPos.x < obj2.m_vPos.x+30) &&
	   (obj1.m_vPos.z > obj2.m_vPos.z-30) && (obj1.m_vPos.z < obj2.m_vPos.z+30))
	{
	   return true;
	}
	else 
	{
		return false;
	}

	return true;
}

/*
Function:msgProc
Purpose:this function detects various messages sent to the window like WM_CLOSE, WM_ACTIVATE and etc.
		Used mainly for the textbox and camera mode switching
*/
LRESULT Game::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	IBaseMenuObject* pSelectedTextbox = pEngine->FindMenuObject(pEngine->m_strSelectedTextbox);

	switch( msg )
	{
		//Sent when the window is activated or deactivated.(pressed alt+tab)
		//Game is paused when the window is inactive, and unpaused when become active again
		case WM_ACTIVATE:
			if( LOWORD(wParam) == WA_INACTIVE )
			{
				pEngine->SetPaused(true);
			}
			else
			{
				pEngine->SetPaused(false);
			}
			return 0;


		//Sent when the user closes the window
		case WM_CLOSE:
			DestroyWindow( pEngine->GetMainWindow() );
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
				case 0x0D:
				{
					//IBaseMenuObject* pSelectedTextbox = pEngine->FindMenuObject(selectedTextbox);


					if( !pEngine->m_strSelectedTextbox.empty() )
					{
					    if( pEngine->m_strSelectedTextbox == "textbox_typeInGame" )
						{
							//IBaseMenuObject* pSelectedTextbox = pEngine->FindMenuObject(selectedTextbox);
							m_pGameObjManager->GetPickedObject()->m_strActorType = pSelectedTextbox->GetText();
							pSelectedTextbox->m_bIsSelected = false;
							pEngine->m_strSelectedTextbox = "";
						}
						else if( pEngine->m_strSelectedTextbox == "textbox_modelName")
						{
							// when we change the model name we must update the map with the new name
							// therefore we delete the old entry in the map and add new object with the updated name
							if( m_pGameObjManager->GetPickedObject()->m_eGameObjectType == EGameObjectType_Skinned )
							{
								GameObject *obj;
								obj = m_pGameObjManager->GetGameObjects().find(m_pGameObjManager->GetPickedObject()->m_strModelName)->second;
								obj = static_cast<SkinnedMesh*>(obj);

								m_pGameObjManager->GetGameObjects().erase(m_pGameObjManager->GetPickedObject()->m_strModelName);

								m_pGameObjManager->GetGameObjects()[pSelectedTextbox->GetText()] = obj;
								m_pGameObjManager->GetPickedObject()->m_strModelName = pSelectedTextbox->GetText();

								pSelectedTextbox->m_bIsSelected = false;

								pEngine->m_strSelectedTextbox = "";
							}
						}
						else if( pEngine->m_strSelectedTextbox == "textbox_titleForQuest" && 
								 m_pGameObjManager->GetPickedObject()->m_eGameObjectType == EGameObjectType_Skinned )
						{
							m_pGameObjManager->GetPickedObject()->m_strTitleForQuest= pSelectedTextbox->GetText();
							pSelectedTextbox->m_bIsSelected = false;
							pEngine->m_strSelectedTextbox = "";
						}
						else if( pEngine->m_strSelectedTextbox == "textbox_bindToAnModel" && 
								 m_pGameObjManager->GetPickedObject()->m_eGameObjectType == EGameObjectType_Static )
						{
							m_pGameObjManager->GetPickedObject()->m_strBindedToAnimatedModelName = pSelectedTextbox->GetText();

							if( !m_pGameObjManager->GetPickedObject()->m_strBindedToBoneName.empty() )
							{
								SkinnedMesh* obj;
								obj = static_cast<SkinnedMesh*>(m_pGameObjManager->GetGameObjects().find(m_pGameObjManager->GetPickedObject()->m_strBindedToAnimatedModelName)->second);

								obj->BindWeaponToModel(m_pGameObjManager->GetPickedObject()->m_strModelName,m_pGameObjManager->GetPickedObject()->m_strBindedToBoneName);

								m_pGameObjManager->GetPickedObject()->m_bIsBindable = true;
							}

							pSelectedTextbox->m_bIsSelected = false;
							pEngine->m_strSelectedTextbox = "";
						}
						else if( pEngine->m_strSelectedTextbox == "textbox_bindToAnModelBone" && 
								 m_pGameObjManager->GetPickedObject()->m_eGameObjectType == EGameObjectType_Static  )
						{
							m_pGameObjManager->GetPickedObject()->m_strBindedToBoneName = pSelectedTextbox->GetText();

							if( !m_pGameObjManager->GetPickedObject()->m_strBindedToAnimatedModelName.empty() )
							{
								SkinnedMesh* obj;
								obj = static_cast<SkinnedMesh*>(m_pGameObjManager->GetGameObjects().find(m_pGameObjManager->GetPickedObject()->m_strBindedToAnimatedModelName)->second);

								obj->BindWeaponToModel(m_pGameObjManager->GetPickedObject()->m_strModelName,m_pGameObjManager->GetPickedObject()->m_strBindedToBoneName);

								m_pGameObjManager->GetPickedObject()->m_bIsBindable = true;
								
							}
							pSelectedTextbox->m_bIsSelected = false;
							pEngine->m_strSelectedTextbox = "";
						}						
					}
				}
				break;

				//here we detect any key pressed and adds it to the selected textbox
				default:
				{
					if( !pEngine->m_strSelectedTextbox.empty() )
					{
						
						//if model name textbox is selected
						if( pEngine->m_strSelectedTextbox == "textbox_modelName" ||
							pEngine->m_strSelectedTextbox == "textbox_typeInGame" ||
							pEngine->m_strSelectedTextbox == "textbox_titleForQuest" ||
							pEngine->m_strSelectedTextbox == "textbox_bindToAnModel" ||
							pEngine->m_strSelectedTextbox == "textbox_bindToAnModelBone" )
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

									if( m_pGameObjManager->GetPickedObject()->m_strModelName.size() > 15 )
									{
										m_pGameObjManager->GetPickedObject()->m_strModelName.erase();
									}

									string strNewText = pSelectedTextbox->GetText() + (TCHAR) wParam;

									pSelectedTextbox->SetText(strNewText);
								}

						}
						else if( pEngine->m_strSelectedTextbox.empty() )
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
					if( camera->GetCameraMode() == ECameraMode_MoveWithoutPressedMouse )
					{
						camera->SetCameraMode(ECameraMode_MoveWithPressedMouse);
					}
					else if( camera->GetCameraMode() == ECameraMode_MoveWithPressedMouse )
					{
						camera->SetCameraMode(ECameraMode_MoveWithoutPressedMouse);
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
					for(int i = 0; i < pEngine->m_vUIObjects.size(); i++)
					{
						if( pEngine->m_vUIObjects[i]->IsMouseOver() )
						{
							bShouldCheck3D = false;
						}

						pEngine->m_vUIObjects[i]->OnClicked();
					}

					if( bShouldCheck3D )
					{
						m_pGameObjManager->OnUpdate();
					}
				}
			}
			return 0;

  }
  return DefWindowProc(pEngine->GetMainWindow(), msg, wParam, lParam);
}



void Game::ImportStaticModel()
{
    static char filename[4096];

    OPENFILENAME ofn;
    memset( &ofn, 0, sizeof(ofn) );

    ofn.lStructSize       = sizeof(ofn);
	ofn.hwndOwner         = pEngine->GetMainWindow();
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

		GameObject* obj = new StaticMesh(strModelName,relativeModelFile,"");

		obj->m_strModelName = strModelName;
		textbox_modelName->SetText(strModelName);

		obj->LoadGameObject();

		m_pGameObjManager->AddGameObject(obj);

		m_pGameObjManager->SetPickedObject(obj);
	}

}

void Game::LoadAnimatedModel()
{
	static char filename[4096];

	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof(ofn) );

	ofn.lStructSize       = sizeof(ofn);
	ofn.hwndOwner         = pEngine->GetMainWindow();
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

		GameObject* obj = new SkinnedMesh(strModelName,relativeModelFile,"");

		textbox_modelName->SetText(strModelName);

		obj->LoadGameObject();

		m_pGameObjManager->AddGameObject(obj);

		if( m_pGameObjManager->GetPickedObject() != NULL )
		{
			m_pGameObjManager->GetPickedObject()->m_bIsPicked = false;	
		}

		m_pGameObjManager->SetPickedObject(obj);
	}

}

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
		  for(auto it=m_pGameObjManager->GetGameObjects().begin();it!=m_pGameObjManager->GetGameObjects().end();it++)
		  {
			  if( (*it).second->m_eGameObjectType == EGameObjectType_Skinned )
			  {	
				  level<<"x = "<< (*it).second->m_vPos.x<<"\n";
				  level<<"y = "<<(*it).second->m_vPos.y<<"\n";
				  level<<"z = "<<(*it).second->m_vPos.z<<"\n";
				  level<<"scale = "<<(*it).second->m_fScale<<"\n";
				  level<<"rotX = "<<(*it).second->m_fRotAngleX<<"\n";
				  level<<"rotY = "<<(*it).second->m_fRotAngleY<<"\n";
				  level<<"rotZ = "<<(*it).second->m_fRotAngleZ<<"\n";
				  level<<"titleRotY = "<<(*it).second->m_fTitleRotationAngleByY<<"\n";
				  level<<"modelName = "<<"\""<<(*it).second->m_strModelName<<"\""<<"\n";
				  level<<"titleForQuest = "<<"\""<<(*it).second->m_strTitleForQuest<<"\""<<"\n";
				  level<<"modelFileName = "<<"\""<<(*it).second->m_strModelFileName<<"\""<<"\n";
				  level<<"textureFileName = "<<"\""<<(*it).second->m_strTextureFileName<<"\""<<"\n";
				  level<<"typeInGame = "<<"\""<<(*it).second->m_strActorType<<"\""<<"\n";

				  level<<"addAnimatedModel();"<<"\n"<<"\n";
			  }
		  }

		  for(auto it=m_pGameObjManager->GetGameObjects().begin();it!=m_pGameObjManager->GetGameObjects().end();it++)
		  {
			  if( (*it).second->m_eGameObjectType == EGameObjectType_Static )
			  {
				  level<<"x = "<<(*it).second->m_vPos.x<<"\n";
				  level<<"y = "<<(*it).second->m_vPos.y<<"\n";
				  level<<"z = "<<(*it).second->m_vPos.z<<"\n";
				  level<<"scale = "<<(*it).second->m_fScale<<"\n";
				  level<<"rotX = "<<(*it).second->m_fRotAngleX<<"\n";
				  level<<"rotY = "<<(*it).second->m_fRotAngleY<<"\n";
				  level<<"rotZ = "<<(*it).second->m_fRotAngleZ<<"\n";
				  level<<"titleRotY = "<<(*it).second->m_fTitleRotationAngleByY<<"\n";
				  level<<"bindable = "<<(*it).second->m_bIsBindable<<"\n";
				  level<<"bindToAnModel = "<<"\""<<(*it).second->m_strBindedToAnimatedModelName<<"\""<<"\n";
				  level<<"bindToAnModelBone = "<<"\""<<(*it).second->m_strBindedToBoneName<<"\""<<"\n";
				  level<<"modelName = "<<"\""<<(*it).second->m_strModelName<<"\""<<"\n";
				  level<<"modelFileName = "<<"\""<<(*it).second->m_strModelFileName<<"\""<<"\n";
				  level<<"addStaticModel();"<<"\n"<<"\n";
			  }
		  }

		  for(auto it=m_pGameObjManager->GetGameObjects().begin();it!=m_pGameObjManager->GetGameObjects().end();it++)
		  {
			  if( (*it).second->m_eGameObjectType == EGameObjectType_Skinned )
			  {	
				  if( (*it).second->m_strActorType == "mainHero" )
				  {
					  level<<"mainHero = "<<"\""<<(*it).second->m_strModelName<<"\""<<"\n";
					  level<<"setUpMainHero();"<<"\n"<<"\n";
				  }
			  }
		  }

		  level.close();
	  }
	  else cout << "Unable to open file";
}

void Game::ImportLevel()
{
	static char filename[4096];

    OPENFILENAME ofn;
    memset( &ofn, 0, sizeof(ofn) );

    ofn.lStructSize       = sizeof(ofn);
	ofn.hwndOwner         = pEngine->GetMainWindow();
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
