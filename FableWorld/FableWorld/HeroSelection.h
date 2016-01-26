#pragma once
#include "IBaseScene.h"
#include "DirectInput.h"
#include "Misc.h"
#include "Menu.h"
#include "Label.h"
using namespace std;

//this is the menu scene and contains all the thing needed in the menu
class HeroSelection: public IBaseScene
{
public:

	HeroSelection();

	virtual void OnLostDevice() override;

	virtual void OnResetDevice() override;

	virtual void OnUpdate(float dt) override;

	virtual void OnRender() override;

	virtual LRESULT MsgProc(UINT msg, WPARAM wParam, LPARAM lParam) override;

private:

	vector<GameObject*> m_SceneObjects;

	GameObject* pPesho;

	Label* m_pLabelEnterWorld;
};