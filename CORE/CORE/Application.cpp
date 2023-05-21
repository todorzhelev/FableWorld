#include <stdafx.h>
#include "Application.h"
#include "Camera.h"
#include "Textbox.h"
#include "TextManager.h" 
#include "Misc.h"
#include "GameObject.h"
#include "IBaseMenuObject.h"
#include "GameObjectManager.h"
#include "DirectInput.h"

/////////////////////////////////////////////////////////////////////////

Application* pApp = nullptr;

/////////////////////////////////////////////////////////////////////////
//LRESULT - long int
//CALLBACK - this means that Windows will call that function, not our program
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (pApp && pApp->GetCurrentScene() != NULL) {
		return pApp->GetCurrentScene()->MsgProc(msg, wParam, lParam);
	}
	else {
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

/////////////////////////////////////////////////////////////////////////

Application::Application(HINSTANCE hInstance, std::string strWindowTitle, D3DDEVTYPE eDeviceType, DWORD vertexProcessingType) {
	m_strWindowTitle		= strWindowTitle;
	m_eDeviceType			= eDeviceType;
	m_hAppInstance			= hInstance;
	m_hMainWindow			= 0;
	m_pD3DObject			= nullptr;
	m_bIsAppPaused			= false;
	m_vertexProcessingType	= vertexProcessingType;
	m_pCurrentScene			= nullptr;

	ZeroMemory(&m_presentParameters, sizeof(m_presentParameters));
	
	InitMainWindow();
	InitDirect3D();
	InitVertexDeclarations();

	m_LogStream.open("log.txt");
}

/////////////////////////////////////////////////////////////////////////

//the game object manager, the direct input and the text manager transcend the scenes, 
//so I think they should be attached to the app and accessible only via the app, not being a singletons
void Application::InitManagers() {
	m_pGameObjManager = std::make_unique<GameObjectManager>(true, true, false, true, false, true);
	m_pDinput = std::make_unique<DirectInput>(DISCL_NONEXCLUSIVE | DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	m_pTextManager = std::make_unique<TextManager>();
}
/////////////////////////////////////////////////////////////////////////

Application::~Application() {
	ReleaseX(m_pD3DObject);
	ReleaseX(m_pDxDevice);
}

/////////////////////////////////////////////////////////////////////////

//returns the instance of the program
HINSTANCE Application::GetAppInstance() {
	return m_hAppInstance;
}

/////////////////////////////////////////////////////////////////////////

//returns the handle of the window
HWND Application::GetMainWindow() {
	return m_hMainWindow;
}

/////////////////////////////////////////////////////////////////////////

//returns the present parameters, used for creating the directx device
D3DPRESENT_PARAMETERS Application::GetPresentParameters() {
	return m_presentParameters;
}

/////////////////////////////////////////////////////////////////////////

bool Application::IsPaused() const {
	return m_bIsAppPaused;
}

/////////////////////////////////////////////////////////////////////////

void Application::SetPaused(bool bPaused) {
	m_bIsAppPaused = bPaused;
}

/////////////////////////////////////////////////////////////////////////

//initialize the window
void Application::InitMainWindow() {
	WNDCLASS wc;
	wc.style			= CS_HREDRAW | CS_VREDRAW;										//indicates that the windows has to be repainted in case the window's size is changed in horizontal or vertical side
	wc.lpfnWndProc		= MainWndProc;													//pointer to the windows procedure function.This function is responsible for processing events
	wc.cbClsExtra		= 0;															//if our program needs extra space
	wc.cbWndExtra		= 0;															//if our program needs extra space
	wc.hInstance		= m_hAppInstance;												//handle to our application instance
	wc.hIcon			= LoadIcon(0, IDI_APPLICATION);									//handle to an icon
	wc.hCursor			= LoadCursorFromFile("../../Resources/cursors/Hand1.cur");	//handle to a cursor, when the mouse is in the client's area of the window
	wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);							//the background of the client's area of the window
	wc.lpszMenuName		= 0;															//if we have menu
	wc.lpszClassName	= "D3DWndClassName";											//the name of the class. We use it later to identify the wndclass structure when creating the window
	
	if (!RegisterClass(&wc)) {								//Windows requires the window to be registered
		MessageBox(0, "RegisterClass FAILED", 0, 0);
		PostQuitMessage(0);
	}

	printf("Window created\n");

	int width  = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);

	RECT R = {0, 0, 1280,768 };
	//RECT R = { 0, 0, 1440, 900 };

	//WS_OVERLAPPEDWINDOW is a style which gives the window title bar, window menu,sizing border,minimize and maximize buttons. 
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	m_hMainWindow = CreateWindow("D3DWndClassName", 
								m_strWindowTitle.c_str(), 
								WS_OVERLAPPEDWINDOW, 
								100, 
								50, 
								R.right, 
								R.bottom, 
								0, 
								0, 
								m_hAppInstance, 
								0); 
	if (!m_hMainWindow) {
		MessageBox(0, "CreateWindow FAILED", 0, 0);
		PostQuitMessage(0);
	}

	ShowWindow(m_hMainWindow, SW_SHOW);
	//the window must be refreshed after its shown with ShowWindow. This is done via UpdateWindow
	UpdateWindow(m_hMainWindow);
}


/////////////////////////////////////////////////////////////////////////

//initialize direct3d
void Application::InitDirect3D() {
	//via d3dObject we can later create the directx device which we will use in the game for rendering and connection with the video card
	//with d3dObject we can find information about the hardware devices on the system(video cards etc.)
    m_pD3DObject = Direct3DCreate9(D3D_SDK_VERSION);
	if (!m_pD3DObject) {
		MessageBox(0, "Direct3DCreate9 FAILED", 0, 0);
		PostQuitMessage(0);
	}

	//D3DSWAPEFFECT_DISCARD - tells how the buffers in the flipping chain will be swapped.
	//Usually there are 2 buffers - front buffer(the one that is currently presented on the screen) 
	//and back buffer(on this buffer are written the new pixels)
	//when the pixels on the back buffer are written, back buffer swaps with the front buffer, in order to refresh the screen with the
	//new pixels and no distortions to occur.

	m_presentParameters.BackBufferWidth				= 0;						//width of the back buffer in pixels. If we set it to 0 it will match the clients area in windowed mode.
	m_presentParameters.BackBufferHeight			= 0;						//height of the back buffer in pixels. If we set it to 0 it will match the clients area in windowed mode.
	m_presentParameters.BackBufferFormat			= D3DFMT_UNKNOWN;			//use the current display mode's pixel format.
	m_presentParameters.BackBufferCount				= 1;
	m_presentParameters.MultiSampleType				= D3DMULTISAMPLE_NONE;
	m_presentParameters.MultiSampleQuality			= 0;
	m_presentParameters.SwapEffect					= D3DSWAPEFFECT_DISCARD;			
	m_presentParameters.hDeviceWindow				= m_hMainWindow;
	m_presentParameters.Windowed					= true;
	m_presentParameters.EnableAutoDepthStencil		= true;						//tells to the directx device to maintain depth buffer automatically
	m_presentParameters.AutoDepthStencilFormat		= D3DFMT_D24S8;
	m_presentParameters.Flags						= 0;
	m_presentParameters.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
	m_presentParameters.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;	//presents the picture immediately

	CheckSuccess(m_pD3DObject->CreateDevice(D3DADAPTER_DEFAULT,				// primary adapter
										m_eDeviceType,                    // device type
										m_hMainWindow,                    // window associated with device
										m_vertexProcessingType,
										&m_presentParameters,
										&m_pDxDevice));

	printf("DX device created\n");
	//SwitchToFullscreen(true);
}

/////////////////////////////////////////////////////////////////////////
/*
Here in a loop are invoked onUpdate and onRender functions of the current scene
*/
int Application::MainLoop() {
   MSG msg;
   msg.message = WM_NULL;

   	__int64 cntsPerSec = 0;
	//evaluate how much counts are in second;for instance 2987294 counts for one second
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	//evaluates how much seconds for one count; for instance 1/40 or 0.025
	float secsPerCnt = 1.0f / (float)cntsPerSec;

	__int64 prevTimeStamp = 0;
	//takes the prev time in counts
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);

	while(msg.message != WM_QUIT) {
		//windows uses event-driven programming model. It waits for an event to occur and
		//when some event occur, windows sends message to the application and adds this message to application's message queue
		//the type of the queue is priority queue(elements are pulled highest-priority first). The application checks for messages
		//in the queue and if there is any it translates it and dispatch it to window procedure function, associated with this window,
		//if there are no messages in the queue this code starts executing the main game logic and rendering.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {	
			if (m_bIsAppPaused) {
				Sleep(20);
				continue;
			}
			if (!IsDeviceLost()) {
				__int64 currTimeStamp = 0;
				//takes the current time in counts
				QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
				//convert the counts into seconds
				float dt = (currTimeStamp - prevTimeStamp)*secsPerCnt;

				if (m_pCurrentScene != nullptr) {
					m_pCurrentScene->OnUpdate(dt);
					m_pCurrentScene->OnRender();
				}

				prevTimeStamp = currTimeStamp;
			}
		}
	}	

	return (int)msg.wParam;
}


/////////////////////////////////////////////////////////////////////////

//switches between fullscreen and windowed mode
void Application::SwitchToFullscreen(bool bSwitch) {
	//switch to fullscreen mode
	if (bSwitch) {
		if (!m_presentParameters.Windowed) {
			return;
		}

		//takes the width and the height of the monitor
		int width  = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);

		m_presentParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
		m_presentParameters.BackBufferWidth  = width;
		m_presentParameters.BackBufferHeight = height;
		m_presentParameters.Windowed = false;

		//with SetWindowLongPtr we make change in the current style of the window
		//WS_POPUP makes the window without visual borders, title bar and closing button
		SetWindowLongPtr(m_hMainWindow, GWL_STYLE, WS_POPUP);

		//we need this so the above functions changes to take effect
		SetWindowPos(m_hMainWindow, HWND_TOP, 0, 0, width, height, SWP_NOZORDER | SWP_SHOWWINDOW);	
	}
	//switch to windowed mode.
	else {	
		if (m_presentParameters.Windowed) {
			return;
		}

		RECT R = {0, 0, 800, 600};
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		m_presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
		m_presentParameters.BackBufferWidth  = 800;
		m_presentParameters.BackBufferHeight = 600;
		m_presentParameters.Windowed = true;
	
		SetWindowLongPtr(m_hMainWindow, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		//SWP_NOZORDER | SWP_SHOWWINDOW doesnt affect the current Z- order of the opened windows and shows our window
		SetWindowPos(m_hMainWindow, HWND_TOP, 100, 100, R.right, R.bottom, SWP_NOZORDER | SWP_SHOWWINDOW);
	}

	//if the scenes are not yet initialized just reset the device, otherwise call on lost and on reset functions
	//the device reset is needed so the directx device can change its parameters with the new values.
	if (m_pCurrentScene == NULL) {
		m_pDxDevice->Reset(&m_presentParameters);
	}
	else {
		m_pCurrentScene->OnLostDevice();
		m_pDxDevice->Reset(&m_presentParameters);
		m_pCurrentScene->OnResetDevice();
	}
}

/////////////////////////////////////////////////////////////////////////

//tests if the directx device is lost(alt+tab)
bool Application::IsDeviceLost() {
	const HRESULT hr = m_pDxDevice->TestCooperativeLevel();

	if (hr == D3DERR_DEVICELOST) {
		Sleep(20);	
		return true;
	}
	else if (hr == D3DERR_DEVICENOTRESET) {
		m_pCurrentScene->OnLostDevice();
		pApp->GetDevice()->Reset(&m_presentParameters);
		m_pCurrentScene->OnResetDevice();

		return false;
	}
	else if (hr == D3DERR_DRIVERINTERNALERROR)
	{
		PostQuitMessage(0);
		return true;
	}
	
	return false;
}

/////////////////////////////////////////////////////////////////////////

void Application::SetCurrentScene(IBaseScene* pScene) {
	m_pCurrentScene = pScene;
}

/////////////////////////////////////////////////////////////////////////

IBaseScene* Application::GetCurrentScene() {
	return m_pCurrentScene;
}

/////////////////////////////////////////////////////////////////////////

void Application::AddScene(std::string strSceneName, IBaseScene* pScene) {
	m_mapScenesContainer[strSceneName] = pScene;
}

/////////////////////////////////////////////////////////////////////////

IBaseScene* Application::GetScene(std::string strSceneName) {
	return m_mapScenesContainer.find(strSceneName)->second;
}

/////////////////////////////////////////////////////////////////////////

bool Application::IsShaderVersionSupported() {
	D3DCAPS9 caps;
	m_pDxDevice->GetDeviceCaps(&caps);
	if (caps.VertexShaderVersion < D3DVS_VERSION(3, 0)) {
		return false;
	}
	if (caps.PixelShaderVersion < D3DPS_VERSION(3, 0)) {
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////

void Application::AddUIObject(IBaseMenuObject* pUIObject) {
	m_vUIObjects.push_back(pUIObject);
}

/////////////////////////////////////////////////////////////////////////

IBaseMenuObject* Application::FindMenuObject(std::string strObjectId) {
	for(int i = 0; i < m_vUIObjects.size(); i++) {
		if(!strObjectId.compare(m_vUIObjects[i]->m_strId)) {
			return m_vUIObjects[i];
		}
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////

auto Application::GetGameObjManager() -> const std::unique_ptr<GameObjectManager>& {
	return m_pGameObjManager;
}

/////////////////////////////////////////////////////////////////////////

auto Application::GetDinput() -> const std::unique_ptr<DirectInput>& {
	return m_pDinput;
}

/////////////////////////////////////////////////////////////////////////

auto Application::GetTextManager() -> const std::unique_ptr<TextManager>& {
	return m_pTextManager;
}

/////////////////////////////////////////////////////////////////////////

std::ofstream& Application::GetLogStream() {
	return m_LogStream;
}

/////////////////////////////////////////////////////////////////////////

IDirect3DDevice9* Application::GetDevice() {
	return m_pDxDevice;
}

/////////////////////////////////////////////////////////////////////////

void Application::InitVertexDeclarations() {
	//position only (12B size)
	D3DVERTEXELEMENT9 vP[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};

	//position and color (28B)
	D3DVERTEXELEMENT9 vPC[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		D3DDECL_END()
	};

	//position and normal (24B size)
	D3DVERTEXELEMENT9 vPN[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		D3DDECL_END()
	};

	//position and 2 textures (28B size)
	D3DVERTEXELEMENT9 vPTT[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
		D3DDECL_END()
	};

	//position,normal and texture (32B size)
	D3DVERTEXELEMENT9 vPNT[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	//56B. D3DCOLOR is 4x4B floats
	D3DVERTEXELEMENT9 vPs[] = {
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
		{ 0, 28, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
		{ 0, 32, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
		{ 0, 36, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },
		{ 0, 40, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END()
	};

	IDirect3DVertexDeclaration9* pDecl = nullptr;
	m_pDxDevice->CreateVertexDeclaration(vPNT, &pDecl);
	m_pVertexPNTDecl = pDecl;

	m_pDxDevice->CreateVertexDeclaration(vPC, &pDecl);
	m_pVertexPCDecl = pDecl;

	m_pDxDevice->CreateVertexDeclaration(vPs, &pDecl);
	m_pVertexParticleDecl = pDecl;

	m_pDxDevice->CreateVertexDeclaration(vPTT, &pDecl);
	m_pVertexPositionNormalDisplacementDecl = pDecl;
}

/////////////////////////////////////////////////////////////////////////

IDirect3DVertexDeclaration9* Application::GetPNTDecl() {
	return m_pVertexPNTDecl;
}

/////////////////////////////////////////////////////////////////////////

IDirect3DVertexDeclaration9* Application::GetParticleDecl() {
	return m_pVertexParticleDecl;
}

/////////////////////////////////////////////////////////////////////////

IDirect3DVertexDeclaration9* Application::GetPCDecl() {
	return m_pVertexPCDecl;
}

/////////////////////////////////////////////////////////////////////////

IDirect3DVertexDeclaration9* Application::GetPositionNormalDisplacementDecl() {
	return m_pVertexPositionNormalDisplacementDecl;
}

/////////////////////////////////////////////////////////////////////////