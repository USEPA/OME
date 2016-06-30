#pragma once
#include "OMEDefines.h"

class Evaluable;
class CPPSerializer;
class CPPLoopCluster;

class CPPLCSubEntry;
typedef std::deque<CPPLCSubEntry> CPPLCSubEntryArray;
typedef std::deque<CPPLCSubEntry*> CPPLCSubEntryPtrArray;

/** Entry for subcluster collection.*/
class CPPLCSubEntry
{
public:
	/** Flag denoting entry class. */
	enum SE_TYPE {
		EVAL = 0		///< Represents an Evaluable Model object.
		, SUBCLST		///< Represents a sub-CPPLoopCluster and optionally an associated submodel.
		, TMPDCLR		///< The declaration statement of a temporary variable.
		, TMPINIT       ///< The initialization statement of a temporary variable.
		, TMPASGN       ///< The assignment statement of a temporary variable.
		, STARTMORT    ///< Point at which to begin checking a model's mortality state.
		, ENDMORT		///< Point at which to stop checking for a model's mortality state.
		, SRCLBL        ///< Comment text to add to generated source code.
		, TMPMARK       ///< Point at which a temporary variable should be created and inserted.
		, CUSTOM		///< Custom source code to be added.
	};

	CPPLCSubEntry(CPPLoopCluster* pParent, Evaluable* pE = NULL, const STLString & upSt = STLString());
	CPPLCSubEntry(CPPLoopCluster* pParent, const SE_TYPE & et, const STLString & upSt = STLString(), const STLString & lbl = STLString(), const size_t & num = 0);
	CPPLCSubEntry(CPPLoopCluster* pParent, const SE_TYPE & et, const STLString & upSt, const STLString & lbl, const STLString & ak, const size_t & num = 0);
	CPPLCSubEntry(CPPLoopCluster* pParent, const SE_TYPE & et, const size_t & num, const STLString & lbl = STLString());
	CPPLCSubEntry(CPPLoopCluster* pParent, CPPSerializer* pCS, const bool & useActive, Model* pMdl, const STLString & lbl, const size_t & iterations);
	CPPLCSubEntry(const CPPLCSubEntry & se);
	CPPLCSubEntry& operator=(const CPPLCSubEntry & rhs);

	~CPPLCSubEntry();

	void FindDependents(std::map<STLString, CPPLCSubEntry*> *pReg = NULL);
	void FindAllDependents(std::map<STLString, CPPLCSubEntry*> *pReg=NULL);
	bool IsDependentOn(CPPLCSubEntry & se);
	bool AccumulatesOn(CPPLCSubEntry & se);
	bool CompatibleExecGroups(CPPLCSubEntry & se);

	CPPLoopCluster* GetSubLoopCluster();
	CPPLoopCluster* GetParentLoopCluster();
	void SetParentLoopCluster(CPPLoopCluster* pClust);
	Evaluable* GetEvaluable();

	/** @return Enum representing the type of the represented value. */
	inline SE_TYPE GetEntryType() const
	{
		return m_eType;
	}

	/** Set condition for loop termination.
		@param chk The condition to apply.
	*/
	inline void SetLoopLimitCheck(const STLString & chk){ m_loopLimitCheck = chk; }
	/** @return String containing the condition for loop termination. */
	inline STLString GetLoopLimitCheck() const { return m_loopLimitCheck; }

	void SetUpdateStatement(const STLString & str);
	STLString GetUpdateStatement() const;

	void SetCount(const size_t & c);
	size_t GetCount() const;
	
	void SetDimCount(const unsigned short & c);
	unsigned short GetDimCount() const;

	std::set<STLString>* GetDependsSetPtr();
	void SetLabel(const STLString & str);
	STLString GetLabel() const;

	void SetAccumKey(const STLString & str);
	STLString GetAccumKey() const;


	bool ContainsVariableWithName(const STLString & name);
	void SetMortModel(Model* pMdl);
	Model* GetModel() const;
	bool HasConditionalModel() const;

	void AddAssigned(const STLString & assigned);
	void AddDepends(const STLString & depends);

private:
	SE_TYPE m_eType;				///< The classification of the Entry.
	STLString m_updateStatement;	///< The base statement to insert into the generated source code.
	STLString m_label;				///< The entry's label, if any.
	STLString m_accumKey;			///< The id of the associated CPPLCAccumEntry.
	size_t m_count;					///< The number of instances expected for the list form of this entry.
	unsigned short m_dimCount;		///< The total number of dimensions associated with represented value.
	STLString m_loopLimitCheck;     ///< Additional checks to do as the run condition for any enclosing for loop.

	CPPLoopCluster* m_pParentCluster;  ///< Pointer to the enclosing CPPLoopCluster object.

	std::set<STLString> m_dependents; ///< Set of IDs involved in the update statement.
	std::set<STLString> m_requiredAccums; ///< List of accumulation loops which must be completed before this entry can be evaluated.
	std::set<STLString> m_contains;		///< IDs of all entries contained within this sub-entry.

	union
	{
		CPPLoopCluster* m_pCluster;  ///< Pointer to a subcluster.
		Evaluable* m_pEval;		     ///< Pointer to an associated Evaluable object.
		Model* m_pMortMdl;			 ///< Pointer to model whose Mortality needs to be checked.
	};

};

