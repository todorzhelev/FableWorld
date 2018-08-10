#pragma once

#ifdef _DEBUG
	#ifndef D3D_DEBUG_INFO
		#define D3D_DEBUG_INFO
	#endif
#endif

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN 
#endif

#include<windows.h>
#include<direct.h> //__getcwd
#include<Commdlg.h> //OPENFILENAME
#include<d3d9.h>
#include<d3dx9.h>
#include<DxErr.h>
#include<tchar.h>
#include<iostream>
#include<sstream>
#include<fstream>
#include<string>
#include<conio.h>
#include<math.h>
#include<map>
#include<unordered_map>
#include<vector>
#include"IBaseScene.h"
