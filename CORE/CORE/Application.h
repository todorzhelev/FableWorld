#pragma once

#ifdef _DEBUG
	#ifndef D3D_DEBUG_INFO
	#define D3D_DEBUG_INFO
	#endif
#endif

#include<windows.h>
#include<d3d9.h>
#include<d3dx9.h>
#include<DxErr9.h>
#include<tchar.h>
#include<sstream>
#include<vector>
#include<string>
#include<fstream>
#include<fmod.hpp>
#include<stdio.h>
#include<conio.h>
#include<map>
#include<unordered_map>
#include<list>
#include"IBaseScene.h"
#include<iostream>

using namespace std;

//////////////////////////////////////////////////////////////////////////////

class GameObject;
class IBaseMenuObject;

//////////////////////////////////////////////////////////////////////////////

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib, "dxerr9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"fmodex_vc.lib")
#pragma comment(lib,"lua5.1.lib")

//////////////////////////////////////////////////////////////////////////////

class Application
{
public:

	Application(HINSTANCE hInstance, string strWindowTitle, D3DDEVTYPE eDeviceType, DWORD requestedVP);
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

	void					AddScene(string strSceneName, IBaseScene* pScene);

	IBaseScene*				GetScene(string strSceneName);

	bool					IsShaderVersionSupported();

	void					SetPositionNormalTextureDecl(IDirect3DVertexDeclaration9* pDecl);

	IDirect3DVertexDeclaration9* GetPositionNormalTextureDecl();

	IDirect3DVertexDeclaration9* GetPosColDeclaration();

	void					SetPosColDeclaration(IDirect3DVertexDeclaration9* pDecl);

	void					AddUIObject(IBaseMenuObject* pUIObject);

	vector<IBaseMenuObject*> m_vUIObjects;

	//holds the selected textbox, this must not be here,
	//MenuManager must be made instead and it has to be moved there.
	string					m_strSelectedTextbox;

	IBaseMenuObject*		FindMenuObject(string strObjectId);

	void					InitDebugGraphicsShader();

	void					DrawLine(const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd);
	
private:

	void					InitMainWindow();

	void					InitDirect3D();

private:

	//the string in the title bar if we are in windowed mode
	string					 m_strWindowTitle;

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
	map<string,IBaseScene*> m_mapScenesContainer;

	//pointer to the current scene
	IBaseScene*				m_pCurrentScene;

	IDirect3DVertexDeclaration9* m_pVertexPositionNormalTextureDecl;

	IDirect3DVertexDeclaration9* m_pVertexPosColDecl;


	ID3DXEffect*			m_pDebugGraphicsEffect;
	D3DXHANDLE 				m_hDebugGraphicsTechnique;
	D3DXHANDLE				m_hDebugGraphicsWVPMatrix;
};

//////////////////////////////////////////////////////////////////////////////

extern Application*		 pApp;

extern IDirect3DDevice9* pDxDevice;

extern ofstream			 fout;

//////////////////////////////////////////////////////////////////////////////
