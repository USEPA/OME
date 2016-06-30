#include <list>
#include <regex>
#include "CPPLoopCluster.h"
#include "CompiledMdlFuncs.h"
#include "CPPSerializer.h"
#include "CPPLCSubEntry.h"


using namespace OMECFuncs;

#pragma region static var declarations
#pragma mark - static var declarations

/// @cond DOX_NO_DOC
std::map<STLString, FuncSig> CPPLoopCluster::s_funcSigs;
std::map<STLString, std::pair<STLString, STLString> > CPPLoopCluster::s_accumPatterns;
StrArray CPPLoopCluster::s_aggSigs;
std::map<STLString, size_t> CPPLoopCluster::s_tempVarNameCounts;
std::map<STLString, CPPLCSubEntry*> CPPLoopCluster::s_entryRegistry;
std::map<STLString, STLString> CPPLoopCluster::s_dimReferences;
std::map<STLString, STLString> CPPLoopCluster::s_definedConstants;
///@endcond

#pragma endregion

/** Detailed Constructor.
	@param pCS Pointer to associated CPPSerializer.
	@param lbl The name of the cluster.
	@param iters The number of iterations to initially assign to the cluster
	@param useActiveCheck Bool indicating that this cluster will require an active model check.
	@param pMdl Pointer to Model associated with CPPLoopCluster.
	@param parent Pointer to the parent CPPLoopCluster.
	@param pPEnt Pointer to the parent CPPLCSubEntry, if any.
	*/
CPPLoopCluster::CPPLoopCluster(CPPSerializer* pCS, const STLString & lbl, const size_t & iters, const bool & useActiveCheck, Model* pMdl, CPPLoopCluster* parent, CPPLCSubEntry* pPEnt)
: m_pParentCluster(parent), m_pSerializer(pCS), m_needsActiveCheck(useActiveCheck)
, m_spawnerCount(0), m_iterations(iters), m_label(lbl), m_forceLoop(false), m_pParentEntry(pPEnt)
, m_markedForCleanup(false), m_execGroup(OME_NO_EXEC_GRP)
{

	if (pMdl)
		m_modelList.push_back(pMdl);
	InitSigsIfNeeded();
	if (pMdl && pMdl->IsConditional())
		m_forceLoop = true;
	if (pMdl && /*pMdl->GetInstMode()==Model::RECORD*/ pMdl->ExpectsSpatial())
	{
		m_iterations = 0;	//filled out later.
		m_iterLimit = pMdl->GetID() + ".GetNumInstances()";
	}
}

CPPLoopCluster::~CPPLoopCluster()
{
	ClearAllEntries();
}

/** Generate C++ code for this entry and all sub entries. The generated update statements are then stored on the loop object itself.
	@param forInit if true, generate statements for initialization; otherwise, generate statements for update.
*/
void CPPLoopCluster::GenUpdateStatements(const bool & forInit)
{
	for (size_t i = 0; i < m_subEntries.size(); ++i)
	{
		CPPLCSubEntry* pSe = m_subEntries[i];
		switch (pSe->GetEntryType())
		{
		case CPPLCSubEntry::EVAL:
			//process Evaluable expression.
			pSe->SetUpdateStatement(ProcessEval(pSe->GetEvaluable(), forInit, i));
			break;
		case CPPLCSubEntry::SUBCLST:
			pSe->GetSubLoopCluster()->GenUpdateStatements(forInit);
			break;
			/*case CPPLCSubEntry::TMPSUB:
			case TMPDCLR:
			*/
		}

	}
}

/** Reduce the number of for loops in the generated code by combining CPPLoopCluster objects based on simple heuristics.
	@param pCurrCluster Pointer to the cluster currently being evaluated.
*/
void CPPLoopCluster::ConsolidateLoops(CPPLoopCluster* pCurrCluster)
{
	//problem: node01884 not merging properly
	//solution: rework merge rules ?


	//first pass: consolidate temploops as best as possible
	CPPLCSubEntryPtrArray & subs = pCurrCluster->m_subEntries;
	CPPLCSubEntry* pEntry;


	//find first root for each type (count backwards to get lowest without check)
	std::map<STLString, size_t> roots;
	for (size_t i = subs.size(); i >0; --i)
	{
		size_t trueInd = i - 1;
		if (subs[trueInd]->GetEntryType() == CPPLCSubEntry::SUBCLST)
			roots[subs[trueInd]->GetSubLoopCluster()->GetIterationLimit()] = trueInd;
	}

	//walk through each group of matching keys, attempting to walk backward and merge with lowest instance
	size_t rootInd;
	STLString key;
	size_t s;
	for (size_t i = 1; i < subs.size(); ++i)
	{
		switch(subs[i]->GetEntryType())
		{
		case CPPLCSubEntry::SUBCLST:
			key = subs[i]->GetSubLoopCluster()->GetIterationLimit();

			rootInd = roots[key];

			if (i != rootInd)
			{
				s = i-1;
				for (; s > rootInd && !subs[i]->IsDependentOn(*subs[s]); --s);
				
				if (s == rootInd && !subs[i]->AccumulatesOn(*subs[rootInd]) && subs[i]->CompatibleExecGroups(*subs[rootInd]))
				{

					subs[rootInd]->GetSubLoopCluster()->AppendClusterContents(subs[i]->GetSubLoopCluster());
					subs[i]->GetSubLoopCluster()->m_markedForCleanup = true;

				}
				else
					roots[key] = i;
			}
			break;
		default:
			// bubble up as far as possible
			s = i;
			for (; s > 0 && !subs[s]->IsDependentOn(*subs[s - 1]) && subs[s]->CompatibleExecGroups(*subs[s-1]); --s)
			{
				if (subs[s - 1]->GetEntryType() == CPPLCSubEntry::SUBCLST && roots[subs[s-1]->GetSubLoopCluster()->GetIterationLimit()] == s - 1)
					roots[subs[s-1]->GetSubLoopCluster()->GetIterationLimit()] = s;
				//swap
				CPPLCSubEntry* pTmp = subs[s];
				subs[s] = subs[s - 1];
				subs[s - 1] = pTmp;
			}

		}

	}

	//remove any merged loops
	pCurrCluster->CleanupDeadSubEntries();

	//rebuild root map (highest index is root this time)
	roots.clear();
	for (size_t i = 0; i < subs.size(); ++i)
	{
		if (subs[i]->GetEntryType() == CPPLCSubEntry::SUBCLST)
			roots[subs[i]->GetSubLoopCluster()->GetIterationLimit()] = i;
	}

	//attempt merge in reverse order
	for (size_t i = subs.size(); i >0; --i)
	{

		size_t trueInd = i - 1;
		switch(subs[trueInd]->GetEntryType())
		{
		case CPPLCSubEntry::SUBCLST:
			key = subs[trueInd]->GetSubLoopCluster()->GetIterationLimit();
			rootInd = roots[key];


			if (trueInd != rootInd)
			{
				s = trueInd + 1;
				for (; s < rootInd && !subs[s]->IsDependentOn(*subs[trueInd]); ++s);

				if (s == rootInd &&!subs[rootInd]->AccumulatesOn(*subs[trueInd]) && subs[rootInd]->CompatibleExecGroups(*subs[trueInd]))
				{
					subs[rootInd]->GetSubLoopCluster()->PrependClusterContents(subs[trueInd]->GetSubLoopCluster());
					subs[trueInd]->GetSubLoopCluster()->m_markedForCleanup = true;

				}
				else
					roots[key] = trueInd;
			}
			break;
		default:
			// bubble down as far as possible
			s = trueInd+1;
			for (; s < subs.size() && !subs[s]->IsDependentOn(*subs[s - 1]) && subs[s]->CompatibleExecGroups(*subs[s - 1]); ++s)
			{
				if (subs[s]->GetEntryType() == CPPLCSubEntry::SUBCLST && roots[subs[s]->GetSubLoopCluster()->GetIterationLimit()] == s)
					roots[subs[s]->GetSubLoopCluster()->GetIterationLimit()] = s-1;
				//swap
				CPPLCSubEntry* pTmp = subs[s];
				subs[s] = subs[s - 1];
				subs[s - 1] = pTmp;
			}
		}

	}
	pCurrCluster->CleanupDeadSubEntries();

	//recursively adjust sub clusters
	for (CPPLCSubEntry* pSe : subs)
	{
		switch (pSe->GetEntryType())
		{
		case CPPLCSubEntry::SUBCLST:
			ConsolidateLoops(pSe->GetSubLoopCluster());
			break;
		}
	}

}

/**@return the cached update statement combined with all statements from sub entries.
*/
STLString CPPLoopCluster::GetClusterStatement()
{
	StrList entries=m_miscInits;
	bool mortStartHit = false;

	entries.push_back(GenSubStatements(m_label + "_i", std::to_string(m_iterations), &mortStartHit));
	STLString body = ConcatStrings(entries, '\n');
	entries.clear();
	if (!body.empty())
	{
		entries.push_back(body);
		//add loop header
		if (m_pParentCluster)
		{
			if (!m_modelList.empty())
			{
				//add loop header
				entries.push_front(AddModelLoopHeader());
				if (mortStartHit)
					m_needsActiveCheck = true;

				//add loop footer
				entries.push_back(AddModelLoopFooter());
			}
			else
			{
				entries.push_front(AddSubLoopHeader());
				entries.push_back(AddSubLoopFooter());
			}
		}
	}
	return ConcatStrings(entries, '\n');
}

/** Generate C++ code for all contained sub CPPLCSubEntries contained within.
	@param parentName The name of the increment variable for the innermost parent loop.
	@param grandParentName The name of the increment variable for the second-innermost parent loop.
	@param pMortalityActive Pointer to a boolean value
*/
STLString CPPLoopCluster::GenSubStatements(const STLString & parentName, const STLString & grandParentName, bool* pMortalityActive)
{
	StrList entries;
	Evaluable* pEval;

	//add as needed

	STLString tmpStr;
	size_t pos;
	for (CPPLCSubEntry* pSe : m_subEntries)
	{
		STLString limitStr = std::to_string(pSe->GetCount());
		const OMEChar* repPatterns[] = { "@ii", parentName.c_str(),
			"@li", limitStr.c_str(),
			"\0" };


		switch (pSe->GetEntryType())
		{
		case CPPLCSubEntry::SRCLBL:
			entries.push_back("//=-=-=-=-=-=-=-=-=-= " + pSe->GetUpdateStatement());
			break;
		case CPPLCSubEntry::STARTMORT:
				//attach mortality check
				entries.push_back(pSe->GetModel()->GetID() + ".GetCurrentInstanceRef()->ApplyMortality();");
				entries.push_back("if (" + pSe->GetModel()->GetID() + ".GetCurrentInstanceActive())");
				entries.push_back("{");
				*pMortalityActive = true;
			
			break;
		case CPPLCSubEntry::ENDMORT:
			entries.push_back("} //" + pSe->GetModel()->GetID());
			break;
		case CPPLCSubEntry::EVAL:
			AssignIndexAccess(pSe);
			pEval = pSe->GetEvaluable();
			entries.push_back("//=-=-=-=-=-=-=-=-=-= " + pEval->GetID() + " (" + pEval->GetExprName() + ")");
			GetEvalStatement(*pSe, entries, parentName);
			break;
		case CPPLCSubEntry::SUBCLST:
			AssignIndexAccess(pSe);
			entries.push_back(pSe->GetSubLoopCluster()->GetClusterStatement());
			break;
		case CPPLCSubEntry::TMPDCLR:
			//we should know the initialization size at this point;
			//use replace to apply it.
			tmpStr = InsertUpLevelInc(pSe,BatchRegexReplace(pSe->GetUpdateStatement(), repPatterns));
			//the entry for this label should be the associate TMPASGN which by this point should know the correct size.
			pos = tmpStr.find("@xi");
			if (pos != STLString::npos)
			{
				if (s_entryRegistry[pSe->GetLabel()]->GetCount() != 1)
				{
					std::list<size_t> dims = FindTmpDims(s_entryRegistry[pSe->GetLabel()]);
					StrList dStrs;
					for (size_t & d : dims)
						dStrs.push_back(std::to_string(d));
					tmpStr.replace(pos, 3, ConcatStrings(dStrs, "]["));
					tmpStr += "\nsize_t " + pSe->GetLabel() + "_dims[]={" + ConcatStrings(dStrs, ',') + "};";
				}
				else
					tmpStr = "OME_SCALAR " + pSe->GetLabel() + ";";
			}
			entries.push_back(tmpStr);
			break;
		case CPPLCSubEntry::TMPINIT:
			//must always initialize, just in case scope is within loop
			if (!pSe->GetAccumKey().empty())
				pSe->SetUpdateStatement(ProcessAccumPattern(s_accumPatterns[pSe->GetAccumKey()].first, parentName, grandParentName, limitStr, *pSe));
			AssignIndexAccess(pSe);
			entries.push_back(pSe->GetUpdateStatement());
			break;
		case CPPLCSubEntry::TMPASGN:
			if (!pSe->GetAccumKey().empty())
				pSe->SetUpdateStatement(ProcessAccumPattern(s_accumPatterns[pSe->GetAccumKey()].second, parentName, parentName, limitStr, *pSe));
			AssignIndexAccess(pSe);
			entries.push_back(pSe->GetUpdateStatement());
			break;
		case CPPLCSubEntry::CUSTOM:
			AssignIndexAccess(pSe);
			tmpStr = InsertUpLevelInc(pSe,BatchRegexReplace(pSe->GetUpdateStatement(), repPatterns));
			entries.push_back(tmpStr);
			break;
		}
	}
	return ConcatStrings(entries, '\n');
}

/** Retrieve the dimensions for every loop from pSe to the root loop, omitting static loops with a size of 1.
	@param pSe Pointer to the CPPLCSubEntry to evaluate for dimensions.
	@return An ordered list of loop limits.
*/
std::list<size_t> CPPLoopCluster::FindTmpDims(CPPLCSubEntry * pSe)
{
	std::list<size_t> dims;
	CPPLoopCluster* pClust = pSe->GetParentLoopCluster();
	while (!pClust->NeedsLoop())
		pClust = pClust->GetParentCluster();
	for (unsigned short i = 0; i < pSe->GetDimCount(); ++i)
	{
		dims.push_front(pClust->m_iterations);
		do
		{
			pClust = pClust->GetParentCluster();
		} while (!pClust->NeedsLoop());
	}

	return dims;
}

/**
	Retrieve the iteration loop limits for all clusters from pSe and up.
	@param pSe Pointer to the sub-entry to start collecting limits.
	@return A list of loop limits.
*/
std::list<STLString> CPPLoopCluster::FindTmpDimsStr(CPPLCSubEntry * pSe)
{
	std::list<STLString> dims;
	CPPLoopCluster* pClust = pSe->GetParentLoopCluster();
	while (pClust && (!pClust->NeedsLoop() || pClust->m_iterations==1))
		pClust = pClust->GetParentCluster();
	while(pClust)
	{
		dims.push_back(pClust->GetIterationLimit());
		do
		{
			pClust = pClust->GetParentCluster();
		} while (pClust && (!pClust->NeedsLoop() || pClust->m_iterations == 1));
	}

	return dims;
}

/** Retrieve the name of increment variables for every loop from pSe (or pVe if it represents a VarArray) to the root loop.
@param pSe Pointer to the CPPLCSubEntry to evaluate for dimensions.
@param pVe Pointer to the CPPLCSubEntry to evaluate for dimensions if it represents a VarArray.
@return An ordered list of increment variable names.
*/
StrList CPPLoopCluster::FindTmpDimIncs(CPPLCSubEntry* pSe, CPPLCSubEntry* pVe)
{
	DimIncList dims;
	CPPLoopCluster* pClust = pSe->GetParentLoopCluster();
	while (pClust && !pClust->NeedsLoop())
		pClust = pClust->GetParentCluster();
	while (pClust)
	{
		dims.emplace_back(pClust->m_label+"_i",pClust->m_iterations);
		do
		{
			pClust = pClust->GetParentCluster();
		} while (pClust && !pClust->NeedsLoop());
	}

	bool hasVarArray = pVe->GetEntryType() == CPPLCSubEntry::EVAL && pVe->GetEvaluable()->IsOMEType(OME_VARARRAY);
	StrList outIncs;
	if (hasVarArray)
	{
		VarArray* pVa = (VarArray*)pVe->GetEvaluable();
		for (unsigned short i = 0; i < pVa->GetDimCount(); ++i)
		{
			unsigned short oldSize = outIncs.size();
			for (auto itr = dims.begin(); itr != dims.end();++itr)
			{
				if (pVa->GetDim(i) == (*itr).second)
				{
					outIncs.push_front((*itr).first);
					dims.erase(itr);
					break;
				}
			}
			if (oldSize == outIncs.size())
				throw OMEObjException("FindTmpDimIncs: VarArray dim mismatch.",pVa);
		}
	}

	if (pVe->GetEntryType() != CPPLCSubEntry::TMPDCLR)
	{
		pClust = pVe->GetParentLoopCluster();
		while (pClust && !pClust->NeedsLoop())
			pClust = pClust->GetParentCluster();
		size_t preHit = 0;
		while (pClust && !dims.empty())
		{
			for (auto itr = dims.begin(); itr != dims.end(); ++itr)
			{
				if (pClust->m_iterations == (*itr).second)
				{
					outIncs.push_front((*itr).first);
					if (!hasVarArray)
					{
						dims.erase(itr);
						break;
					}
				}
			}
			
			do
			{
				pClust = pClust->GetParentCluster();
			} while (pClust && !pClust->NeedsLoop());

			++preHit;
		}
	}
	else
	{
		for (auto & d : dims)
		{
			outIncs.push_front(d.first);
		}
	}


	return outIncs;
}
/** Retrieve update statement.
	@param se The CPPLCSubEntry containing the update statement to modify and return.
	@param statements List of statements to populate with new expression.
	@param parentIncName The name to use for the increment variable to use, if needed.
*/
void CPPLoopCluster::GetEvalStatement(CPPLCSubEntry & se, StrList & statements, const STLString & parentIncName)
{

	//check to see if we need to calculate any temps before declaration.
	//process Evaluable expression.
	StrList listAggs;
	VarArray* pVa;
	oSTLSStream initBuff;
	oSTLSStream aggBuff;
	STLString incName = se.GetEvaluable()->GetID() + "_i";
	STLString limitName = se.GetEvaluable()->GetID() + ".GetSize()";

	const OMEChar* repPatterns[] = { "@ii", parentIncName.c_str(), "\0" };
	statements.push_back(InsertUpLevelInc(&se,BatchRegexReplace(se.GetUpdateStatement(), repPatterns)));
}

/** 
	Generate linkages between a given CPPLoopCluster and any entries that require its logic to be completed before its
	update statement can be executed.
*/
void CPPLoopCluster::FindAllDependents()
{

	StrList loopLimits;
	STLString lim;
	//find names
	for (CPPLCSubEntry* pSe : m_subEntries)
	{
		pSe->FindAllDependents(&s_entryRegistry);

		//use this opportunity to build loop limit string
		lim = pSe->GetLoopLimitCheck();
		if (!lim.empty())
			loopLimits.push_back(lim);
	}

	m_additionalLoopLimits = ConcatStrings(loopLimits, " && ");

	
}


/** Generate C++ update expressions by recursively processing cluster tree.
	@param forInit If true, produce statements for initialization; otherwise produce update statements.
	@return A string containing all update statements up to this level.
	*/
STLString CPPLoopCluster::ProcessTree(const bool & forInit)
{
	if (m_pParentCluster)
		throw OMEFuncOpException("CPPLoopCluster::ProcessTree:", "cluster must be root");

	ResetTmpCounts();	 //static call.

	GenUpdateStatements(forInit);
	ApplyTmpDeclarations();
	ProcessTempPlaceholders();
	ProcessListStatements();
	ProcessListStatementsForTemps();

	FindAllDependents();

	ConsolidateLoops(this);

	return GetClusterStatement();

}

/** Add a new Evaluable to the LoopCluster.
	@param pE Pointer to the Evaluable object to add.
	@param upst The update statement to use.
	*/
void CPPLoopCluster::AppendEval(Evaluable* pE, const STLString & upst)
{
	CPPLCSubEntry* pEntry = new CPPLCSubEntry(this, pE, upst);
	m_subEntries.push_back(pEntry);
	s_entryRegistry[pE->GetID()] = pEntry;
	if (pE->IsOMEType(OME_SPAWNER))
		++m_spawnerCount;
}

/** Generate an entry representing a comment in the C++ source code and append to the end of the entry list.
	@param lbl The body of the comment text.
*/
void CPPLoopCluster::AppendSrcLabel(const STLString & lbl)
{
	m_subEntries.push_back(new CPPLCSubEntry(this, CPPLCSubEntry::SRCLBL, lbl));
}

/** Generate and add a new LoopCluster to the sub collection of this cluster.
	@param pMdl Pointer to the represented Model.
	@param useActive If true add active checks for submodel when statements are generated; otherwise omit checks.
	@return Pointer to the newly generated Cluster.
	*/
CPPLoopCluster* CPPLoopCluster::NewSubLoopCluster(Model* pMdl, const bool & useActive)
{
	CPPLCSubEntry* pRet = new CPPLCSubEntry(this, m_pSerializer, useActive, pMdl, pMdl->GetID(), pMdl->IsDynamic() ? 0 : pMdl->GetInitNumInstances());
	m_subEntries.push_back(pRet);
	return pRet->GetSubLoopCluster();
}

/** Generate and add a new LoopCluster to the sub collection of this cluster.
	@param currInd The index at which to insert the new CPPLoopCluster.
	@param len The number of loop iterations to associate with the new entry.
	@param label The name of the new CPPLoopCluster.
	@return Pointer to the newly generated Cluster.
*/
CPPLoopCluster* CPPLoopCluster::NewSubLoopCluster(const size_t & currInd, const size_t & len, const STLString & label)
{
	CPPLCSubEntry* pRet = new CPPLCSubEntry(this, m_pSerializer, false, NULL, label, len);
	if (m_subEntries.size())
		m_subEntries.insert(m_subEntries.begin() + currInd, pRet);
	else
		m_subEntries.push_back(pRet);
	return pRet->GetSubLoopCluster();
}

/** Generate and add a new LoopCluster to the end of the sub collection of this cluster.
	@param len The number of loop iterations to associate with the new entry.
	@param label The name of the new CPPLoopCluster.
	@return Pointer to the newly generated Cluster.
*/
CPPLoopCluster* CPPLoopCluster::AppendSubLoopCluster(const size_t & len, const STLString & label)
{
	CPPLCSubEntry* pRet = new CPPLCSubEntry(this, m_pSerializer, false, NULL, label, len);
	m_subEntries.push_back(pRet);
	return pRet->GetSubLoopCluster();
}

/** Generate and add a new temporary variable declaration to the sub collection of this cluster.
	@param index The index at which to insert the new temporary variable declaration.
	@param upSt The C++ code to associate with the new declaration.
	@param lbl The name of the temporary variable.
	@param ak The accumulation function key to associate, if any.
	@param num The count to associate with this variable.
	@return Pointer to the newly generated CPPLCSubEntry.
*/
CPPLCSubEntry* CPPLoopCluster::NewTempDeclaration(const size_t & index, const STLString & upSt, const STLString & lbl, const STLString & ak, const size_t & num)
{
	CPPLCSubEntry* pNewEntry = AddSubEntry(CPPLCSubEntry::TMPDCLR, m_subEntries.begin() + index, upSt, lbl, ak, num);
	pNewEntry->AddAssigned(lbl);
	if (s_entryRegistry.find(pNewEntry->GetLabel()) == s_entryRegistry.end())
		s_entryRegistry[pNewEntry->GetLabel()] = pNewEntry;
	return pNewEntry;
}

/** Generate and add a subentry with C++ code where a temporary variable is used as part of overall model logic.
	@param index The index at which to insert the new temporary variable application.
	@param upSt The C++ code to use.
	@param lbl The name of the temporary variable.
	@param ak The accumulation function key to associate, if any.
	@param num The count to associate with this variable.
	@return Pointer to the newly generated CPPLCSubEntry.
*/
CPPLCSubEntry* CPPLoopCluster::NewTempApplication(const size_t & index, const STLString & upSt, const STLString & lbl, const STLString & ak, const size_t & num)
{
	CPPLCSubEntry* pNewEntry = AddSubEntry(CPPLCSubEntry::TMPASGN, m_subEntries.begin() + index, upSt, lbl, ak, num);
	pNewEntry->AddAssigned(lbl);
	s_entryRegistry[pNewEntry->GetLabel()] = pNewEntry;
	return pNewEntry;
}

/** Generate and add a new temporary variable initialization statement to the sub collection of this cluster.
	@param index The index at which to insert the new temporary variable initialization.
	@param upSt The C++ code to associate with initialization of the temporary variable.
	@param lbl The name of the temporary variable.
	@param ak The accumulation function key to associate, if any.
	@param num The count to associate with this variable.
	@return Pointer to the newly generated CPPLCSubEntry.
*/
CPPLCSubEntry* CPPLoopCluster::NewTempInitialization(const size_t & index, const STLString & upSt, const STLString & lbl, const STLString & ak, const size_t & num)
{
	CPPLCSubEntry* pNewEntry = AddSubEntry(CPPLCSubEntry::TMPINIT, m_subEntries.begin() + index, upSt, lbl, ak, num);
	pNewEntry->AddAssigned(lbl);
	s_entryRegistry[pNewEntry->GetLabel()] = pNewEntry;
	return pNewEntry;
}

/** Generate and append a new temporary variable declaration to the sub collection of this cluster.
	@param upSt The C++ code to associate with the new declaration.
	@param lbl The name of the temporary variable.
	@param ak The accumulation function key to associate, if any.
	@param num The count to associate with this variable.
	@return Pointer to the newly generated CPPLCSubEntry.
*/
CPPLCSubEntry* CPPLoopCluster::AppendTempDeclaration(const STLString & upSt, const STLString & lbl, const STLString & ak, const size_t & num)
{
	CPPLCSubEntry* pNewEntry = AddSubEntry(CPPLCSubEntry::TMPDCLR, m_subEntries.end(), upSt, lbl, ak, num);
	pNewEntry->AddAssigned(lbl);
	s_entryRegistry[pNewEntry->GetLabel()] = pNewEntry;
	return pNewEntry;
}

/** Generate and append a subentry with C++ code where a temporary variable is used as part of overall model logic.
	@param upSt The C++ code to use.
	@param lbl The name of the temporary variable.
	@param ak The accumulation function key to associate, if any.
	@param num The count to associate with this variable.
	@return Pointer to the newly generated CPPLCSubEntry.
*/
CPPLCSubEntry* CPPLoopCluster::AppendTempApplication(const STLString & upSt, const STLString & lbl, const STLString & ak, const size_t & num)
{
	CPPLCSubEntry* pNewEntry = AddSubEntry(CPPLCSubEntry::TMPASGN, m_subEntries.end(), upSt, lbl, ak, num);
	pNewEntry->AddAssigned(lbl);
	s_entryRegistry[pNewEntry->GetLabel()] = pNewEntry;
	return pNewEntry;
}

/** Generate and append a new temporary variable initialization statement to the sub collection of this cluster.
	@param upSt The C++ code to associate with initialization of the temporary variable.
	@param lbl The name of the temporary variable.
	@param ak The accumulation function key to associate, if any.
	@param num The count to associate with this variable.
	@return Pointer to the newly generated CPPLCSubEntry.
*/
CPPLCSubEntry* CPPLoopCluster::AppendTempInitialization(const STLString & upSt, const STLString & lbl, const STLString & ak, const size_t & num)
{
	CPPLCSubEntry* pNewEntry = AddSubEntry(CPPLCSubEntry::TMPINIT, m_subEntries.end(), upSt, lbl, ak, num);
	pNewEntry->AddAssigned(lbl);
	if (s_entryRegistry.find(pNewEntry->GetLabel())==s_entryRegistry.end())
		s_entryRegistry[pNewEntry->GetLabel()] = pNewEntry;
	return pNewEntry;
}

/** Generate and prepend a new temporary variable declaration to the sub collection of this cluster.
	@param upSt The C++ code to associate with the new declaration.
	@param lbl The name of the temporary variable.
	@param ak The accumulation function key to associate, if any.
	@param num The count to associate with this variable.
	@return Pointer to the newly generated CPPLCSubEntry.
*/
CPPLCSubEntry* CPPLoopCluster::PrependTempDeclaration(const STLString & upSt, const STLString & lbl, const STLString & ak, const size_t & num)
{
	CPPLCSubEntry* pNewEntry = AddSubEntry(CPPLCSubEntry::TMPDCLR, m_subEntries.begin(), upSt, lbl, ak, num);
	pNewEntry->AddAssigned(lbl);
	if (s_entryRegistry.find(pNewEntry->GetLabel()) == s_entryRegistry.end())
		s_entryRegistry[pNewEntry->GetLabel()] = pNewEntry;
	return pNewEntry;
}

/** Generate and prepend a subentry with C++ code where a temporary variable is used as part of overall model logic.
	@param upSt The C++ code to use.
	@param lbl The name of the temporary variable.
	@param ak The accumulation function key to associate, if any.
	@param num The count to associate with this variable.
	@return Pointer to the newly generated CPPLCSubEntry.
*/
CPPLCSubEntry* CPPLoopCluster::PrependTempApplication(const STLString & upSt, const STLString & lbl, const STLString & ak, const size_t & num)
{
	CPPLCSubEntry* pNewEntry = AddSubEntry(CPPLCSubEntry::TMPASGN, m_subEntries.begin(), upSt, lbl, ak, num);
	pNewEntry->AddAssigned(lbl);
	s_entryRegistry[pNewEntry->GetLabel()] = pNewEntry;
	return pNewEntry;
}

/** Generate and prepend a new temporary variable initialization statement to the sub collection of this cluster.
	@param upSt The C++ code to associate with initialization of the temporary variable.
	@param lbl The name of the temporary variable.
	@param ak The accumulation function key to associate, if any.
	@param num The count to associate with this variable.
	@return Pointer to the newly generated CPPLCSubEntry.
*/
CPPLCSubEntry* CPPLoopCluster::PrependTempInitialization(const STLString & upSt, const STLString & lbl, const STLString & ak, const size_t & num)
{
	CPPLCSubEntry* pNewEntry = AddSubEntry(CPPLCSubEntry::TMPINIT, m_subEntries.begin(), upSt, lbl, ak, num);
	pNewEntry->AddAssigned(lbl);
	return pNewEntry;
}

/** Generate and add a new placeholder to the point at which a new temporary variable is used.
	This placeholder is used to generate declaration, initialization and application statements
	for a temporary variable at the proper places in the update code.
	@param index The index at which to insert the new placeholder.
	@param upSt The C++ code to associate with temporary variable application statement to be generated.
	@param lbl The name of the temporary variable to be generated.
	@param ak The accumulation function key to associate, if any.
	@param num The count to associate with this variable.
	@param dimCount The number of dimensions to associate with the generated temporary variable.
	@return Pointer to the newly generated CPPLCSubEntry.
*/
CPPLCSubEntry* CPPLoopCluster::NewTempPlaceholder(const size_t & index, const STLString & upSt, const STLString & lbl, const STLString & ak, const size_t & num, const unsigned short dimCount)
{
	CPPLCSubEntry* pNewEntry = AddSubEntry(CPPLCSubEntry::TMPMARK, m_subEntries.begin() + index, upSt, lbl, ak, num);
	pNewEntry->SetDimCount(dimCount);
	return pNewEntry;
}

/** Generate and append a nonspecifc C++ code  to the sub collection of this cluster.
	@param upSt The C++ code to insert into the final code.
	@param lbl The name of the temporary variable.
	@param assigned Identifier of variable/sub entry represented in custom code.
	@param ak The accumulation function key to associate, if any.
	@param num The count to associate with this variable.
	@return Pointer to the newly generated CPPLCSubEntry.
*/
CPPLCSubEntry* CPPLoopCluster::AppendCustomEntry(const STLString & upSt, const STLString & lbl, const STLString & assigned,const STLString & ak, const size_t & num)
{
	CPPLCSubEntry* pNewEntry = AddSubEntry(CPPLCSubEntry::CUSTOM, m_subEntries.end(), upSt, lbl, ak, num);
	if (!assigned.empty())
		pNewEntry->AddAssigned(assigned);
	s_entryRegistry[pNewEntry->GetLabel()] = pNewEntry;
	return pNewEntry;
}

/** Generate and append a nonspecifc C++ code  to the sub collection of this cluster.
	@param index The index at which to insert the custom code.
	@param upSt The C++ code to insert into the final code.
	@param lbl The name of the temporary variable.
	@param assigned Identifier of variable/sub entry represented in custom code.
	@param ak The accumulation function key to associate, if any.
	@param num The count to associate with this variable.
	@return Pointer to the newly generated CPPLCSubEntry.
*/
CPPLCSubEntry* CPPLoopCluster::NewCustomEntry(const size_t & index, const STLString & upSt, const STLString & lbl, const STLString & assigned, const STLString & ak, const size_t & num)
{
	CPPLCSubEntry* pNewEntry = AddSubEntry(CPPLCSubEntry::CUSTOM, m_subEntries.begin()+index, upSt, lbl, ak, num);
	if (!assigned.empty())
		pNewEntry->AddAssigned(assigned);
	s_entryRegistry[pNewEntry->GetLabel()] = pNewEntry;
	return pNewEntry;
}

/** @return The total number of sub-objects (Evaluables and LoopClusters).*/
size_t CPPLoopCluster::GetObjectCount() const
{
	return m_subEntries.size();
}

/** @return true if the CPPLoopCluster represents a loop object with a fixed number of iterations; false otherwise.*/
bool CPPLoopCluster::FixedIterationCount() const
{
	/// \todo update when working with dynamic model instances and/or dynamic list sizes.

	if (!m_modelList.empty())
	{
		for (Model* pMdl : m_modelList)
		{
			if (pMdl->IsDynamic())
				return false;
		}
	}
	return true;
}

/** @return The number of iterations associated with an CPPLoopCluster object. */
size_t CPPLoopCluster::GetIterations() const
{
	return m_iterations;
}

/** Access a specific sub-LoopCluster.
	@param i The index of the LoopCluster to retrieve.
	@return A pointer to the CPPLoopCluster at index i, or NULL if i references an Evaluable object.
	*/
CPPLoopCluster* CPPLoopCluster::GetSubLoopCluster(const size_t & i)
{
	return m_subEntries[i]->GetSubLoopCluster();
}

/** Access a specific sub-Evaluable.
@param i The index of the Evaluable to retrieve.
@return A pointer to the Evaluable at index i, or NULL if i references a CPPLoopCluster object.
*/
Evaluable* CPPLoopCluster::GetEvaluable(const size_t & i)
{
	return  m_subEntries[i]->GetEvaluable();
}
/** Clear all subEntries from the cluster. */
void CPPLoopCluster::ClearAllEntries()
{

	for (CPPLCSubEntry* pSe : m_subEntries)
	{
		if (pSe->GetParentLoopCluster() == this)
		{

			switch (pSe->GetEntryType())
			{
			case CPPLCSubEntry::EVAL:
				s_entryRegistry.erase(pSe->GetEvaluable()->GetID());
				break;
			case CPPLCSubEntry::TMPASGN:
				s_entryRegistry.erase(pSe->GetLabel());
				break;
			}
			delete pSe;
		}
	}
	m_subEntries.clear();
}

/** @return Pointer to the parent CPPLoopCluster. */
CPPLoopCluster* CPPLoopCluster::GetParentCluster()
{
	return m_pParentCluster;
}

/** Retrieve the enclosing sub-entry
	@return Pointer to the parent sub-entry.
*/
CPPLCSubEntry* CPPLoopCluster::GetParentEntry()
{
	return m_pParentEntry;
}

/** Set the parent CPPLoopCluster.
	@param pClust Pointer to the CppLoopCluster to designate as the parent.
*/
void CPPLoopCluster::SetParentCluster(CPPLoopCluster* pClust)
{
	m_pParentCluster = pClust;
}

/** @return Pointer to the CPPLoopCluster of the cluster tree that contains the queried CPPLoopCluster.*/
CPPLoopCluster* CPPLoopCluster::GetRootCluster()
{
	if (!m_pParentCluster)
		return this;
	return m_pParentCluster->GetRootCluster();
}

/** Add a miscellaneous initialization statement to prepend to the C++ code generated by the CPPLoopCluster.
	@param initStr The string enclosing the C++ source code to inject.
*/
void CPPLoopCluster::AddInitializationStatement(const STLString & initStr)
{
	m_miscInits.push_back(initStr);
}

/** @return True if one or more Model objects are associated with the CPPLoopCluster; false otherwise. */
bool CPPLoopCluster::HasModel()
{
	return !m_modelList.empty();
}

/** @return Pointer to the first associated model object.*/
Model* CPPLoopCluster::GetRepModel()
{
	return !m_modelList.empty() ? m_modelList.front() : NULL;
}

/** @return Pointer to the Model object associated with the root CPPLoopCluster. */
Model* CPPLoopCluster::GetRootModel()
{
	if (!m_pParentCluster)
		return m_modelList.front(); //root should only have one model
	else
		return m_pParentCluster->GetRootModel();
}

/** Reset the temporary variable name counts used to generate unique names.*/
void CPPLoopCluster::ResetTmpCounts()
{
	s_tempVarNameCounts.clear();
	//s_entryRegistry.clear();
	s_dimReferences.clear();
}

/** Generate a unique variable name.
	@param prefix The base string to use for the generated name.
	@param pEntry Optional pointer to CPPLCSubEntry. The unique ID will be assigned as a dependency to pEntry if present.
	@return A unique name that is suitable to use as a variable name in the generated C++ code.
*/
STLString CPPLoopCluster::GenVarName(const STLString & prefix, CPPLCSubEntry* pEntry)
{
	if (s_tempVarNameCounts.find(prefix) == s_tempVarNameCounts.end())
		s_tempVarNameCounts[prefix] = 0;
	STLString ret = "__OME__" + prefix + "_" + std::to_string(s_tempVarNameCounts[prefix]++);

	if (pEntry)
		pEntry->AddDepends(ret);
	return ret;
}

/** Add the text closing a model loop.
	@return Text containing the closing statements.
	*/
STLString CPPLoopCluster::AddModelLoopFooter()
{
	oSTLSStream outStrm;

	for (Model* pMdl : m_modelList)
	{

		if (pMdl->IsDynamic())
		{
			outStrm << '}' << endl;

		}
	}
	/// \todo perform killMarked() elsewhere in sim.
	//m_cache << pMdl->GetID() << "->KillMarked();" << endl;
	if (NeedsLoop())
	{
		outStrm << '}' << endl;
		for (Model* pMdl : m_modelList)
			outStrm << pMdl->GetID() << ".SetCurrentInstance(0);" << endl;
	}
	return outStrm.str();
}

/** Add the text opening a model loop.
@return Text containing the opening statements.
*/
STLString CPPLoopCluster::AddModelLoopHeader()
{
	oSTLSStream outStrm;
	if (NeedsLoop())
	{
		STLRegex incReg("@ii");
		STLString incName =m_label + "_i";
		outStrm << "for(size_t " << incName << "=0;" << incName << "<"<<std::regex_replace(GetIterationLimit(),incReg,incName);
		
		outStrm << ";++" << incName << ')' << endl << '{' << endl;

		for (Model* pMdl : m_modelList)
			outStrm << pMdl->GetID() << ".SetCurrentInstance(" << incName << ");" << endl;
	}

	for (Model* pMdl : m_modelList)
	{
		if (pMdl->IsDynamic())
		{
			outStrm << "if(!" << pMdl->GetID() << ".GetCurrentInstanceDead())" << endl << '{' << endl;
		}
	}

	return outStrm.str();
}

/** @return String containing the loop header for a non-model associated CPPLoopCluster.*/
STLString CPPLoopCluster::AddSubLoopHeader()
{
	if (m_label.empty())
		m_label = GenVarName("tmp",m_pParentEntry);

	oSTLSStream outStrm;
	if (NeedsLoop())
	{
		STLRegex incReg("@ii");
		STLString incName = m_label + "_i";
		//DBG_PRINTC(GetIterationLimit(), DBG_CYAN);
		outStrm << "for(size_t " << incName << "=0;" << incName << "<" << std::regex_replace(GetIterationLimit(), incReg, incName);
		outStrm << ";++" << incName << ')' << endl << '{' << endl;

	}
	return outStrm.str();
}

/** @return String containing the loop footer for a non-model associated CPPLoopCluster.*/
STLString CPPLoopCluster::AddSubLoopFooter()
{
	oSTLSStream outStrm;
	if (NeedsLoop())
	{
		outStrm << "}";
	}
	return outStrm.str();
}

/** Convert a model expression into a series of C++ statements.
	@param expr The expression to evaluate.
	@param pCaller Pointer to the Evaluable object that owns the expression in expr.
	@param isInInit If true, generate statements for initialization; otherwise generate statements for update.
	@param currInd The index of the current subentry being evaluated.
	@param noProcess If true, only return the base update statement; otherwise add additional code to further package.
	@param outRType Optional point to a FS_ARGS variable. If not NULL, the source variable is set to the return type of the generated update statement.
	@return The C++ statement(s) representing the expression
	*/
STLString CPPLoopCluster::CPPizeExpression(const STLString & expr, Evaluable* pCaller, const bool & isInInit, size_t & currInd, const bool & noProcess, FuncSig::FS_ARGS* outRType)
{

	//expr may not be needed
	oSTLSStream outBuff;

	TokenSet assignSet;
	FuncSig::FS_ARGS  retType = FuncSig::NO_ARGS;
	unsigned short depth = 0;


	STLString newExpr = CPPizeExpression(expr, pCaller, retType, depth, isInInit, currInd, pCaller->IsOMEType(OME_VARARRAY) ? &assignSet : NULL);
	//capture return type if requested
	if (outRType)
		*outRType = retType;
	if (noProcess)
		return newExpr;

	if (!newExpr.empty())
	{
		if (pCaller->IsOMEType(OME_SPAWNER))
			outBuff << pCaller->GetID() << ".EvalWithVal(" << newExpr << ");" << endl;
		else
		{
			//if retType is no args, then any assignment clause was applied elsewhere.
			if (retType != FuncSig::NO_ARGS)
				ProcessEvaluableAssignment(outBuff, pCaller, newExpr, isInInit, retType);
			if (pCaller->IsOMEType(OME_CONVEYOR))
			{
				newExpr = CPPizeExpression(((Conveyor*)pCaller)->GetTransitExpr(), pCaller, retType, depth, isInInit, currInd, pCaller->IsOMEType(OME_VARARRAY) ? &assignSet : NULL);
				outBuff << pCaller->GetID() << ".SetTransitTime(" << newExpr << ");" << endl;
			}


		}
	}

	
	return outBuff.str();

}

/** Convert a model expression into a series of C++ statements.
	@param expr The expression to evaluate.
	@param pCaller Pointer to the Evaluable object that owns the expression in expr.
	@param retType Contains the return type flag on return.
	@param listDepth Contains the scoping list depth on return.
	@param isInInit If true, generate statements for initialization; otherwise generate statements for update.
	@param currInd The index of the current subentry being evaluated.
	@param retSet Optional pointer to a Token set. If not NULL generated tokens are appended to the source token set.
	@return The C++ statement(s) representing the expression
	*/
STLString CPPLoopCluster::CPPizeExpression(const STLString & expr, Evaluable* pCaller, FuncSig::FS_ARGS & retType, unsigned short & listDepth, const bool & isInInit, size_t & currInd, TokenSet* retSet)
{
	size_t currStart = 0;
	size_t currEnd = expr.size();
	size_t lastExprLen = 0;
	size_t currExprLen;
	STLRegex openReg("[{\\[(]");
	STLsmatch openMatch;
	oSTLSStream outBuff;
	STLString tmpStr;
	STLString suffixMod;
	bool applySuffix = false;
	size_t tmpPos, tmpLen;
	OMEObject* pObj;
	Model* pRtMdl = GetRootModel();
	TokenSet tokens;
	std::map<STLString, FuncSig>::iterator fItr;

	//NOTE: ordering of tokenization here is important! ifs should go first, then everyone else
	TokenizeIfBlocks(expr, tokens, pCaller, isInInit, retSet == NULL, currInd);
	TokenizeFuncBlocks(expr, tokens, pCaller, isInInit, currInd);
	TokenizeSubBlocks(expr, tokens, pCaller, isInInit, currInd);
	TokenizeListBlocks(expr, tokens, pCaller, isInInit, currInd);
	TokenizeValues(expr, tokens, pCaller);
	TokenizeBoolOperators(expr, tokens);
	TokenizeMathOperators(expr, tokens);

	//with tokens assembled, we can now parse this statement;
	auto currTItr = tokens.begin();
	//StrList indexNames;
	size_t i = 0;

	while (i < expr.size())
	{
		applySuffix = !suffixMod.empty();
		lastExprLen = outBuff.str().length();
		currExprLen = 0;

		if (currTItr != tokens.end() && i == (*currTItr).index)
		{
			//parse token
			switch ((*currTItr).tType)
			{
			case NOTOKEN:
				throw OMESupportException("CPPLoopCluster::CPPizeExpression: Undefined Token.");
				break;
			case DISCARD:
				//skip section of expr
				break;
			case VAR:
				currExprLen = (*currTItr).subExpr.length();
				outBuff << (*currTItr).subExpr;
				/*if ((*currTItr).aType == FuncSig::LIST_ARG && !indexNames.empty())
				{
				tmpStr = "[" + indexNames.front() + "]";
				currExprLen += tmpStr.length();
				outBuff << tmpStr;
				}*/
				break;
			case CONSTVAL:
				tmpStr = expr.substr((*currTItr).index, (*currTItr).length);
				currExprLen = tmpStr.length();

				outBuff << tmpStr;
				if (tmpStr.find('.') != STLString::npos)
					outBuff << OME_SCLR_TAG;
				break;
			case ENUMVAL:
				tmpStr = expr.substr((*currTItr).index, (*currTItr).length);
				tmpPos = tmpStr.find(':');
				//eee.pMdl = (Model*)pRtMdl->ObjectForKey(tmpStr.substr(0, tmpPos));
				//enumSplit = SplitString(tempStr.substr(tmpPos + 1), '.');
				//eee.pEEnum = eee.pMdl->ModelEnumForName(enumSplit[0]);
				//outBuff << "en_" << eee.pMdl->GetID() << '_' << ScrubName(enumSplit[0]);
				//if (enumSplit.size()>1)
				//{
				//	eee.pEEntry = eee.pEEnum->AddrForLabel(enumSplit[1]);
				//	outBuff << '_' << eee.pEEntry->ind;
				//}
				//else
				//{
				//	eee.pEEntry = NULL;
				//}


				break;
			case IFBLOCK:
				currExprLen = (*currTItr).subExpr.length();
				outBuff << '(' << (*currTItr).subExpr << " ? ";
				break;
			case THENBLOCK:
				currExprLen = (*currTItr).subExpr.length();
				outBuff << (*currTItr).subExpr << " : ";
				break;
			case ELSEBLOCK:
				currExprLen = (*currTItr).subExpr.length();
				outBuff << (*currTItr).subExpr << ')';
				break;
			case POWOPOPEN:
				currExprLen = 15;
				outBuff << "OMECFuncs::pow(";
				break;

			case ARGSEP:
				currExprLen = 1;
				outBuff << ',';
				break;
			case MATHOP:
				//for now, we will just copy operator.
				currExprLen = 1;
				tmpStr = expr.substr((*currTItr).index, (*currTItr).length);
				outBuff << tmpStr;
				break;
			case SUBBLOCK:
				currExprLen = (*currTItr).subExpr.length() + 2;
				outBuff << '(' << (*currTItr).subExpr << ')';

				break;
			case FUNCBLOCK:
				if (m_accums.find((*currTItr).subExpr) == m_accums.end())
				{
					currExprLen = (*currTItr).subExpr.length();
					outBuff << (*currTItr).subExpr;
				}
				else
				{
					//add placeholder marker for where accum function will be applied
					NewTempPlaceholder(currInd++, m_accums[(*currTItr).subExpr].GetInvocation(), m_accums[(*currTItr).subExpr].GetVarName(), m_accums[(*currTItr).subExpr].GetFuncName(), 
						m_accums[(*currTItr).subExpr].GetSuffix().empty() ? 1 : 0, CountChar(m_accums[(*currTItr).subExpr].GetSuffix(),'#'));
					//add reference here
					outBuff << m_accums[(*currTItr).subExpr].GetVarNameWithSuffix();

				}
				break;
			case LISTBLOCK:
				if ((*currTItr).subExpr.find(',') == STLString::npos)
				{
					STLRegex idReg("^[[[:s:]]*(.*)((?:#[!0-9*])*)[[:s:]\\]]*$");
					STLsmatch match;
					std::regex_match((*currTItr).subExpr, match, idReg);

					//Evaluable* pEval = dynamic_cast<Evaluable*>(pCaller->GetParentModel()->GetRootModel()->ObjectForKey(match[1].str()));
						//#0 means to attempt to recheck size later 
						outBuff << match[1].str() << match[2].str() << "#0";//<<pEval->IsOMEType(OME_VARARRAY) ? ((VarArray*)pEval)->GetDims().CalcRepLength() : 0;
				}
				else
				{


					tmpStr = GenVarName("tempArray",m_pParentEntry);
					outBuff << tmpStr << "#" << std::to_string((*currTItr).listDepth);

					m_tempArrayDclrs[pCaller->GetID()].push_back(tmpStr + ":OME_SCALAR " + tmpStr + "[]={" + (*currTItr).subExpr + "};\nsize_t " + tmpStr + "_dims[]={" + std::to_string(CountArrayArgs((*currTItr).subExpr)) + "};");
				
				}
				break;
			case BOOLOP:
				tmpStr = expr.substr((*currTItr).index, (*currTItr).length);

				currExprLen = 2;
				if (tmpStr == "and")
					outBuff << "&&";
				else if (tmpStr == "or")
					outBuff << "||";
				else
					outBuff << tmpStr;
				break;
			case SUFFIXMOD:
				suffixMod = (*currTItr).subExpr;
				break;
			}

			if (applySuffix)
			{
				outBuff << suffixMod;
				currExprLen += suffixMod.length();
				suffixMod = STLString();
			}
			if (retSet)
			{
				//record this chunk of new expr for future evaluation.
				retSet->emplace((*currTItr).tType, lastExprLen, currExprLen, (*currTItr).listDepth, (*currTItr).aType);
			}

			//resolve return types and list depth
			retType = ResolveFSArg(retType, (*currTItr).aType);
			listDepth = std::max(listDepth, (*currTItr).listDepth);

			//find next valid token
			i += (*currTItr).length;
			//for zero-length tokens
			++currTItr;
			for (; currTItr != tokens.end() && (*currTItr).index < i; ++currTItr);

		}
		else
		{
			//add current char, omitting white space
			if (!std::isspace(expr[i]))
				outBuff << expr[i];
			++i;
		}
	}

	return  outBuff.str();
}

/** Find the base components for a function's argument list.
	@param funcArgs The string containing the function arguments as passed to the function.
	@param foundArgs A list of individual arguments found when parsing funcArgs. Populated on return.
	@param retTypes A list of return types that correspond to entries in foundArgs. Populated on return.
	@param pCaller Pointer to the Evaluable object containing the call to the function being processed.
	@param listDepth The present depth in the evaluation of a list value. Populated on return.
	@param isInInit If true, initialization statements are generated; otherwise, update statements are generated.
	@param currInd The current insertion point for any generated statements. May be modified on return.
*/
void CPPLoopCluster::ProcessFuncArgs(const STLString & funcArgs, StrList & foundArgs, FSArgList & retTypes, Evaluable* pCaller, unsigned short & listDepth, const bool & isInInit, size_t & currInd)
{
	std::pair<size_t, size_t> currArg(0, 0);
	FuncSig::FS_ARGS subType = FuncSig::NO_ARGS;
	//locate each argument passed in to the function.
	for (size_t i = 0; i < funcArgs.size(); i++)
	{
		subType = FuncSig::NO_ARGS;
		switch (funcArgs[i])
		{
		case ',':
			TrimRange(funcArgs, currArg.first, currArg.second);
			foundArgs.push_back(CPPizeExpression(funcArgs.substr(currArg.first, currArg.second), pCaller, subType, listDepth, isInInit, currInd));
			retTypes.push_back(subType);

			currArg = std::pair<size_t, size_t>(i + 1, 0);
			break;
		case '(':
			i = FindMatching(funcArgs, i, '(', ')');
			currArg.second = 1 + i - currArg.first;
			break;
		case '[':
			i = FindMatching(funcArgs, i, '[', ']');
			currArg.second = 1 + i - currArg.first;
			break;
		default:
			currArg.second++;
		}
	}

	//grab last argument if needed
	if (currArg.second)
	{
		if (!FullTrimString(funcArgs.substr(currArg.first, ++currArg.second)).empty())
		{
			subType = FuncSig::NO_ARGS;
			foundArgs.push_back(CPPizeExpression(funcArgs.substr(currArg.first, ++currArg.second), pCaller, subType, listDepth, isInInit, currInd));
			retTypes.push_back(subType);
		}
	}
}

/** Parse individual function args from a function argument string.
	@param funcArgs The string containing the function arguments as passed to the function.
	@param foundArgs A list of individual arguments found when parsing funcArgs. Populated on return.
*/
void CPPLoopCluster::FindFuncArgs(const STLString & funcArgs, StrList & foundArgs)
{
	std::pair<size_t, size_t> currArg(0, 0);
	FuncSig::FS_ARGS subType = FuncSig::NO_ARGS;
	//locate each argument passed in to the function.
	for (size_t i = 0; i < funcArgs.size(); i++)
	{
		subType = FuncSig::NO_ARGS;
		switch (funcArgs[i])
		{
		case ',':
			TrimRange(funcArgs, currArg.first, currArg.second);
			foundArgs.push_back(funcArgs.substr(currArg.first, currArg.second));

			currArg = std::pair<size_t, size_t>(i + 1, 0);
			break;
		case '(':
			i = FindMatching(funcArgs, i, '(', ')');
			currArg.second = 1 + i - currArg.first;
			break;
		case '[':
			i = FindMatching(funcArgs, i, '[', ']');
			currArg.second = 1 + i - currArg.first;
			break;
		default:
			currArg.second++;
		}
	}

	//grab last argument if needed
	if (currArg.second)
	{
		if (!FullTrimString(funcArgs.substr(currArg.first, ++currArg.second)).empty())
		{
			subType = FuncSig::NO_ARGS;
			foundArgs.push_back(funcArgs.substr(currArg.first, ++currArg.second));
		}
	}
}
/** Process found function call.
@param outBuff Buffer to write statements to.
@param listDepth Contains the scoping list depth on return.
@param call the full function call, including the argument list.
@param sig Function signature struct that contains details about the function's invocation.
@param pCaller Pointer to the object who is invoking the function.
@param isInInit Flag indicating if statement is in initialization function.
@param currInd The entry index for the CPPLCSubEntry being processed.
@return A flag indicating the return type of the function.
*/
FuncSig::FS_ARGS CPPLoopCluster::ProcessFunc(oSTLSStream & outBuff, unsigned short & listDepth, const STLString & call, const FuncSig & sig, Evaluable* pCaller, const bool & isInInit, size_t & currInd)
{
	oSTLSStream argBuff;
	size_t split = call.find('(');
	FuncSig::FSA_TYPE inTypes = FuncSig::NO_ARGS;
	std::list<FuncSig::FS_ARGS> retTypes;
	unsigned short tmpDepth;
	STLString funcName(call.substr(0, split));


	STLString hdr = ProcessFuncHdr(funcName, sig, pCaller, isInInit);
	STLString argPats = sig.arg_pattern;

	//for Initialization, configure look up for functions that reference previous values
	if (isInInit && sig.flags & FuncSig::PAST_LOOKUP)
	{
		hdr = "lookupInit_" + hdr;
		if (argPats.find("@BM") == STLString::npos)
			argPats = "@BM," + argPats;
	}

	STLString funcArgs = call.substr(split + 1, call.size() - (split + 2));

	//count args
	std::list<size_t> patArgInds;
	for (size_t i = 0; i < argPats.size(); i++)
	{
		if (argPats[i] == '@')
			patArgInds.push_back(i + 1);
	}

	STLString className = m_pSerializer->m_className;
	StrList foundArgs;

	ProcessFuncArgs(funcArgs, foundArgs, retTypes, pCaller, listDepth, isInInit, currInd);

	for (FuncSig::FS_ARGS & rt : retTypes)
		inTypes |= rt;

	if (sig.flags & FuncSig::LOCAL_ACCUM)
	{
		//deal with local accumulation functions which require loops (sum,product,any,all,...)
		FuncSig::FS_ARGS retType = sig.ArgFunc(inTypes, listDepth);
		auto rItr = retTypes.begin();
		auto fItr = foundArgs.begin();
		//the list that the argument is applied to should always be the first argument of a LOCAL_ACCUM function;
		
		//longest contiguous chain of sizes denotes dims of tmpvar
		STLRegex suffReg("(?:#[!0-9]+)+");

	
		STLString expr = (*fItr);
		//blank out subexpressions to prevent false suffix matches.
		int paramStack = 0;
		for (size_t i = 0; i<expr.length(); ++i)
		{
			switch (expr[i])
			{
			case '(':
				paramStack++;
				break;
			case ')':
				paramStack--;
				break;
			default:
				if (paramStack != 0)
					expr[i] = '_';
			}
		}




		STLString arrSuff;
		unsigned short poundCount = 0;
		STLsmatch mtch;
		while (std::regex_search(expr, mtch, suffReg))
		{
			unsigned int currCount = CountChar(mtch[0].str(), '#');
			if (currCount > poundCount)
			{
				arrSuff = mtch[0].str();
				poundCount = currCount;
			}
			expr = mtch.suffix();
		}

		STLString key = ConcatStrings(foundArgs, ',');

		if (m_accums.find(key) == m_accums.end())
		{

			STLString varName = GenVarName(funcName, m_pParentEntry);
			m_accums[key] = CPPLCAccumEntry(varName, funcName, key, pCaller->GetID(), arrSuff.substr(0, arrSuff.rfind('#')));

			//more here?
		}
		else //identical call appears elsewhere; make sure that its use is re-applied.
			m_accums[key].AddAppliesToList(pCaller->GetID());
		outBuff << key;//m_accums[key].GetVarName()<<arrSuff;
		--listDepth;
		return retType;
	}

	//add hdr here so as not to interfere with agg substitutions
	outBuff << hdr;

	//based on list depth, alter any functions that reduce a list to a scalar if it only has one dimension
	if ((sig.flags & FuncSig::SHALLOW_DIFF))
	{
		//for now, assume that indices are not needed
	  foundArgs.front() = std::regex_replace(foundArgs.front(), STLRegex("#[!0-9]+"), STLString());
		if (listDepth)
			--listDepth;

		if (!listDepth)
			outBuff << "Sclr";

	}

	//first check for special variadic arg
	if (!patArgInds.empty())
	{
		//actually process arguments

		if (patArgInds.size() == 2 && (argPats[patArgInds.back()] == '#' || argPats.substr(patArgInds.back(), 2) == "VI"))
		{
			//build initializer list to handle variadic arg.
			argBuff << "({";
			if (argPats[patArgInds.front()] != '#')
			{
				if (foundArgs.size() > 1)
				{
					//add values to argument list
					auto rItr = retTypes.begin();
					for (auto itr = foundArgs.begin(); itr != foundArgs.end(); ++itr, ++rItr)
					{
						if (itr != foundArgs.begin())
							argBuff << ',';
						argBuff << *itr;
					}
				}
				else
					argBuff << "," << foundArgs.front();
			}
			argBuff << '}';
		}
		else
		{
			if (sig.flags & FuncSig::MULTI_ARG)
			{

				argBuff << '(';

				auto itr = patArgInds.begin();

				if (TestArgCode2Char(argPats, (*itr), "CI"))
				{
					//add caller
					argBuff << pCaller->GetID();

					++itr;
					if (itr != patArgInds.end())
						argBuff << ",";
				}
				else if (TestArgCode2Char(argPats, (*itr), "BM"))
				{
					//add Simulation manager
					argBuff << className << "Bm";
					++itr;
					if (itr != patArgInds.end())
						argBuff << ",";
				}
				else if (TestArgCode2Char(argPats, (*itr), "EC"))
				{
					//pass in as table
					argBuff << "getAsTable(" << pCaller->GetID() << ',';
					++itr;
				}
				else if (TestArgCode2Char(argPats, (*itr), "SD"))
				{
					//Add SDP
					argBuff << className << "SDP";
					if (patArgInds.size() > 1)
						argBuff << ",";
					++itr;
				}
				auto reset = itr;


				for (auto fItr = foundArgs.begin(); fItr != foundArgs.end(); ++fItr)
				{
					if (fItr != foundArgs.begin())
						argBuff << ',';

					argBuff << (*fItr);
					retTypes.pop_front();
					++itr;
					if (itr == patArgInds.end())
						itr = reset;
				}

			}
			else if (foundArgs.size() <= patArgInds.size())
			{
				argBuff << '(';
				auto fItr = foundArgs.begin();
				for (auto itr = patArgInds.begin(); itr != patArgInds.end(); ++itr)
				{

					if (TestArgCode2Char(argPats, (*itr), "CI"))
					{
						//add Caller
						if (itr != patArgInds.begin())
							argBuff << ',';
						argBuff << pCaller->GetID();
					}
					else if (TestArgCode2Char(argPats, (*itr), "BM"))
					{
						//Add simulation manager
						if (itr != patArgInds.begin())
							argBuff << ',';
						argBuff << className << "Bm";
					}
					else if (TestArgCode2Char(argPats, (*itr), "SD"))
					{
						//Add SDP
						if (itr != patArgInds.begin())
							argBuff << ',';
						argBuff << className << "SDP";
					}
					else if (TestArgCode2Char(argPats, (*itr), "FI"))
					{
						//Add anonymous function
						if (itr != patArgInds.begin())
							argBuff << ',';
						//create inline function
						FuncSig subSig;

						size_t f, l;
						for (f = (*itr) + 2, l = 0; f < argPats.length() && argPats[f] != ','; f++, l++);
						STLString funcName = argPats.substr((*itr) + 2, l);

						if (s_funcSigs.find(funcName) != s_funcSigs.end())
							subSig = s_funcSigs[funcName];

						//Add any predefined values to function invocation
						STLString expr = *fItr;
						STLString invoke = subSig.name;
						STLString match1 = (className + "Bm");
						STLString match2 = (className + "SDP");
						STLString match3 = (className + "Time");
						const OMEChar* patterns[] = { "@BM", match1.c_str(),
							"@SD", match2.c_str(),
							"@TI", match3.c_str(),
							"@EE", expr.c_str(),
							"\0" };

						argBuff << BatchRegexReplace(invoke, patterns) << '{';

						//optionally use arg pattern for body template
						if (!subSig.arg_pattern.empty())
							argBuff << BatchRegexReplace(subSig.arg_pattern, patterns);
						else
							argBuff << expr;

						argBuff << "}";
						++fItr;
					}
					else
					{
						if (fItr != foundArgs.end())
						{
							if (itr != patArgInds.begin())
								argBuff << ',';
							//if (TestArgCode2Char(argPats, (*itr), "EI") && (retTypes.front() == FuncSig::EVAL_ARG))
							//	*fItr = std::regex_replace((*fItr), valReg, "");

							retTypes.pop_front();
							argBuff << '(' << (*fItr) << ')';
							++fItr;
						}
						else if (argPats[(*itr) + 1] != '?')
							throw OMEFuncOpException("ProcessFunc","No more arguments");
					}
				}
			}
			else
			{
				//DBG_PRINTC("Error processing function string: '" + call + "'", DBG_RED);
				throw OMEFuncOpException("ProcessFunc", ("Error processing function string: '" + call + "'").c_str());
			}
		}
	}
	else //assume block starts as sub
		argBuff << '(';

	outBuff << argBuff.str() << ')';
	return sig.ArgFunc(inTypes, listDepth);
}

/** Convert a function name itself into the appropriate form for C++ representation.
@param invoke the function name used in the invocation, excluding the argument list.
@param sig Function signature struct that contains details about the function's invocation.
@param pCaller Pointer to the object who is invoking the function.
@param isInInit Flag indicating if statement is in initialization function.
@return A version of the function name that can be compiled as part of a C++ representation.
*/
STLString CPPLoopCluster::ProcessFuncHdr(const STLString & invoke, const FuncSig & sig, Evaluable* pCaller, const bool & isInInit)
{
	oSTLSStream ret;
	size_t ind = sig.name.find('@');

	STLString className = m_pSerializer->m_className;

	if (ind != STLString::npos)
	{
		STLString sub = sig.name.substr(ind + 1, 2);
		if (sub == "BM")
		{
			ret << sig.name.substr(0, ind);
			ret << className << "Bm";
			ret << sig.name.substr(ind + 3);
		}
		else if (sub == "SD")
		{
			ret << sig.name.substr(0, ind);
			ret << className << "SDP";
			ret << sig.name.substr(ind + 3);
		}
		else if (sub == "TI")
		{
			ret << sig.name.substr(0, ind);
			ret << className << "Time";
			ret << sig.name.substr(ind + 3);
		}
		else
		{
			throw OMEFuncOpException("ProcessFuncHdr", ("Unrecognized pattern: " + sub).c_str());
		}
	}
	else
		ret << sig.name;

	return ret.str();
}

/** Derive a C++ statement for a specific Evaluable object.
@param pEval Pointer to an Evaluable object to be evaluated.
@param init Flag indicating if Evaluable object is being evaluated for an initialization statement.
@param currInd The entry index for the CPPLCSubEntry being processed.
*/
STLString CPPLoopCluster::ProcessEval(Evaluable* pEval, const bool & init, size_t & currInd)
{
	DBG_PRINTC("Processing " + pEval->GetName(), DBG_MAGENTA);

	using namespace std;
	bool initOnly = false;
	STLString expr;
	FuncSig::FS_ARGS retType = FuncSig::NO_ARGS;
	STLString id = pEval->GetID();
	bool dynMod;
	oSTLSStream outBuff, varBuff;
	STLString className = m_pSerializer->m_className;
	size_t startPos, endPos;

	bool transExpr=false;

	switch (pEval->GetOMEType())
	{
	case OME_CONVEYOR:
	case OME_STATEVAR:
		dynMod = !init && pEval->GetParentModel()->IsDynamic();
		if (init || dynMod)
		{
			if (dynMod)
				outBuff << "if(!" << id << ".IsInitialized())" << endl << '{' << endl;
			outBuff << id << ".Reset(" << className << "Bm);" << endl;
			outBuff << CPPizeExpression(FullTrimString(pEval->GetInitExpr()), pEval, init, currInd);

			outBuff << id << ".BatchInitialize(" << className << "Time," << className << "Bm);" << endl;
			if (dynMod)
				outBuff << '}' << endl;
		}

		break;
	case OME_VARIABLE:
		initOnly = initOnly || (((Variable*)pEval)->GetInitOnly() && !pEval->GetParentModel()->IsDynamic() && !pEval->GetParentModel()->IsConditional());
	case OME_FLOW:
		

		//if value is constant, don't do update
		if (!init && !initOnly)
		{
			if (!pEval->GetIsConstant() || pEval->GetParentModel()->IsDynamic() || pEval->GetParentModel()->IsConditional())
			{
				outBuff << CPPizeExpression(GetExpr(pEval,init), pEval, init, currInd);
			}
		}
		else if (init)
		{
			
			outBuff << id << ".Reset(" << className << "Bm);" << endl;
			outBuff << CPPizeExpression(pEval->GetInitExpr(), pEval, init, currInd);
			outBuff << id << ".BatchInitialize(" << className << "Time," << className << "Bm);" << endl;
		}

		break;
	case OME_VARARRAY:

			expr = GetExpr(pEval,init);

		expr = CPPizeExpression(expr, pEval, init, currInd);

		startPos = expr.find("=({");
		if (startPos != STLString::npos)
		{
			endPos = expr.rfind("})") + 2;
			STLString initName = pEval->GetID() + "_initArray";

			std::pair<STLString, STLString> initExpr = GenTempArray(expr.substr(startPos, endPos - startPos), initName, initOnly);

			if (init || !initOnly)
			{
				outBuff << "const OME_SCALAR " << initExpr.second << "[]={" << initExpr.first << "};" << endl;
				expr =expr.substr(0, startPos+1) + initName + "[" + pEval->GetID() + "_i]" + expr.substr(endPos);
			}
			else
			{
				startPos = expr.rfind('\n', startPos);
				endPos = expr.find('\n', endPos);
				expr = expr.substr(0, startPos) + expr.substr(endPos);
			}

		}


		if (!expr.empty())
		{
			outBuff << expr;
		}
		if (init)
			outBuff << id << ".BatchInitialize(" << className << "Time," << className << "Bm);" << endl;
		break;
	case OME_TSVAR:
		if (init)
			outBuff << id << ".BatchInitialize(" << className << "Time," << className << "Bm);" << endl;
		outBuff << id << ".Evaluate(" << className << "Time," << className << "Bm);" << endl;
		break;
	case OME_SUBPORT:
		if (((SubmodelPort*)pEval)->LinkedToFlow())
			outBuff << id << ".Evaluate(" << className << "Time," << className << "Bm);" << endl;
		break;
	case OME_SPAWNER:
		//spawner updates will need to be handled in a different set of functions
		initOnly = ((Spawner*)pEval)->RunsOnce();

		//expr = CPPizeExpression(pEval->GetExpr(), pEval, init);
		if (init)
			outBuff << id << ".BatchInitialize(" << className << "Time," << className << "Bm);" << endl;
		if (init || !initOnly)
			outBuff << CPPizeExpression(GetExpr(pEval,init), pEval, init, currInd);


	case OME_INFLUENCE:
	case OME_MODEL:
	case OME_ITERCOND:
	case OME_SUBASSOC:
	default:
		break;
	}

	return outBuff.str();
}

/** Retrieve a string for a given type.
	@param aType The argument type to translate to string.
	@return A string representation of the underlying type of aType.
	*/
STLString CPPLoopCluster::StrForType(const FuncSig::FS_ARGS & aType)
{
	STLString ret;
	switch (aType)
	{
	case FuncSig::EVAL_ARG:
	case FuncSig::ALIAS_ARG:
	case FuncSig::SCLR_ARG:
		ret = "OME_SCALAR";
		break;
	case FuncSig::ENUM_ARG:
	case FuncSig::INT_ARG:
		ret = "int";
		break;
	/*case FuncSig::LIST_ARG:
		if (listLength.empty())
			listLength = "LSIZE_BEST_GUESS";
		ret = "DynamicTempArray<" + listLength + ">";
		break;*/
	}
	return ret;
}

/** Generate and return a declaration string for a given argument type.
	@param aType The type of function arg.
	@param varName The name of the variable to use in the declaration.
	@param listLength optional argument of the predefined list length. Applies only to list types.
	@return String containing required declaration statements for variable with the name in varName.
*/
STLString CPPLoopCluster::DclrStrForType(const FuncSig::FS_ARGS & aType, const STLString & varName, const STLString & listLength)
{
	if (aType == FuncSig::LIST_ARG)
	{
		try
		{
			int len = OME_SCLR_READSTR(listLength);
			return "const size_t " + varName + "_count=" + std::to_string(len) + ";\n"
				"OME_SCALAR " + varName + "[" + varName + "_count]; ";
		}
		catch (...)
		{
			//if not integer, continue on and process normally
		}
	}

	return StrForType(aType) + ' ' + varName + ';';

}

/** Generate tokens for If-then-else blocks in a given expression.
	@param expr The expression to evaluate.
	@param tokens The set of tokens to accept any new tokens.
	@param pCaller Pointer to Evaluable that contains expr.
	@param isInInit If true, evaluate for initialization; otherwise, evaluate for update.
	@param resolveTypes If true, attempt to resolve conflicting return types from the two logic branches.
	@param currInd The entry index for the CPPLCSubEntry being processed.
	*/
void CPPLoopCluster::TokenizeIfBlocks(STLString expr, TokenSet & tokens, Evaluable* pCaller, const bool & isInInit, const bool & resolveTypes, size_t & currInd)
{
	//arg type depends on subblocks
	const STLRegex ifReg("(?:^|[\\s(])(if)[\\s(]");


	size_t ifPos = STLString::npos;
	size_t ifLen = STLString::npos;
	size_t thenPos = STLString::npos;
	size_t thenLen = STLString::npos;
	size_t elsePos = STLString::npos;
	size_t elseLen = STLString::npos;

	size_t offset = 0;
	size_t subStart = 0;
	STLsmatch matches;
	STLString ifStr;
	STLString thenStr;
	STLString elseStr;
	unsigned short ifLd = 0;
	unsigned short thenLd = 0;
	unsigned short elseLd = 0;
	FuncSig::FS_ARGS ifSubType,thenSubType = FuncSig::NO_ARGS,elseSubType=FuncSig::NO_ARGS;
	FuncSig::FS_ARGS retType = FuncSig::NO_ARGS;

	//only grab if blocks on current depth
	//walk all portions of an if block so we don't accidentally step below
	while (std::regex_search(expr, matches, ifReg))
	{

		//find each block of if...then...else...end, discarding keywords
		tokens.emplace(DISCARD, matches.position(1), matches.length(1), 0);
		ifPos = matches.position(1) + matches.length(1);

		ifLen = FindMatching(expr, ifPos, "if", "then");

		TrimRange(expr, ifPos, ifLen);
		thenPos = ifPos + ifLen;
		tokens.emplace(DISCARD, thenPos, 4, 0);
		thenPos += 4;

		thenLen = FindMatching(expr, thenPos, "then", "else(?:if)?");
		TrimRange(expr, thenPos, thenLen);
		elsePos = thenPos + thenLen;
		tokens.emplace(DISCARD, elsePos, 4, 0);
		elsePos += 4;


		//add tokens
		ifStr = CPPizeExpression(expr.substr(ifPos, ifLen), pCaller, ifSubType, ifLd, isInInit, currInd);
		thenStr = CPPizeExpression(expr.substr(thenPos, thenLen), pCaller, thenSubType, thenLd, isInInit, currInd);

		elseLen = 0;

		//each elseif corresponds to single end in chain; multiple ends will only occur
		//with multiple elses
		if (expr.substr(elsePos, 2) != "if")
		{
			elseLen = FindMatching(expr, elsePos, "else", "end");
			TrimRange(expr, elsePos, elseLen);
			tokens.emplace(DISCARD, elsePos + elseLen, 3, 0);
			elseStr = expr.substr(elsePos, elseLen);
			elseLen += 3;
		}
		else
		{
			elseLen = expr.find("end", elsePos) - elsePos;
			TrimRange(expr, elsePos, elseLen);
			elseLen += 3;
			elseStr = expr.substr(elsePos, elseLen);
		}

		elseStr = CPPizeExpression(elseStr, pCaller, elseSubType, elseLd, isInInit, currInd);

		if (resolveTypes)
			retType = ResolveFSArg(thenSubType, elseSubType);

		//ensure scalars are scalars
		if (thenSubType != FuncSig::LIST_ARG)
			thenStr = "(OME_SCALAR)" + thenStr;
		if (elseSubType != FuncSig::LIST_ARG)
			elseStr = "(OME_SCALAR)" + elseStr;


		//types will be resolved later
		tokens.emplace(IFBLOCK, ifPos, ifLen, ifLd, retType, ifStr);
		tokens.emplace(THENBLOCK, thenPos, thenLen, thenLd, thenSubType, thenStr);
		tokens.emplace(ELSEBLOCK, elsePos, elseLen, elseLd, elseSubType, elseStr);

		expr = expr.substr(elsePos + elseLen);

	}
}

/** Generate tokens for sub-expression blocks in a given expression.
@param expr The expression to evaluate.
@param tokens The set of tokens to accept any new tokens.
@param pCaller Pointer to Evaluable that contains expr.
@param isInInit If true, evaluate for initialization; otherwise, evaluate for update.
@param currInd The entry index for the CPPLCSubEntry being processed.
*/
void CPPLoopCluster::TokenizeSubBlocks(const STLString & expr, TokenSet & tokens, Evaluable* pCaller, const bool & isInInit, size_t & currInd)
{
	//arg type depends on subblocks
	//dereference of tItr is ExprToken struct
	auto tItr = tokens.begin();

	size_t i = 0;
	size_t endPos;
	STLString tmpStr;
	size_t frontSkip = 0;
	size_t backSkip = expr.length() - 1;

	while (i < expr.length())
	{
		OMEChar closeChar = '\0', openChar = '\0';
		if (tItr != tokens.end() && i == (*tItr).index)
		{
			//skip to next position using length
			//for now, assume all tokens signify skipping
			i += (*tItr).length;
			++tItr;
		}
		else
		{
			openChar = expr[i];
			switch (openChar)
			{
			case '{':
				closeChar = '}';
				break;
			case '(':
				closeChar = ')';
				break;
			default:
				openChar = '\0';
			}

			if (openChar != '\0')
			{
				FuncSig::FS_ARGS retType = FuncSig::NO_ARGS;
				unsigned short ld = 0;
				endPos = FindMatching(expr, i, openChar, closeChar);
				if (i != frontSkip || endPos != backSkip)
				{
					tmpStr = CPPizeExpression(expr.substr(i + 1, endPos - i - 1), pCaller, retType, ld, isInInit, currInd);
					tokens.emplace(SUBBLOCK, i, (endPos - i) + 1, ld, retType, tmpStr);
				}
				else
				{
					endPos = 0;
					tokens.emplace(DISCARD, frontSkip, 1, ld);
					tokens.emplace(DISCARD, backSkip, 1, ld);
					for (; frontSkip < expr.length() && std::isspace(expr[frontSkip]); ++frontSkip);
					for (; backSkip + 1 > 0 && std::isspace(expr[backSkip]); --backSkip);

				}
				i = endPos + 1;
			}
			else
				++i;
		}

	}
}

/** Generate tokens for functiona calls in a given expression.
@param expr The expression to evaluate.
@param tokens The set of tokens to accept any new tokens.
@param pCaller Pointer to Evaluable that contains expr.
@param isInInit If true, evaluate for initialization; otherwise, evaluate for update.
@param currInd The entry index for the CPPLCSubEntry being processed.

*/
void CPPLoopCluster::TokenizeFuncBlocks(STLString  expr, TokenSet & tokens, Evaluable* pCaller, const bool & isInInit, size_t & currInd)
{
	//arg type depends on subblocks

	//this will possibly produce some overlap with tokens in other levels,
	//but proper parsing should make this irrelevant

	const STLRegex funcReg("\\b([a-zA-Z_][a-zA-Z0-9_]*)[(]");

	size_t offset = 0;
	size_t funcPos;
	size_t funcLen;
	STLsmatch matches;
	oSTLSStream tmpBuff;

	const OMEChar* excludes[] = { "^\\s*if[(]*$", "^\\s*then[(]\\s*$", "^\\s*else[(]\\s*$", "^\\s*elseif[(]\\s*$", "^\\s*and[(]\\s*$", "^\\s*or[(]\\s*$", "^\\s*end[(]\\s*$", "\0" };

	while (std::regex_search(expr, matches, funcReg))
	{
		//DBG_PRINTC(matches[0].str(),DBG_CYAN);
		size_t start = matches.position(1) + matches.length(1);
		size_t findRet = FindMatching(expr, start, '(', ')');
		if (!BatchRegexMatchAny(matches[0].str(), excludes, 0))
		{
			funcPos = matches.position(1);
			//+1 to make last paren inclusive
			funcLen = findRet - matches.position(1) + 1;

			STLString staticLimit;
			STLString funcName = expr.substr(funcPos, expr.find('(', funcPos) - funcPos);
			auto fItr = s_funcSigs.find(funcName);
			if (fItr != s_funcSigs.end())
			{
				tmpBuff.clear();
				tmpBuff.str(STLString());
				unsigned short ld = 0;
				FuncSig::FS_ARGS retType;
				
				if (!((*fItr).second.flags & FuncSig::CUSTOM_SIG))
					retType = ProcessFunc(tmpBuff, ld, expr.substr(funcPos, funcLen), (*fItr).second, pCaller, isInInit, currInd);
				else
					retType = (*fItr).second.CustFunc(m_pSerializer->m_className,tmpBuff, pCaller, this,currInd, expr.substr(funcPos, funcLen), isInInit);
				funcPos += offset;
				tokens.emplace(FUNCBLOCK, funcPos, funcLen, ld, retType, tmpBuff.str());
			}
			else
			{
				EvalTable* pET = pCaller->GetParentModel()->GetEvalTable(funcName);
				
				if (pET)
				{
					///\todo replace this quick fix with proper code to handle multiple explicit table calls
					//if direct table call. replace with graph call
					pCaller->AppendTableID(funcName);
					tokens.emplace(FUNCBLOCK, funcPos, funcLen, 0, FuncSig::SCLR_ARG, "graph"+expr.substr(start,findRet-start+1));
					
				}
				else
				{
					DBG_PRINTC("Function called, but not defined: " + expr.substr(funcPos, expr.find('(', funcPos) - funcPos), DBG_RED);
				}	
			}
		}
		offset += findRet;

		expr = expr.substr(findRet);
	}
}

/** Generate tokens denoting the beginning and end of list statements.
	@param expr The expression to parse for lists.
	@param tokens The set to add tokens to if any are found.
	@param pCaller Pointer to Evaluable containing expr.
	@param isInInit If true, initialization statements are being generated; otherwise, update statements are being created.
	@param currInd The current insertion point for any generated statements. May be modified on return.
*/
void CPPLoopCluster::TokenizeListBlocks(const STLString & expr, TokenSet & tokens, Evaluable* pCaller, const bool & isInInit, size_t & currInd)
{
	auto tItr = tokens.begin();

	size_t i = 0;
	size_t endPos;
	STLString tmpStr;


	while (i < expr.length())
	{
		if (tItr != tokens.end() && i == (*tItr).index)
		{
			//skip to next position using length
			//for now, assume all tokens signify skipping
			i += (*tItr).length;
			++tItr;
		}
		else
		{
			if (expr[i] == '[')
			{
				std::pair<size_t, size_t> currArg(0, 0);
				StrList foundArgs;
				FuncSig::FS_ARGS retType = FuncSig::LIST_ARG;
				unsigned short ld = 0;
				unsigned int compCount = 0;
				endPos = FindMatching(expr, i, '[', ']');

				tmpStr = expr.substr(i + 1, endPos - i - 1);
				for (size_t j = 0; j < tmpStr.size(); j++)
				{
					switch (tmpStr[j])
					{
					case ',':
						TrimRange(tmpStr, currArg.first, currArg.second);
						foundArgs.push_back(CPPizeExpression(tmpStr.substr(currArg.first, currArg.second), pCaller, retType, ld, isInInit, currInd));

						++compCount;
						currArg = std::pair<size_t, size_t>(j + 1, 0);
						break;
					case '(':
						j = FindMatching(tmpStr, j, '(', ')');
						currArg.second = 1 + j - currArg.first;
						break;
					case '[':
						j = FindMatching(tmpStr, j, '[', ']');
						currArg.second = 1 + j - currArg.first;
						break;
					default:
						currArg.second++;
					}
				}

				//grab last argument if needed
				if (currArg.second)
				{
					if (!FullTrimString(tmpStr.substr(currArg.first, ++currArg.second)).empty())
					{
						++compCount;
						foundArgs.push_back(CPPizeExpression(tmpStr.substr(currArg.first, ++currArg.second), pCaller, retType, ld, isInInit, currInd));
					}
				}

				tokens.emplace(LISTBLOCK, i, (endPos - i) + 1, compCount, retType, ConcatStrings(foundArgs, ','));
				
				i = endPos + 1;
			}
			else
				++i;
		}

	}
}

/** Generate tokens for Math operators in a given expression.
	@param expr The expression to evaluate.
	@param tokens The set of tokens to accept any new tokens.
*/
void CPPLoopCluster::TokenizeMathOperators(const STLString & expr, TokenSet & tokens)
{
	//dereference of tItr is ExprToken struct
	TokenSet::iterator tItr = tokens.begin();
	TokenSet::iterator preTokenItr;
	TokenSet::iterator postTokenItr;

	TokenSet listArgTokens;
	TokenSet openTokens;
	TokenSet closeTokens;
	const ExprToken* pCurrOpen = NULL;
	const ExprToken* pCurrClose = NULL;

	FuncSig::FS_ARGS retType = FuncSig::NO_ARGS;
	size_t i = 0;
	unsigned short currDepth;

	while (i < expr.length())
	{
		if (tItr != tokens.end() && i == (*tItr).index)
		{
			//skip to next position using length
			//for now, assume all tokens signify skipping
			i += (*tItr).length;
			++tItr;
		}
		else
		{
			preTokenItr = tokens.end();
			postTokenItr = tokens.end();
			switch (expr[i])
			{
			case '+':
			case '-':
			case '*':
			case '/':
			case '%':
				preTokenItr = FindTokenPrecedingIndex(i, tokens);
				postTokenItr = FindTokenFollowingIndex(i, tokens);

				if (preTokenItr == tokens.end())
				{
					retType = (*postTokenItr).aType;
					currDepth = (*postTokenItr).listDepth;
				}
				else
				{
					currDepth = std::max((*preTokenItr).listDepth, (*postTokenItr).listDepth);
					retType = ResolveFSArg((*preTokenItr).aType, (*postTokenItr).aType);

					if ((*preTokenItr).aType == FuncSig::LIST_ARG)
						listArgTokens.insert(*preTokenItr);
					if (!pCurrOpen)
					{
						pCurrOpen = &(*preTokenItr);
						openTokens.insert(*pCurrOpen);
					}
				}

				if ((*postTokenItr).aType == FuncSig::LIST_ARG)
					listArgTokens.insert(*postTokenItr);

				pCurrClose = &(*postTokenItr);

				preTokenItr = tokens.emplace(MATHOP, i, 1, currDepth, ResolveFSArg(retType, FuncSig::SCLR_ARG)).first;
				if (!pCurrOpen)
				{
					pCurrOpen = &(*preTokenItr);
					openTokens.insert(*pCurrOpen);
				}
				break;
			case '^':

				//find tokens preceding and following index i;
				preTokenItr = FindTokenPrecedingIndex(i, tokens);
				postTokenItr = FindTokenFollowingIndex(i, tokens);
				if (preTokenItr == tokens.end() || postTokenItr == tokens.end())
					throw OMESupportException("Malformed Power operator ('^')");

				retType = ResolveFSArg(ResolveFSArg((*preTokenItr).aType, (*postTokenItr).aType), FuncSig::SCLR_ARG);
				//preTokenItr = 
					tokens.emplace(POWOPOPEN, (*preTokenItr).index, 0, retType).first;
				if (!pCurrOpen)
				{
					pCurrOpen = &(*preTokenItr);
					openTokens.insert(*pCurrOpen);
				}
				pCurrClose = &(*postTokenItr);
				tokens.emplace(ARGSEP, (*preTokenItr).index + (*preTokenItr).length, 0, retType);

				tokens.emplace(SUFFIXMOD, (*postTokenItr).index, 0, 0, FuncSig::NO_ARGS, ")");
				tokens.emplace(DISCARD, i, 1, FuncSig::NO_ARGS);
				break;
			case ',':
				tokens.emplace(ARGSEP, i, 1, 0).first;
				if (pCurrOpen)
				{
					closeTokens.insert(*pCurrClose);
					pCurrOpen = NULL;
				}
				break;
			}

			++i;

			//skip irrelevant tokens
			while (tItr != tokens.end() && (*tItr).index < i)
				++tItr;
		}

	}

	if (pCurrOpen)
		closeTokens.insert(*pCurrClose);

}

/** Generate tokens for boolean operators in a given expression.
@param expr The expression to evaluate.
@param tokens The set of tokens to accept any new tokens.
*/
void CPPLoopCluster::TokenizeBoolOperators(const STLString & expr, TokenSet & tokens)
{

	//dereference of tItr is ExprToken struct
	auto tItr = tokens.begin();

	size_t i = 0;
	size_t len;

	TokenSet listArgTokens;

	const ExprToken* pCurrOpen = NULL;
	const ExprToken* pCurrClose = NULL;

	while (i < expr.length())
	{
		if (tItr != tokens.end() && i == (*tItr).index)
		{
			//skip to next position using length
			//for now, assume all tokens signify skipping
			i += (*tItr).length;

			//advance to next upcoming token
			for (; tItr != tokens.end() && (*tItr).index < i; ++tItr);

		}
		else
		{
			//the and and or parts will produce false positives, but should be handled
			//by token parsing/ordering in set.
			len = 0;
			switch (expr[i])
			{
			case '=':
			case '!':
			case '~':
			case '<':
			case '>':
				len = 1;
				if (i < expr.length() - 1 && expr[i + 1] == '=')
					++len;
				break;
			case 'a':
				if (i>0 && std::isspace(expr[i - 1]) && i < expr.length() - 3 && expr[i + 1] == 'n' && expr[i + 2] == 'd'&& std::isspace(expr[i + 3]))
					len = 3;
				break;
			case 'o':
				if (i>0 && std::isspace(expr[i - 1]) && i < expr.length() - 2 && expr[i + 1] == 'r' && std::isspace(expr[i + 2]))
					len = 2;
				break;
			}

			if (len)
			{
				TokenSet::iterator preTokenItr = FindTokenPrecedingIndex(i, tokens);
				TokenSet::iterator postTokenItr = FindTokenFollowingIndex(i, tokens);

				if ((*preTokenItr).aType == FuncSig::LIST_ARG)
					listArgTokens.insert(*preTokenItr);
				if ((*postTokenItr).aType == FuncSig::LIST_ARG)
					listArgTokens.insert(*postTokenItr);

				if (!pCurrOpen)
					pCurrOpen = &(*preTokenItr);
				pCurrClose = &(*postTokenItr);

				FuncSig::FS_ARGS retType = FuncSig::INT_ARG;
				unsigned short currDepth = std::max((*preTokenItr).listDepth, (*postTokenItr).listDepth);
				if ((*preTokenItr).aType == FuncSig::LIST_ARG || (*postTokenItr).aType == FuncSig::LIST_ARG)
					retType = FuncSig::LIST_ARG;

				tokens.emplace(BOOLOP, i, len, currDepth, retType);

				i += len;
			}
			else
				++i;
		}

	}

}

/** Generate tokens for constant and variable values in a given expression.
@param expr The expression to evaluate.
@param tokens The set of tokens to accept any new tokens.
@param pCaller Pointer to Evaluable that contains expr.
*/
void CPPLoopCluster::TokenizeValues(const STLString & expr, TokenSet & tokens, Evaluable* pCaller)
{
	const STLRegex numReg("(?:^|[^_.a-zA-Z])((?:[0-9]+(?:[.][0-9]*)?|[0-9]*[.][0-9]+)(?:[Ee][+-][0-9]+)?)(?![._a-zA-Z])");
	const STLRegex enumReg("^[a-zA-Z0-9_.]+[:][a-zA-Z0-9_.]+");
	const STLRegex valReg("^[a-zA-Z_][a-zA-Z0-9_.]*(?![(])\\b");

	const OMEChar* valExcludes[] = { "^\\s*if\\s*$", "^\\s*then\\s*$", "^\\s*else\\s*$", "^\\s*elseif\\s*$", "^\\s*and\\s*$", "^\\s*or\\s*$", "^\\s*end\\s*$", "\0" };

	STLsmatch matches;
	STLString tmpStr;
	//dereference of tItr is ExprToken struct
	auto tItr = tokens.begin();

	size_t i = 0;
	size_t len;

	while (i < expr.length())
	{
		if (tItr != tokens.end() && i == (*tItr).index)
		{
			//skip to next position using length
			//for now, assume all tokens signify skipping
			i += (*tItr).length;

			//advance to next upcoming token
			for (; tItr != tokens.end() && (*tItr).index < i; ++tItr);
		}
		else
		{
			if (expr[i] == '_' || std::isalpha(expr[i]))
			{
				tmpStr = expr.substr(i);
				
				//check for enum or named variable
				if (std::regex_search(tmpStr, matches, enumReg))
				{
					//handle Enum
					tokens.emplace(ENUMVAL, i, matches.length(0), 0, FuncSig::ENUM_ARG);
					i += matches.length(0);
					DBG_PRINTC("Enum: " + std::to_string(i), DBG_GREEN);
				}
				else if (std::regex_search(tmpStr, matches, valReg))
				{
					if (!BatchRegexMatchAny(matches[0].str(), valExcludes, 0))
					{
						OMEObject* pObj = GetRootModel()->NestedObjectForName(matches[0].str());
						if (pObj)
						{
							if (pObj->IsOMEType(OME_ALIAS))
								pObj = ((EvalAlias*)pObj)->GetSrc();
							tokens.emplace(VAR, i, matches.length(0), FindDepthDifference(pCaller, (Evaluable*)pObj), pObj->IsOMEType(OME_VARARRAY) ? FuncSig::LIST_ARG : FuncSig::EVAL_ARG, pObj->GetID());
						}
						else
						{
							auto fItr = s_definedConstants.find(matches[0].str());
							if (fItr != s_definedConstants.end())
								tokens.emplace(VAR, i, matches.length(0), 0, FuncSig::SCLR_ARG, (*fItr).second);
						}
					}
					i += matches.length(0);
				}
				else
				{
					throw OMEFuncOpException("TokenizeValues", "Unrecognized token.");
				}
			}
			else if (expr[i] == '.' || std::isdigit(expr[i]))
			{
				// +/- prefixes will be picked up elsewhere 
				//find extent of constant value;

				size_t offset = 0;
				STLString tmpSub = expr.substr(i);
				if (std::regex_search(tmpSub, matches, numReg))
				{
					tokens.emplace(CONSTVAL, i, matches.length(1), 0, matches[1].str().find('.') != STLString::npos ? FuncSig::SCLR_ARG : FuncSig::INT_ARG);
					i += matches.length(1);
				}
				else
				{
					DBG_PRINT(tmpSub);
					throw OMEFuncOpException("TokenizeValues", "Unrecognized digit pattern.");
				}
			}
			else
			{
				++i;
			}
		}
	}

}

/** Find the token containing the index nearest to, but still less than, the provided index.
	@param ind The Index to search against.
	@param tokens The set to search through.
	@return An iterator from tokens pointing to the preceding token.
	*/
CPPLoopCluster::TokenSet::iterator CPPLoopCluster::FindTokenPrecedingIndex(const size_t & ind, const TokenSet & tokens)
{
	auto currItr = tokens.begin();
	auto prevItr = tokens.end();
	size_t nextPos = 0;

	if (tokens.size() == 1 && (*currItr).index < ind)
		return currItr;

	while (currItr != tokens.end())
	{
		if ((*currItr).index >= ind)
			return prevItr;

		prevItr = currItr;
		nextPos = (*currItr).index + (*currItr).length;

		for (; currItr != tokens.end() && ((*currItr).index < nextPos || (!(*currItr).length)); ++currItr);
	}

	return tokens.end();
}

/** Find the token containing the index nearest or equal to the provided index.
	@param ind The Index to search against.
	@param tokens The set to search through.
	@return An iterator from tokens pointing to the preceding token.
*/
CPPLoopCluster::TokenSet::iterator CPPLoopCluster::FindTokenPrecedingOrMatchingIndex(const size_t & ind, const TokenSet & tokens)
{
	auto currItr = tokens.begin();
	auto prevItr = tokens.end();
	size_t nextPos = 0;
	if (tokens.size() == 1 && (*currItr).index <= ind)
		return currItr;

	while (currItr != tokens.end())
	{
		if ((*currItr).index == ind)
			return currItr;
		if ((*currItr).index > ind)
			return prevItr;

		prevItr = currItr;
		nextPos = (*currItr).index + (*currItr).length;

		for (; currItr != tokens.end() && ((*currItr).index < nextPos || (!(*currItr).length)); ++currItr);
	}

	return tokens.end();
}

/** Find the token containing the index nearest to, but still greater than, the provided index.
	@param ind The Index to search against.
	@param tokens The set to search through.
	@return An iterator from tokens pointing to the following token.
*/
CPPLoopCluster::TokenSet::iterator CPPLoopCluster::FindTokenFollowingIndex(const size_t & ind, const TokenSet & tokens)
{
	auto currItr = tokens.begin();

	size_t nextPos = 0;
	while (currItr != tokens.end())
	{
		if ((*currItr).index > ind)
			return currItr;

		nextPos = (*currItr).index + (*currItr).length;

		for (; currItr != tokens.end() && ((*currItr).index < nextPos || (!(*currItr).length)); ++currItr);
	}

	return tokens.end();
}

/** Find the token containing the index nearest or equal to the provided index.
	@param ind The Index to search against.
	@param tokens The set to search through.
	@return An iterator from tokens pointing to the following token.
*/
CPPLoopCluster::TokenSet::iterator CPPLoopCluster::FindTokenFollowingOrMatchingIndex(const size_t & ind, const TokenSet & tokens)
{
	auto currItr = tokens.begin();

	size_t nextPos = 0;
	while (currItr != tokens.end())
	{
		if ((*currItr).index >= ind)
			return currItr;

		nextPos = (*currItr).index + (*currItr).length;

		for (; currItr != tokens.end() && ((*currItr).index < nextPos || (!(*currItr).length)); ++currItr);
	}

	return tokens.end();
}

/** Determine which type of argument will be returned by a statement involving two arument types.
	@param lhs The first argument type to compare.
	@param rhs The second argument type to compare.
	@return The resolved argument type.
	*/
FuncSig::FS_ARGS CPPLoopCluster::ResolveFSArg(const FuncSig::FS_ARGS & lhs, const FuncSig::FS_ARGS & rhs)
{
	if (lhs == rhs)
		return lhs;

	if (lhs == FuncSig::NO_ARGS)
		return rhs;
	if (rhs == FuncSig::NO_ARGS)
		return lhs;

	if (lhs == FuncSig::LIST_ARG || lhs == FuncSig::VA_ARG || rhs == FuncSig::LIST_ARG || rhs == FuncSig::VA_ARG)
		return FuncSig::LIST_ARG;

	if (lhs == FuncSig::SCLR_ARG || rhs == FuncSig::SCLR_ARG)
		return FuncSig::SCLR_ARG;

	if ((lhs == FuncSig::INT_ARG || rhs == FuncSig::INT_ARG))
		return FuncSig::INT_ARG;

	return FuncSig::NO_ARGS;
}

/** Process assignment statements for an Evaluable object, producing useable C++ code.
	@param outBuff buffer to write C++ code to.
	@param pCaller Pointer to Evaluable object being processed.
	@param newExpr The expression equation originating from pCaller.
	@param isInInit If true, initialization-specific code may be included; otherwise only update code is generated.
	@param retType The return value type expected by pCaller.
*/
void CPPLoopCluster::ProcessEvaluableAssignment(oSTLSStream & outBuff, Evaluable* pCaller, const STLString & newExpr,const bool & isInInit,const OMECFuncs::FuncSig::FS_ARGS & retType)
{

	outBuff << pCaller->GetID();
	if (pCaller->IsOMEType(OME_VARARRAY))
	{

		VarArray* pVa = (VarArray*)pCaller;
		if (!pVa->GetDimCount() && retType == FuncSig::LIST_ARG)
		{
			STLRegex reg("(?:#([!0-9]+))+");
			STLsmatch mtch;

			if (std::regex_search(newExpr, mtch, reg))
			{
				StrArray dims = SplitString(mtch[1].str(), '#');
				if (isInInit)
					outBuff << ".OverrideDims({" << ConcatStrings(dims, ',') << "});" << endl << pCaller->GetID() << ".Allocate();" << endl << pCaller->GetID();

				for (STLString & d : dims)
				{
					pVa->GetDims().AppendDim(std::stoul(d));
				}
			}

		}

		if (pVa->GetDimCount())
		{

			for (unsigned short i = 0; i < pVa->GetDimCount(); ++i)
				outBuff << '#' << pVa->GetDim(i);
		}
		else
			outBuff << "#0";
	}
	
	outBuff << "=(OME_SCALAR)" <<newExpr << ";" << endl;
}

/** Generate list statements for various model logic components. The resulting C++ statements
	are stored in the originating CPPLCSubEntry objects.
*/
void CPPLoopCluster::ProcessListStatements()
{
	for (size_t i = 0; i < m_subEntries.size(); ++i)
	{
		switch (m_subEntries[i]->GetEntryType())
		{
		case CPPLCSubEntry::EVAL:
		case CPPLCSubEntry::TMPASGN:
			ProcessListStatements(i, i, this);
			break;
		case CPPLCSubEntry::SUBCLST:
			m_subEntries[i]->GetSubLoopCluster()->ProcessListStatements();
			break;
		}
	}
}

/** Generate list statements for various generated temporary variables. 
The resulting C++ statements are stored in the originating CPPLCSubEntry objects.
*/
void CPPLoopCluster::ProcessListStatementsForTemps()
{
	CPPLCSubEntry* pToMove,*pSrc;
	CPPLoopCluster* pSub, *pRefSub;
	size_t tmpIniSize;
	size_t insert;
	
	std::list<size_t> counts;

	for (size_t i = 0; i < m_subEntries.size(); ++i)
	{

		switch (m_subEntries[i]->GetEntryType())
		{
		case CPPLCSubEntry::TMPINIT:
			pToMove = m_subEntries[i];

			pSrc = s_entryRegistry[pToMove->GetLabel()];
			if (pSrc->GetDimCount())
			{
				m_subEntries.erase(m_subEntries.begin() + i);
				counts = FindTmpDims(pSrc);
				counts.reverse();

				insert = i;
				pSub = this;
				for (size_t & c : counts)
				{
					pSub = pSub->NewSubLoopCluster(insert, c, GenVarName("iniLoop"));
					insert = 0;
				}

				pToMove->SetParentLoopCluster(pSub);
				pSub->m_subEntries.push_back(pToMove);
			}
			break;
		//case CPPLCSubEntry::CUSTOM:
		case CPPLCSubEntry::TMPDCLR:
			ProcessListStatements(i, i, this);
			break;
		case CPPLCSubEntry::SUBCLST:
			m_subEntries[i]->GetSubLoopCluster()->ProcessListStatementsForTemps();
			break;
		}
	}
}

/** Convert temporary variable placeholders into the appropriately positioned 
	variable declaration, initialization, and application statements.
*/
void CPPLoopCluster::ApplyTmpDeclarations()
{

	for (size_t i = 0; i < m_subEntries.size(); ++i)
	{
		std::map<STLString, StrList>::iterator fItr;
		switch (m_subEntries[i]->GetEntryType())
		{
		case CPPLCSubEntry::EVAL:
			fItr = m_tempArrayDclrs.find(m_subEntries[i]->GetEvaluable()->GetID());
			if (fItr != m_tempArrayDclrs.end())
			{
				for (STLString & str : (*fItr).second)
				{
					size_t split = str.find(':');
					size_t currInsert = i;
					//insert before any statements derived from current eval.
					for (; currInsert > 0 && m_subEntries[currInsert - 1]->GetEntryType() == CPPLCSubEntry::TMPMARK; --currInsert);
					NewTempDeclaration(currInsert, str.substr(split + 1), str.substr(0, split));
					++i;
				}
			}
			break;
		case CPPLCSubEntry::SUBCLST:
			m_subEntries[i]->GetSubLoopCluster()->ApplyTmpDeclarations();
			break;
		}
	}
}

/** Generate list statements for various model logic components. The resulting C++ statements
are stored in the originating CPPLCSubEntry objects.
	@param trgInd The index of the CPPLCSubEntry that is being processed.
	@param replaceInd The index of the CPPLCSubEntry to replace. This is used as part of the recursive processing task.
	@param pLocalRoot Pointer to the base root of the local CPPLoopCluster subtree. This is used in determining how large list loops should be.
	@param depth Value used to determine what scope should be processed for the statement of the entry found at trgInd.
*/
void CPPLoopCluster::ProcessListStatements(const unsigned int & trgInd, const unsigned int & replaceInd, CPPLoopCluster* pLocalRoot, unsigned short depth)
{
	CPPLCSubEntry* pEntry = m_subEntries[trgInd];

	const STLRegex opsReg("\\b([_a-zA-Z][_a-zA-Z0-9]*)((?:#[!0-9]+)+)");
	const STLRegex endParenReg("[)]((?:#[!0-9]+)+)");
	STLsmatch match;
	int longest = 0;
	int numDims;
	bool subLoop = false;
	STLString depthStr;

	//check to see if we need to do anything
	STLString expr = pEntry->GetUpdateStatement();

	if (expr.find('#') != STLString::npos)
	{
		size_t newSize, size = 0;
		size_t splitLoc;
		size_t endPos;
		size_t startPos;

		while (std::regex_search(expr, match, opsReg))
		{
			splitLoc = RFindNth(match[2].str(), '#', depth + 1);
			
			//pullSizes, make sure they match,
			//if zero, ignore.
			if (splitLoc != STLString::npos)
			{
				numDims = CountChar(match[2].str(), '#');
				subLoop = true;
				++splitLoc;
				for (endPos = splitLoc; endPos < match[2].str().size() && std::isdigit(match[2].str()[endPos]); ++endPos);
				newSize = std::stoul(match[2].str().substr(splitLoc, endPos - splitLoc));
				if (size == 0)
					size = newSize;
				//else if (newSize != 0 && size != newSize)
				//	throw std::invalid_argument("CPPLoopCluster::ProcessListStatements: list mismatch");
				if (numDims>=longest)
				{
					longest = numDims;
					depthStr = match[0].str();
				}
			}

			expr = match.suffix().str();
		}

		//look for any depth tags associated with a function call (usually element)
		expr = pEntry->GetUpdateStatement();
		while (std::regex_search(expr, match, endParenReg))
		{
			splitLoc = RFindNth(match[1].str(), '#', depth + 1);

			//pullSizes, make sure they match,
			//if zero, ignore.
			if (splitLoc != STLString::npos)
			{
				numDims = CountChar(match[2].str(), '#');
				if (numDims >= longest)
				{
					longest = numDims;
					subLoop = true;
					startPos = FindMatching(expr, match.position(0), ')', '(', true);
					for (; startPos >= 0 && !std::isalnum(expr[startPos]); --startPos);
					for (; startPos > 0 && std::isalnum(expr[startPos - 1]); --startPos);

					auto fItr = s_dimReferences.find(expr.substr(startPos, match.length(0) + (match.position(0) - startPos)));
					if (fItr != s_dimReferences.end())
						depthStr = (*fItr).second;
				}
			}
			expr = match.suffix().str();
		}


		//add subloop at this spot, and move entry into it
		//should only need to add loop if left side of assignment needs value
		//assume first '=' is for assignment
		expr = pEntry->GetUpdateStatement();

		CPPLoopCluster* pRefClust = NULL;
		if (RFindNth(depthStr, '#', depth + 1) != STLString::npos)
		{
			if (!size)
			{
				//resolve dimension
				size = FindCountForEntry(depthStr, depth, pLocalRoot, &pRefClust);

			}

			

			if (!pEntry->GetCount())
				pEntry->SetCount(size);
		}

		if (subLoop)
		{
			CPPLCSubEntry* pToMove = m_subEntries[replaceInd];
			m_subEntries.erase(m_subEntries.begin() + replaceInd);
			CPPLoopCluster* pSub = NewSubLoopCluster(replaceInd, size, GenVarName("tmpLoop"));

			if (!size)
			{
				if (pRefClust == NULL)
				{
					STLString fName = depthStr.substr(0, depthStr.find("#"));
					auto fItr = s_entryRegistry.find(fName);
					
					if (fItr!=s_entryRegistry.end() && !(*fItr).second->GetLoopLimitCheck().empty())
						pSub->SetIterationLimit((*fItr).second->GetLoopLimitCheck());
					else
						throw OMESupportException("todo: add var name as limit");
				}
				else
					pSub->SetIterationLimit(pRefClust->m_label + ".GetNumInstances()");
			}
			//likely from conditional model; ensure we have loop to prevent breakage
			if (size==1)
				pSub->SetForceLoop(true);
			pToMove->SetParentLoopCluster(pSub);
			pSub->m_subEntries.push_back(pToMove);

			pSub->ProcessListStatements(0, 0,pLocalRoot,depth + 1);
		}

	}
}

/** Find the number of iterations needed for the loop found at depth.
	@param depthStr The string to parse for depth patterns.
	@param depth The depth to parse.
	@param pLocalRoot Pointer to CPPLoopCluster to use as a reference point for the location of the base depth.
	@param hLimitClust Optional Handle to hold a pointer to the bottom-most cluster on return.
	@return The number of iterations derived from depthStr.
*/
size_t CPPLoopCluster::FindCountForEntry(const STLString & depthStr, const size_t & depth, CPPLoopCluster* pLocalRoot,CPPLoopCluster** hLimitClust)
{
	/* 
		Longest sequence of # = longest chain for depth
		-add +1 for any #! found.
		Add Loop per depth
		Use known sizes to calculate loop size.
	*/

	size_t ret = 0;

	STLString searchStr = depthStr.substr(0, depthStr.find('#'));
	int depthOffs = depthStr[depthStr.find('#') + 1] == '!' ? 1 : 0;
	size_t count = CountChar(depthStr, '#')-depth;

	CPPLCSubEntry* pSe = s_entryRegistry[searchStr];
	CPPLoopCluster* pCurrClust,*pCommonClust;
	std::list<std::pair<size_t,CPPLoopCluster*>> dimList;
	size_t start = 0;
	size_t i;

	if (pSe->GetEvaluable() && pSe->GetEvaluable()->IsOMEType(OME_VARARRAY))
	{
		VarArray* pVa = ((VarArray*)pSe->GetEvaluable());
		for (unsigned short i = 0; i < pVa->GetDimCount(); ++i)
			dimList.emplace_back(pVa->GetDim(i), (CPPLoopCluster*)NULL);
	}

	pCurrClust = pSe->GetParentLoopCluster();

	pCommonClust = FindCommonLoopCluster(pCurrClust, pLocalRoot);
	while (pCommonClust && !pCommonClust->HasModel())
		pCommonClust = pCommonClust->GetParentCluster();

	while (pCurrClust != pCommonClust && !pCurrClust->NeedsLoop())
		pCurrClust = pCurrClust->GetParentCluster();


	while (pCurrClust && pCurrClust != pCommonClust)
	{

		while (pCurrClust != pCommonClust && !pCurrClust->NeedsLoop())
		{
			pCurrClust = pCurrClust->GetParentCluster();
			if (pCurrClust == NULL)
				break;
		}
		if (pCurrClust && pCurrClust!=pCommonClust)
		{
			dimList.emplace_back(pCurrClust->m_iterations,pCurrClust);
			pCurrClust = pCurrClust->GetParentCluster();
		}
	}

	for (i = 0; !dimList.empty() && i < depth+depthOffs; ++i)
		dimList.pop_back();

	while (dimList.size() > count)
		dimList.pop_back();

	if (!dimList.empty())
	{
		ret = dimList.back().first;
		if (hLimitClust)
			*hLimitClust = dimList.back().second;
	}
	else
	{
		throw OMEFuncOpException("FindCountForEntry", "No dimensions found.");
	}

	return ret;
}

/** Assigned increment variables to places where they are used to access list content.
	@param pEntry Pointer to the CPPLCSubEntry that contains the update statement to modify.
*/
void CPPLoopCluster::AssignIndexAccess(CPPLCSubEntry* pEntry)
{
	STLRegex searchReg("([_a-zA-Z][_a-zA-Z0-9]*)((?:#[!0-9]+)+)(\\[.*\\])?");
	STLString expr = pEntry->GetUpdateStatement();

	STLsmatch mtch;
	size_t count;
	oSTLSStream outBuff;

	while (std::regex_search(expr, mtch, searchReg))
	{
		//add what was skipped;
		outBuff << mtch.prefix().str();

		//grab information on the variable.
		STLString id = mtch[1].str();

		//varEntry is used to grab dims. use makearray if present
		CPPLCSubEntry* pVarEntry =s_entryRegistry[id];
		CPPLCSubEntry* pDepthEntry = s_entryRegistry.find(id + "_ma") == s_entryRegistry.end() ? pVarEntry : s_entryRegistry[id + "_ma"];

		CPPLoopCluster* pClust = this;
		StrList valIncs;
		
		valIncs = FindTmpDimIncs(pEntry, pDepthEntry);
		//count the number of indices needed
		count = CountChar(mtch[2].str(), '#');

		size_t offs = (count && mtch[2].str()[mtch[2].str().find('#')+1]=='!') ? 1 : 0;

		bool modelHit = false;
		if (pVarEntry->GetEntryType() == CPPLCSubEntry::EVAL)
		{
			Model* pCurrMdl = pVarEntry->GetEvaluable()->GetParentModel();
			for (; pCurrMdl && std::find(m_modelList.begin(), m_modelList.end(), pCurrMdl) == m_modelList.end(); pCurrMdl = pCurrMdl->GetParentModel());
			modelHit = pCurrMdl != NULL;
		}

		if (pVarEntry->GetEntryType() != CPPLCSubEntry::EVAL || !modelHit)
		{
			while (valIncs.size()>count + offs)
				valIncs.pop_front();
		}

		if (valIncs.size() > count)
		{

			for (size_t i = 0; i < offs; ++i)
				valIncs.pop_front();

		}

		//add index string
		if (pVarEntry->GetEntryType() == CPPLCSubEntry::EVAL)
		{
				if (pVarEntry->GetEvaluable()->IsOMEType(OME_VARARRAY) || pVarEntry->GetParentLoopCluster()->NeedsLoop() || count > 1)
				{
					//outBuff << "SubIndexVal<ValueArchive&>(" << id << "," << count << ").Sub(" << ConcatStrings(valIncs, ").Sub(") << ")";
					auto itr = valIncs.rbegin();

					Model* pTrgMdl = pVarEntry->GetEvaluable()->GetParentModel();

					/*if (std::find(m_modelList.begin(), m_modelList.end(), pTrgMdl) != m_modelList.end())*/
					//if (modelHit && !pVarEntry->GetEvaluable()->IsOMEType(OME_VARARRAY))
					//	outBuff << "(*root.GetValueArchive())[" << pTrgMdl->GetID() << ".GetArchiveIndex(" << id << ".GetModelIndex())";
					//else
						outBuff << "(*root.GetValueArchive())[" << id << ".GetModelIndex()";
						CPPLoopCluster* pParent = pEntry->GetParentLoopCluster();
						for (; !pParent->NeedsLoop() && pParent->m_iterations == 1; pParent = pParent->GetParentCluster());
						STLString offs = DeriveIndexOffset(pEntry, pDepthEntry);
						if (!offs.empty())
							outBuff << "+" << offs;
						outBuff<< "]";
						
				}
				else
					outBuff << id;
		
		}
		else
		{
			outBuff << id << '[';
			outBuff << ConcatStrings(valIncs, "][");
			outBuff << ']';
		}
		expr = mtch.suffix().str();
	}

	
	if (!outBuff.str().empty() && !expr.empty())
		outBuff << expr;

	//look for function with index access
	if (!outBuff.str().empty())
		expr = outBuff.str();

	outBuff.clear();
	outBuff.str(STLString());

	STLRegex fReg("([)])((?:#[!0-9]+)+)");
	CPPLoopCluster* pClust;
	STLsmatch mtch2;
	while (std::regex_search(expr, mtch2, fReg))
	{
		outBuff << mtch2.prefix().str();
		outBuff << mtch2[1].str();
		pClust=pEntry->GetParentLoopCluster();
		for (size_t i = 0; i < CountChar(mtch2[2].str(), '#'); ++i, pClust = pClust->GetParentCluster())
			outBuff << '[' << pClust->m_label << "_i]";

		expr = mtch2.suffix().str();
	}
	if (!outBuff.str().empty())
	{
		//add anything that was missed.
		if (!expr.empty())
			outBuff << expr;
		expr = outBuff.str();
	}

	if (!outBuff.str().empty() && !expr.empty())
		outBuff << expr;

	if (!expr.empty())
		pEntry->SetUpdateStatement(expr);
}

/** Replace any expression token placeholders with the appropriate values.*/
void CPPLoopCluster::ProcessTempPlaceholders()
{
	size_t tempsAdded = 0;
	CPPLCAccumEntry* pAe;

	for (size_t i = 0; i < m_subEntries.size(); ++i)
	{
		CPPLCSubEntry* pSe = m_subEntries[i];

		switch (pSe->GetEntryType())
		{
		case CPPLCSubEntry::TMPMARK:

			//presently placeholders only really work with accumulation statements
			pAe = &m_accums[pSe->GetUpdateStatement()];
			if (!pAe->HasBeenPlaced())
			{


				std::pair<STLString, STLString> pattrn = s_accumPatterns[pAe->GetFuncName()];

				//bool hasSecondary = pattrn.first.find("@b") != STLString::npos || pattrn.second.find("@b") != STLString::npos;
				CPPLCSubEntry * pNewEntry;
				CPPLoopCluster* pRoot = pAe->FindCommonRootUsingEntryRegistry(s_entryRegistry);
				//place initialization first, followed by declaration

				if (pAe->GetSuffix().empty())
				{
					//scalar declaration
					pNewEntry = pRoot->PrependTempDeclaration("OME_SCALAR " + pSe->GetLabel() + ";" /*" = 0.0; "*/, pSe->GetLabel(), pSe->GetAccumKey(), 1);
				}
				else
				{
					//array declaration
					
					pNewEntry = pRoot->PrependTempDeclaration("OME_SCALAR " + pSe->GetLabel() + "[@xi];" /*"[@xi]={};"*/, pSe->GetLabel(), pSe->GetAccumKey(), pSe->GetCount());

				}
				
				
				pNewEntry=pRoot->NewTempInitialization(++tempsAdded, pSe->GetUpdateStatement(), pSe->GetLabel(), pSe->GetAccumKey(), pSe->GetCount());
				pNewEntry->SetDimCount(pSe->GetDimCount());
				
				//compensate for changes in present list
				if (pRoot == this)
					i += 2;
				//place application here
				pNewEntry=NewTempApplication(i++, pSe->GetUpdateStatement(), pSe->GetLabel(), pSe->GetAccumKey(), pSe->GetCount());
				pNewEntry->SetDimCount(pSe->GetDimCount());
				//marked as places
				pAe->MarkAsPlaced();

			}
			break;
		}
	}


	//recurse after changes so resizes don't affect parent loop
	for (size_t i = 0; i < m_subEntries.size(); ++i)
	{
		CPPLCSubEntry* pSe = m_subEntries[i];

		switch (pSe->GetEntryType())
		{
		case CPPLCSubEntry::SUBCLST:
			pSe->GetSubLoopCluster()->ProcessTempPlaceholders();
		}
	}
}

/** Find the difference in list depths between two evaluables.
	@param pCaller Pointer to the Evaluable that is referencing another.
	@param pInExpr Pointer to the Evaluable that is being referenced.
	@return The difference in depth detween teh two Evaluables.
	*/
unsigned int CPPLoopCluster::FindDepthDifference(Evaluable* pCaller, Evaluable* pInExpr)
{
	unsigned int depth = 0;
	VarArray* pVa;

	//add values for inExpr dims
	if (pInExpr->IsOMEType(OME_VARARRAY))
	{
		unsigned int toAdd = 0;
		pVa = (VarArray*)pInExpr;

		toAdd += pVa->GetDimCount();
		size_t tmpPos;
		unsigned int maCount = 0;
		for (tmpPos = pVa->GetExpr().find("makearray"); tmpPos != STLString::npos; tmpPos = pVa->GetExpr().find("makearray", tmpPos + 9), ++maCount);


		depth += std::max(maCount, toAdd);
	}

	//if inExpr is outside of pCaller
	if (pCaller->GetParentModel() != pInExpr->GetParentModel())
	{
		//find the highest common submodel

		Model* pTopModel = pInExpr->GetParentModel();

		for (; pTopModel && !pTopModel->IsNested(pCaller->GetParentModel()); pTopModel = pTopModel->GetParentModel());

		//if pCaller is on the outside
		if (!pTopModel)
		{
			pTopModel = pCaller->GetParentModel();
			for (; pTopModel && !pTopModel->IsNested(pInExpr->GetParentModel()); pTopModel = pTopModel->GetParentModel());
		}

		for (Model* pCurrMdl = pInExpr->GetParentModel(); pCurrMdl != pTopModel; pCurrMdl = pCurrMdl->GetParentModel())
		{
			if (pCurrMdl->GetInitNumInstances() != 1 || pCurrMdl->IsDynamic())
				++depth;
		}
	}

	return depth;
}

/** Insert a marker designating the beginning of mortality checks at this point in entry processing.
	@param pMdl Pointer to the Model to apply the mortality checks to.
*/
void CPPLoopCluster::StartMortalityHere(Model* pMdl)
{
	CPPLCSubEntry* pMortEntry = new CPPLCSubEntry(this, CPPLCSubEntry::STARTMORT);
	pMortEntry->SetMortModel(pMdl);
	m_subEntries.push_back(pMortEntry);
}

/** Insert a marker designating the end of mortality checks at this point in entry processing.
@param pMdl Pointer to the Model to apply the mortality checks to.
*/
void CPPLoopCluster::EndMortalityHere(Model* pMdl)
{
	CPPLCSubEntry* pMortEntry = new CPPLCSubEntry(this, CPPLCSubEntry::ENDMORT);
	pMortEntry->SetMortModel(pMdl);
	m_subEntries.push_back(pMortEntry);
}

/** Initialize static list of signatures if they haven't already been loaded. */
void CPPLoopCluster::InitSigsIfNeeded()
{
	if (s_funcSigs.empty())
	{
		LoadSigs(s_funcSigs);
		//extract regex statements for aggs
		for (auto itr = s_funcSigs.begin(); itr != s_funcSigs.end();++itr)
		if ((*itr).second.flags & FuncSig::CROSS_AGG)
			s_aggSigs.push_back(STLString("\\b") + (*itr).second.name + "[(]");
		LoadAccumPatterns(s_accumPatterns);
		LoadConstants(s_definedConstants);
	}
}

/** Use a pattern describing how to expand an accumulation function to generate viable C++ code.
	@param pattern String containing the pattern to process.
	@param incName The increment variable name to apply.
	@param varIncName The variable increment variable name to apply.
	@param limitName The limit name to apply.
	@param se The source CPPLCSubEntry.
*/
STLString CPPLoopCluster::ProcessAccumPattern(const STLString & pattern, const STLString & incName, const STLString & varIncName, const STLString & limitName, CPPLCSubEntry & se)
{
	//assume invocation has been modified with accumPatterns as well.

	const OMEChar* invokePats[] = { "@ii", incName.c_str(), "\0" };

	STLString vName = se.GetLabel();
	STLString vName2 = se.GetLabel() + "_second";

	//if secondary variable is needed, make sure it is registered
	if (pattern.find("@b") != STLString::npos && s_entryRegistry.find(vName2) == s_entryRegistry.end())
		s_entryRegistry[vName2] = &se;

	oSTLSStream suff;
	for (size_t i = 0; i < se.GetDimCount(); ++i)
		suff << "#!";
		//all but last goes to update
		//updateStr += suffix.substr(0, split);
		//last num goes to names
	vName += suff.str();
	vName2 += suff.str();
	STLString updateStr = se.GetUpdateStatement();


	STLString listInitStr;
	if (pattern.find("@la") != STLString::npos)
	{
		//determine new temp array initializiation.
		try
		{
			//is number?
			int temp = std::stoi(limitName);
			listInitStr = "OME_SCALAR " + vName + "[" + limitName + "]();";
		}
		catch (...)
		{
			listInitStr = "DynamicTempArray<LSIZE_BEST_GUESS> " + vName + "(" + limitName + ");";
		}
	}

	const OMEChar* codes[] = {
		"@ai", vName.c_str(),
		"@as", vName.c_str(),
		"@al", vName.c_str(),
		"@bs", vName2.c_str(),
		"@vs", updateStr.c_str(),
		"@ii", incName.c_str(),
		"@li", limitName.c_str(),
		"@la", listInitStr.c_str(),
		"@vi", varIncName.c_str(),
		"\0"
	};

	//se.SetDimOffset(1);

	return BatchRegexReplace(pattern, codes);
}

/** Produce a temporary array entry for generated C++ code.
	@param src The original expression to be replaced.
	@param varName The name of the array.
	@param isConst bool indicating if the array values are all constant or not. Value assigned on return.
	@return A std::pair with the modified src string as the first field and varName as the second entry. 
*/
std::pair<STLString, STLString> CPPLoopCluster::GenTempArray(const STLString & src, const STLString & varName, bool & isConst)
{
	std::pair < STLString, STLString> ret;
	ret.second = varName;

	const OMEChar* patterns[] = { "[(][{]", "", "[}][)]", "", "\0" };

	ret.first = BatchRegexReplace(src, patterns);

	STLRegex reg("^[0-9.,]+$");
	STLsmatch match;
	isConst = std::regex_match(ret.first, match, reg);

	return ret;
}

/** Find the common root shared between two CPPLoopClusters within a CPPLoopCluster tree.
	@param lhs Pointer to the first CPPLoopCluster to compare.
	@param rhs CPPLoopCluster Pointer to the second CPPLoopCluster to compare.
	@return Pointer to the closest common root CPPLoopCluster, or NULL if the clusters are not in the same tree.
*/
CPPLoopCluster* CPPLoopCluster::FindCommonLoopCluster(CPPLoopCluster* lhs, CPPLoopCluster* rhs)
{
	//return if clusters are equal
	if (lhs == rhs)
		return lhs;

	CPPLoopCluster* pCurrCluster;

	//check if rhs is within lhs
	for (pCurrCluster = rhs; pCurrCluster; pCurrCluster = pCurrCluster->GetParentCluster())
	{
		if (pCurrCluster == lhs)
			return pCurrCluster;
	}

	//check if lhs is within rhs
	for (pCurrCluster = lhs; pCurrCluster; pCurrCluster = pCurrCluster->GetParentCluster())
	{
		if (pCurrCluster == rhs)
			return pCurrCluster;
	}

	//look for common root
	for (CPPLoopCluster* pCommon = rhs->GetParentCluster(); pCommon; pCommon = pCommon->GetParentCluster())
	{
		for (pCurrCluster = lhs; pCurrCluster; pCurrCluster = pCurrCluster->GetParentCluster())
		{
			//if clusters don't match, return cluster if it originates from the same model
			//assume first model is originating model
			if (pCurrCluster == pCommon || (!pCurrCluster->m_modelList.empty() && pCurrCluster->m_modelList.front() == pCommon->m_modelList.front()))
				return pCurrCluster;
		}
	}

	//no match
	return NULL;
}

/** Determine if a CPPLoopCluster contains an entry with the given name.
	@param name The name to search for.
	@param pOutInd Optional pointer to a size_t. If pOutInd is not NULL, and a match is found, the variable that 
		pOutInd points to will be set to the found entries' index in its parent's list.
	@param recurse If true, continue the search through any children CPPLoopClusters; otherwise limit the search
		to the current CPPLoopCluster.
	@return true if an entry matching name is found; false otherwise.
*/
bool CPPLoopCluster::ContainsVariableWithName(const STLString & name, size_t* pOutInd,const bool & recurse)
{
	bool ret = false;

	size_t i;
	for (i = 0; i < m_subEntries.size() && !ret; ++i)
	{
		switch (m_subEntries[i]->GetEntryType())
		{
		case CPPLCSubEntry::EVAL:
			ret = name == m_subEntries[i]->GetEvaluable()->GetID();
			break;
			//case CPPLCSubEntry::TMPASGN:
		case CPPLCSubEntry::TMPDCLR:
			ret = name == m_subEntries[i]->GetLabel();
			break;
		case CPPLCSubEntry::SUBCLST:
			if (recurse)
				ret = m_subEntries[i]->GetSubLoopCluster()->ContainsVariableWithName(name, pOutInd, recurse);
			break;
		}
	}

	if (ret && pOutInd)
		*pOutInd = i;
	return ret;
}

/** General purpose function for adding a new subEntry.
	 @param et The entry type to create.
	 @param inItr An iterator object pointing to the insertion point in the internal subentries list.
	 @param upSt The update statement to include.
	 @param lbl The name of the new entry.
	 @param ak The accumulation key to use, if any.
	 @param num The count to associate with this entry.
	 @return A pointer to the newly generated CPPLCSubEntry.
*/
CPPLCSubEntry* CPPLoopCluster::AddSubEntry(const CPPLCSubEntry::SE_TYPE & et, const CPPLCSubEntryPtrArray::iterator & inItr, const STLString & upSt, const STLString & lbl, const STLString & ak, const size_t & num)
{
	return (*m_subEntries.insert(inItr, new CPPLCSubEntry(this, et, upSt, lbl, ak, num)));

}

/** Set String for iteration limit.
	@param lim The C++ code to use for limit.
*/
void CPPLoopCluster::SetIterationLimit(const STLString & lim)
{
	m_iterLimit = lim;
}

/** @return String containing the iteration limit for an associated for loop.*/
STLString CPPLoopCluster::GetIterationLimit() const
{
	STLString ret;
	if (m_iterLimit.empty())
		ret=std::to_string(m_iterations);
	else
		ret=m_iterLimit;

	if (!m_additionalLoopLimits.empty())
		ret += " && " + m_additionalLoopLimits;
	return ret;
}

/** @return true If the generated code for the cluster will require a for loop. */
bool CPPLoopCluster::NeedsLoop() const
{
	return m_iterations != 1 || m_forceLoop;
}

/** Force a for loop to be generated regardless of the internal details of the CPPLoopCluster.
	@param fl true to force loop generation; false otherwise.
*/
void CPPLoopCluster::SetForceLoop(const bool & fl)
{
	m_forceLoop = fl;
}

/** Count the number of entries in a statically declared array.
	@param arrStr The string with the body of the array declaration.
	@return The number declared array entries.
*/
size_t CPPLoopCluster::CountArrayArgs(const STLString & arrStr)
{
	unsigned int brackLevel=0;
	unsigned int parenLevel=0;

	//assume there is at least one entry
	size_t ret = 1;
	for (const OMEChar & c : arrStr)
	{
		switch (c)
		{
		case '[':
			brackLevel++;
			break;
		case ']':
			brackLevel--;
			break;
		case '(':
			parenLevel++;
			break;
		case ')':
			parenLevel--;
			break;
		case ',':
			if (brackLevel == 0 && parenLevel == 0)
				++ret;
		}
	}
	return ret;
}

/** Append the contents of one CPPLoopCluster to the calling cluster.
	@param pClust Pointer to the CPPLoopCluster whose contents will be applied to the caller's contents.
*/
void CPPLoopCluster::AppendClusterContents(CPPLoopCluster* pClust)
{
	if (!pClust->m_subEntries.empty())
	{

		DBG_PRINTC(m_label + "<----" + pClust->m_label, DBG_RED);

		//assign new parent
		for (CPPLCSubEntry * pEntry : pClust->m_subEntries)
		{
			pEntry->SetParentLoopCluster(this);
		}
		//insert
		m_subEntries.insert(m_subEntries.end(), pClust->m_subEntries.begin(), pClust->m_subEntries.end());
		m_accums.insert(pClust->m_accums.begin(), pClust->m_accums.end());

		AppendUnique<Model*>(m_modelList, pClust->m_modelList);

		//remove old responsible
		pClust->m_subEntries.clear();

		if (m_pParentEntry)
			m_pParentEntry->FindAllDependents();
	}
}

/** 
	Prepend the contents of one cluster to another.
	@param pClust Pointer to the CPPLoopCluster that contains the contents to prepend.
*/
void CPPLoopCluster::PrependClusterContents(CPPLoopCluster* pClust)
{
	if (!pClust->m_subEntries.empty())
	{

		DBG_PRINTC(pClust->m_label + "---->" + m_label, DBG_RED);

		//assign new parent
		for (CPPLCSubEntry * pEntry : pClust->m_subEntries)
		{
			pEntry->SetParentLoopCluster(this);
		}
		//insert
		m_subEntries.insert(m_subEntries.begin(), pClust->m_subEntries.begin(), pClust->m_subEntries.end());

		AppendUnique<Model*>(m_modelList, pClust->m_modelList);

		//remove old responsible
		pClust->m_subEntries.clear();

		if (m_pParentEntry)
			m_pParentEntry->FindAllDependents();
	}
}

/** Determine if a CPPLCSubEntry needs to be evaluated after the contents of this loop.
	@param pSe Pointer to the CPPLCSubEntry to query.
	@return true if pSe can be evaluated before the caller; false otherwise.
*/
bool CPPLoopCluster::CrossesModelBound(CPPLCSubEntry* pSe)
{
	//grab all models in this cluster and children
	ModelList mdls;
	CollectAllModels(mdls);
	//Retrieve all eval entries from pClust
	
	
	EvalArray evals;
	
	for (Model* pMdl : mdls)
		pMdl->GetAllEvals(evals);
	
	std::set<STLString>& dSet = *pSe->GetDependsSetPtr();
	
	//see if any of the evals have mdls as their parents
	if (!evals.empty())
	{

		for (const STLString & dID : dSet)
		{
			for (Evaluable* pEval : evals)
			{
				if (pEval->GetID() == dID)
					return true;
			}
		}
	}

	return false;

}

/** Retrieve all represented Model objects from the caller and all sub-CPPLoopClusters.
	@param mdls The List object to populate with pointers to Model objects.
*/
void CPPLoopCluster::CollectAllModels(ModelList & mdls)
{
	mdls.insert(mdls.end(), m_modelList.begin(), m_modelList.end());
	
	for (CPPLCSubEntry* pSe : m_subEntries)
	{
		if (pSe->GetEntryType() == CPPLCSubEntry::SUBCLST)
		{
			pSe->GetSubLoopCluster()->CollectAllModels(mdls);
		}
	}
}

/** Collect a list of all Evaluable objects in the caller and sub-CPPLoopClusters.
	@param entries List to populate with pointers of Evaluables.
*/
void CPPLoopCluster::CollectAllEvals(EvalList & entries)
{
	for (CPPLCSubEntry* pSe : m_subEntries)
	{
		switch (pSe->GetEntryType())
		{
		case CPPLCSubEntry::EVAL:
			entries.push_back(pSe->GetEvaluable());
			break;
		case CPPLCSubEntry::SUBCLST:
			pSe->GetSubLoopCluster()->CollectAllEvals(entries);
			break;
		}
	}
}

/** Create a record of the reference string for a named entry.
	@param id The id to associate with the dimension string.
	@param ref The string containing the dimension information.
*/
void CPPLoopCluster::AddDimReference(const STLString & id, const STLString & ref)
{
	s_dimReferences[id] = ref;
}

/** Remove any sub-entries that are marked for cleanup.*/
void CPPLoopCluster::CleanupDeadSubEntries()
{
	
	for (size_t i = m_subEntries.size(); i > 0; --i)
	{
		size_t currInd = i - 1;
		if (m_subEntries[currInd]->GetEntryType() == CPPLCSubEntry::SUBCLST && m_subEntries[currInd]->GetSubLoopCluster()->m_markedForCleanup)
		{
			delete m_subEntries[currInd];
			m_subEntries.erase(m_subEntries.begin() + currInd);
		}
	}
}

/** Insert the appropriate increment vairable based on the placeholder pattern.
	The pattern is \@ix, where __x__ is a digit representing how far up the loop heirarchy to retrieve the increment variable from.
	@param pSe The entry to operate upon.
	@param src The string to search for the increment variable placeholder.
	@return The increment variable.
*/
STLString CPPLoopCluster::InsertUpLevelInc(CPPLCSubEntry* pSe, STLString src)
{
	const STLRegex upIndPat("@i([0-9]+)");
	STLsmatch match;

	oSTLSStream outBuff;

	//find starting cluster
	CPPLoopCluster* pBaseClust = pSe->GetParentLoopCluster();
	for (; pBaseClust && !pBaseClust->NeedsLoop(); pBaseClust = pBaseClust->GetParentCluster());

	while (std::regex_search(src, match, upIndPat))
	{
		outBuff << match.prefix();
		
		//grab count
		size_t count = OME_SCLR_READSTR(match[1].str());

		//find target cluster
		CPPLoopCluster* pTrgClust = pBaseClust;

		for (size_t i = 1; i < count; ++i)
		{
			pTrgClust = pTrgClust->GetParentCluster();
			for (; pTrgClust && !pTrgClust->NeedsLoop(); pTrgClust = pTrgClust->GetParentCluster());
		}

		outBuff << pTrgClust->m_label << "_i";
		src = match.suffix();
	}

	outBuff <<src;

	return outBuff.str();
}

/**
	Derive equation to calculate current singular index by combining all nested increment variables.
	@param pEntry Pointer to the root entry.
	@param pVarEntry Pointer to the target entry.
	@return The equation to derive the current index.
*/
STLString CPPLoopCluster::DeriveIndexOffset(CPPLCSubEntry* pEntry, CPPLCSubEntry* pVarEntry)
{
	unsigned short vCount = 0;

	CPPLoopCluster* pStartCluster = pEntry->GetParentLoopCluster();

	//assume that pVarEntry is at a depth where all dims are represented.
	std::list<STLString> vDimStrs = FindTmpDimsStr(pVarEntry);

	STLString ret;
	if (!vDimStrs.empty())
	{

		//find candidate clusters working outward
		auto vItr = vDimStrs.begin();
		size_t offsCount = 0;
		std::list<CPPLoopCluster*> pLCs;

		for (STLString & lim : vDimStrs)
		{
			for (; pStartCluster && pStartCluster->GetIterationLimit() != lim; pStartCluster = pStartCluster->GetParentCluster());
			if (!pStartCluster)
				throw OMESupportException("CPPLoopCluster::DeriveIndexOffset: Cluster mismatch");
			pLCs.push_front(pStartCluster);
			pStartCluster = pStartCluster->GetParentCluster();
		}

		//build index eqn
		if (!pLCs.empty())
		{
			ret = pLCs.front()->m_label + "_i";
			pLCs.pop_front();
			for (CPPLoopCluster* pLC : pLCs)
			{
				ret = pLC->m_label + "_i+" + pLC->GetIterationLimit() + "*(" + ret + ")";

			}
		}
	}
	return ret;
}
