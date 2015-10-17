#include "Application.h"
#include "Camera.h"
#include "Textbox.h"
#include "TextManager.h" 
#include "Misc.h"
#include "IBaseScene.h"
#include "GameObject.h"
#include "IBaseMenuObject.h"
#include "GameObjectManager.h"

Application* pApp = 0;
IDirect3DDevice9* pDxDevice = 0;


/////////////////////////////////////////////////////////////////////////
//LRESULT - long int
//CALLBACK - this means that Windows will call that function, not our program
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (pApp && pApp->GetCurrentScene() != NULL)
	{
		return pApp->GetCurrentScene()->MsgProc(msg, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}


/////////////////////////////////////////////////////////////////////////
/*
Function:GamepApp
Purpose:constructor
*/
Application::Application(HINSTANCE hInstance, string strWindowTitle, D3DDEVTYPE eDeviceType, DWORD vertexProcessingType)
{
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

	InitDebugGraphicsShader();
}


/////////////////////////////////////////////////////////////////////////
/*
Function:~GamepApp
Purpose:destructor
*/
Application::~Application()
{
	releaseX(m_pD3DObject);
	releaseX(pDxDevice);
}


/////////////////////////////////////////////////////////////////////////
/*
Function:getAppInst
Purpose:returns the instance of the program
*/
HINSTANCE Application::GetAppInstance()
{
	return m_hAppInstance;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:getMainWnd
Purpose:returns the handle of the window
*/
HWND Application::GetMainWindow()
{
	return m_hMainWindow;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:GetPresentParameters
Purpose:returns the present parameters, used for creating the directx device
*/
D3DPRESENT_PARAMETERS Application::GetPresentParameters()
{
	return m_presentParameters;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:isPaused
Purpose:checks if the app is paused
*/
bool Application::IsPaused() const
{
	return m_bIsAppPaused;
}

/////////////////////////////////////////////////////////////////////////

void Application::SetPaused(bool bPaused)
{
	m_bIsAppPaused = bPaused;
}


/////////////////////////////////////////////////////////////////////////
/*
Function:initMainWindow
Purpose:initialize the window
*/
void Application::InitMainWindow()
{
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
	
	if( !RegisterClass(&wc) )								//Windows requires the window to be registered
	{
		MessageBox(0, "RegisterClass FAILED", 0, 0);
		PostQuitMessage(0);
	}

	fout<<"window created\n";

	int width  = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);

	//RECT R = {0, 0, 1280,768 };
	RECT R = { 0, 0, 1440, 900 };

	//WS_OVERLAPPEDWINDOW is a style which gives the window title bar, window menu,sizing border,minimize and maximize buttons. 
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	m_hMainWindow = CreateWindow("D3DWndClassName", 
								m_strWindowTitle.c_str(), 
								WS_OVERLAPPEDWINDOW, 
								250, 
								100, 
								R.right, 
								R.bottom, 
								0, 
								0, 
								m_hAppInstance, 
								0); 
	if( !m_hMainWindow )
	{
		MessageBox(0, "CreateWindow FAILED", 0, 0);
		PostQuitMessage(0);
	}

	ShowWindow(m_hMainWindow, SW_SHOW);
	//the window must be refreshed after its shown with ShowWindow. This is done via UpdateWindow
	UpdateWindow(m_hMainWindow);
}


/////////////////////////////////////////////////////////////////////////
/*
Function:initDirect3D
Purpose:initialize direct3d
*/
void Application::InitDirect3D()
{
	//via d3dObject we can later create the directx device which we will use in the game for rendering and connection with the video card
	//with d3dObject we can find information about the hardware devices on the system(video cards etc.)
    m_pD3DObject = Direct3DCreate9(D3D_SDK_VERSION);
	if( !m_pD3DObject )
	{
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

	if(FAILED(m_pD3DObject->CreateDevice(D3DADAPTER_DEFAULT,				// primary adapter
										  m_eDeviceType,                    // device type
										  m_hMainWindow,                    // window associated with device
										  m_vertexProcessingType,        
										  &m_presentParameters,                  
										  &pDxDevice)))            
	{
		MessageBox(0, "Creating Device FAILED", 0, 0);
	}

	fout<<"directx device created\n";
	//SwitchToFullscreen(true);
}


/////////////////////////////////////////////////////////////////////////
/*
Function:MainLoop
Purpose:Here in a loop are invoked onUpdate and onRender functions of the current scene
*/
int Application::MainLoop()
{
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

	while(msg.message != WM_QUIT)
	{
		//windows uses event-driven programming model. It waits for an event to occur and
		//when some event occur, windows sends message to the application and adds this message to application's message queue
		//the type of the queue is priority queue(elements are pulled highest-priority first). The application checks for messages
		//in the queue and if there is any it translates it and dispatch it to window procedure function, associated with this window,
		//if there are no messages in the queue this code starts executing the main game logic and rendering.
		if( PeekMessage(&msg, 0, 0, 0, PM_REMOVE) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{	
			if( m_bIsAppPaused )
			{
				Sleep(20);
				continue;
			}
			if( !IsDeviceLost() )
			{
				__int64 currTimeStamp = 0;
				//takes the current time in counts
				QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
				//convert the counts into seconds
				float dt = (currTimeStamp - prevTimeStamp)*secsPerCnt;

				if (m_pCurrentScene != nullptr)
				{
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
/*
Function:enableFullScreenMode
Purpose:switches between fullscreen and windowed mode
*/
void Application::SwitchToFullscreen(bool bSwitch)
{
	//switch to fullscreen mode
	if( bSwitch )
	{
		if( !m_presentParameters.Windowed ) 
		{
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
	else
	{	
		if( m_presentParameters.Windowed ) 
		{
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
	if( m_pCurrentScene == NULL )
	{
		pDxDevice->Reset(&m_presentParameters);
	}
	else
	{
		m_pCurrentScene->OnLostDevice();
		pDxDevice->Reset(&m_presentParameters);
		m_pCurrentScene->OnResetDevice();
	}
}


/////////////////////////////////////////////////////////////////////////
/*
Function:isDeviceLost
Purpose:tests if the directx device is lost(alt+tab)
*/
bool Application::IsDeviceLost()
{
	HRESULT hr = pDxDevice->TestCooperativeLevel();

	if( hr == D3DERR_DEVICELOST )
	{
		Sleep(20);	
		return true;
	}
	else if( hr == D3DERR_DEVICENOTRESET )
	{
		m_pCurrentScene->OnLostDevice();
		pDxDevice->Reset(&m_presentParameters);
		m_pCurrentScene->OnResetDevice();

		return false;
	}
}


/////////////////////////////////////////////////////////////////////////

IDirect3DVertexDeclaration9* Application::GetPositionNormalTextureDecl()
{ 
	return m_pVertexPositionNormalTextureDecl;
}

/////////////////////////////////////////////////////////////////////////

void Application::SetPositionNormalTextureDecl(IDirect3DVertexDeclaration9* pDecl)
{
	m_pVertexPositionNormalTextureDecl = pDecl;
}

/////////////////////////////////////////////////////////////////////////

IDirect3DVertexDeclaration9* Application::GetPosColDeclaration()
{ 
	return m_pVertexPosColDecl;
}

/////////////////////////////////////////////////////////////////////////

void Application::SetPosColDeclaration(IDirect3DVertexDeclaration9* pDecl)
{
	m_pVertexPosColDecl = pDecl;
}

/////////////////////////////////////////////////////////////////////////

void Application::SetCurrentScene(IBaseScene* pScene)
{
	m_pCurrentScene = pScene;
}

/////////////////////////////////////////////////////////////////////////

IBaseScene* Application::GetCurrentScene()
{
	return m_pCurrentScene;
}

/////////////////////////////////////////////////////////////////////////

void Application::AddScene(string strSceneName, IBaseScene* pScene)
{
	m_mapScenesContainer[strSceneName] = pScene;
}

/////////////////////////////////////////////////////////////////////////

IBaseScene* Application::GetScene(string strSceneName)
{
	return m_mapScenesContainer.find(strSceneName)->second;
}

/////////////////////////////////////////////////////////////////////////

bool Application::IsShaderVersionSupported()
{
	D3DCAPS9 caps;
	pDxDevice->GetDeviceCaps(&caps); 
	if( caps.VertexShaderVersion < D3DVS_VERSION(2, 0) )
	{
		return false;
	}
	if( caps.PixelShaderVersion < D3DPS_VERSION(2, 0) )
	{
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////


void Application::AddUIObject(IBaseMenuObject* pUIObject)
{
	m_vUIObjects.push_back(pUIObject);
}

/////////////////////////////////////////////////////////////////////////

IBaseMenuObject* Application::FindMenuObject(string strObjectId)
{
	for(int i = 0; i < m_vUIObjects.size(); i++)
	{
		if(!strObjectId.compare(m_vUIObjects[i]->m_strId))
		{
			return m_vUIObjects[i];
		}
	}

	return NULL;

}

/////////////////////////////////////////////////////////////////////////

void Application::InitDebugGraphicsShader()
{
	D3DXCreateEffectFromFile(pDxDevice, "../../CORE/CORE/shaders/DebugGraphicsShader.fx", 0, 0, D3DXSHADER_DEBUG, 0, &m_pDebugGraphicsEffect, 0);
	m_hDebugGraphicsTechnique  = m_pDebugGraphicsEffect->GetTechniqueByName("DebugGraphics3DTech");
	m_hDebugGraphicsWVPMatrix  = m_pDebugGraphicsEffect->GetParameterByName(0, "WVP");
}

/////////////////////////////////////////////////////////////////////////

/*
Function:DrawLine
Purpose:draws a line
*/
void Application::DrawLine(const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd)
{
	//pDxDevice->BeginScene();

	m_pDebugGraphicsEffect->SetTechnique(m_hDebugGraphicsTechnique);

	m_pDebugGraphicsEffect->SetMatrix(m_hDebugGraphicsWVPMatrix, &(camera->GetViewProjMatrix()));
	UINT numPasses = 0;
	m_pDebugGraphicsEffect->Begin(&numPasses, 0);
	m_pDebugGraphicsEffect->BeginPass(0);

		IDirect3DVertexBuffer9* pVertexBuffer;

		pDxDevice->CreateVertexBuffer(2 * sizeof (VertexPositionColor), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &pVertexBuffer, 0);

		VertexPositionColor* v = 0;
		pVertexBuffer->Lock(0, 0,  (void**)&v, 0);

		v[0].m_vPos = vStart;
		//v[0].m_color = D3DXCOLOR(1.0f,1.0,1.0f,1.0f);

		v[1].m_vPos = vEnd;
		//v[1].m_color = D3DXCOLOR(1.0f,1.0,1.0f,1.0f);

		pVertexBuffer->Unlock();

		pDxDevice->SetStreamSource(0, pVertexBuffer, 0, sizeof(VertexPositionColor));

		pDxDevice->SetVertexDeclaration(pApp->GetPosColDeclaration());

		pDxDevice->DrawPrimitive(D3DPT_LINELIST,0,1);

	m_pDebugGraphicsEffect->EndPass();
	m_pDebugGraphicsEffect->End();


	//pDxDevice->EndScene();

	//pDxDevice->Present(0, 0, 0, 0);
}
