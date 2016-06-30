// ModelProject.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ModelProject.h"
#include "TestModel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern "C" CompiledInterface* PASCAL EXPORT GenModel()
{
	return new OME_MDL_CLASS();
}

extern "C" void PASCAL EXPORT DisposeModel(CompiledInterface* pMdlInt )
{
	delete pMdlInt;
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

#undef OME_MDL_CLASS
#undef OME_MDL_NAME