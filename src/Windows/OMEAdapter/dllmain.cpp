// dllmain.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxwin.h>
#include <afxdllx.h>

#include "EnvContext.h"

#include "OMEAdapter.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//MyEvalModel    *theModel = NULL;
OMEProcess  *theProcess = NULL;

static AFX_EXTENSION_MODULE OMEAdapterDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("OMEAdapter.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(OMEAdapterDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(OMEAdapterDLL);
     
      /*** TODO:  instantiate any models/processes ***/
      //ASSERT( theModel == NULL );
      //theModel = new MyEvalModel;
      //ASSERT( theModel != NULL );

      ASSERT( theProcess == NULL );
      theProcess = new OMEProcess;
      ASSERT( theProcess != NULL );
	}
    
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("OMEAdapter.DLL Terminating!\n");

      /**** TODO: Delete any instantiated models ****/
      //if ( theModel != NULL)
      //   delete theModel;

      if ( theProcess != NULL)
         delete theProcess;

		// Terminate the library before destructors are called
      /**** TODO: Update module name in AfxTermExtensionModule ****/
		AfxTermExtensionModule(OMEAdapterDLL);
	}
	return 1;   // ok
}

extern "C" void PASCAL EXPORT GetExtInfo( ENV_EXT_INFO *pInfo );

/**** TODO: Comment out any unneeded interfaces ****/
// for eval model
//extern "C" BOOL PASCAL EXPORT EMInit( EnvContext *pContext, LPCTSTR initStr );
//extern "C" BOOL PASCAL EXPORT EMInitRun( EnvContext *pContext, bool useInitialSeed );
//extern "C" BOOL PASCAL EXPORT EMRun( EnvContext *pContext );
//extern "C" BOOL PASCAL EXPORT EMEndRun( EnvContext *pContext );
//extern "C" BOOL PASCAL EXPORT EMSetup( EnvContext *pContext, HWND hWnd );
//extern "C" int  PASCAL EXPORT EMInputVar( int modelID, MODEL_VAR** modelVar );
//extern "C" int  PASCAL EXPORT EMOutputVar( int modelID, MODEL_VAR** modelVar );

// for autonomous processes
extern "C" BOOL PASCAL EXPORT APInit( EnvContext *pEnvContext, LPCTSTR initStr );
extern "C" BOOL PASCAL EXPORT APInitRun( EnvContext *pEnvContext, bool useInitialSeed );
extern "C" BOOL PASCAL EXPORT APRun( EnvContext *pEnvContext );
extern "C" BOOL PASCAL EXPORT APEndRun( EnvContext *pEnvContext );
extern "C" BOOL PASCAL EXPORT APSetup( EnvContext *pContext, HWND hWnd );
extern "C" int  PASCAL EXPORT APInputVar( int modelID, MODEL_VAR** modelVar );
extern "C" int  PASCAL EXPORT APOutputVar( int modelID, MODEL_VAR** modelVar );

// for visualizers
//extern "C" BOOL PASCAL EXPORT VInit        ( EnvContext*, LPCTSTR initInfo ); 
//extern "C" BOOL PASCAL EXPORT VInitRun     ( EnvContext*, bool ); 
//extern "C" BOOL PASCAL EXPORT VRun         ( EnvContext* );
//extern "C" BOOL PASCAL EXPORT VEndRun      ( EnvContext* );
//extern "C" BOOL PASCAL EXPORT VInitWindow  ( EnvContext*, HWND );
//extern "C" BOOL PASCAL EXPORT VUpdateWindow( EnvContext*, HWND );
//extern "C" BOOL PASCAL EXPORT VSetup       ( EnvContext*, HWND );

/////////////////////////////////////////////////////////////////////////////////////
///////////               E X T E N S I O N     I N F O                 /////////////
/////////////////////////////////////////////////////////////////////////////////////

void PASCAL GetExtInfo( ENV_EXT_INFO *pInfo ) 
   { 
   // TODO:  update as needed
   pInfo->types = EET_AUTOPROCESS;  // & EET_EVALMODEL;
   pInfo->description = "AutoProcesses for hosting OME-based models in Envision";
   }


/////////////////////////////////////////////////////////////////////////////////////
///////////               A U T O N O M O U S    P R O C E S S          /////////////
/////////////////////////////////////////////////////////////////////////////////////

BOOL PASCAL APInit( EnvContext *pEnvContext, LPCTSTR initStr )    { return theProcess->Init( pEnvContext, initStr ); }
BOOL PASCAL APInitRun( EnvContext *pEnvContext, bool useInitSeed ){ return theProcess->InitRun( pEnvContext, useInitSeed ); }
BOOL PASCAL APRun( EnvContext *pEnvContext )                      { return theProcess->Run( pEnvContext );  }
BOOL PASCAL APEndRun( EnvContext *pEnvContext )                   { return theProcess->EndRun( pEnvContext );  }
BOOL PASCAL APSetup( EnvContext *pContext, HWND hWnd )            { return theProcess->Setup( pContext, hWnd ); }
int  PASCAL APInputVar( int id, MODEL_VAR** modelVar )            { return theProcess->InputVar( id, modelVar ); }
int  PASCAL APOutputVar( int id, MODEL_VAR** modelVar )           { return theProcess->OutputVar( id, modelVar ); }


/////////////////////////////////////////////////////////////////////////////////////
//////////////      E V A L     M O D E L            ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

//BOOL PASCAL EMInit( EnvContext *pEnvContext, LPCTSTR initStr )    { return theModel->Init( pEnvContext, initStr ); }
//BOOL PASCAL EMInitRun( EnvContext *pEnvContext, bool useInitSeed ){ return theModel->InitRun( pEnvContext, useInitSeed ); }
//BOOL PASCAL EMRun( EnvContext *pEnvContext )                      { return theModel->Run( pEnvContext );  }
//BOOL PASCAL EMEndRun( EnvContext *pEnvContext )                   { return theModel->EndRun( pEnvContext );  }
//BOOL PASCAL EMSetup( EnvContext *pContext, HWND hWnd )            { return theModel->Setup( pContext, hWnd ); }
//int  PASCAL EMInputVar( int id, MODEL_VAR** modelVar )            { return theModel->InputVar( id, modelVar ); }
//int  PASCAL EMOutputVar( int id, MODEL_VAR** modelVar )           { return theModel->OutputVar( id, modelVar ); }


/////////////////////////////////////////////////////////////////////////////////////
///////////                        V I S U A L I Z E R                  /////////////
/////////////////////////////////////////////////////////////////////////////////////

//BOOL PASCAL VInit        ( EnvContext *pEnvContext, LPCTSTR initInfo )    { return theViz->Init( pEnvContext, initInfo ); }
//BOOL PASCAL VInitRun     ( EnvContext *pEnvContext, bool useInitialSeed ) { return theViz->InitRun( pEnvContext, useInitialSeed ); }
//BOOL PASCAL VRun         ( EnvContext *pEnvContext )                      { return theViz->Run( pEnvContext );  }
//BOOL PASCAL VEndRun      ( EnvContext *pEnvContext )                      { return theViz->EndRun( pEnvContext );  }
//BOOL PASCAL VInitWindow  ( EnvContext *pEnvContext, HWND hWnd )           { return theViz->InitWindow( pEnvContext, hWnd );  }
//BOOL PASCAL VUpdateWindow( EnvContext *pEnvContext, HWND hWnd )           { return theViz->UpdateWindow( pEnvContext, hWnd );  }
//BOOL PASCAL VSetup       ( EnvContext *pEnvContext, HWND hWnd )           { return theViz->Setup( pEnvContext, hWnd );  }

/**** TODO:  Update *.def file exports to match what is exposed here ****/
