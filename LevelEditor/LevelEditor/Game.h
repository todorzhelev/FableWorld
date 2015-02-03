#include "../../../TheCore/TheCore/TheCore/Misc.h"
#include "../../../TheCore/TheCore/TheCore/TextManager.h"
#include "../../../TheCore/TheCore/TheCore/DirectInput.h"
#include "../../../TheCore/TheCore/TheCore/Terrain.h"
#include "../../../TheCore/TheCore/TheCore/Camera.h"
#include "../../../TheCore/TheCore/TheCore/StaticMesh.h"
#include "../../../TheCore/TheCore/TheCore/SkinnedMesh.h"
#include "../../../TheCore/TheCore/TheCore/Button.h"
#include "../../../TheCore/TheCore/TheCore/Textbox.h"
#include "../../../TheCore/TheCore/TheCore/Label.h"
#include "../../../TheCore/TheCore/TheCore/Checkbox.h"
#include "../../../TheCore/TheCore/TheCore/Sky.h"
#include "../../../TheCore/TheCore/TheCore/SoundSyst.h"
#include "../../../TheCore/TheCore/TheCore/DialogueManager.h"
#include "Menu.h"
#include "MenuInGame.h"
#include "../../../TheCore/TheCore/TheCore/LuaFunctions.h"
#include <iostream>
#include <fstream>


class IBaseMenuObject;

using namespace std;
//this is the game scene and contains all the things in the game - terrain, models, etc
//the Game class inherits from Scenes class so later we can change the scenes for instance from menu to game and etc
class Game : public IBaseScene
{
public:
	Game();
	~Game();

	bool checkDeviceCaps();

	void OnLostDevice();
	void OnResetDevice();

	void OnUpdate(float dt);
	void OnRender();

    LRESULT MsgProc(UINT msg, WPARAM wParam, LPARAM lParam);

	bool isNear(GameObject& obj1,GameObject& obj2);
	void objectMovement(string objectTitle,float dt);
	void objectRotation(string objectTitle,float dt);
	void healthControl();
	void ImportStaticModel();
	void LoadAnimatedModel();

	void LoadUIComponents();
	void pickedModelControl(GameObject& obj, float dt);
	void ExportLevel();
	void ImportLevel();

	string GetModelNameFromFilePath(string strFilePath);
	
private:
	
	IBaseMenuObject* button_LoadStaticModel;
	IBaseMenuObject* button_LoadAnimatedModel;
	IBaseMenuObject* button_Import;
	IBaseMenuObject* button_Export;

	//GuiObject label_Translation;
	IBaseMenuObject* button_Translation;
	IBaseMenuObject* checkbox_TranslationX;
	IBaseMenuObject* checkbox_TranslationY;
	IBaseMenuObject* checkbox_TranslationZ;

	//GuiObject label_Rotation;
	IBaseMenuObject* button_Rotation;
	IBaseMenuObject* checkbox_RotationX;
	IBaseMenuObject* checkbox_RotationY;
	IBaseMenuObject* checkbox_RotationZ;

	//GuiObject label_Scaling;
	IBaseMenuObject* button_Scaling;
	IBaseMenuObject* checkbox_Scaling;
	
	IBaseMenuObject* label_modelName;
	IBaseMenuObject* textbox_modelName;

	IBaseMenuObject* label_typeInGame;
	IBaseMenuObject* textbox_typeInGame;

	IBaseMenuObject* label_titleForQuest;
	IBaseMenuObject* textbox_titleForQuest;

	IBaseMenuObject* label_bindToAnModel;
	IBaseMenuObject* textbox_bindToAnModel;

	IBaseMenuObject* label_bindToAnModelBone;
	IBaseMenuObject* textbox_bindToAnModelBone;

	IBaseMenuObject* labelStartGame;
	IBaseMenuObject* labelQuit;
	
	Sky* sky;


	bool enter_pressed;
	TCHAR ch;
	bool cameraMode;
	bool typingMode; //used when the user types something in textbox
	//ID3DXSprite* spriteForInterface;
	IDirect3DTexture9* healthBarText;
	IDirect3DTexture9* healthBarFillText;
	IDirect3DTexture9* healthBarFillEnemyText;
	IDirect3DTexture9* heroIcon;
	RECT healthBarRect;
	RECT enemyHealthBarRect;
	D3DXVECTOR3 posForIcon;
	D3DXVECTOR3 posForHealthBar;
	D3DXVECTOR3 posForEnemyHealthbar;
	map<string,QuestObject> activeQuests;

	
	
};