#pragma once
#include "BaseEvalSerializer.h"
#include "omeobjects/OMEObject.h"
#include "omeobjects/Model.h"
#include <sstream>

class LuaOMEParser;

/** Expression serializer for Lua interpreter. */
class __EXPORT__ LuaEvalSerializer : public BaseEvalSerializer
{
public:
	LuaEvalSerializer();
	LuaEvalSerializer(LuaOMEParser* pParser,const bool & useDbg=true);
	LuaEvalSerializer(const LuaEvalSerializer & les);

	virtual ~LuaEvalSerializer();

	virtual bool SerializeEval(Model* pRootModel);
	virtual bool SerializeEval(const EvalArray & entries,const bool forInit=false);
	virtual bool SerializeEval(Evaluable* entry,const bool forInit=false);
	virtual STLString GetInitStatements(const bool forPrint=false);
	virtual STLString GetEvalStatements(const bool forPrint=false);
	virtual bool WriteSerialExpressions(const STLString & initPath, const STLString & evalPath);
	virtual void ClearSerializer();
	virtual bool PopulateBreaks(BreakSet & breaks,const bool forInit);
	virtual bool SetBreak(const size_t & line, const bool & enabled, const bool & forInit);
	virtual size_t GetLineCount(const bool forInit);
	 /** Assignment operator.
		@param rhs The LuaEvalSerializer object to use in assignment.
		@return A reference to the LuaEvalSerializer object.
   */
	LuaEvalSerializer& operator=(const LuaEvalSerializer & rhs) {Duplicate(rhs); return *this;};

	virtual void SetInitBreakPath(const STLString & path) { m_ibPath=path; }
	virtual STLString GetInitBreakPath() const {return m_ibPath; }

	virtual void SetEvalBreakPath(const STLString & path) { m_ebPath=path; }
	virtual STLString GetEvalBreakPath() const {return m_ebPath; }

   virtual void SetDbgEnabled(const bool & dbg) { m_usingDebug=dbg;}
   virtual bool GetDbgEnabled() const { return m_usingDebug;}

private:


	STLString m_ibPath;					///< Path to file containing init break points.
	STLString m_ebPath;					///< Path to file containing eval break points.
   bool m_usingDebug;					///< Flag indicating if Debugging hints should be used in the serialization process.

	LuaOMEParser* m_pEvalParser;		///< Evaluable parser.

	const OMEChar** m_selfReferences;	///< A list of self-referencing functions.

	std::map<STLString, unsigned int> m_funcArgReferences; ///< Name mapping of function indices.
	static std::map<STLString, STLString> m_boolFuncs;		///< map of OME boolean operators and Lua boolean operators
	static std::map<STLString, STLString> m_initFuncs;		///< map of OME boolean operators and Lua boolean operators

	std::list<Model*> m_spawnersAdded;	///< List of models with spawner objects.

	oSTLSStream m_updateCache;			///< Buffer of update statements.
	oSTLSStream m_initCache;			///< Buffer for init statements.

	std::set<size_t> m_initBreaks;		///< Collection of line numbers for initialization breakpoints.
	std::set<size_t> m_updateBreaks;		///< Collection of line numbers for update breakpoints.

	Model* m_pRootModel;				///< Pointer to the root model object.

	size_t m_constArrayCount;			///< The total number of const arrays.
	std::map<STLString, STLString> m_constArrays;	///< The name-initializer pairs used to define const arrays
	std::map<STLString, STLString> m_convertedExprs;	///< Cache of expressions that have been converted.

	void ProcessEval(Evaluable* eval, oSTLSStream & cache,const bool & isInit);

	void Duplicate(const LuaEvalSerializer & les);
	//void SerializeLevel(Model* pMdl,bool useInit,const unsigned short lvl=0);
	void SerializeModel(Model *pRootModel,const bool useInit);

	void AddModelLoopHeader(Model* pMdl, oSTLSStream & cache, const bool & useActive, const bool & forInit);
	static void AddModelLoopFooter(Model* pMdl, const bool & wasActive, oSTLSStream & cache);
	
	void AddHeaderDeclarations(oSTLSStream & cache,const bool & forInit);
	void AddTailDeclarations(oSTLSStream & cache);

	void ExtractSelfReferencesFromParser();
	void ExtractFunctionArgsAndInitsFromParser();
	void LocalizeFuncs(oSTLSStream & out);
	void LocalizeModelComponents(oSTLSStream & out);
	void LocalizeModels(Model* pMdl, oSTLSStream & out);
	void AllocateVarArrays(Model* pMdl, oSTLSStream & cache);
	bool FindArgPos(const unsigned int argNum, const STLString & expr, const size_t & found, size_t & pos, size_t & argLen);
	STLString RegisterConstArrays(STLString expr);
	bool CheckConstArray(const STLString & expr);

	STLString UpdateFuncArgs(const STLString & funcName, const unsigned int & argNum, const STLString & expr);
	STLString ProcessLists(const STLString & exprArgs);
	STLString ConvertBools(STLString exprArgs);

	STLString ConvertExprNamesToIds(const STLString & inExpr);
	STLString ConvertInitFuncs(STLString inExpr);

	STLString ApplyFactories(Evaluable* pCaller, STLString inExpr, const bool & inInit);

	/** Utility function that constructs a Lua-friendly model path to a given object.
		@param pObj Pointer to the object that is the target of the path.
		@return A STLString that contains a Lua model path.
	*/
	static inline STLString FullPath(OMEObject* pObj)
	{
		StrList path;
		Model* pMdl=pObj->GetParentModel();
		if(pMdl)
			pMdl->GetUpPath(path);
		path.push_back(pObj->GetExprName());

		return ConcatStrings(path,'.');
	}

	static bool NeedsLoop(Model* pBm);

	STLString AssignBreaks(const STLString & orig,const std::set<size_t> & breaks);
	void LoadBreaks(const STLString & path,const bool forInit);
	//void ProcessSliceTree(const SliceCollection & coll, EvalArray & out,const bool & useInit);
	//
	// The following less() comparison functions are used to appropriately group objects
	// by priority, then by associated model, then by whether or not the objects are submodelports
	//
	
	/** Sorting comparison func for use with Update function serialization.
		@param lhs The left hand object for the comparison.
		@param rhs The right hand object for the comparison.
		@return true if lhs should come before rhs.
	*/
	static bool CompareSerialEvalPriority(const OMEObject* lhs, const OMEObject* rhs) 
	{
		bool ret=true;
		Model* pLMdl=lhs->GetParentModel();
		Model* pRMdl=rhs->GetParentModel();

		if(lhs->GetEvalPriority() != rhs->GetEvalPriority())
			ret=OMEObject::CompareEvalPriority(lhs,rhs);
		else if(pLMdl!=pRMdl)
			ret=pLMdl<pRMdl;
		else
			ret=lhs->GetOMEType()!=OME_SUBPORT && rhs->GetOMEType()==OME_SUBPORT;
		return ret;
	}

	/** Sorting comparison func for use with Initialization function serialization.
	@param lhs The left hand object for the comparison.
	@param rhs The right hand object for the comparison.
	@return true if lhs should come before rhs.
	*/
   static bool CompareSerialInitPriority(const OMEObject* lhs, const OMEObject* rhs) 
   { 
	   bool ret=true;
		Model* pLMdl=lhs->GetParentModel();
		Model* pRMdl=rhs->GetParentModel();

	   if(lhs->GetInitPriority() != rhs->GetInitPriority())
		   ret= OMEObject::CompareInitPriority(lhs,rhs);
	   else if(pLMdl!=pRMdl)
			ret=pLMdl<pRMdl;
		else
			ret=lhs->GetOMEType()!=OME_SUBPORT && rhs->GetOMEType()==OME_SUBPORT;
		return ret;
   }
};

