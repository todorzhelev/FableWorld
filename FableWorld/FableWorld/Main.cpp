#include <stdafx.h>
#include "Application.h"
#include "Game.h"
#include "Menu.h"
#include "MenuInGame.h"

//////////////////////////////////////////////////////////////////////////////

//entry point for the game
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,PSTR cmdLine, int showCmd) {
	AllocConsole();
	freopen( "CONOUT$", "wb", stdout);
	fout.open("log.txt");

	pApp = new Application(hInstance, "War Lord", D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING);

	pApp->InitManagers();

	Menu menu;
	pApp->AddScene("menu", &menu);

	Game game;
	pApp->AddScene("game", &game);

	MenuInGame menuInGame;
	pApp->AddScene("menuInGame", &menuInGame);

	pApp->SetCurrentScene(&menu);

	pApp->MainLoop();

	delete pApp;
}

//////////////////////////////////////////////////////////////////////////////
