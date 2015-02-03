#include "Misc.h"
#include "TextManager.h"
#include "Camera.h"
#include "IBaseScene.h"
#include  "Label.h"
#include "Button3D.h"
//#include "IBase3DMenuObject.h"

class Tree;
class Textbox;


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

	

private:
	
	Label* m_pLabelStartGame;

	Button3D* m_pTestButton;

	GameObject m_pObj;

	Tree* m_pTree;

	GameObject* m_pTestObject;

	Textbox* m_pTestTextbox;
};