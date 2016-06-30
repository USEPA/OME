#pragma once
#include "OMEDefines.h"
#include "OMEObject.h"
#include "Param.h"
#include "Variable.h"
#include "StateVar.h"
#include "Flow.h"
#include "Influence.h"
#include "SimpleLabel.h"
#include "AssocPort.h"
#include "OMEParser.h"
#include "EvalTable.h"
#include "EvalAlias.h"

#include "ModelInstance.h"
#include "DataSrc.h"

#include "OMEIntegrationBlock.h"
#include "ValueArchive.h"

class SubmodelAssoc;
class VarArray;
class ISpatialDataProvider;

//#ifndef _OPENMP
#define OME_THREAD_COUNT	1
#define OME_CURR_THREAD 0
//#endif

/*
	BIT 0 : full(1) or shallow(0)
	BIT 1 : ask(1), or no ask(0)
	BIT 2 : include(1) or exclude(0) aliases
	BIT 3 : exclude(1)  or include(0) autogens
*/
enum  { DT_FULL=0x01
	, DT_SHALLOW=0x00
	, DT_ASK=0x02
	, DT_ALIAS=0x04
	, DT_NO_AUTOGEN=0x08
	, DT_CONSTS_ONLY=0x10
};

/** Model representation.
Used to represent both the root model and any additional submodels.
*/
class __EXPORT__ Model : public OMEObject
{   
	friend class DrawDetails;
	friend class RecordMgr;
public:
	/** Flag indicating the behavior of instance propagation. */
	enum INSTMODE { STATIC=0,	///< Instance count is set during initialization from model file, and does not change throughout simulation.
						RECORD,		///< Like Static, except the number of instances are determined by the number of entries in an external data source.
						DYNAMIC		///< The number of instances can change throughout a simulation run.
						};
	///@cond DOX_NO_DOC
    enum { TAG_TBL = 0, TAG_INTTBL, TAG_XINTTBL, TAG_VAR,  TAG_VA,   TAG_TS,   TAG_ITR,       TAG_STATE, 
           TAG_INFL,    TAG_FLOW,   TAG_LBL,     TAG_ASSC, TAG_SPWN, TAG_PORT, TAG_ASSOCPORT, TAG_ALIAS,
           TAG_CONVEYOR, 
           TAG_COUNT };
	///@endcond
#pragma region =========== Constructors ============================

	Model();
	Model(const STLString & id, const STLString & n=STLString());
	Model(TI2Element* pCurrElem,const STLString & tag, const STLString & filename,Model* parent=NULL);

	/** Copy constructor
	@param mdl the Model to copy.
	*/
	Model(const Model & mdl);
	/** Assignment operator.
	@param mdl The model to copy.
	@return A reference to the Model.
	*/
	Model& operator=(const Model & mdl) { Duplicate(mdl); return *this; };
	bool operator==(const Model & rhs);

	/** Destructor */
	virtual ~Model(void);


	virtual Model* Clone() { return new Model(*this); }
#pragma endregion
#pragma region =========== Initialization ===============
	
	virtual bool InitRun(const OME_SCALAR startTime, BaseManager* sm);
	void PreloadInstances();
	int JoinLinks(const bool & forceAssign=false,const bool & recurse=false);
	void JoinAliases();
	void JoinAliases(Model* pRootMdl);
	/** Add a DataFileSource to be used during initialization.
	@param dse The DataSourceEntry to add.
	*/
	void AddDataFileSource(const DataSourceEntry & dse) { m_dataFilePaths[dse.m_path]=dse; }
	/** @param em The DSEntryMap to assign to the Model. */
	void SetDataFileSources(const DSEntryMap & em) { m_dataFilePaths=em; }
	void FindColValForEval(const int & dSInd, const int & indCol, Evaluable* pEval);
#pragma endregion
#pragma region =========== Status Details ==========================
	/** Accessor for isRunning
	@return true if model is currently executing, false otherwise
	*/
	bool IsModelRunning() { return m_isRunning; }
	  /** Determine if instance count can change over duration of run. 
		@return true If there can be a variable number of instances, false otherwise.
			This function assumes that if there are any spawners associated with this model, then it
			is dynamic.
   */
	bool IsDynamic() const;
	bool IsConditional() const;
	/** @return true if model has spatial components; false otherwise.*/
	inline bool ExpectsSpatial() const { return m_expectsSpatial; }
	/** Set whether or not a Model expects to have its instance count determined by a SDP relationship.
		@param es If true, an association with an SDP is expected; otherwise, no expected relationship.
	*/
	inline void SetExpectsSpatial(const bool & es) { m_expectsSpatial = es; }
    bool IsMarkedBySpawner(int inst, Spawner* pSpwn);
	bool IsInIntegrate();
	
	bool IsMarkedBySpawner(Spawner* pSpwn);
	bool IsNested(const Model* pBm,ModelList* found=NULL) const;
	STLString GetNestedPath(const Model* pBm) const;
	void GetUpPath(StrList & outPath);
	void GetUpPath(STLString & outPath);
	bool IsAssociated(Model* pM) const;
	/** @return the number of iterations since the model's initial run  */
	inline unsigned int GetIterationCount() const { return m_iterations; }
	/** @return The current step size used by the model. */
		inline OME_SCALAR GetStepSize() const { return m_stepSize; }
		/** Set the current step size used by the model.
		@param stepSize The temporal step size to use.
		*/
		inline void SetStepSize(const OME_SCALAR & stepSize) { m_stepSize=stepSize; }
	/** @return The time at the start of the simulation. */
	inline OME_SCALAR GetInitTime() const { return m_initTime; }
	/** @return The index of the currently active model instance. */
	inline size_t GetCurrentInstance() const {return m_currentInstance[OME_CURR_THREAD]; }
	/** @return The current raw/global index of the currently active ModelInstance.*/
	inline size_t GetCurrentRawInstance() const { return m_currentInstance[OME_CURR_THREAD] + m_indMultiplier[OME_CURR_THREAD]; }
		virtual OMETYPE GetOMEType() const { return OME_MODEL; }
	virtual bool IsOMEType(OMETYPE type) const { return type==OME_MODEL ? true : OMEObject::IsOMEType(type); }

	virtual void EventAction(const CentralEvent & ev, void* extra);
	/** @return The instancing mode used by the model. */
	inline INSTMODE GetInstMode() const { return m_instMode; }
	/** @param im The instancing mode to assign to the model.*/
	inline void SetInstMode(const INSTMODE & im) { m_instMode=im; }
#pragma endregion
#pragma region =========== XML Stuff ===============================
	virtual STLString GetXMLRepresentation(const unsigned int indent=0, const unsigned int inc=4);
	virtual void GetXMLAttributes(StrArray & out) const;
	virtual void GetXMLSubNodes(StrArray & out, const unsigned int indent=0, const unsigned int inc=4);
	virtual const OMEChar* GetXMLTag() const { return "model"; }

	//model-specific xmls;
	void GetXMLNonDrawableSubNodes(StrArray & out, const unsigned int indent, const unsigned int inc);
	void GetXMLDrawableSubNodes(StrArray & out, const unsigned int indent, const unsigned int inc);

#pragma endregion

#pragma region =========== Report Stuff ============================
	virtual void Report(ModelReportFunction resFunc, OME_SCALAR reportTime);
	virtual const OMEChar* ToString(const OMEChar tabChar=' ', const unsigned int indent=0, const unsigned int inc=4) const;
#pragma endregion
#pragma region =========== queries for internal array sizes ========
	/** @return The number of Param objects stored within this model. */
	inline size_t GetParamCount() const { return m_paramArray.size();}
	/** @return The number of Variable objects stored within this model. */
	inline size_t GetVarCount() const { return m_varArray.size();}
	/** @return The number of StateVar objects stored within this model. */
	inline size_t GetStateVarCount() const { return m_stateVarArray.size();}
	/** @return The number of Flow objects stored within this model. */
	inline size_t GetFlowCount() const { return m_flowArray.size();}
	/** @return The number of Influence objects stored within this model. */
	inline size_t GetInfluenceCount() const { return m_influenceArray.size();}
	/** @return The number of Model objects stored within this model. */
	inline size_t GetSubmodelCount() const { return m_submodelArray.size();}
	/** @return The number of SubmodelPort objects stored within this model. */
	inline size_t GetSubPortCount() const { return m_portArray.size();}
	/** @return The number of AssocPort objects stored within this model. */
	inline size_t GetAssocPortCount() const { return m_assocPortArray.size(); }
	/** @return The number of Label objects stored within this model. */
	inline size_t GetLabelCount() const { return m_labelArray.size();}
	/** @return The number of IterConditional objects stored within this model. */
	inline size_t GetIterConditionalCount() const { return m_iterCondArray.size();}
	/** @return The number of SubmodelAssoc objects stored within this model. */
	inline size_t GetAssocCount() const { return m_subAssocArray.size(); }
	/** @return The number of Spawner objects stored within this model. */
	inline size_t GetSpawnerCount() const { return m_spawnerArray.size(); }
	/** @return The number of EvalAlias objects stored within this model. */
	inline size_t GetAliasCount() const { return m_aliasArray.size(); }

#pragma endregion
#pragma region =========== accessors for internal model components =
	/** Retrieve a specific Param, or NULL if i is out of bounds.
		@param i The index of the Param to retrieve.
		@return Pointer to the requested Param, or NULL if i is is invalid.
	*/
	inline Param& GetParam(const size_t i) { return m_paramArray[i]; }
	/** Retrieve a specific Variable, or NULL if i is out of bounds.
		@param i The index of the Variable to retrieve.
		@return Pointer to the requested Variable, or NULL if i is is invalid.
	*/
	inline Variable* GetVariable(const size_t i) { return i<m_varArray.size() ? m_varArray[i] : NULL; }
	/** Retrieve a specific StateVar, or NULL if i is out of bounds.
		@param i The index of the StateVar to retrieve.
		@return Pointer to the requested StateVar, or NULL if i is is invalid.
	*/
	inline StateVar* GetStateVar(const size_t i) { return i<m_stateVarArray.size() ? m_stateVarArray[i] : NULL; }
	/** Retrieve a specific Flow, or NULL if i is out of bounds.
		@param i The index of the Flow to retrieve.
		@return Pointer to the requested Flow, or NULL if i is is invalid.
	*/
	inline Flow* GetFlow(const size_t i) { return i<m_flowArray.size() ? m_flowArray[i] : NULL; }
	/** Retrieve a specific Influence, or NULL if i is out of bounds.
		@param i The index of the Influence to retrieve.
		@return Pointer to the requested Influence, or NULL if i is is invalid.
	*/
	inline Influence* GetInfluence(const size_t i) { return i<m_influenceArray.size() ? m_influenceArray[i] : NULL; }
	/** Retrieve a specific Model, or NULL if i is out of bounds.
		@param i The index of the Model to retrieve.
		@return Pointer to the requested Model, or NULL if i is is invalid.
	*/
	inline Model* GetSubmodel(const size_t i) { return i<m_submodelArray.size() ? m_submodelArray[i] : NULL; }
	/** Retrieve a specific SubmodelPort, or NULL if i is out of bounds.
		@param i The index of the SubmodelPort to retrieve.
		@return Pointer to the requested SubmodelPort, or NULL if i is is invalid.
	*/
	inline SubmodelPort* GetSubPort(const size_t i) { return i<m_portArray.size() ? m_portArray[i] : NULL; }
	/** Retrieve a specific AssocPort, or NULL if i is out of bounds.
	@param i The index of the AssocPort to retrieve.
	@return Pointer to the requested AssocPort, or NULL if i is is invalid.
	*/
	inline AssocPort* GetAssocPort(const size_t i) { return i<m_assocPortArray.size() ? m_assocPortArray[i] : NULL; }
	/** Retrieve a specific Label, or NULL if i is out of bounds.
		@param i The index of the Label to retrieve.
		@return Pointer to the requested Label, or NULL if i is is invalid.
	*/
	inline SimpleLabel* GetLabel(const size_t i) { return i<m_labelArray.size() ? m_labelArray[i] : NULL; }
	/** Retrieve a specific IterConditional, or NULL if i is out of bounds.
		@param i The index of the IterConditional to retrieve.
		@return Pointer to the requested IterConditional, or NULL if i is is invalid.
	*/
	inline IterConditional* GetItrCond(const size_t i) { return i<m_iterCondArray.size() ? m_iterCondArray[i] : NULL; }
	/** Retrieve a specific SubmodelAssoc, or NULL if i is out of bounds.
		@param i The index of the SubmodelAssoc to retrieve.
		@return Pointer to the requested SubmodelAssoc, or NULL if i is is invalid.
	*/
	inline SubmodelAssoc* GetAssoc(const size_t i) { return i<m_subAssocArray.size() ? m_subAssocArray[i] : NULL; }

	/** Retrieve a specific Spawner, or NULL if i is out of bounds.
		@param i The index of the Spawner to retrieve.
		@return Pointer to the requested Spawner, or NULL if i is is invalid.
	*/
	inline Spawner* GetSpawner(const size_t i) { return i<m_spawnerArray.size() ? m_spawnerArray[i] : NULL; }
	/** Retrieve a specific EvalAlias, or NULL if i is out of bounds.
	@param i The index of the Spawner to retrieve.
	@return Pointer to the requested Spawner, or NULL if i is is invalid.
	*/
	inline EvalAlias* GetAlias(const size_t i) { return i<m_aliasArray.size() ? m_aliasArray[i] : NULL; }

	/** Set the intespatailarnal ValueArchive to use to store model component values.
		@param pVArch Pointer to ValueArchive to associate with the Model.
	*/
	inline void SetValueArchive(ValueArchive* pVArch) { m_pValArchive = pVArch; }
	/** @return Pointer to the ValueArchive being used by the model.*/
	inline ValueArchive* GetValueArchive() const { return m_pValArchive; }

#pragma endregion
#pragma region =========== add specific components =================
	int AddParameter(const STLString & name, const OME_SCALAR & val);
	int AddParameter(const STLString & name, SDPMapEntry* pME);
	int AddDSParameter(const STLString & name);
	int AddParameter(const Param & param);
	int AddVariable ( Variable *pVar        );
	int AddStateVar ( StateVar *pStateVar   );
	int AddFlow     ( Flow *pFlow           );
	int AddInfluence( Influence *pInfluence );
	int AddSubmodel ( Model *pModel  );
	int AddSubPort  ( SubmodelPort *pPort, bool setParent=false   );
	int AddAssocPort(AssocPort *pPort, bool setParent = false);
	int AddLabel(SimpleLabel* pLbl);
	int AddIterConditional (IterConditional * pIc );
	int AddBaseMCAssoc(SubmodelAssoc* pSma);
	int AddSubMCAssoc(SubmodelAssoc* pSma);
	int AddSpawner(Spawner *s);
	int AddAlias(EvalAlias *pA);
	virtual int AddObject(OMEObject* pObj);

#pragma endregion
#pragma region =========== Remove specific element from model ======
	void RemoveParameter(const int & index);
	void RemoveParameter(Param* pParam);
	void RemoveVariable ( Variable *pVar        );
	void RemoveStateVar ( StateVar *pStateVar   );
	void RemoveFlow     ( Flow *pFlow           );
	void RemoveInfluence( Influence *pInfluence );
	void RemoveSubmodel ( Model *pModel);
	void RemoveSubPort  ( SubmodelPort *pPort   );
	void RemoveAssocPort(SubmodelPort *pPort);
	void RemoveIterConditional ( IterConditional *pIc	 );
	void RemoveLabel(SimpleLabel* pLbl);
	void RemoveBaseMCAssoc(SubmodelAssoc* pSma);
	void RemoveSubMCAssoc(SubmodelAssoc* pSma);
	void RemoveSpawner( Spawner* s );
	void RemoveAlias(EvalAlias* pA);
	virtual void RemoveObject(OMEObject* pObj);

#pragma endregion
#pragma region =========== general object queries ==================
	virtual OMEObject* ObjectForKey(const STLString key);
	virtual OMEObject* ObjectForName(const STLString name, const bool & recurse = false);
	virtual OMEObject* ObjectForName(const STLString name, const OMETYPE & oType,const bool & recurse = false);
	virtual OMEObject* ObjectForExprName(const STLString name, const bool & recurse = false);
	virtual OMEObject* ObjectForExprName(const STLString name, const OMETYPE & oType, const bool & recurse = false);
	virtual OMEObject* ObjectForInsensitiveName(const STLString name, const bool & recurse = false);
	virtual Model* SubmodelForName(const STLString & name, bool recurse = false);
	virtual Model* SubmodelForExprName(const STLString & name, bool recurse = false);
	OMEObject* NestedObjectForName(const STLString path, const OMEChar delim='.');
	StrArray GetObjectKeys(const DT_TYPE & depth=DT_SHALLOW);		
	virtual void GetAllEvals(EvalArray & out,const DT_TYPE & depth=DT_FULL);
	virtual void GetAllEvals(EvalSet & out,const DT_TYPE & depth=DT_FULL);
	

	//	/** Return a reference to the all-evaluable cash for this model; non-recursive.
	//@return Reference to internal Evaluable object cache for this model.
	//*/
	//EvalArray& GetAllEvalsRef() { return m_allEvals; };
	virtual void GetEvalReps(EvalRepArray &out, const DT_TYPE & flags=DT_FULL);
	void GetAllModels(ModelArray & bma);

	void GetVarsToRegister(EvalArray & out,bool includePorts=true);
	
	inline bool ContainsSubmodel(Model* pMdl);

	inline Model* GetRootModel();
	
#pragma endregion
#pragma region =========== evaluative slicing Functions ============
	void GetOrderedEvalsForUpdates(EvalArray & evals,const bool & useInit=true);
#pragma endregion
#pragma region =========== refreshers and overall modifiers ========
	void ImportFromObjectMaps(bool clearMap=false);
	virtual void RefreshObjectMaps(const bool & recurse=false);
	virtual void UpdateObjectName(const STLString oldName, const STLString newName);
	void RegisterWithManager();
	virtual unsigned int PrioritizeObjects(unsigned int basePriority, const bool includeInitial=false);
	virtual void PurgeInfluences(const bool recurse=true);
	virtual void PurgeUnusedPorts(const bool recurse = true);
	void UpdateSupports(OME_SCALAR time);
	bool InitializeEvalFromDataSource(Evaluable* pEval);

	void UnpackModelComponentExpressions();
	void PackModelComponentExpressions();
	
#pragma endregion
#pragma region =========== clear and cleanup functions =============
	void ClearObjectMaps();
	virtual void ClearInitialization();
	virtual void ClearPriorities();
	 inline void ClearInstances();
	virtual void Cleanup();

#pragma endregion
#pragma region =========== Data table accessors ====================
	/** Add an EvalTable to the model.
	@param val Pointer to the EvalTable to add.
	*/
	void AddEvalTable(EvalTable* val) {m_tables[val->GetID()]=val;    }
	EvalTable* GetEvalTable(const STLString & et);
	StrArray GetIDsForTables() const;

#pragma endregion
#pragma region =========== enumeration sets ========================
	
	void RegisterEnums(OMEParser* evalParser);
	/** Add an expression enumeration set to the model.
	@param ee the enumeration set to add.
	*/
	void AddModelEnum(ModelEnum & ee) { m_modelEnums.push_back(ee); };
	void AddModelEnum(const STLString & enumPattern);
	/** @return The number of ModelEnum collections. */
	size_t GetModelEnumCount() const { return m_modelEnums.size(); };
	/** Retrieve a specified ModelEnum.
		@param i The index of the ModelEnum to retrieve.
		@return A reference to the requested ModelEnum.
	*/
	ModelEnum& GetModelEnum(const size_t & i) { return m_modelEnums[i]; };
	ModelEnum* ModelEnumForName(STLString name);
	short ModelEnumValueForName(const STLString & name);

#pragma endregion
#pragma region =========== Submodel Association-related ============
	void BindSubmodelAssoc(SubmodelAssoc* pSa, const int instVal);
	ModelArray GetInstancesForAssoc(SubmodelAssoc* pSa);
	/** Determine if a SubmodelAssoc is bound to this model.
	@param pSa Pointer to SubmodelAssoc that may be bound to model.
	@return true if SubmodelAssoc is bound to this model, false otherwise.
	*/
	inline bool BoundToAssoc(SubmodelAssoc* pSa) {return m_parentAssocMap.find(pSa)!=m_parentAssocMap.end(); };
	int GetAssocIndex(const SubmodelAssoc* pSa);

#pragma endregion
#pragma region =========== parser and manager details ==============
	/** Get a pointer to the model's current expression parser
	@return a pointer to the current expression parser
	*/
	OMEParser* GetEvalParser() {return m_pEvalParser;};
	/** Set manager used for regulating run logistics. 
	@param pSm The SimManager to apply.
	*/
	inline void SetManager(BaseManager* pSm) { m_pSimManager=pSm; std::for_each(m_submodelArray.begin(), m_submodelArray.end(),[&pSm](Model* pM){pM->SetManager(pSm);});}
	/** @return A pointer to the manager associated with this model. */
	inline BaseManager* GetManager() {return m_pSimManager;}

	void GenRecordRep(RecordRepArray & recs);
	void CollectReadableRecords(RecordRepArray & reps);
	void GetReadableValues(RecordRep & obj);

#pragma endregion
#pragma region =========== instance related ========================
	virtual ModelInstance* GenerateInstance(const size_t & ind);
	virtual ModelInstance* GenerateInstance(const size_t & ind,const size_t & parent,Spawner* pSpwn);
	/** Number of current Instances.
	@return Number of active instances, including dead instances.
	*/
	virtual size_t GetNumInstances() const { return m_totInstCount; };
	/** @return The total number of instances of this Model across all scopes.*/
	inline size_t GetNumRawInstances() const { return m_instances.size(); }
	/** @return The initial number of instances that the model collection starts each simulation with.*/
	virtual size_t GetInitNumInstances() { return m_initialNumInstances; };

	virtual ModelInstance* GetInstance(size_t i);
	/**Retrieve instance using a raw index.
		@param i The raw/absolute index for the instance to retrieve.
		@return Pointer to the requested instance.
	*/
	virtual ModelInstance* GetRawInstance(const size_t & i) { return m_instances[i]; }
	/**Retrieve iterator for parsing through raw instances.
	@param i The raw/absolute index to be used to initialize the returned iterator.
	@return An iterator pointing to the the Model instance at raw index i.
	*/
	MdlInstanceArray::iterator GetRawInstanceItr(const size_t & i = 0) { return m_instances.begin() + i; }
	/** @return Pointer to currently active model instance. */
	inline ModelInstance* GetCurrentInstanceRef() { return m_instances[m_currentInstance[OME_CURR_THREAD] + m_indMultiplier[OME_CURR_THREAD]]; }
	
	/** Get currently selected index mapped to provided model-relative index.
		@param ind The index to be translated.
		@param step The optional stride to the index.
		@return The modified index.
	*/
	inline size_t GetArchiveIndex(const size_t & ind, const size_t & step = 1) const { return ind + (m_currentInstance[OME_CURR_THREAD] + m_indMultiplier[OME_CURR_THREAD])*step; }
	//inline ValueArchive::iterator GetCurrentInstanceValItr(const size_t & ind, const size_t & step = 1) { return m_pValArchive->GetIteratorForSlot(GetArchiveIndex(ind, step)); }
	/** Get current instance value for index.
		@param ind The index to query.
		@param step The optional stride to the index.
		@return The value found at index ind for the current instance.
	*/
	inline OME_SCALAR  GetCurrentInstanceVal(const size_t & ind, const size_t & step = 1) const { return (*m_pValArchive)[GetArchiveIndex(ind,step)]; }
	/** Set current instance value for a given index.
	@param val The value to assign at the index.
	@param ind The index to modify.
	@param step The optional stride to the index.
	*/
	inline void  SetCurrentInstanceVal(const OME_SCALAR & val, const size_t & ind, const size_t & step = 1) { (*m_pValArchive)[GetArchiveIndex(ind, step)]=val; }
	/** Get current instance value reference for index.
		@param ind The index to query.
		@param step The optional stride to the index.
		@return The reference for value found at index ind for the current instance.
	*/
	inline OME_SCALAR& GetCurrentInstanceVal(const size_t & ind, const size_t & step = 1) { return (*m_pValArchive)[GetArchiveIndex(ind, step)]; }
	/** Get Pointer to current instance value reference for index.
		@param ind The index to query.
		@param step The optional stride to the index.
		@return Pointer to the value found at index ind for the current instance.
	*/
	inline OME_SCALAR* GetCurrentInstanceValAddr(const size_t & ind, const size_t & step = 1) { return &(*m_pValArchive)[GetArchiveIndex(ind, step)]; }
	
	OME_SCALAR GetCurrentInstanceFirstVal(const size_t & ind, const size_t & step = 1);
	OME_SCALAR GetCurrentInstanceLastVal(const size_t & ind, const size_t & step = 1);
	OME_SCALAR GetCurrentInstancePreviousVal(const size_t & ind, const OME_SCALAR & time, const size_t & step = 1);
	OME_SCALAR GetCurrentInstanceBackVal(const size_t & ind, const OME_SCALAR & offset, const size_t & step = 1);

	/** @return true If the current ModelInstance is active; false otherwise. */
	inline bool GetCurrentInstanceActive() { return m_instances[m_currentInstance[OME_CURR_THREAD] + m_indMultiplier[OME_CURR_THREAD]]->IsActive(); }
	/** @return true If the current ModelInstance is dead; false otherwise. */
	inline bool GetCurrentInstanceDead() { return m_instances[m_currentInstance[OME_CURR_THREAD] + m_indMultiplier[OME_CURR_THREAD]]->IsDead(); }

	/** @return Multiplier that is applied to instance index queries. */
	inline size_t GetInstMultiplier() { return m_indMultiplier[OME_CURR_THREAD]; };
	void DisperseMultiplier(const size_t & tot=0);
	size_t GetParentOffset() const;
	size_t GetTotParentInstances() const;
	size_t GetActiveInstanceCount() const;

	/** Set the current instance for value queries.
		@param i index of the instance to set as current.
	*/
	void SetCurrentInstance(const size_t i) { m_currentInstance[OME_CURR_THREAD] = i; DisperseMultiplier(/*GetUpMult()*/m_indMultiplier[OME_CURR_THREAD] + m_currentInstance[OME_CURR_THREAD]);/** \question add error checking here?*/ }

	/** @param im The integration method to apply to model. _MAY BE OBSOLETE_ */
	inline void SetPreferredIntMethod(const OMEIntegrationBlock::INT_METHOD im) { m_preferredIntMethod=im; }
	/** @return The model's preferred integration method. */
	inline OMEIntegrationBlock::INT_METHOD GetPreferredIntMethod() const { return m_preferredIntMethod; }
	/** @param ri The value of how often report functions should be called.*/
	inline void SetReportInterval(const OME_SCALAR ri) { m_reportInterval=ri; }
	/** @return The interval for calls to report functions. */
	inline OME_SCALAR GetReportInterval() const { return m_reportInterval; }
	/** @param count The number of initial instances for this Model. */
	inline void SetInitNumInstances(const size_t & count) { m_initialNumInstances=(int)count; }

#pragma endregion
#pragma region =========== instance-value accessors ================

	bool GetValForInstIndex(const size_t & inst, const size_t & ind, OME_SCALAR & out);
	bool GetValAddrForInstIndex(const size_t & inst, const size_t & ind, OME_SCALAR* & out);
	bool GetValForRawInstIndex(const size_t & inst, const size_t & ind, OME_SCALAR & out);
	bool GetValAddrForRawInstIndex(const size_t & inst, const size_t & ind, OME_SCALAR* & out);

	bool GetValForInstIndex(const size_t & inst, const size_t  & ind, const size_t & step, OME_SCALAR & out);
	bool GetValAddrForInstIndex(const size_t & inst, const size_t  & ind, const size_t & step, OME_SCALAR* & out);
	bool GetValForRawInstIndex(const size_t & inst, const size_t  & ind, const size_t & step, OME_SCALAR & out);
	bool GetValAddrForRawInstIndex(const size_t & inst, const size_t  & ind, const size_t & step, OME_SCALAR* & out);

#pragma endregion
#pragma region =========== dynamic instance stuff ==================
	int SpawnInstance(const int useIndex=-1,const int instParent=-1,Spawner* pSpwn=NULL);
   void SpawnInstances(const int num,const int instParent=-1,Spawner* pSpwn=NULL);
   void KillInstance(int inst);
   /** Mark the model instance for destruction at the end of the current timestep.
		@param rawInst the Instance to mark.
   */
   inline void MarkForDeath(const size_t & rawInst) { m_instances[rawInst]->Kill(); }
   /** Mark the current instance for death. */
   inline void MarkForDeath(){ MarkForDeath(GetCurrentRawInstance()); }
   /** Mark an instance as active or inactive.
		@param rawInst The instance to mark.
		@param isActive Will mark rawInst as active if true and as inactive if false.
   */
   inline void MarkActive(const size_t & rawInst, const bool & isActive) { m_instances[rawInst]->SetActive(isActive); }
   /** Mark the current instance as active or inactive.
		@param isActive Will mark the current instance as active if true and as inactive if false.
   */
   inline void MarkActive(const bool & isActive) { MarkActive(GetCurrentRawInstance(), isActive); }
   void KillMarked();
   void KillAllMarked();

#pragma endregion
#pragma region =========== Static Model-relative utilities =========
   static Model* FindCommonRoot(Model* pMdl1, Model* pMdl2);
#pragma endregion


protected:
	ParamArray m_paramArray;				///< List of all parameters.
	VariableArray m_varArray;				///< List of all variables.
	SVArray m_stateVarArray;				///< List of all state variables.
	FlowArray m_flowArray;					///< List of all flows.
	InflArray m_influenceArray;				///< List of all influences.
	ModelArray m_submodelArray;				///< List of all submodels.
	PortArray m_portArray;					///< List of all submodelPorts, which either reside in this model or an immediate submodel.
	ARRAY_TYPE<AssocPort*> m_assocPortArray;	///< List of all AssocPorts, which either reside in this model or an immediate submodel.
	ICArray m_iterCondArray;				///< List of all IterConditionals (alarms).
	ARRAY_TYPE<SimpleLabel*> m_labelArray;	///< List of SimpleLabels found in the Model.
	AssocArray m_subAssocArray;				///< List of all submodel associations contained within this model.
	AliasArray m_aliasArray;				///< List of all aliases.
	bool m_isUnPacked;						///< Flag indicating if internal model components have been unpacked as needed.

	OME_SCALAR  m_stepSize;						///< preferred step size.
	//float  m_minStepSize;					///< minimal acceptable step size
	//float  m_maxStepSize;					///< maximum acceptable step size

	OME_SCALAR m_reportInterval;					///< how often the Res Func will be called throughout the simulation
	OME_SCALAR m_initTime;						///< record of when this model was created;

	OMEIntegrationBlock::INT_METHOD m_preferredIntMethod;		///< preferred integration method

	size_t m_currentInstance[OME_THREAD_COUNT];	///< Indicates which instance is currently active (one for each potential thread).
	OME_SCALAR m_totalIterations;				///< keeps count of iterations to reduce redundant calcs

	

	DSEntryMap m_dataFilePaths;				///< Optional filepaths that define instances.

	OMESclrArray m_portVals;					///< Array for storing port values.
	
	bool m_isInitialized;					///< true if the model has been initialized; false otherwise.
	bool m_prioritized;						///< true if objects within the model have been prioritized.
	ObjMap m_objectIDMap;					///< Map of all objects contained in model; used primarily in setup.
	ObjMap m_objectNameMap;					///< Map of all objects by name. Shallow, and is updated anytime included object has a name change.

	ModelEnumArray m_modelEnums;				///< A collection of enumerated types and their values.

	EvalArray m_allEvals;					///< temporary array that stores all evaluable objects for quick reference through sim run
	BaseManager* m_pSimManager;			///< Pointer to parent simulation manager.
	OMEParser* m_pEvalParser;				///< parser used to evalute model object equations
	int m_initialNumInstances;				///< The initial number of model instances at the start of each simulation.
	size_t m_indMultiplier[OME_THREAD_COUNT];					///< Multiplier to be applied to indexing objects. _MAY NOT BE NEEDED_

	INSTMODE m_instMode;						///< Type of instances generated by model.

	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename);

	void Duplicate(const Model & mdl);
	void Duplicate(const Model & mdl,bool copyPorts);
	
	void FindEvaluables(const bool & noRebuild=false);

	void UpdateNumInstances();
	int CalcBaseNumInstances();
	void DistributeSubAssocs();

	void AddToObjectIDMap(OMEObject* pVal);
	void AddToObjectNameMap(OMEObject* pVal);
	OMEObject* NestedObjectForName(StrArray::const_iterator sItr,StrArray::const_iterator eItr);
	void FindDSIndexedVal(const DataSrc & dsInd, const int & indCol, DataSourceValue & dsVal, Evaluable* pEval);
	void FindDSIndexedVal(const DataSrc & dsInd, const int & indCol, DataSourceValue & dsVal, VarArray* pVa);
#pragma region =========== Utility Templates =======================
	template<class T>
	int AddItem( OMEObject* pObj, ARRAY_TYPE<T> & itemList );

	template<class T>
	void RemoveItem( OMEObject* pObj, ARRAY_TYPE<T> & itemList );

	template<class T>
	void DeallocPtrArray(ARRAY_TYPE<T> & itemList);
	template<class T>
	void DeallocPtrArray(std::vector<T> & itemList);

	template<class T>
	void CollectionXMLRepresentation(const STLString & collName, const ARRAY_TYPE<T> & arr, StrArray & out, const unsigned int indent, const unsigned int inc,const bool parentCheck=false);
	
	template<class T>
	void LoadCollectionFromXML(TI2Element* pCurrElem, const STLString & filename, const OMEChar* tag, const OMEChar* subgroup, ARRAY_TYPE<T*> & objArray);
	template<class Tp,class Ta>
	void LoadCollectionFromXML(TI2Element* pCurrElem, const STLString & filename, const OMEChar* tag, const OMEChar* subgroup, ARRAY_TYPE<Ta*> & objArray);

#pragma endregion

private:
	std::map<STLString, EvalTable*> m_tables;				///< tables of data that can be associated with evals.
	bool m_isRunning;										///< flag set to true if the model is running through iterations, false otherwise.
	unsigned int m_iterations;								///< Number of iterations that this model has gone through in the current run.
	std::map<const SubmodelAssoc*,int> m_parentAssocMap;    ///< If parent Model is the sub portion of one or more associations, then this points to the relevant association.
	int m_assocIndex;										///< index of Base model to associate against.

	ARRAY_TYPE<bool> m_valRecAccess;						///< List of value record fields that are accessible to the outside world.
	
	MdlInstanceArray m_instances;							///< Array of active and killed instances.
	SpawnerArray m_spawnerArray;							///< Array that stores Spawner objects.
	AssocArray m_baseAssocArray;							///< Collection of SubmodelAssocs who have their tail ends anchored in this model.
	bool m_preSpawned;										///< True if instances were spawned prior to the initialization process.
	size_t m_totInstCount;									///< Total number of instances per representation of model.

	//ECMap m_evalColls;										///< Collections of arrays of Evaluable pointers that point to representation in each active instance.
	ARRAY_TYPE<DataSrc> m_dataSources;						///< An array of data sources used to initialize instances.

	ValueArchive* m_pValArchive;							///< Where all model values are stored.
	bool m_expectsSpatial;								///< Flag indicating whether model expects to have its number of instances determined by a spatial data provider.

	//for consolidating tags.

	static const OMEChar* m_xmlTags[TAG_COUNT];			///< Array acting as xml tag lookup table.
	
	static bool m_tagsInitialized;						///< set to true once InitXMLTags() has been called.

	static void InitXMLTags();

	STLString DoNestedPath(const Model* pBm) const;
	void JoinAssocs();
	//void InitializeFromDataSource();

	void InitializeAccessPermissions();
	
	int GetMaxDataSourceRowCount();
	void CollectDeadInstances();
	void ReportErrToMgr(const OMEChar* rhs, OMEObject* pObj=NULL,EC_ERR_TYPE eType=EC_CRITICAL,const bool addFront=false);
	void ReportErrToMgr(const STLString & rhs, OMEObject* pObj=NULL,EC_ERR_TYPE eType=EC_CRITICAL,const bool addFront=false);
	void AllocReadableInds(const OMEArray & srcs, ARRAY_TYPE<size_t> & inds, ARRAY_TYPE<bool> & access, STLString* & labels);

	static IndexNode* WalkIndexPath(Model* pMdl, IndexNode* pRootNode, const size_t & indCount);
	
	//for sorting evals for Execution
	inline void DoSwapStep(EvalArray & evals, int & ind, const int & inc);
	void DoSwapMarch(EvalArray & evals, int & ind, const int & stepsDir);

}; 

#pragma region Inline definitions
/** Remove all instances from the instance array. */
void Model::ClearInstances()
{ 
	 if(!m_preSpawned)
	 {
		//#pragma omp parallel for
		 for(int i=0; i< m_instances.size(); i++)
		 {
			 if(m_instances[i])
				 KillInstance(i);
		 }

		 m_instances.clear();
	}
}

/** Check to see if a model encloses another model.
	@param pMdl The model to search for within this model.
	@return true if a path of pMdl is found; false otherwise.
*/
bool Model::ContainsSubmodel(Model* pMdl)
{
	bool ret=false;

	for(int i=0; !ret && i<m_submodelArray.size(); i++)
		if(pMdl==(Model*)&m_submodelArray[i])
			ret=true;

	return ret;
}

/** Retrieve the root Model of the Model tree.
	@return Pointer to the root model.
*/
Model* Model::GetRootModel()
{
	if (m_pParentModel == NULL)
		return this;
	return m_pParentModel->GetRootModel();
}

/** Perform a swap and increment the provided index.
	@param evals The EvalArray to perform the swap against.
	@param ind The index of the first object in the swap; will contain the new index of the object on return.
	@param inc The distance to the second object to swap and the increment to apply to ind on return.
*/
void Model::DoSwapStep(EvalArray & evals, int & ind, const int & inc)
{
	int oldInd = ind;
	ind += inc;
	if (ind >= 0 && ind < evals.size())
	{
		//DBG_PRINTC(evals[oldInd]->GetName() + "--->" + evals[ind]->GetName(),DBG_MAGENTA);
		//swap 
		Evaluable* temp = evals[oldInd];
		evals[oldInd] = evals[ind];
		evals[ind] = temp;
	}
}



/** Convenience function for converting the various member lists to XML
@param collName The name of the collection / subtree.
@param arr The collection to parse.
@param out The variable to hold the resulting xml string.
@param indent The base indent to use for each newline.
@param inc The amount to increase the indent by for each indentation level.
@param parentCheck If true, only get xml representation for each item whose parent is the current model.
*/
template<class T>
void Model::CollectionXMLRepresentation(const STLString & collName, const ARRAY_TYPE<T> & arr, StrArray & out, const unsigned int indent, const unsigned int inc,const bool parentCheck)
{
	const STLString baseIndent(indent,' ');
	oSTLSStream temp;
	bool hit=false;
	if(!arr.empty())
	{
		temp<<baseIndent<<'<'<<collName<<'>'<<std::endl;
		for(auto itr=arr.begin(); itr!=arr.end();++itr)
		{
			if(!parentCheck || (*itr)->GetParentModel()==this)
			{
				hit=true;
				temp<<(*itr)->GetXMLRepresentation(indent+inc,inc)<<std::endl;
			}
		}
		if(hit)
		{
			temp<<baseIndent<<"</"<<collName<<'>';
			out.push_back(temp.str());
		}
	}
}

#ifndef OME_MDL_BUILD
/** Takes a subnode that records a list of T type items and loads them into the model.
@param pCurrElem The TiXml context.
@param filename The current filename.
@param tag The object identifier to process.
@param subgroup The name of the group's enclosing tags.
@param objArray The destination of for the loaded objects.
*/
template<class T>
void Model::LoadCollectionFromXML(TI2Element* pCurrElem, const STLString & filename, const OMEChar* tag, const OMEChar* subgroup, ARRAY_TYPE<T*> & objArray)
{
	STLString stlTag(tag);
	TI2Element* pXmlSub = pCurrElem->FirstChildElement( subgroup );
	if ( pXmlSub )
	{
		pXmlSub=pXmlSub->FirstChildElement(tag);
		while(pXmlSub!=NULL)
		{
			T* pObj=new T(pXmlSub,stlTag,filename);
			pObj->SetParentModel(this);
			objArray.push_back(pObj);
			pXmlSub=pXmlSub->NextSiblingElement(tag);
		}
	}
}

/** Takes a subnode that records a list of Tp type items and loads them into a compatible array of type Ta within the model.
@param pCurrElem The TiXml context.
@param filename The current filename.
@param tag The object identifier to process.
@param subgroup The name of the group's enclosing tags.
@param objArray The destination of for the loaded objects.
*/
template<class Tp,class Ta>
void Model::LoadCollectionFromXML(TI2Element* pCurrElem, const STLString & filename, const OMEChar* tag, const OMEChar* subgroup, ARRAY_TYPE<Ta*> & objArray)
{
	STLString stlTag(tag);
	TI2Element* pXmlSub = pCurrElem->FirstChildElement( subgroup );
	if ( pXmlSub )
	{
		pXmlSub=pXmlSub->FirstChildElement(tag);
		while(pXmlSub!=NULL)
		{
			Tp* pObj=new Tp(pXmlSub,stlTag,filename);
			pObj->SetParentModel(this);
			objArray.push_back(pObj);
			pXmlSub=pXmlSub->NextSiblingElement(tag);
		}
	}
}

#endif

//careful with these template utilities; only pass in OMEObject subclass that matches T

/** Generic function used to add an item to a specific list.
@param pObj The object to add to the model.
@param itemList The internal list to add the object to.
@return index of newly added object.
*/
template<class T>
int Model::AddItem( OMEObject* pObj, ARRAY_TYPE<T> & itemList )
{
	itemList.push_back( (T)pObj );
	int ret=itemList.size();
	pObj->SetParentModel(this);

	return ret;
}

/** Remove pointer object from a specific list.
@param pObj the object to remove.
@param itemList The object list to remove pObj from.
*/
template<class T>
void Model::RemoveItem( OMEObject* pObj, ARRAY_TYPE<T> & itemList )
{

	int ind=ObjectPresent<T>((T)pObj, itemList);
	if(ind >=0)
	{
		if(pObj->GetParentModel()==this)
			pObj->SetParentModel(NULL);

		itemList.erase(itemList.begin()+ind);
	}
}

/** Deallocate all pointers in a pointer container that belong to this model.
@param itemList The object list to deallocate all objects from.
*/
template<class T>
void Model::DeallocPtrArray(ARRAY_TYPE<T> & itemList)
{
	std::for_each(itemList.begin(),itemList.end(),[&](T obj){ if(obj->GetParentModel()==this) delete obj; });
	itemList.clear();
	//for(i=0; i< m_submodelArray.size(); i++)
	//	delete m_submodelArray[i];
}

/** Deallocate all pointers in a pointer container that belong to this model.
@param itemList The object vector to deallocate all objects from.
*/
template<class T>
void Model::DeallocPtrArray(std::vector<T> & itemList)
{
	std::for_each(itemList.begin(), itemList.end(), [&](T obj){ if (obj->GetParentModel() == this) delete obj; });
	itemList.clear();
	//for(i=0; i< m_submodelArray.size(); i++)
	//	delete m_submodelArray[i];
}

#pragma endregion
