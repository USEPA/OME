// LuaLib.h : main header file for the LuaLib DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CLuaLibApp
// See LuaLib.cpp for the implementation of this class
//

class CLuaLibApp : public CWinApp
{
public:
	CLuaLibApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
