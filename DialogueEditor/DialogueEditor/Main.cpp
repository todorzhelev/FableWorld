#include"Application.h"
#include"Game.h"

//////////////////////////////////////////////////////////////////////////////

//entry point for the game
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,PSTR cmdLine, int showCmd)
{
	AllocConsole();
	//must research this
	FILE* pCout;
    freopen_s(&pCout, "conout$", "w", stdout);

	fout.open("logs/log.txt");

	pApp = new Application(hInstance, "Fable World", D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING);
	
	////adds 3 scenes in the scenesContainer and set the active scene to be the menu scene.
	//Menu menu;
	//pEngine->AddScene("menu",&menu);

	Game game;
	pApp->AddScene("game",&game);

	//MenuInGame menuInGame;
	//pEngine->AddScene("menuInGame",&menuInGame);

	//HeroSelection heroSelection;
	//pEngine->AddScene("heroSelect",&heroSelection);

	pApp->SetCurrentScene(&game);

	pApp->MainLoop();
}

//////////////////////////////////////////////////////////////////////////////