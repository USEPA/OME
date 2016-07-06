// SDPDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "SDPDLL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern "C" ISpatialDataProvider* PASCAL EXPORT GenSDP(SimManager* pSim,Model* pRoot, TI2Element* pSDPElem, const char* modelPath);
{
	//return a new instance of the target model, parsing xml data for appropriate arguments
	//if pSDPElem is NULL, either return NULL (if xml is required) or default SDP implementation
	//NOTE: you will need to include tinyxml2.h and .cpp as provided in the OME distribution, or found here:
	// http://www.grinninglizard.com/tinyxml2/
}

extern "C" void PASCAL EXPORT DisposeSDP(ISpatialDataProvider* pSDP)
{
	//cleanup sdp passed out from the dll.
	//default handler
	delete pSDP;
}

extern "C" const char* PASCAL EXPORT GetSDPLabel()
{
	//label identifier for SDP module
	
}
//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CModelProjectApp

BEGIN_MESSAGE_MAP(CModelProjectApp, CWinApp)
END_MESSAGE_MAP()


// CModelProjectApp construction

CModelProjectApp::CModelProjectApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CModelProjectApp object

CModelProjectApp theApp;


// CModelProjectApp initialization

BOOL CModelProjectApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
