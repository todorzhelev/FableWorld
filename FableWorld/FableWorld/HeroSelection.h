#pragma once
#include "../../../TheCore/TheCore/TheCore/IBaseScene.h"
#include "../../../TheCore/TheCore/TheCore/DirectInput.h"
#include "../../../TheCore/TheCore/TheCore/Misc.h"
#include "Menu.h"
#include "../../../TheCore/TheCore/TheCore/Label.h"
using namespace std;

//this is the menu scene and contains all the thing needed in the menu
class HeroSelection: public IBaseScene
{
public:

	HeroSelection();

	virtual void OnLostDevice();

	virtual void OnResetDevice();

	virtual void OnUpdate(float dt);

	virtual void OnRender();

	LRESULT		 MsgProc(UINT msg, WPARAM wParam, LPARAM lParam);

private:

	vector<GameObject*> m_SceneObjects;

	GameObject* pPesho;

	Label* m_pLabelEnterWorld;
};