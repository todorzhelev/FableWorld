#include "../../CORE/CORE/Misc.h"
#include "../../CORE/CORE/TextManager.h"
#include "../../CORE/CORE/DirectInput.h"
#include "../../CORE/CORE/Terrain.h"
#include "../../CORE/CORE/Camera.h"
#include "../../CORE/CORE/StaticModel.h"
#include "../../CORE/CORE/SkinnedModel.h"
#include "../../CORE/CORE/Button.h"
#include "../../CORE/CORE/Textbox.h"
#include "../../CORE/CORE/Label.h"
#include "../../CORE/CORE/Sky.h"
#include "../../CORE/CORE/SoundSyst.h"
#include "../../CORE/CORE/DialogueManager.h"
#include "../../CORE/CORE/IBaseScene.h"
#include "Menu.h"
#include "MenuInGame.h"
#include "../../CORE/CORE/LuaFunctions.h"
#include "../../CORE/CORE/GameObjectManager.h"


using namespace std;
//this is the game scene and contains all the things in the game - terrain, models, etc
//the Game class inherits from IBaseScene class so later we can change the scenes for instance from menu to game and etc
class Game : public IBaseScene
{
public:
	Game();

	~Game();

	virtual void OnLostDevice() override;

	virtual void OnResetDevice() override;

	virtual void OnUpdate(float dt) override;

	virtual void OnRender() override;

    virtual LRESULT MsgProc(UINT msg, WPARAM wParam, LPARAM lParam) override;

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

	SkinnedModel* 	pMainHero;

	ID3DXEffect* m_pDebugGraphicsEffect;
	D3DXHANDLE 	 m_hDebugGraphicsTechnique;
	D3DXHANDLE   m_hDebugGraphicsWVPMatrix;

	//map holding active quests
	map<string,QuestObject> m_mapActiveQuests;
};