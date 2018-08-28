#pragma once
#include<stdafx.h>

//////////////////////////////////////////////////////////////////////////////

class GameObject;
class IBaseMenuObject;

//////////////////////////////////////////////////////////////////////////////
      
class Application
{
public:

	Application(HINSTANCE hInstance, std::string strWindowTitle, D3DDEVTYPE eDeviceType, DWORD requestedVP);
	~Application();

	HINSTANCE				GetAppInstance();

	HWND					GetMainWindow();

	D3DPRESENT_PARAMETERS	GetPresentParameters();

	int						MainLoop();

	bool					IsDeviceLost();

	bool					IsPaused() const;

	void					SetPaused(bool bPaused);

	void					SwitchToFullscreen(bool bSwitch);

	void					SetCurrentScene(IBaseScene* pScene);

	IBaseScene*				GetCurrentScene();

	void					AddScene(std::string strSceneName, IBaseScene* pScene);

	IBaseScene*				GetScene(std::string strSceneName);

	bool					IsShaderVersionSupported();

	//position normal texture
	void					SetPNTDecl(IDirect3DVertexDeclaration9* pDecl);
	auto					GetPNTDecl() ->IDirect3DVertexDeclaration9*;

	//position color
	void					SetPCDecl(IDirect3DVertexDeclaration9* pDecl);
	auto					GetPCDecl() ->IDirect3DVertexDeclaration9*;

	//particles
	void					SetParticleDecl(IDirect3DVertexDeclaration9 * pDecl);
	auto					GetParticleDecl() ->IDirect3DVertexDeclaration9*;

	void					AddUIObject(IBaseMenuObject* pUIObject);

	std::vector<IBaseMenuObject*> m_vUIObjects;

	//holds the selected textbox, this must not be here,
	//MenuManager must be made instead and it has to be moved there.
	std::string					m_strSelectedTextbox;

	IBaseMenuObject*		FindMenuObject(std::string strObjectId);
	
private:

	void					InitMainWindow();

	void					InitDirect3D();

private:

	//the std::string in the title bar if we are in windowed mode
	std::string					 m_strWindowTitle;

	//handle to the current application instance.
	HINSTANCE				m_hAppInstance;

	//handle to the current window
	HWND					m_hMainWindow;

	//the type of the device we want to create - HAL, SW, REF
	D3DDEVTYPE				m_eDeviceType;

	//the type of vertex processing - software vertex processing or hardware vertex processing
	DWORD					m_vertexProcessingType;

	IDirect3D9*				m_pD3DObject;

	//present parameters - used for describing the directx device we want to create
	D3DPRESENT_PARAMETERS	m_presentParameters;

	bool					m_bIsAppPaused;

	//map containing all the scenes in the game, currently Menu scene, Game scene and MenuInGame Scene
	std::map<std::string,IBaseScene*> m_mapScenesContainer;

	//pointer to the current scene
	IBaseScene*				m_pCurrentScene;

	IDirect3DVertexDeclaration9* m_pVertexPNTDecl;
	IDirect3DVertexDeclaration9* m_pVertexPCDecl;
	IDirect3DVertexDeclaration9* m_pVertexParticleDecl;
};

//////////////////////////////////////////////////////////////////////////////

extern Application*		 pApp;

extern IDirect3DDevice9* pDxDevice;

extern std::ofstream			 fout;

//////////////////////////////////////////////////////////////////////////////
