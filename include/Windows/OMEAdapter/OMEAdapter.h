#pragma once

#include <EnvExtension.h>
#include "OMEDefines.h"
#include "EnvSpatialDataProvider.h"
#include "SimManager.h"
#include "FDATAOBJ.H"
#include "Eventable.h"

#include "DebugOutput.h"

/*---------------------------------------------------------------------------------------------------------------------------------------
* general idea:
*    1) Create subclasses of EnvEvalModel, EnvAutoProcess for any models you wnat to implement
*    2) Add any exposed variables by calling
*         bool EnvExtension::DefineVar( bool input, int index, LPCTSTR name, void *pVar, TYPE type, MODEL_DISTR distType, VData paramLocation, VData paramScale, VData paramShape, LPCTSTR desc );
*         for any exposed variables
*    3) Override any of the methods defined in EnvExtension classes as needed
*    4) Update dllmain.cpp
*    5) Update *.def file
*
*
*  Required Project Settings (<All Configurations>)
* 
*  Right-click on your project in Visual Studio’s Solution Pane and select <Properties>.  This allows you to
*  set various project settings.  Change the following, being sure to select <All Configurations>.
*  
*  Required Project Settings (<All Configurations>)
*   1) General->Character Set: Not Set
*   2) C/C++ ->General->Additional Include Directories: ..\;..\libs;    
*   3) Linker->General->Additional Library Directories:  $(SolutionDir)$(ConfigurationName)
*   4) Linker->Input->Additional Dependencies: libs.lib
*   
*  For each configuration (Debug and Release):
*   1) C/C++ ->Preprocessor->Preprocessor Definitions: add "__EXPORT__=__declspec( dllimport )";  Do not include the quotes!
*---------------------------------------------------------------------------------------------------------------------------------------
*/


//typedef CArray<OME_SCALAR,OME_SCALAR> ConstArray;
//typedef ARRAY_TYPE<OME_SCALAR> ConstArray;		//use STL instead of CArray for copy constructor.

/** Instance record for OME process*/
struct OMEAInst
   {
   SimManager *m_pMgr;					  ///< Pointer to the associated SimManager instance.
   OME_SCALAR* m_intermedArray;			  ///< Array used to interchange values between OME and Envision.
   EnvSpatialDataProvider* m_pProvider;   ///< Envision spatial data provider that relies on MapLayer.
   RecordRepArray m_recs;				  ///< Array of representative records for OME model components.

   OMEAInst() : m_intermedArray(NULL), m_pMgr(NULL),m_pProvider(NULL) {}

   };

/** Instance map for OMEProcess. */
typedef CMap<int,int,OMEAInst,OMEAInst> InstMap;

/** Autonomous process for running OME in Envision. */
class OMEProcess : public EnvAutoProcess, public Eventable
   {
   public:

	  OMEProcess() : EnvAutoProcess( 0, 0 ) { DBG_ALT_PRINT_FUNC(DebugPrint); }
	  ~OMEProcess( void );

	  virtual BOOL Init   ( EnvContext *pEnvContext, LPCTSTR initStr );    
	  virtual BOOL InitRun( EnvContext *pEnvContext, bool useInitialSeed );
	  virtual BOOL Run    ( EnvContext *pContext );                        
	  virtual BOOL Setup  (EnvContext *pContext, HWND hWnd);
	  virtual BOOL EndRun ( EnvContext *pContext );

	  //for pref pane
	  //virtual BOOL Setup(EnvContext *pContext, HWND hWnd)      { return FALSE; }
	  /** @return Pointer to the last EnvContext used. */
	  inline EnvContext* GetLastContext() {return m_pCurrContext;}

	  //for catching reported errors
	  virtual void EventAction(const CentralEvent & ev, void* extra);
	  virtual void* GetEventExtra() { return m_pCurrContext; }
	  virtual bool RefreshExtraOnCall() const { return true; }

   protected:

	  InstMap m_instMap;             ///< The instance map associated with the process.
	  EnvContext* m_pCurrContext;	 ///< The Envision context to operate against.


	  static void BindReadableVars(OMEAInst & inst);
	  static void SyncDeltas(const int row, const int col, const OME_SCALAR val, void* extra);
	  static void DebugPrint(const TCHAR* message, bool noEnd,const OMEDBG_TYPE type);
   };
