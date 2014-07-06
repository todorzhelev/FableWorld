#include "Misc.h"
#include "TextManager.h"
#include "DirectInput.h"
#include "Terrain.h"
#include "Camera.h"
#include "StaticMesh.h"
#include "SkinnedMesh.h"
#include "Button.h"
#include "Textbox.h"
#include "Label.h"
#include "Sky.h"
#include "SoundSyst.h"
#include "DialogueManager.h"
#include "IBaseScene.h"
#include "Menu.h"
#include "MenuInGame.h"
#include "LuaFunctions.h"


using namespace std;
//this is the game scene and contains all the things in the game - terrain, models, etc
//the Game class inherits from IBaseScene class so later we can change the scenes for instance from menu to game and etc
class Game : public IBaseScene
{
public:
	Game();

	~Game();

	virtual void OnLostDevice();

	virtual void OnResetDevice();

	virtual void OnUpdate(float dt);

	virtual void OnRender();

    LRESULT		 MsgProc(UINT msg, WPARAM wParam, LPARAM lParam);

	bool		 IsObjectNear(GameObject* obj1,GameObject* obj2);

	void		 MoveObject(string objectTitle,float dt);

	void		 RotateObject(string objectTitle,float dt);

	void		 ManageHealthBars();

	void		DrawLine(const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd);
	
private:

	void		InitDebugGraphicsShader();

private:
	
	//the skybox in the game
	Sky*		pSky;

	bool		m_bIsEnemyHealthBarVisible;

	//controls how much of our healthbar to be visible. this way we can simulate the drop in the health
	RECT		m_rHealthBarRectangle;

	//controls how much of enemy healthbar to be visible. this way we can simulate the drop in the health
	RECT		m_rEnemyHealthBarRectangle;

	D3DXVECTOR3	m_vHealthBarPosition;
	D3DXVECTOR3	m_vEnemyHealthBarPosition;

	//the sprite for the interface
	ID3DXSprite*			m_pInterfaceSprite;

	//healthbars textures
	IDirect3DTexture9*		m_pHealthBarTexture;
	IDirect3DTexture9*		m_pHealthBarFilledTexture;
	IDirect3DTexture9*		m_phealthBarFilledEnemyTexture;

	Button*					m_pHealSpell;

	SkinnedMesh* 	pMainHero;

	ID3DXEffect* m_pDebugGraphicsEffect;
	D3DXHANDLE 	 m_hDebugGraphicsTechnique;
	D3DXHANDLE   m_hDebugGraphicsWVPMatrix;

	//map holding active quests
	map<string,QuestObject> m_mapActiveQuests;
};