// ModelProject.h : main header file for the ModelProject DLL
//

#pragma once
#include "TestModel.h"
#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

extern "C" CompiledInterface* PASCAL EXPORT GenModel();
extern "C" void PASCAL EXPORT DisposeModel(CompiledInterface* pMdlInt );
// CModelProjectApp
// See ModelProject.cpp for the implementation of this class
//

class CModelProjectApp : public CWinApp
{
public:
	CModelProjectApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
