#pragma once

#include <string.h>
#include "OMEObject.h"
#include "OMEParser.h"
#include "XMLLoadable.h"
#include "ErrCollection.h"
#include "EvalTable.h"
#include "EvalXSetInterpTable.h"
#include "Param.h"

class OMEParser;
class Model;

/** Representation of an evaluative function.
	This class is used to enhance OMEObjects that require the ability to evaluate equations and store values.
*/
class __EXPORT__ Evaluable : public OMEObject
{
   //allow OMEParser to access addresses of fields
	friend class OMEParser;
	friend class ValueArchive;
public:
	
	/** Default constructor. */
	Evaluable() : OMEObject(), m_evalFlags(0), m_pInitializer(NULL), m_pEnumParent(NULL),m_execGroup(OME_NO_EXEC_GRP) {};

	/** Base constructor
		@param v initial value
	*/
	Evaluable(OME_SCALAR v) : OMEObject(), m_evalFlags(0), m_pInitializer(NULL), m_pEnumParent(NULL), m_execGroup(OME_NO_EXEC_GRP) {};

	/** Constructor.
		@param id Unique identifier.
		@param val initial value.
	*/
	Evaluable(STLString id, OME_SCALAR val)
		:OMEObject(id), m_evalFlags(0), m_pInitializer(NULL), m_pEnumParent(NULL), m_execGroup(OME_NO_EXEC_GRP) {};

	/** Constructor.
			@param id Unique identifier for Evaluable object.
			@param name user-defined identifier.
			@param val initial value.
	*/
	Evaluable(STLString id, STLString name, OME_SCALAR val) 
		:OMEObject(id, name), m_evalFlags(0), m_pInitializer(NULL), m_pEnumParent(NULL), m_execGroup(OME_NO_EXEC_GRP) {};

	  /**	XML Constructor.
		This constructor instantiates the object based on the arguments passed in.
			@param pCurrElem  the current Tiny XML Element
			@param tag   the name of the desired tag containing the info.
			@param filename  the filename associated with the XML info.
			@see	GetXMLForObject()
   */

   Evaluable(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
	   : m_pInitializer(NULL), m_pEnumParent(NULL), m_execGroup(OME_NO_EXEC_GRP)
   {
	   OMEObject::PopulateFromXML(pCurrElem,tag,filename);
	   CheckConstant();
   }

	/** Copy Constructor
		@param func Evaluable object to duplicate.
	*/
	Evaluable(const Evaluable & func) { Duplicate(func); };
	
	virtual ~Evaluable(){};

	/** Scalar cast operator.*/
	virtual operator const OME_SCALAR() const
	{
		return GetValue();
	}

	/** Assign value from scalar.
		@param val The value to assign.
		@return Reference to this.
	*/
	Evaluable& operator=(OME_SCALAR & val)
	{
		SetValue(val);
		return *this;
	}

	/** Assign value from a model enumeration.
	@param pEE Pointer to the value to assign.
	@return Reference to this.
	*/
	Evaluable& operator=(EnumEntry* pEE)
	{
		SetValue(pEE);
		return *this;
	}

	/** Assign value from Enumeration class.
	@param pME Pointer to the value to assign.
	@return Reference to this.
	*/
	Evaluable& operator=(ModelEnum* pME)
	{
		SetValue(pME);
		return *this;
	}

	virtual void Detach();

	virtual OME_SCALAR Evaluate(const OME_SCALAR time,BaseManager* pBm);
	virtual OME_SCALAR Initialize(const OME_SCALAR time,BaseManager* pBm);

	virtual void BatchInitialize(const OME_SCALAR time,BaseManager* pBm);
	//OMEParser* GetParser();
	
	virtual void GetXMLAttributes(StrArray & out) const;
	virtual STLString GetXMLRepresentation(const unsigned int indent=0,const unsigned int inc=4) { return GetXMLForObject(indent,inc);};
	virtual void GetXMLSubNodes(StrArray & out,const unsigned int indent=0,const unsigned int inc=4);

   virtual int AddUpObject  ( Evaluable *pObject );
   virtual int AddDownObject( Evaluable *pObject );

   virtual void RemoveUpObject  ( Evaluable *pObject );
   virtual void RemoveDownObject( Evaluable *pObject );
   /** Get count of registered upObjects
		@return count of immediate upObjects
	*/
   size_t UpObjectCount() { return m_upObjects.size(); }
 /** Get count of registered downObjects
		@return count of immediate downObjects
	*/
   size_t DownObjectCount() { return m_downObjects.size(); }

   /** returns the ith up object
		@param i the index to the up object
		@return the Evaluable up object at the ith index
	*/
   Evaluable* GetUpObject(const int i ) { return m_upObjects[i]; } 
   /** returns the ith down object
		@param i the index to the down object
		@return the Evaluable down object at the ith index
	*/
	Evaluable* GetDownObject(const int i ) { return m_downObjects[i]; } 

	/** @return An EvalArray containing pointers to all objects immediately downstream from this Evaluable object. */
	inline EvalArray GetDownObjects() const { return m_downObjects;}
	/** @return An EvalArray containing pointers to all objects immediately upstream from this Evaluable object. */
	inline EvalArray GetUpObjects() const { return m_upObjects; }

   void FindAllUpObjects(EvalArray & out, int exclude=-1);
	void FindAllDownObjects(EvalArray & out, int exclude=-1);
	//void FindTieredDownObjects(ARRAY_TYPE<EvalArray> & out, int exclude=-1);

	virtual void FindNextDownObjects(EvalArray & out, bool excludeInfl=true,bool bypassPorts=true);
	virtual void FindNextUpObjects(EvalArray & out, bool excludeInfl=true,bool bypassPorts=true);

	virtual bool IsDownStream(Evaluable* pTrg, const bool & useShallow = false);
	virtual bool IsUpStream(Evaluable* pTrg,const bool & useShallow=false);

	Evaluable* UpObjectForName(const STLString & name);

	/** Query to determine if Evaluable is at the end of a prioritization chain.
		@return true If Evaluable is at the end of a prioritization chain; false otherwise.
	*/
	inline bool IsTerminal() const { return m_downObjects.empty(); }
	
	/** Query to determine if Evaluable is at the beginning of a prioritization chain.
		@return true If Evaluable is at the beginning of a prioritization chain; false otherwise.
	*/
	inline bool IsEvalRoot() const { return m_upObjects.empty(); }
	
	virtual OME_SCALAR GetValue() const;
	virtual OME_SCALAR GetValue(const size_t & inst) const;
	virtual OME_SCALAR GetValueForRawInst(const size_t & inst) const;

	virtual ModelEnum* GetEnumClass();
	virtual EnumEntry* GetEnumEntry();
	virtual bool IsEnumClass() const;
	virtual bool IsEnumValue() const;

	virtual void SetValue(const OME_SCALAR val);
	virtual void SetValue(const OME_SCALAR val,const size_t & inst);

	virtual void SetValue(ModelEnum * me);
	virtual void SetValue(EnumEntry * ee);

	/** Reset evaluable object to default values */
	virtual void Reset(BaseManager* pBm);

	virtual OMETYPE GetOMEType() const { return OME_EVAL; };
	virtual bool IsOMEType(OMETYPE type) const { return type==OME_EVAL || OMEObject::IsOMEType(type); };

	virtual unsigned int Prioritize(unsigned int currPriority, const bool isInitial=false);

	virtual void SetName(const STLString & name);
	virtual void SetName(const OMEChar* name) { Evaluable::SetName(STLString(name)); };
	virtual void SetParentModel(Model* pMdl);

	/** @return True if the Evaluable object contains a constant value; false otherwise. */
	bool GetIsConstant() const;
   /** @param ic Boolean indicating whether or not this Evaluable only contains constants in its expression. */
   inline void SetIsConstant(const bool ic) { if (ic) m_evalFlags |= CONST_FLAG; else m_evalFlags &= ~CONST_FLAG; }

   /** Return a read-only reference to the value storage.
	  @return A read-only pointer to the address where the Evaluable's value is stored.
	*/
   virtual const OME_SCALAR* GetValueAddr();
   virtual const OME_SCALAR* GetValueAddr(const size_t & inst);

   void SetPEvalTable(EvalTable* pEt,const size_t & ind);
   /** @param id The ID used to identify the associated tableID.
		@param ind The index of the table to modify.
   */
   inline void SetTableID(const STLString & id,const size_t & ind) { m_tableIDs[ind]=id; } 
   /** @param id The ID used to identify the associated tableID as a raw C-String.
		@param ind The index of the table to modify.
	*/
   inline void SetTableID(const OMEChar* id, const size_t & ind) { m_tableIDs[ind] = STLString(id); }

	/** @param ind The index of the table to query.
	@return Id of table associated with this Evaluable. 
	*/
   inline STLString GetTableID(const size_t & ind) const { return m_tableIDs[ind]; }

   /** Add a new table ID to the object.
		@param id The id to append.
		@return The index of the appended ID.
   */
   inline size_t AppendTableID(const STLString & id) 
   { 
	   m_tableIDs.push_back(id); return m_tableIDs.size() - 1; 
   }
	/** @param ind The index of the table to query.
	@return Pointer to EvalTable associated with this Evaluable.
	*/
   virtual EvalTable* GetPEvalTable(const size_t & ind) const { return GrabTData(ind); }
	/** Determine if Evaluable is associated with a table.
		@return true if associated with table, false otherwise.
	*/
   virtual bool HasTable() const { return !m_pTables.empty() || !m_tableIDs.empty(); }


   virtual size_t GetInstValCount() const;

   /** @return The number of tables associated with the Evaluable. */
   inline size_t GetTableCount() const { return m_tableIDs.size(); }
	/** Retrieve number of dimensions of associated table.
		@param ind The index of the table to grab the count from.
		@return the number of dimensions of associated table, or 0 if there is no associated table.
	*/
   inline unsigned int GetTableDimCount(const size_t & ind) { return GrabTData(ind) ? m_pTables[ind]->GetDimensionCount() : 0; }
   
	/** Get Value from associated table.
		@param val OME_SCALAR set to value at location indicated by inds if GetTableValue returns true.
		@param inds Pointer to an array of indices used to locate the value.
		@param indCount the number of indices in inds.
		@param tblInd The index of the table to retrieve.
		@return true if indCount is equal to the number of dimensions in the associated EvalTable and inds are all valid indices; false otherwise.
	*/
   virtual bool GetTableValue(OME_SCALAR & val, const unsigned int* inds, const unsigned int indCount, const size_t & tblInd = 0) { return GrabTData(tblInd) && m_pTables[0]->GetValue(val, inds, indCount); }

   virtual bool GetInterpTableValue(const OME_SCALAR & inVal, OME_SCALAR & outVal, const size_t & ind);

   virtual size_t GetModelIndex() const;
   virtual bool GetModelIndex(size_t & out) const;

	/** @return The expression used in the evaluation of this object. */
    inline STLString GetExpr() const { return m_expr; }
	/** @param e The expression to assign to this object. */
   inline void SetExpr(const STLString & e) { m_expr = e; }
   /** @param e The expression to assign to this object as a raw C-String. */
   inline void SetExpr(const OMEChar* e) { m_expr = STLString(e); }

   /** @return The initialization expression if present, or the expression returned by GetExpr.
		@sa GetExpr()
   */
   inline STLString GetInitExpr() const { return m_initExpr.empty() ? m_expr : m_initExpr; }
   /** @param e The value to assign as the initialization expression. */
   inline void SetInitExpr(const STLString & e) { m_initExpr = e; }
   /** @return true if object has been initialized; false otherwise. */
   virtual bool IsInitialized() const { return m_evalFlags & INIT_FLAG;}

   /** Mark the Evaluable to be treated as an integer rather than a floating point value.
		@param io true will set the Evaluable as int only; false will cause the evaluable to be treated as a floating point scalar.
   */
   inline void SetAsInt(const bool & io) { if (io) m_evalFlags |= INTONLY_FLAG; else m_evalFlags &= ~INTONLY_FLAG; }
   /** Mark the Evaluable to ask for a parameter value during the initialization of a simulation run; how this flag is handled is determined by the SimManager.
		@param av If true, the Evaluable will be expecting a runtime-defined value during initialization.
   */
   inline void SetAskVal(const bool & av) { if (av) m_evalFlags |= ASK_FLAG; else m_evalFlags &= ~ASK_FLAG; }
   
   virtual const OMEChar* ToString(const OMEChar tabChar=' ', const unsigned int indent=0, const unsigned int inc=4) const;

   /** @return true if Evaluable is treated as an Integer value; false otherwise.*/
   inline bool IsAsInt() const { return m_evalFlags & INTONLY_FLAG; }
   /** @return true if the Evaluable will ask for a parameter value at runtime; false otherwise.*/
   inline bool GetAskVal() const { return m_evalFlags & ASK_FLAG; }

   /** Set parameter to use during initialization.
		@param init Pointer to Param to use for initialization.
   */
   inline void SetInitializer(Param* init) { m_pInitializer = init; }
   /** @return Pointer to Param used in initialization. */
   inline Param* GetInitializer() const { return m_pInitializer; }

   /** @return the execution group number of the Evaluable, or OME_NO_EXEC_GRP if it does not belong to an execution group.*/
   inline unsigned int GetExecGroup() const { return m_execGroup; }
   /** Optionally set an execution group for the Evaluable object.
	@param eg The execution group to assign.
   */
   inline void SetExecGroup(const unsigned int & eg) { m_execGroup=eg; }

   //inline void SetAggregatesAcrossInstances(const bool & agg) { if (agg) m_evalFlags |= AGGR_FLAG; else m_evalFlags &= ~AGGR_FLAG; }
   //inline bool GetAggregatesAcrossInstances() const { return m_evalFlags & AGGR_FLAG; }
   /** @return the number of slots that a single instance of the object requires.*/
   virtual size_t GetRequestedSlotCount() const { return 1; }

protected:
	
	STLString m_expr;						///< expression to be evaluated.
	STLString m_initExpr;					///< only used if evaluable has an alternate initialization expression
	EvalArray m_upObjects;					///< Objects that are evaluated upstream.
	EvalArray m_downObjects;				///< Objects that are evaluated downstream.
	size_t m_mdlIndex;						///< Index for value lookup that is set during initialization.
	Param* m_pInitializer;					///< Pointer to optional parameter object used in initialization.
	ModelEnum* m_pEnumParent;				///< Parent Enumeration object to use in enum interpretation
	OME_SCALAR m_lastTime;		///< last Iteration for which this object was evaluated	\question move to modelInstance?
	unsigned int m_execGroup;  ///< index used for breaking up items within a submodel that need to finish one after another.


	ARRAY_TYPE<EvalTable*> m_pTables;	///< Stores associated table data that can be indirectly accessed through expressions.
	StrArray m_tableIDs;	///< Unique id of table associated with this Evaluable, if any.

	static bool DescendTree(Evaluable* currObj, EvalArray & outArray, int exclude=-1);
	static bool DescendTree(Evaluable* currObj, EvalArray & outArray, Evaluable* pTrg);
	static bool AscendTree(Evaluable* currObj, EvalArray & outArray, int exclude=-1);
	static bool AscendTree(Evaluable* currObj, EvalArray & outArray, Evaluable* pTrg);
	static bool AscendTree(Evaluable* currObj, ARRAY_TYPE<EvalArray> & outArrays, int exclude);

	void Duplicate(const Evaluable & func);

	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename);
		
	virtual void CheckConstant();

	/** Explicitly mark an Evaluable as initialized or uninitialized; intended to be used by subclasses for self-management.
		@param ini If true, Evaluable is treated as if it has been initialized; otherwise, it is treated as uninitialized.
	*/
	inline void SetInitialized(const bool & ini){ if (ini) m_evalFlags |= INIT_FLAG; else m_evalFlags &= ~INIT_FLAG; }
	
	/** Explicitly mark an Evaluable as Enumerated value class; intended to be used by subclasses for self-management.
	@param ec If true, Evaluable is treated as if it is an enumerated value class.
	*/
	inline void SetIsEnumClass(const bool & ec){ if (ec) m_evalFlags |= ENCLASS_FLAG; else m_evalFlags &= ~ENCLASS_FLAG; }

	EvalTable* GrabTData(const size_t & ind);
	EvalTable* GrabTData(const size_t & ind) const;

	//virtual functions for queries from ValueArchive
	
	/** Set the index used to reference the model value.
		@param index The index to assign to the Evaluable.
	*/
	virtual void SetModelIndex(const size_t & index) { m_mdlIndex = index; }


	private:
		enum {
			INIT_FLAG = 0x01,		///< Flag indicating if the object has gone through its initialization function since it was last reset.
			CONST_FLAG = 0x02,		///< Flag indicating if the object is simply a numerical value.
			INTONLY_FLAG = 0x04,	///< Flag indicating if floats should be truncated to ints.
			ASK_FLAG = 0x08,		///< Flag indicating if value is expecting input either through param or runtime.
			ENCLASS_FLAG = 0x10		///< Flag indicating if enumerated value is enum class (enum value count).
			//AGGR_FLAG = 0x20		///< Flag indicating that value will aggregate across instances (important for sorting)
		};

		char m_evalFlags;			///< Storage for various flags regulating eval behavior

};

//OME_SCALAR __EXPORT__ operator + (const OME_SCALAR & lhs, Evaluable & rhs);
//OME_SCALAR __EXPORT__ operator-(const OME_SCALAR & lhs, Evaluable & rhs);
//OME_SCALAR __EXPORT__ operator*(const OME_SCALAR & lhs, Evaluable & rhs);
//OME_SCALAR __EXPORT__ operator/(const OME_SCALAR & lhs, Evaluable & rhs);
//
//bool __EXPORT__ operator==(const OME_SCALAR & lhs, Evaluable & rhs);
//bool __EXPORT__ operator!=(const OME_SCALAR & lhs, Evaluable & rhs);
//bool __EXPORT__ operator<=(const OME_SCALAR & lhs, Evaluable & rhs);
//bool __EXPORT__ operator>=(const OME_SCALAR & lhs, Evaluable & rhs);
//bool __EXPORT__ operator<(const OME_SCALAR & lhs, Evaluable & rhs);
//bool __EXPORT__ operator>(const OME_SCALAR & lhs, Evaluable & rhs);
//
//OME_SCALAR __EXPORT__ operator+(Evaluable & lhs, const OME_SCALAR & rhs);
//OME_SCALAR __EXPORT__ operator-(Evaluable & lhs, const OME_SCALAR & rhs);
//OME_SCALAR __EXPORT__ operator*(Evaluable & lhs, const OME_SCALAR & rhs);
//OME_SCALAR __EXPORT__ operator/(Evaluable & lhs, const OME_SCALAR & rhs);
//												    
//bool __EXPORT__	operator==(Evaluable & lhs, const OME_SCALAR & rhs);
//bool __EXPORT__	operator!=(Evaluable & lhs, const OME_SCALAR & rhs);
//bool __EXPORT__	operator<=(Evaluable & lhs, const OME_SCALAR & rhs);
//bool __EXPORT__	operator>=(Evaluable & lhs, const OME_SCALAR & rhs);
//bool __EXPORT__ operator<(Evaluable & lhs, const OME_SCALAR & rhs);
//bool __EXPORT__ operator>(Evaluable & lhs, const OME_SCALAR & rhs);


//================================

/** Simple summary for an Evaluable object for summary reports.
	@sa Evaluable
*/
struct __EXPORT__ EvalRep
{
   STLString name;			///< Name of represented Evaluable.
   STLString id;			///< Id of represented Evaluable.
   STLString desc;			///< Description of represented Evaluable.
   OMETYPE type;			///< Type identifier of represented Evaluable
   bool isConstant;			///< True if Evaluable is considered a constant/lever.
   bool isArray;			///< True if value represents an array from a submodel.
   unsigned int count;		///< Count of objects if isArray is true.
	STLString parentModel;	///< ID of parent model of represented Evaluable.
	STLString modelPath;	///< Full modelPath to represented Evaluable.
	OMEColor color;			///< Color used when displaying visual representation of object.
	std::vector<size_t> selected;  ///< Coordinates of selected value (optional)

   /** Default constructor */
   EvalRep();

   /** Convenience constructor. 
		@param eval Pointer to Evaluable to represent.
		@param ia If true, EvalRep is considered to be representing an array.
		@param c Number of objects in represented array, if applicable.
   */
   EvalRep(Evaluable *eval,bool ia=false,unsigned int c=0);

   EvalRep(const EvalRep & er);

};

ostream& operator<<(ostream& outStrm, const EvalRep & rhs);
