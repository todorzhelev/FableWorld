#include"../../CORE/CORE/Application.h"
#include"Game.h"
#include"Menu.h"
#include"MenuInGame.h"

//////////////////////////////////////////////////////////////////////////////

//entry point for the game
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,PSTR cmdLine, int showCmd)
{
	AllocConsole();
	//must research this
	freopen("CONOUT$", "wb", stdout);

	fout.open("logs/log.txt");

	pApp = new Application(hInstance, "Fable World", D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING);
	
	m_pGameObjManager = new GameObjectManager(false,true,true,false);

	//adds 3 scenes in the scenesContainer and set the active scene to be the menu scene.
	Menu menu;
	pApp->AddScene("menu", &menu);

	Game game;
	pApp->AddScene("game", &game);

	MenuInGame menuInGame;
	pApp->AddScene("menuInGame", &menuInGame);

	pApp->SetCurrentScene(&menu);

	pApp->MainLoop();
}

//////////////////////////////////////////////////////////////////////////////
