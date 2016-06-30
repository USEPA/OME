#pragma once
#include "OMEDefines.h"
#include "omeobjects/Param.h"
#include "OMEIntegrationBlock.h"
#include "events/Eventable.h"
#include "events/CentralEvent.h"
#include "ISpatialDataProvider.h"
#include "Units.h"
#include "ValueArchive.h"
#include "SDPMapEntry.h"

//#include "Model.h"
#include "ErrCollection.h"

class CentralEventDispatcher;

/** Class used to manage model elements and organize simulation runs.*/
class __EXPORT__ BaseManager : public Eventable
{
public:

	BaseManager(const bool isSingleton=false);
	BaseManager(Model* rootModel,OME_SCALAR start=0.0, OME_SCALAR stop=0.0, OME_SCALAR interval=0.0, OME_SCALAR stepSize=1.0,const bool isSingleton=true);
	BaseManager(const BaseManager & bm);

	/** Assignment Operator.
		@param rhs The right-hand side argument for the operator.
	*/
	BaseManager& operator=(const BaseManager & rhs) { Duplicate(rhs); return *this; }

	virtual ~BaseManager(void);

	void SetRootModel(Model* rmdl); 
	Model* GetRootModel() const { return m_pRootModel; }		///< @return Pointer to root model.

	/** Sets the simulation start time.
		@param st Time at which the model starts.
	*/
	void SetStartTime(const OME_SCALAR st) { m_startTime=st; }
	OME_SCALAR GetStartTime() const { return m_startTime; }		///< @return Simulation's start time.

	/** Sets the simulation stop time.
		@param st Time at which the model stops.
	*/
	void SetStopTime(const OME_SCALAR st) { m_stopTime=st; }
	OME_SCALAR GetStopTime() const { return m_stopTime; }		///< @return Simulation's stop time.

	/** @return Current time of simulation run. */
	OME_SCALAR GetPresentTime() const { return m_currentTime;/*m_eventDispatcher->GetPresentTime();*/ }

	/** Sets the simulation time interval.
		@param i The interval between time steps.
	*/
	virtual void SetInterval(const OME_SCALAR i) { m_interval=i; }
	OME_SCALAR GetInterval() const { return m_interval; }		///< @return Interval between time steps.

	/** Sets the integration step size.
		@param s The size of the integration step.
	*/
	virtual void SetStepSize(const OME_SCALAR s) { m_stepSize=s; }
	OME_SCALAR GetStepSize() const { return m_stepSize; }		///< @return Integration step size.

	/** Check to see if BaseManager is in the process of integration
		@return true if in integration process, false otherwise.
	*/
	virtual bool GetHasStartedIntegrate() const { return m_startedIntegrate; }

	/** Sets the Integration method to use throughout the simulation.
		@param im The integration method to use.
		Available integration methods:
		- __IM_EULER:__ Use Euler's integration method to solve for state variables.
		- __IM_RK4:__ Use the 4th order Runge-Kutta method to solve for state variables.
	*/
	void SetIntMethod(const OMEIntegrationBlock::INT_METHOD im) { m_intMethod=im; }
	OMEIntegrationBlock::INT_METHOD GetIntMethod() const { return m_intMethod; }			///< @return Currently used integration method.

	/** Sets the result function for the model.
		@param func The result function (applied to model).
	*/
	void SetModelReportFunction(const ModelReportFunction & func) { m_mdlRepFunc=func;}

	/** Retrieve the model report function.
		@return Function pointer to current Model Report Function.
	*/
	const ModelReportFunction GetModelReportFunction() const { return m_mdlRepFunc; }

	/** Set the Report Function used by the manager.
		@param func The manager report function.
		@param extra Optional pointer to additional data to pass to func on call.
	*/
	void SetManagerReportFunction(const ManagerReportFunction & func,void* extra=NULL) { m_resFunc=func; m_mgrRprtExtra=extra;}
	
	/** Initialize simulation, setting all objects to their initial values. Expected to call PrepSimulation.
		@return true if initialization successful, false otherwise.*/
	virtual bool InitSim() = 0;

	/** Prepare Simulation for run.
		@return true if prep was successful, false otherwise.*/
	virtual bool PrepSimulation()=0;

	/** Run the model simulation between the previously set start and end times. 
		Expected to call InitSim if necessary.*/
	virtual void RunSimulation()=0;
	
	/** Mark a variable to be recorded for record-keeping practices.
		This is typically used for previous value lookups during simulations running with minimal records.
		@param pEval Pointer to the Evaluable to mark for tracking.
	*/
	virtual void MarkVariableForTracking(Evaluable* pEval)=0;

	virtual void EventAction(const CentralEvent & ev, void* extra);


	void DispatchEvent(CentralEvent & ce);
	void PostEvent(CentralEvent & ce);

	//virtual bool AddParameter(const Param & parm);
	/** Clears the Parameter list.
	*/
	//void ClearParameters() { m_simParamArray.clear(); };

   virtual void SetSpatialDataProvider(ISpatialDataProvider* pSdp,const SubmodelSDPEntryMap & eMap,const bool & ownIt=false);
   virtual void SetSpatialDataProvider(ISpatialDataProvider* pSdp,const bool & ownIt=false);

   /** @return Pointer to the associated SpatialDataProvider. */
	inline ISpatialDataProvider* GetSpatialDataProvider() {return m_pSdp;}
   /** Set the units used for time.
		@param timeType A UNIT_MEASURE the represents a period of time. 
	*/
   inline void SetTimeUnits(const UNIT_MEASURE timeType) { m_timeUnits=timeType; }
   /** Get the units used for time.
		@return A UNIT_MEASURE representing the time units used. 
	*/
   UNIT_MEASURE GetTimeUnits() const {return m_timeUnits;}
	static void SortByPriority(OMEArray & out, bool useInitPriority=false);
	static void SortByPriority(EvalArray & out, bool useInitPriority=false);
	static void SortByPriority(EvalSet & out, bool useInitPriority=false);

	/* Register StateVars with integration block.
		@param svars An array of StateVars to register.
		@param supports An array of Evaluables that must be updated each time the objects in svars are evaluated.
	*/
	//virtual void RegisterVars(SVArray & svars, EvalArray & supports)=0;
	/** Registers a model for tracking in the manager.
		@param mdl The Model to register.
	*/
	virtual void RegisterModel(Model* mdl)=0;

	virtual void PreloadSubmodels(const STLString & cacheDir=STLString());
	//RecordRepArray GetReadableRecords();
	void UpdateRecords(RecordRepArray & records);

   void SyncCoverage(CoverageSyncFunction syncFunc=NULL,void* pfuncExtra=NULL);

   /** @return true if the Simulation is running; false otherwise. */
   bool IsRunning() const { return m_isRunning;}

   /** Reports an error to the BaseManager which is then broadcast through the event system to any relevant listeners. 
		@param rhs A description of the error.
		@param pObj Optional pointer to the object that generated or reported the error.
		@param eType The type/urgency of the error.
		@param addFront Add to the front of the error queue instead of the end.
	*/
   void ReportError(const STLString & rhs, OMEObject* pObj=NULL,EC_ERR_TYPE eType=EC_CRITICAL,const bool addFront=false) 
		{ m_errors.AddErr(rhs,eType,pObj,addFront); }
   /** Reports an error to the BaseManager which is then broadcast through the event system to any relevant listeners. 
		@param rhs A description of the error.
		@param pObj Optional pointer to the object that generated or reported the error.
		@param eType The type/urgency of the error.
		@param addFront Add to the front of the error queue instead of the end.
	*/
   void ReportError(const OMEChar* rhs, OMEObject* pObj=NULL,EC_ERR_TYPE eType=EC_CRITICAL,const bool addFront=false) 
		{ m_errors.AddErr(rhs,eType,pObj,addFront); }

   /** Report several errors at once. 
		@param errs The collection of errors to report.
   */
	inline void ReportErrors(const ErrCollection & errs) 
		{ m_errors+=errs; }


	void RegisterErrListener(Eventable* listener);
	void RegisterIncListener(Eventable* listener);
	void RegisterPauseListener(Eventable* listener);

   /** Update the eval list to reflect current priorities.
	*/
   virtual void UpdatePriorities()=0;
	/** Evaluate all supporting objects.
		@param time The current timestep.
	*/
	virtual void UpdateSupportObjects(const OME_SCALAR time)=0;

	/** @return pointer to current OMEParser subclass defined by EVAL_PARSER. */
   inline OMEParser* GetPEvalParser() const {return m_pEvalParser;}

   /** Set the visibility rules for the contained model.
		@param rule The visibility rule or rules for the contained model.*/
   inline void SetVisRule(const STLString & rule) { m_visRule=rule;}
	/** Set the visibility rules for the contained model.
	@param rule The visibility rule or rules for the contained model.*/
	inline void SetVisRule(const OMEChar* rule) { m_visRule=STLString(rule);}
	/** @return The current visibility rule(s) for the contained model. */
   inline STLString GetVisRule() const {return m_visRule;}

   /** @return true if Records written to disk will be retained after run; false otherwise.*/
   inline bool GetKeepRecords() const { return m_keepRecords; }
   /** Set whether or not records will be retained after a simulation run is finished. 
		@param keep If true, keep records after run; otherwise, discard records when run completes.
   */
   inline void SetKeepRecords(const bool & keep) { m_keepRecords=keep; }

   /** Assign EvalParser to be associated with this manager.
		@param pPrsr Pointer to the parser to associate.
		@param isPrioritized Flag indicating whether or not the objects associated with the parser have already prioritized. 
			Passing a value of false will cause the BaseManager to ask the parser to prioritize objects as part of the initialization process.
   */
   virtual void SetEvalParser(OMEParser* pPrsr, const bool & isPrioritized = false) { m_pEvalParser = pPrsr; m_prioritized = isPrioritized; m_externalEvalParser = true; }
	/** Set mode label for Expression parser.
		@param mode The label for the current mode.
	*/ 
   inline void PushParserModeID(const STLString & mode) { if(m_pEvalParser) m_pEvalParser->PushModeID(mode);}
	/** Return Expression parser Mode to previous value.
		@return Restored Mode label.
	*/
   inline STLString PopParserModeID() { return m_pEvalParser ? m_pEvalParser->PopModeID() : STLString();}

	 static bool CompareEvalObject(const OMEObject* lhs, const OMEObject* rhs);
	static bool CompareInitObject(const OMEObject* lhs, const OMEObject* rhs);

	/** @return Pointer to Singleton manager if it exists; otherwise NULL. */
	static BaseManager* GetSingleton();

	/** Free memory reserved for reporting out of memory conditions. */
	inline void ClaimReserveMem() { if(m_reserveMem) {delete[] m_reserveMem; m_reserveMem=NULL;} }

	/** Utility function to ensure that models exist on the same side of the library boundaries as the manager object.
		@param typ OMETYPE to request;
		@return A newly allocated object of type T using the default constructor. The memory is allocated in the same space that the manager exists in. 
	*/
	static OMEObject* GenModelObject(const OMETYPE & typ);

	STLString GetExecMode();
	void LoadValuesForTime(const OME_SCALAR & time);
protected:
	bool m_prioritized;								///< True if internal objects have been prioritized; false otherwise.
	Model* m_pRootModel;							///< The model that acts as the entry point to the simulation run.
	ModelList m_MCList;								///< List of all Model objects found in submodel; used for spawn tracking.

	CentralEventDispatcher* m_eventDispatcher;		///< Pointer to the eventDispatcher that runs the simulation. Typically this points to CED_singleton.

	ManagerReportFunction m_resFunc;				///< Optional function to call when reporting model status.
	ModelReportFunction m_mdlRepFunc;				///< Optional function to call when reporting manager status.

	OME_SCALAR m_startTime;								///< Time at which simulation starts.
	OME_SCALAR m_stopTime;								///< Time at which simulation stops.
	OME_SCALAR m_interval;								///< Interval between time steps.
	OME_SCALAR m_stepSize;								///< Integration step size.
	OME_SCALAR m_currentTime;							///< Current time in simulation.
	OMEIntegrationBlock::INT_METHOD m_intMethod;	///< Integration method to use with m_pIntegerator.

	//ARRAY_TYPE<Param> m_simParamArray;				///< Parameters that are applied to the model from the outside.
	bool m_startedIntegrate;						///< Flag indication whether or not SimManager is performing integrations.
	bool m_isRunning;								///< True if simulation is running; false otherwise.
	ISpatialDataProvider* m_pSdp;					///< Optional Spatial Data provider
	bool m_SDPisInternal;							///< if true, m_pSdp must be cleaned up by manager on destruction.
	bool m_externalEvalParser;						///< if true, the object pointed to by m_pEvalParser will not be deallocated during BaseManager's destruction.
   
	SubmodelSDPEntryMap m_sdpEntries;				///< Map of submodels that are used for strict input into Envision.

	UNIT_MEASURE m_timeUnits;					///< Units of time. Used to synchronize time steps with outside models (Envision).

	ErrCollection m_errors;						///< Object that Collects and stores any reported errors.
	OMEParser* m_pEvalParser;					///< Evaluative Parser used in simulation runs. The intention is for subclass to initialize the parser with the accessibility interface defined here.

	//moved from simManager
	STLString m_visRule;						///< The Visibility rule used by this manager.

	bool m_keepRecords;							///< If true, record/history files written to disk are not cleaned up when a run is finished.
	bool m_isPrepped;							///< Flag that tracks whether or not the simulation run has been prepared.

	void Duplicate(const BaseManager & bm);

	void FindMCs(Model* currModel, ModelList & out);
	 void InitSDP();
	 /** @return A list of EventTypes that the manager responds to. */
	 virtual StrList GetDefaultEventTypes() {return StrList();};

	 virtual void RegisterDefaultListeners();
	 virtual void UnregisterDefaultListeners();

	 virtual void DisposeSDP(ISpatialDataProvider* pSDP);

	 virtual void UnpackComponents();
	 void* m_mgrRprtExtra;						///< Optional pointer to be passed to m_resFunc when it is called.
	 void* m_mdlRprtExtra;						///< Optional pointer to be passed to m_mdlRepFunc when it is called.

	 ValueArchive m_staticValues;				///< Archive of values from static models.
	 ValueArchive m_dynamicValues;				///< Archive of values from dynamic models.

	 private:
		 char* m_reserveMem;		///< memory put aside for use in out of memory exception.
};

