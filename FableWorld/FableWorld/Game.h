#include "Misc.h"
#include "TextManager.h"
#include "DirectInput.h"
#include "Terrain.h"
#include "Camera.h"
#include "StaticModel.h"
#include "SkinnedModel.h"
#include "Button.h"
#include "Textbox.h"
#include "Label.h"
#include "Sky.h"
#include "DialogueManager.h"
#include "IBaseScene.h"
#include "Menu.h"
#include "MenuInGame.h"
#include "GameObjectManager.h"
#include "LuaFunctions.h"

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

	void		 MoveObject(std::string objectTitle,float dt);

	void		 RotateObject(std::string objectTitle,float dt);

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
};