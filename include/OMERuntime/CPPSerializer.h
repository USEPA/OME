#pragma once
#include"OMEDefines.h"
#include "BaseEvalSerializer.h"
#include "CompiledMdlFuncs.h"
#include "CPPLoopCluster.h"
#include <stack>

class Model;


/** Class used to serialize a model into a C++ representation of model logic. */
class __EXPORT__ CPPSerializer : public BaseEvalSerializer
{
	friend class CPPLoopCluster;
public:
	CPPSerializer(Model* pRootModel=NULL);
	virtual ~CPPSerializer();

	virtual bool SerializeEval(Model* pRootModel);
	virtual bool SerializeEval(const EvalArray & entries,const bool forInit=false);
	virtual bool SerializeEval(Evaluable* entry,const bool forInit);
	virtual STLString GetInitStatements(const bool forPrint=false);
	virtual STLString GetEvalStatements(const bool forPrint=false);
	virtual void ClearSerializer();
	virtual bool WriteSerialExpressions(const STLString & initPath, const STLString & evalPath);
	virtual size_t GetLineCount(const bool forInit);
	
	//CPPSerializer specific
	bool WriteToCPPFile(Model* pMdl, const STLString & className, STLString path, ISpatialDataProvider* pSDP);

	//debug stuff not used with cpp code
	virtual bool PopulateBreaks(BreakSet & breaks,const bool forInit) {return false;};
	
	virtual void SetInitBreakPath(const STLString & path) {};
	virtual STLString GetInitBreakPath() const { return STLString(); };
	virtual void SetEvalBreakPath(const STLString & path) {} ;
	virtual STLString GetEvalBreakPath() const { return STLString(); };

	virtual void SetDbgEnabled(const bool & dbg) {};
	virtual bool GetDbgEnabled() const { return false;};
	bool WriteMainFile(const STLString & className, STLString path, BaseManager* pBm);

	/** Set the path to the directory containing the template files.
		@param tDir The path to the directory containing the template files.
	*/
    inline void SetTemplateDir(const STLString & tDir) { m_templateDir=tDir;}
	/** @return Path to directory containging template files. */
    inline STLString GetTemplateDir() { return m_templateDir; }
    
private:

	/** Struct for Model enumerated value entry. */
	struct ModelEnumEntry
	{
		Model* pMdl;			///< Pointer to model containing enumerated value declarations.
		ModelEnum* pEEnum;		///< Pointer to class of enumerated value.
		EnumEntry* pEEntry;		///< Pointer to enumerated value entry.
		ModelEnumEntry() :pMdl(NULL), pEEnum(NULL), pEEntry(NULL){}

		/** Equality Operator.
			@param rhs The ModelEnumEntry to compare Against
			@return true if ModelEnumEntry and rhs are equivalent; false otherwise.
		*/
		bool operator==(const ModelEnumEntry & rhs)
		{	return pMdl == rhs.pMdl && pEEnum == rhs.pEEnum && pEEntry == rhs.pEEntry; }

		/** Inequality Operator.
		@param rhs The ModelEnumEntry to compare Against
		@return true if ModelEnumEntry and rhs are equivalent; false otherwise.
		*/
		bool operator!=(const ModelEnumEntry & rhs)
		{	return pMdl != rhs.pMdl || pEEnum != rhs.pEEnum || pEEntry != rhs.pEEntry;}
	};

	std::list<ModelEnumEntry> m_enumsToWrap;        ///< List of enumerated values found in serialization process.

	oSTLSStream m_cache;							///< ostringstream containing text to be written out.
	oSTLSStream m_arrayCache;						///< Buffer of simple array declarations.

	int m_constArrayCount;							///< The total number of constant arrays found in serialized declarations.
	STLString m_className;							///< Name of generated class.
	Model* m_pRootModel;							///< Root model object that is being serialized
    STLString m_templateDir;						///< Path to directory containing template files.
	CPPLoopCluster m_rootCluster;					///< Root CPPLoopCluster of cluster tree.

	bool SerializeModel(Model *pRootModel,const bool useInit);

	bool NeedsHeader(Model* pBm);

	//bool CheckConstArray(const STLString & expr, oSTLSStream & out,const bool & useInit);

	void AddEnumForWrap(const ModelEnumEntry & eee);

	STLString ConstructHeader(const STLString & hName, ISpatialDataProvider* pSDP);
	void WriteModelKeys(Model* pMdl);
	void WriteModelInitializations(Model* pMdl,oSTLSStream & decCache, oSTLSStream & iniCache);
	void WriteEnumInitializations(oSTLSStream & decCache, oSTLSStream & iniCache);
	//void WriteSpawnerUpdates(Model* pMdl);
	//void ProcessSpawnerUpdateLevel(Model* pMdl,const bool & init);
	void WriteEvalInitialization(Evaluable* pEval,oSTLSStream & decCache, oSTLSStream & iniCache);
	void WriteVAAllocs(Model* pRoot, oSTLSStream & iniCache);
	//void AddFooter();

	/** Fix escape character tagging.
		@param str String containing the escape characters to correct.
		@return a copy of str with escape characters properly fixed.
	*/
	inline STLString TagEscapes(const STLString & str)
	{

		const char* escapes[] = { "\n", "\\n",
			"\t", "\\t",
			"\r", "\\r",
			"\"", "\\\"",
			"\0"
		};
		return BatchRegexReplace(str, escapes);
	}

	/** Close a cluser loop declaration.
		@param mdlStack The stack of cluster-associated models.
		@param pCluster Pointer to the current CPPLoopCluster being closed.
		@return Pointer to the parent CPPLoopCluster of pCluster.
	*/
	inline CPPLoopCluster* EndCluster(std::stack< std::pair<Model*, bool> > & mdlStack, CPPLoopCluster* pCluster)
	{
		if (mdlStack.top().second)
			pCluster->EndMortalityHere(mdlStack.top().first);
		pCluster = pCluster->GetParentCluster();
		if (!pCluster)
			pCluster = &m_rootCluster;

		mdlStack.pop();
		return pCluster;
	}

	static const OMEChar* m_sOmeTypeStrs[];
};

