#include"../../../TheCore/TheCore/TheCore/GameEngine.h"
#include"Game.h"
#include"Menu.h"
#include"MenuInGame.h"
#include"HeroSelection.h"

//////////////////////////////////////////////////////////////////////////////

//entry point for the game
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,PSTR cmdLine, int showCmd)
{
	AllocConsole();
	//must research this
	freopen( "CONOUT$", "wb", stdout);

	fout.open("logs/log.txt");

	pEngine = new GameEngine(hInstance, "Fable World", D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING);
	
	m_pGameObjManager = new GameObjectManager(true,false);

	//adds 3 scenes in the scenesContainer and set the active scene to be the menu scene.
	Menu menu;
	pEngine->AddScene("menu",&menu);

	Game game;
	pEngine->AddScene("game",&game);

	MenuInGame menuInGame;
	pEngine->AddScene("menuInGame",&menuInGame);

	HeroSelection heroSelection;
	pEngine->AddScene("heroSelect",&heroSelection);

	pEngine->SetCurrentScene(&menu);

	pEngine->MainLoop();
}

//////////////////////////////////////////////////////////////////////////////
