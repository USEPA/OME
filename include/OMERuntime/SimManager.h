#pragma once
#include "omeobjects/SimObjs.h"
#include "BaseManager.h"
#include "OMEIntegrationBlock.h"
#include "InitializerSet.h"
#include "BaseEvalSerializer.h"
#include "CompiledInterface.h"

/** Class used to manage model elements and organize simulation runs.*/
class __EXPORT__ SimManager : public BaseManager
{
public:
	/** SimManager behavior modifier flags. */
	enum
	{
		NO_FLAGS=0,				///< No additional modifier flags.
		IS_SINGLETON=0x1,		///< If set, only one universal SimManager instance.
		NO_DEBUG=0x2,			///< If set, do not use debugging-related features.
		NO_SDP=0x4,				///< If set, ignore SDP-related features.
		NO_OVERRIDES=0x8,		///< If set, ignore outside overrides for model values.
		NO_COVERAGE=0x10,		///< If set, ignore general spatial coverage related features.
		NO_PARSING=0x20,		///< If set, do not perform any model logic parsing.
		NO_SERIALIZING=0x40,	///< If set, do not perform model serialization processing.
		NO_COMPILED=0x80,		///< If set, do not link to any compiled model code, even if directed to by a control file.
		NO_FULL_RECORD=0x100	///< If set, all RecordMgrs use minimum record keeping.
	};

	/** Identifiers for default solver options.*/
	enum SOLVER_TYPE
	{
		DEFAULT=0,		///< Use basic solver.
		HIRES,			///< Use high-resolution solver.

		SOLVER_COUNT	///< Total number of solver options.
	};
	
	SimManager(const bool isSingleton=false);
	//SimManager(Model* rootModel,OME_SCALAR start=0.0, OME_SCALAR stop=0.0, OME_SCALAR interval=0.0,OME_SCALAR stepSize=1.0);

   SimManager(STLString modelPath,OME_SCALAR start=0.0, OME_SCALAR stop=0.0, OME_SCALAR interval=0.0,OME_SCALAR stepSize=1.0,unsigned int flags=IS_SINGLETON);
   SimManager(CompiledInterface* pInterface, const STLString & cachePath, OME_SCALAR start, OME_SCALAR stop, OME_SCALAR interval, OME_SCALAR stepSize, unsigned int flags = IS_SINGLETON);

   /** Copy Constructor.
	  @param sm The SimManager to copy.
   */
	SimManager(const SimManager & sm) { Duplicate(sm); }
	/** Assignment Operator.
		@param rhs The SimManager to copy.
		@return Reference to the SimManager.
	*/
	SimManager& operator=(const SimManager & rhs) { Duplicate(rhs); return *this; }

	~SimManager(void);
	virtual bool InitSim();
	virtual bool PrepSimulation();
	virtual void RunSimulation();
	virtual void RegisterModel(Model* pMdl);
	virtual void MarkVariableForTracking(Evaluable* pEval);

	virtual void EventAction(const CentralEvent & ev, void* extra);

	/// \todo rename update / init functions to better reflect their current behaviors.
	virtual void UpdatePriorities();
	virtual void UpdateSupportObjects(const OME_SCALAR time);

   /** @return Pointer to any user-defined data assigned by SetExtra().*/
	virtual void* GetExtra() const { return m_extra; }
   /** @param ex Pointer to data to be stored as the extra value.*/
	virtual void SetExtra(void* ex) { m_extra=ex; }

//	virtual void SetStepSize(const OME_SCALAR i) { m_integrator.m_timeStep = i; BaseManager::SetStepSize(i); }
   EvalRepArray GetReadableObjectList(const DT_TYPE & flags=DT_FULL);
   EvalRepArray GetConstantObjectList();

   bool GetReadableValue(const STLString id, OME_SCALAR & outVal);
   bool GetReadableValue(const EvalRep & rep, OME_SCALAR & outVal);

   bool SetConstantValue(STLString id, const OME_SCALAR inVal);
   bool SetConstantValue(const EvalRep & rep, const OME_SCALAR inVal);

   const OME_SCALAR* GetReadableRef(STLString id);
   const OME_SCALAR* GetReadableRef(const EvalRep & rep);

   /** @return true if cache files are being written to disk; false otherwise.*/
	inline bool GetUsingFileCache() const { return m_useFileCache; }
	/** Disables the record file cache.
		@param cachePath Valid path to a directory to hold temporary record cache files.
	*/
	void EnableFileCache(const STLString & cachePath=STLString());
	/** Disables the record file cache.*/
	inline void DisableFileCache() { m_useFileCache=false; }

   virtual void PreloadSubmodels();
   virtual void SetEvalParser(OMEParser* pPrsr, const bool & isPrioritized = false) { BaseManager::SetEvalParser(pPrsr, isPrioritized); m_serializer = pPrsr->GenerateSerializer(); }

   /** Enable or disable script debugging.
		@param dbg true will enable script debugging; false will disable script debugging.
   */
   inline void SetScriptDebugging(const bool & dbg) { m_serializer->SetDbgEnabled(dbg); }
   /** @return true if script debugging is enabled; false otherwise.*/
   inline bool GetScriptDebugging() const { return m_serializer->GetDbgEnabled(); }
   /// \question should the following SDPXML functions be moved to BaseManager?
   void ReadCoverageMappingXML(TI2Element *pCovElem);
   bool ReadMappingEntryXML(TI2Element *pXMLVarInfo, SDPMapEntry & newEntry, const SDPMapSubEntry::ENTRY_FLAGS & actFlags);
   bool ReadMappingEntryXML(TI2Element *pXMLVarInfo, SDPMapEntry & newEntry, const STLString & nameOrId, const bool & isName, const SDPMapSubEntry::ENTRY_FLAGS & actFlags);
   void ReadParamXML(TI2Element *pXml,const STLString & mPath);
   void ReadModelXML(TI2Element *pXml,const STLString & mPath);
   void ReadSDPXML(TI2Element *pXml,const STLString & mPath);
   void ReadDBGXML(TI2Element *pXml,const STLString & mPath);
   void ReadModelOverridesXML(TI2Element *pXml);
   //virtual void SetSpatialDataProvider(ISpatialDataProvider* pSdp,SubmodelSDPEntryMap & eMap=SubmodelSDPEntryMap());

   void PopulateFromXMLTree(TI2Element* pXmlRoot, const STLString & modelPath, unsigned int & flags, oSTLSStream & errStrm);

   bool ExportLoadedValuesToCSV(const STLString & path);
   bool ExportAllValuesToCSV(const STLString & path);
   /** @param func Function used to handle parameter initialization.*/
   inline void SetInitParamFunc(ManagerInitVarParamFunction func) { m_varParamFunc = func; }
   /** @param extra Optional pointer to additional data to pass to the function set by SetInitParamFunc().*/
   inline void SetInitParamExtra(void* extra){ m_varParamExtra = extra; }
   /** Enable or disable minimum record keeping.
		@param minRec The flag indicating minimal record keeping is enabled or disabled.
   */
   inline void SetMinimalRecords(const bool & minRec) { m_noRecord = minRec; }
   /** @return true if Minimal record keeping is enabled; false otherwise.*/
   inline bool GetMinimalRecords() const { return m_noRecord; }

   /** Set boundary increment behavior.
		@param itb Whether or not to use partial increments to step all the way to temporal boundaries.
   */
   inline void SetIncToBounds(const bool & itb) { m_incToBounds = itb; }
   /** @return whether or not SimManager increments to temporal boundary if it does not line up with integration step.*/
   inline bool GetIncToBounds() const { return m_incToBounds; }

   bool AddUserParameter(const Param & p);

   void SetSolverType(const SOLVER_TYPE & t);
   /** @return Identifier of currently selected solver type.*/
   inline SOLVER_TYPE GetSolverType() const { return m_solveType; };

protected:
	

	std::vector<Model*> m_svModels;					///< List of Models containing StateVars that will be integrated.
	EvalSet m_svDependents;							///< Evaluble objects that need to be updated in lockstep with state variables. 

	std::map<STLString,InitializerSet> m_inits;			///< Values loaded from a parameter file.

	BaseEvalSerializer* m_serializer;					///< Object used to serialize evaluable expressions.

	static void GetDerivatives( OME_SCALAR time, OME_SCALAR timestep, int svCount, OME_SCALAR *deriveArray, void* extra );
	static void PostIntegrateStep(OME_SCALAR time, OME_SCALAR timestep, void* extra);
	/** Function to be called when state variable addresses need to be refreshed.
		@param extra Pointer to any additional data to be referenced within the function. In this case, it is the targetted SimManager.
	*/
	static void UpdateStateVarAddrs(void* extra) { ((SimManager*)extra)->m_reInitIntegrator = true; }
	OMEIntegrationBlock* m_pIntegrator;					///< Object that handles state variable integration.
	SOLVER_TYPE m_solveType;						///< The current OMEIntegrationBlock type in use.

	void* m_extra;									///< for use with MultiSimManagers.
	bool m_useFileCache;							///< If true, use directory at m_cachePath to store record cache files.
	STLString m_cachePath;							///< Path to cache storage directory; only used if m_useFileCache is true.
	
	void Duplicate(const SimManager & sm);

	void InitIntegrator();
	void ApplyModelInits(Model* pRoot);

	virtual StrList GetDefaultEventTypes();
	virtual void DisposeSDP(ISpatialDataProvider* pSDP);

	void FlushErrors(ErrCollection & errs,const OME_SCALAR time);

	//virtual void RegisterDefaultListeners();
	//virtual void UnregisterDefaultListeners();

	//debug stuff
	STLString m_evalBatchPath;		///< Optional path to where an eval batch statement is written.
	STLString m_initBatchPath;		///< Optional path to where an init batch statement is written.
	bool m_writeEvalBatch;			///< Write eval batch statements to designated file if true.
	bool m_writeInitBatch;			///< Write init batch statements to designated file if true.
	bool m_reInitIntegrator;		///< If true, InitIntegrator needs to be called to refresh the OMEIntegrationBlock.
	bool m_noRecord;				///< If true, minimum record keeping will be used for all RecordMgrs.
	bool m_incToBounds;				///< If true, a partial step is initiated to temporal boundary if it doesn't match the integration interval.

	ManagerInitVarParamFunction m_varParamFunc;			///< Optional function used to handle populating uninitialized Parameters
	void* m_varParamExtra;								///< Optional additional data to pass to m_varParamFunc when invoked.

	static void DefaultInitParamFn(EvalArray & evals, void* extra);

	//sdp dll stuff 
	typedef ISpatialDataProvider* (PASCAL *GenSDPFunc)(SimManager*,Model*, TI2Element*, const char*);	///< Function prototype for retrieving SDP from external lib.
	typedef void (PASCAL *DisposeSDPFunc)(ISpatialDataProvider*);		///< Function prototype for retrieving SDP cleanup code from external lib.
	typedef const char* (PASCAL *GetSDPLabelFunc)();				///< Function prototype for retrieving SDP label.

	GenSDPFunc GenSDPFromLib;										///< Pointer to function to call for creating instance of SDP.
	DisposeSDPFunc DisposeSDPFromLib;									///< Pointer to function to call for cleaning up after SDP.
	GetSDPLabelFunc GetSDPLabelFromLib;									///< Pointer to function to retrieve SDP label

	void* m_dlHandle;											///< Handle for dynamic library used in POSIX environments.

	bool LoadSDPLib(const STLString & libPath);

};

