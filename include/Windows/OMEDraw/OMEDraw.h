// OMEDraw.h : main header file for the OMEDraw DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// COMEDrawApp
// See OMEDraw.cpp for the implementation of this class
//

class COMEDrawApp : public CWinApp
{
public:
	COMEDrawApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
