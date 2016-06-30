#pragma once

#include <deque>
#include "omeobjects/Evaluable.h"
#include "omeobjects/Model.h"
#include "CompiledMdlFuncs.h"
#include "CPPLCSubEntry.h"
#include "CPPLCAccumEntry.h"
class CPPSerializer;

/** Container for functional chunks of expression logic. Also used to generate CPP code from expressions.

	CPPLoopClusters are assembled in a tree heirarchy, reflecting the collection of logic similar to the submodel heirarchy
	in the Submodel itself.

*/
class __EXPORT__ CPPLoopCluster
{
	friend class CPPLCSubEntry;
public:
	CPPLoopCluster(CPPSerializer* pCS, const STLString & lbl, const size_t & iters, const bool & useActiveCheck = false, Model* pMdl = NULL, CPPLoopCluster* parent = NULL,CPPLCSubEntry* pPEnt=NULL);
	~CPPLoopCluster();
	void FindAllDependents();
	STLString ProcessTree(const bool & forInit);

	void AppendEval(Evaluable* pEval,const STLString & upst=STLString());
	void AppendSrcLabel(const STLString & lbl);
	CPPLCSubEntry* AppendCustomEntry(const STLString & upSt, const STLString & lbl, const STLString & assigned,const STLString & ak = STLString(), const size_t & num = 0);
	CPPLCSubEntry* NewCustomEntry(const size_t & index, const STLString & upSt, const STLString & lbl, const STLString & assigned, const STLString & ak = STLString(), const size_t & num = 0);

	CPPLoopCluster* NewSubLoopCluster(Model* pMdl,const bool & useActive);
	CPPLoopCluster* NewSubLoopCluster(const size_t & currInd,const size_t & len,const STLString & label);
	CPPLoopCluster* AppendSubLoopCluster(const size_t & len, const STLString & label);

	CPPLCSubEntry* NewTempDeclaration(const size_t & index, const STLString & upSt, const STLString & lbl, const STLString & ak = STLString(), const size_t & num = 0);
	CPPLCSubEntry* NewTempApplication(const size_t & index, const STLString & upSt, const STLString & lbl, const STLString & ak = STLString(), const size_t & num = 0);
	CPPLCSubEntry* NewTempInitialization(const size_t & index, const STLString & upSt, const STLString & lbl, const STLString & ak = STLString(), const size_t & num = 0);

	CPPLCSubEntry* AppendTempDeclaration(const STLString & upSt, const STLString & lbl, const STLString & ak = STLString(), const size_t & num = 0);
	CPPLCSubEntry* AppendTempApplication(const STLString & upSt, const STLString & lbl, const STLString & ak = STLString(), const size_t & num = 0);
	CPPLCSubEntry* AppendTempInitialization(const STLString & upSt, const STLString & lbl, const STLString & ak = STLString(), const size_t & num = 0);

	CPPLCSubEntry* PrependTempDeclaration(const STLString & upSt, const STLString & lbl, const STLString & ak = STLString(), const size_t & num = 0);
	CPPLCSubEntry* PrependTempApplication(const STLString & upSt, const STLString & lbl, const STLString & ak = STLString(), const size_t & num = 0);
	CPPLCSubEntry* PrependTempInitialization(const STLString & upSt, const STLString & lbl, const STLString & ak = STLString(), const size_t & num = 0);

	CPPLCSubEntry* NewTempPlaceholder(const size_t & index, const STLString & upSt, const STLString & lbl, const STLString & ak = STLString(), const size_t & num = 0, const unsigned short dimCount = 1);


	size_t GetObjectCount() const;
	bool FixedIterationCount() const;
	size_t GetIterations() const;

	CPPLoopCluster* GetSubLoopCluster(const size_t & i);
	Evaluable* GetEvaluable(const size_t & i);

	void ClearAllEntries();

	CPPLoopCluster* GetParentCluster();
	CPPLCSubEntry* GetParentEntry();
	void SetParentCluster(CPPLoopCluster* pClust);
	CPPLoopCluster* GetRootCluster();
	bool HasModel();
	Model* GetRepModel();
	Model* GetRootModel();

	/** @return true if the Cluster is marked for cleanup; false otherwise.*/
	inline bool IsDead() const { return m_markedForCleanup; }

	void AddInitializationStatement(const STLString & initStr);

	/** @return The total count of spawners hit so far.*/
	size_t GetSpawnerCount() const { return m_spawnerCount; }
	
	/** @return the execution group associated with the cluster.*/
	inline unsigned int GetExecGroup() const { return m_execGroup; }
	/** Set the cluster's execution group.
		@param eg The execution group to assign.
	*/
	inline void SetExecGroup(unsigned int eg) { m_execGroup = eg; }

	void StartMortalityHere(Model* pMdl);
	void EndMortalityHere(Model* pMdl);

	bool ContainsVariableWithName(const STLString & name, size_t* pOutInd=NULL,const bool & recurse=false);
	STLString CPPizeExpression(const STLString & expr, Evaluable* pCaller, const bool & isInInit, size_t & currInd, const bool & noProcess = false, OMECFuncs::FuncSig::FS_ARGS* outRType=NULL);
	void ProcessFuncArgs(const STLString & funcArgs, StrList & foundArgs, OMECFuncs::FSArgList & retTypes, Evaluable* pCaller, unsigned short & listDepth, const bool & isInInit, size_t & currInd);
	

	void SetIterationLimit(const STLString & lim);
	STLString GetIterationLimit() const;

	bool NeedsLoop() const;
	void SetForceLoop(const bool & fl);

	void CleanupDeadSubEntries();

	static void FindFuncArgs(const STLString & funcArgs, StrList & foundArgs);
	static inline bool ExprHasAggFuncs(const STLString & expr);
	static CPPLoopCluster* FindCommonLoopCluster(CPPLoopCluster* lhs, CPPLoopCluster* rhs);
	static STLString GenVarName(const STLString & prefix,CPPLCSubEntry* pEntry=NULL);
	static unsigned int FindDepthDifference(Evaluable* pCaller, Evaluable* pInExpr);
	static void AddDimReference(const STLString & id, const STLString & ref);

	/** Retrieve appropriate expression based on the condition of init.
		@param pEval Pointer to the Evaluable object to query.
		@param init If true, pEval is considered to be in initialization;
		otherwise, it is considered to be in an incremental update.
		@return The initialization expression if init is true, or the 
		update expression if init is false.
	*/
	static inline STLString GetExpr(Evaluable* pEval, const bool & init)
	{
		return init ? pEval->GetInitExpr() : pEval->GetExpr();
	}
	
private:


	/** Type indicator for expression statement tokens.
	_NOTE:_ ordering of enum values below is somewhat important since it is used for a poor man's
	precedence heirarchy when coupled with std::set.
	*/
	enum TOKEN_TYPE
	{ NOTOKEN=0	   ///< Undefined token.
	, POWOPOPEN	   ///< The beginning of a power statement.
	, POWOPCLOSE   ///< The closing of a power statement.
	, SUFFIXMOD	   ///< Suffix to add to statement at position of token.
	, ARGSEP	   ///< Insert an argument seperation character.
	, DISCARD	   ///< Discard a chunk of the expression.
	, VAR		   ///< Variable token.
	, CONSTVAL	   ///< Constant value token.
	, ENUMVAL	   ///< Model enumeration token.
	, IFBLOCK	   ///< If section of If-then-else block.
	, THENBLOCK	   ///< Then section of If-then-else block.
	, ELSEBLOCK	   ///< Else section of If-then-else block.
	, MATHOP	   ///< Math operator token (+-*/%).
	, LISTBLOCK		///< block denoting a list object definition of some sort.
	, SUBBLOCK	   ///< Sub-expression token.
	, FUNCBLOCK	   ///< Function expression block.
	, BOOLOP	   ///< Boolean token (and, or).
	};

	/** Token representing facets of a piece of an expression statement. */
	struct ExprToken
	{
		TOKEN_TYPE tType;	 ///< The type of Token.
		size_t index;		 ///< The index of the character in the expression where the token applies.
		size_t length;		 ///< The length of the substring over which the token applies.
		STLString subExpr;	 ///< Optional field carrying an token-specific expression information.
		OMECFuncs::FuncSig::FS_ARGS aType; ///< The type of argument either represented or returned by the expression represented by the token.
		unsigned short listDepth; ///< The depth of list handling associated with this token.

		ExprToken()
			:tType(NOTOKEN), index(STLString::npos), length(0), aType(OMECFuncs::FuncSig::NO_ARGS), listDepth(0)
		{}

		/** Detailed constructor. 
			@param t The type of token to construct.
			@param i The index of where the token applies in the expression.
			@param l The length of the affected region of the expression.
			@param d The list depth of the token.
			@param a The type of value the represented expression chunk. Optional.
			@param subStr Additional subExpression data to associate. Optional.
		*/
		ExprToken(const TOKEN_TYPE & t, const size_t & i, const size_t & l, const unsigned short & d,const OMECFuncs::FuncSig::FS_ARGS & a = OMECFuncs::FuncSig::NO_ARGS,const STLString & subStr=STLString())
			:tType(t), index(i), length(l), aType(a),subExpr(subStr),listDepth(d) {}

		
		/** Operator used For sorting in a set container.
			Sort by lowest index, then longest length, then type.
			@param rhs The ExprToken to evaluate against.
			@return true if this preceeds rhs; false otherwise.
		*/
		bool operator<(const ExprToken& rhs) const
		{
			if (index != rhs.index)
				return index < rhs.index;
			if (tType != rhs.tType)
				return tType < rhs.tType;
			return length > rhs.length;
		}
	};

	/** Set to store ExprTokens.*/
	typedef std::set<ExprToken> TokenSet;
	
	/** map of CPPLCAccumEntry structs*/
	typedef std::map<STLString,CPPLCAccumEntry> SubArgMap;

	/** List of dimension sizes and the representative variables.*/
	typedef std::list< std::pair<STLString, size_t>> DimIncList;

	CPPLCSubEntryPtrArray m_subEntries; ///< List of Evaluables and SubLoopClusters associated with this cluster.
	CPPLoopCluster* m_pParentCluster;	///< Pointer to the Cluster that this cluster belongs to, or NULL if this cluster is root.
	ModelList m_modelList;				///< List of associated Models.
	CPPSerializer* m_pSerializer;		///< Pointer to the associated CPPSerializer.
	bool m_needsActiveCheck;			///< True if this cluster requires an active check for the model.
	size_t m_spawnerCount;				///< The total number of spawners included in this Cluster.
	bool m_forceLoop;					///< Require the generation of a loop, regardless of characteristics of contents

	CPPLCSubEntry* m_pParentEntry;		///< Pointer to parent cluster, if any.
	STLString m_label;					///< Cluster label.
	size_t m_iterations;				///< The number of fixed iterations for any loops generated from the cluster.
	STLString m_iterLimit;              ///< Statement to use if m_iterations is 0.
	StrList m_miscInits;				///< Any misc. initialization statements to be inserted at the beginning of the generated code.
	std::map<STLString,StrList> m_tempArrayDclrs; ///< Map of temporary variable array declarations.

	SubArgMap m_accums;			///< Map of accumulation invocations and their details.

	STLString m_additionalLoopLimits;	///< Additional limits for loops.

	bool m_markedForCleanup;			///< If true Cluster is marked to be cleaned up during next collection.

	unsigned int m_execGroup;		///< optional execution group represented by cluster.

	//statics
	static std::map<STLString, size_t> s_tempVarNameCounts;			///< The counts for given temporary variable names. Used for creating unique variable names.
	static std::map<STLString, CPPLCSubEntry*> s_entryRegistry;		///< Global registry for all generated CPPLCSubEntry objects.
	static std::map<STLString, STLString> s_dimReferences;			///< Map of dimension references for look up with use with element() calls.
	static StrArray s_aggSigs;										///< Accumulation functoin signatures.
	static std::map<STLString, OMECFuncs::FuncSig> s_funcSigs;		///< General Function signatures.
	static std::map<STLString, STLString> s_definedConstants;		///< Predefined constant values.
	static std::map<STLString, std::pair<STLString, STLString> > s_accumPatterns; ///< Patterns describing how to unroll accumulation patterns.

	static void ResetTmpCounts();

	void GenUpdateStatements(const bool & forInit);
	void ProcessTempPlaceholders();
	void ApplyTmpDeclarations();
	STLString GetClusterStatement();
	STLString GenSubStatements(const STLString & parentName, const STLString & grandParentName, bool* pMortalityActive = NULL);
	void GetEvalStatement(CPPLCSubEntry & se, StrList & statements, const STLString & parentIncName);

	static void ConsolidateLoops(CPPLoopCluster* pCurrCluster);

	STLString AddModelLoopFooter();
	STLString AddModelLoopHeader();
	STLString AddSubLoopHeader();
	STLString AddSubLoopFooter();

	STLString CPPizeExpression(const STLString & expr, Evaluable* pCaller, OMECFuncs::FuncSig::FS_ARGS & retType, unsigned short & listDepth, const bool & isInInit, size_t & currInd, TokenSet* retSet = NULL);

	OMECFuncs::FuncSig::FS_ARGS ProcessFunc(oSTLSStream & outBuff, unsigned short & listDepth, const STLString & call, const OMECFuncs::FuncSig & sig, Evaluable* pCaller, const bool & isInInit, size_t & currInd);
	STLString ProcessFuncHdr(const STLString & invoke, const OMECFuncs::FuncSig & sig, Evaluable* pCaller, const bool & isInInit);
	STLString ProcessEval(Evaluable* pEval, const bool & init, size_t & currInd);

	void TokenizeIfBlocks(STLString expr, TokenSet & tokens, Evaluable* pCaller, const bool & isInInit, const bool & resolveTypes, size_t & currInd);
	void TokenizeSubBlocks(const STLString & expr, TokenSet & tokens, Evaluable* pCaller, const bool & isInInit, size_t & currInd);
	void TokenizeFuncBlocks(STLString expr, TokenSet & tokens, Evaluable* pCaller, const bool & isInInit, size_t & currInd);
	void TokenizeListBlocks(const STLString & expr, TokenSet & tokens, Evaluable* pCaller, const bool & isInInit, size_t & currInd);
	void TokenizeMathOperators(const STLString & expr, TokenSet & tokens);
	void TokenizeBoolOperators(const STLString & expr, TokenSet & tokens);
	void TokenizeValues(const STLString & expr, TokenSet & tokens,Evaluable* pCaller);
	TokenSet::iterator FindTokenPrecedingIndex(const size_t & ind, const TokenSet & tokens);
	TokenSet::iterator FindTokenPrecedingOrMatchingIndex(const size_t & ind, const TokenSet & tokens);
	TokenSet::iterator FindTokenFollowingIndex(const size_t & ind, const TokenSet & tokens);
	TokenSet::iterator FindTokenFollowingOrMatchingIndex(const size_t & ind, const TokenSet & tokens);

	OMECFuncs::FuncSig::FS_ARGS ResolveFSArg(const OMECFuncs::FuncSig::FS_ARGS & lhs, const OMECFuncs::FuncSig::FS_ARGS & rhs);
	void ProcessEvaluableAssignment(oSTLSStream & outBuff, Evaluable* pCaller, const STLString & newExpr, const bool & isInInit, const OMECFuncs::FuncSig::FS_ARGS & retType);
	void ProcessListStatements();
	void ProcessListStatementsForTemps();
	void ProcessListStatements(const unsigned int & trgInd, const unsigned int & replaceInd, CPPLoopCluster* pLocalRoot, unsigned short depth = 0);
	size_t FindCountForEntry(const STLString & depthStr, const size_t & depth, CPPLoopCluster* pLocalRoot, CPPLoopCluster** hLimitClust=NULL);
	void AssignIndexAccess(CPPLCSubEntry* pEntry);

	static STLString StrForType(const OMECFuncs::FuncSig::FS_ARGS & aType);
	static STLString DclrStrForType(const OMECFuncs::FuncSig::FS_ARGS & aType, const STLString & varName, const STLString & listLength = STLString());
	static void InitSigsIfNeeded();
	static STLString DeriveIndexOffset(CPPLCSubEntry* pEntry, CPPLCSubEntry* pVarEntry);

	static inline bool TestArgCode2Char(const STLString & toTest, const size_t & ind, const OMEChar* char2Code);

	STLString ProcessAccumPattern(const STLString & pattern, const STLString & incName, const STLString & varIncName,const STLString & limitName, CPPLCSubEntry & se);
	static std::pair<STLString, STLString> GenTempArray(const STLString & src, const STLString & varName, bool & isConst);
	CPPLCSubEntry* AddSubEntry(const CPPLCSubEntry::SE_TYPE & et, const CPPLCSubEntryPtrArray::iterator & inItr, const STLString & upSt, const STLString & lbl, const STLString & ak, const size_t & num);
	static std::list<size_t> FindTmpDims(CPPLCSubEntry * pSe);
	static std::list<STLString> FindTmpDimsStr(CPPLCSubEntry * pSe);
	StrList FindTmpDimIncs(CPPLCSubEntry* pSe, CPPLCSubEntry* pVe);
	size_t CountArrayArgs(const STLString & arrStr);

	void AppendClusterContents(CPPLoopCluster* pClust);
	void PrependClusterContents(CPPLoopCluster* pClust);
	bool CrossesModelBound(CPPLCSubEntry* pSe);
	void CollectAllModels(ModelList & mdls);
	void CollectAllEvals(EvalList & entries);

	STLString InsertUpLevelInc(CPPLCSubEntry* pSe, STLString src);

};

/** Check to see if an expression contains any calls to functions that perform cross-instance aggregation.
	@param expr The expression to check for cross-instance aggregation functions.
	@return true if a cross-instance aggregation function is present; false otherwise.
*/
bool CPPLoopCluster::ExprHasAggFuncs(const STLString & expr)
{
	InitSigsIfNeeded();

	return BatchRegexMatchAny(expr, s_aggSigs, 0, true);
}

/** Test to see if the current index in a string matches a char[2] sequence.
	@param toTest The string to test for the code.
	@param ind The index of the first character to test.
	@param char2Code The char[2] pattern to test for.

	@return true if the contents at toTest[ind] matches the contents of char2Code; false otherwise.
*/
bool CPPLoopCluster::TestArgCode2Char(const STLString & toTest, const size_t & ind, const OMEChar* char2Code)
{
	return toTest[ind] == char2Code[0] && toTest[ind + 1] == char2Code[1];
}
