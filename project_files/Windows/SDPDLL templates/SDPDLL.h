#pragma once
//#ifndef __AFXWIN_H__
//#error "include 'stdafx.h' before including this file for PCH"
//#endif

#include "ISpatialDataProvider.h"

extern "C" ISpatialDataProvider* PASCAL EXPORT GenSDP(SimManager* pSim,Model* pRoot = NULL, TI2Element* pSDPElem = NULL, const char* modelPath = "");
extern "C" void PASCAL EXPORT DisposeSDP(ISpatialDataProvider* pSDP);
extern "C" const char* PASCAL EXPORT GetSDPLabel();

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