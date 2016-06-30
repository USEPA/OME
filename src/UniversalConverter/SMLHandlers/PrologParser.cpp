#include "SMLHandlers/PrologParser.h"
#include "SMLHandlers/pltokens/PLParserDefines.h"
#include "ModifierProcessor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "DebugOutput.h"
#include "DrawDetails.h"
#include "ConverterUtils.h"

PrologParser::PrologParser(void)
{
	PopulateTokenMap();
}


PrologParser::~PrologParser(void)
{

	delete m_tokenReferenceMap["node"];
	delete m_tokenReferenceMap["source"];
	//delete m_tokenReferenceMap["roots"];
	delete m_tokenReferenceMap["properties"];
	delete m_tokenReferenceMap["links"];
	delete m_tokenReferenceMap["arc"];

	clearMaps();
}

/** Clears model creation maps.
	m_objectIDMap, m_linkMap, and m_submodelMap are all cleared. If fullDelete is set, all entries are also deleted.
	@param fullDelete delete all pointers before clearing the maps.
*/
void PrologParser::clearMaps(bool fullDelete)
{
	//clean up pointers for OMEObjects (should all be in objectMap)

	//delete anything in the object map that inherits OMENull.
	//these are temp objects that shouldn't be passed on from the parser
	
	//only use fullDelete if the pointers are not being passed on to anywhere else
	for(OMEIter it=m_objectIDMap.begin(); it!=m_objectIDMap.end(); it++)
	{
		if((*it).second && (fullDelete || !(*it).second->GetOMEType()))
			delete (*it).second;
	}

	//all other objects should be passed on as part of the loaded model
	//clean up references to now-dead pointers
	m_objectIDMap.clear();
	m_linkMap.clear();
	m_submodelMap.clear();
}

Model* PrologParser::ParseModelFromFile(const STLString & path)
{
	Model* newModel=NULL;

	//load file
	std::ifstream fIn(path);

	DBG_PRINT(STLString("Loading file: ") + path);

	if(fIn.good())
	{	
		DBG_PRINT("File Loaded");

		//pass file stream in for evaluation
		newModel=ParseStream(fIn);


	}
	else
	{
		DBG_COLOR(DBG_PARTIAL_PRINT("WARN: "),DBG_YELLOW);
		DBG_PRINTC("file " + path + " failed to load.", DBG_WHITE);
	}
	
	fIn.close();
	DBG_POPLEVEL();
	return newModel;
}

/** Process stream from input.
	
	@param inStrm istream drawing from body of input
	@return a full model instance if successful, NULL otherwise.
*/
Model* PrologParser::ParseStream(std::istream & inStrm)
{
		STLString currLine;

		DBG_PRINT("Parsing Data...");
		DBG_PUSHLEVEL();

			//parse lines

		StrArray lines;
			while(inStrm.good())
			{
				std::getline(inStrm,currLine);
				//some environments use '\r\n' for line termination; this breaks regex.
				//if errant '\r' found at end, replace with null/terminal char '\0'
				if(currLine.length() && currLine[currLine.length()-1]=='\r')
					currLine[currLine.length()-1]='\0';
				lines.push_back(currLine);
				
			}

	
//#pragma omp parallel for default(none) shared(lines)
			for(int i=0; i<lines.size(); i++)
			{
				ParseLine(lines[i]);
			}

		DBG_POPLEVEL();
		DBG_PRINT("Data Loaded");
		//bind references

		DBG_PRINT("Filling in Source Detail...");
		DBG_PUSHLEVEL();

			Model* newModel= new Model("root");
			newModel->SetID("root");

        /// \todo fill in any canvas-level drawing details for rootModel;
		DrawDetails* pDD=new DrawDetails(newModel);

			if(!ExtractSource(newModel))
				DBG_PRINT("Source Data Not Found");

		DBG_POPLEVEL();
		DBG_PRINT("Source Done");

		DBG_PRINT("Extracting Properties...");
		DBG_PUSHLEVEL();

			if(!ExtractProperties(newModel))
				DBG_PRINT("Properties Data Not Found");

		DBG_POPLEVEL();
		DBG_PRINT("Properties Extracted");

		DBG_PRINT("Building Links..");
		DBG_PUSHLEVEL();

			DBG_PRINT("Populate Links...");
			DBG_PUSHLEVEL();
		
				//fill in links
				STLStringList remList;
				BindLinks(remList);

			DBG_POPLEVEL();
			DBG_PRINT("Links Populated");
			
			DBG_PRINT("Assembling submodel ports...");
			DBG_PUSHLEVEL();

				BindPorts();
				FixAliasPortBinding();

				DBG_PRINT("Ports ready");
			DBG_POPLEVEL();

			DBG_PRINT("Applying VarArrays");
			ConstructVarArrays();

			DBG_PRINT("Determining Time Series");
			ConstructTimeSeries();

			DBG_PRINT("Attach functions to flows");

			for (auto itr = m_attachedMap.begin(); itr != m_attachedMap.end(); ++itr)
			{
				NullFunc* pFunc = (NullFunc*)m_objectIDMap[(*itr).second];
				pFunc->SetOutObject((*itr).first);
				m_objectIDMap[(*itr).second] = (*itr).first;
			}

			DBG_PARTIAL_PRINT("Applying Any Name Aliases");
			DBG_POPLEVEL();
			for (auto itr = m_linkMap.begin(); itr != m_linkMap.end(); ++itr)
			{
				if((*itr).second->GetLinkType()==LN_INFLUENCE)
					FixAlias((Influence*)((*itr).second));

				DBG_PARTIAL_PRINT(".");
				
			}
			remList.unique();
			DBG_PUSHLEVEL();

			//removing flagged links found so far
			RemoveMarkedIDs(remList);
		DBG_POPLEVEL();
		DBG_PRINT("Links Built");
		if(!m_assocList.empty())
		{
			DBG_PRINT("Adjusting Role/Association Syntax");
			UpdateAssocReferences();
		}

		DBG_PRINT("Populate submodels...");
		DBG_PUSHLEVEL();
		OMEMap objMap=m_objectIDMap;
			PopulateSubmodels();

		DBG_POPLEVEL();
		DBG_PRINT("Submodels Populated");
		DBG_PRINT("Populate main Model...");
		DBG_PUSHLEVEL();

			PopulateModel(newModel);

		DBG_POPLEVEL();

		
		//move links bound to ports to models based on their relationship
		DBG_PRINT("Adjust port linkages");

		AnchorPortLinks(remList);

		AnchorLinks(remList);

		AnchorAssocs();

		DBG_PRINT("Applying modifiers...");
		
				//refresh all object maps before doing upgroup stuffs.
		newModel->RefreshObjectMaps();
		std::for_each(m_submodelMap.begin(), m_submodelMap.end(),[](pair<STLString,Model*> p){ p.second->RefreshObjectMaps(); });

		OMEArray omittot;
		for (auto itr = objMap.begin(); itr != objMap.end(); ++itr)
		{
			if((*itr).second!=NULL)
			{
				OMEObject* oldPtr = (*itr).second;
				ModifierProcessor::ProcessObject((*itr).second,GetModifier((*itr).second),omittot,NULL,PLToken::ConvertExpression);
				if (oldPtr != (*itr).second)
				{
					DrawDetails::ReplaceObjectForDD(oldPtr, (*itr).second);
				}
				//if omissions occurred, future pointers may have been invalidated. set those pointers to null.
			}
		}

		m_objectIDMap=objMap;
		//removing flagged links round 2
		//RemoveMarkedIDs(remList);

		DBG_PARTIAL_PRINT("Adding Scope Syntax");
		AddScopeSyntax(m_objectIDMap);
		ApplyScopeToEnums(newModel);
		DBG_PRINT("\n");

		//DBG_PRINT("Determining UpGroups");
		//DetermineUpGroups(objMap,newModel);
		//DBG_PRINT("\n");



		//
	//clean up any omitted entries
		//std::for_each(omittot.begin(),omittot.end(),[&objMap](OMEObject* pObj){ objMap.erase(pObj->m_id); delete pObj;});
		//clearMaps(true);
		
		//finish model construction, return result
		return newModel;

}

/** Process a single line.
	Workhorse function; processes a single line, generating the necessary objects.
	@param line the prolog line to parse.
*/
void PrologParser::ParseLine(STLString line)
{
	std::smatch match;

	//grab id token
	//grab args string
	static const STLRegex idReg("^(.*?)[(](.*)[)].*$");
	std::regex_match(line,match,idReg);
	STLString id=match[1];
	STLString args=match[2];

	PLToken* currToken;
	STLString currCatalogID;

	
	if(m_tokenReferenceMap.count(id))
	{
		if((currToken=m_tokenReferenceMap[id]))
		{

			OMEObject* currObj=NULL;

			currObj=currToken->buildOMEObject(args, *this,currCatalogID);
			if(currObj)
			{
				
				currObj->SetID(currCatalogID);
					m_objectIDMap[currCatalogID]=currObj;
					if (currObj->IsOMEType(OME_ALIAS))
						m_ghostArray.push_back(dynamic_cast<EvalAlias*>(currObj));
					if(currObj->GetOMEType()==OME_NULL)
					{
						NULLTYPE nt=((OMENull*)currObj)->GetNullType();
						//catch ghosts
						/*if(nt==NULL_GHOST)
							m_ghostArray.push_back(dynamic_cast<NullGhost*>(currObj));*/
						//catch funcs
						if(nt==NULL_FUNC)
							m_funcArray.push_back((NullFunc*)currObj);
					}
					else if(currObj->GetOMEType()==OME_SUBASSOC)
						m_assocList.push_back((SubmodelAssoc*)currObj);
				
				DBG_PRINT("'"+id+":"+currObj->GetID()+"' added");
			}
			else
				DBG_PRINT("'"+id+":"+currCatalogID+"' Construction failed");
			
		}
		else
			DBG_PRINT("'"+id+":"+currCatalogID+"' not found");
	}
	
}

/** Duplicate Prolog parser fields
	@param pp the parser to copy
*/
void PrologParser::Duplicate(const PrologParser & pp)
{
	//duplicate everything parsed so far
	OMEObject *newEntry, *curr;
	STLString key;
	for (OMEIter itr = m_objectIDMap.begin(); itr != m_objectIDMap.end(); ++itr)
	{
		newEntry=NULL;
		key=(*itr).first;
		curr=(*itr).second;
		if(curr)
		{
			/// \todo update prolog parser Duplicate() function
			//ugh, there may be a better way to do this;
			//find the proper cast based on returned type flag
			switch (curr->GetOMEType())
			{
			case OME_NULL:
				switch(((OMENull*)curr)->GetNullType())
				{
				case NULL_BASE:
					newEntry=new OMENull(*((OMENull*)curr));
					break;
				case NULL_ALM:
					newEntry=new NullArcLinkMap(*((NullArcLinkMap*)curr));
					//add to ALMMap if necessary
					if(pp.m_interModelLinkMap.find(key)!=pp.m_interModelLinkMap.end())
						m_interModelLinkMap[key]=(NullArcLinkMap*)newEntry;
					break;
				case NULL_FUNC:
					newEntry=new NullFunc(*((NullFunc*)curr));
					break;
				case NULL_SOURCE:
					newEntry=new NullSource(*((NullSource*)curr));
					break;
				case NULL_PROPERTIES:
					newEntry=new NullProperties(*((NullProperties*)curr));
					break;
				};
				break;
			case OME_STATEVAR:
				newEntry=new StateVar(*((StateVar*)curr));
				break;
			case OME_VARIABLE:
				newEntry=new Variable(*((Variable*)curr));
				break;
			case OME_FLOW:
				newEntry=new Flow(*((Flow*)curr));
				if(pp.m_linkMap.find(key)!=pp.m_linkMap.end())
					m_linkMap[key]=(Link*)newEntry;
				break;
			case OME_INFLUENCE:
				newEntry=new Influence(*((Influence*)curr));
				if(pp.m_linkMap.find(key)!=pp.m_linkMap.end())
					m_linkMap[key]=(Link*)newEntry;
				break;
			case OME_MODEL:
				 newEntry=new Model(*((Model*)curr));
				if(pp.m_submodelMap.find(key)!=pp.m_submodelMap.end())
					m_submodelMap[key]=(Model*)newEntry;
				break;
			};
		
			if(newEntry)
				m_objectIDMap[key]=newEntry;
		}
	}

	//tokenMap does not need to be copied
}
	
/** Add tokenizer classes to Token map if not already initialized
*/
void PrologParser::PopulateTokenMap()
{
	if(m_tokenReferenceMap.empty())
	{
		//init top_level token map
		m_tokenReferenceMap["node"]=new PLTNode;
		m_tokenReferenceMap["source"]=new PLTSource;
		//m_tokenReferenceMap["roots"]=new PLTRoots;
		m_tokenReferenceMap["properties"]=new PLTProperties;
		m_tokenReferenceMap["links"]=new PLTLinks;
		m_tokenReferenceMap["arc"]=new PLTArc;
		//m_tokenReferenceMap["references"]=new PLTReference; //<----- IMPLEMENT?
	}
}

/** Convert any Variable object that has an array expression to a VarArray. 

*/
void PrologParser::ConstructVarArrays()
{
	NullFunc* pFunc;
	Evaluable* pEval;
	std::list<Model*> mdls;

	for (auto mItr = m_modelSubIDs.begin(); mItr != m_modelSubIDs.end(); ++mItr)
		mdls.push_back((*mItr).first);
	
	for (auto itr = m_funcArray.begin(); itr != m_funcArray.end(); ++itr)
	{
		pFunc=*itr;
		pEval=pFunc->GetOutObject();

		if(pFunc->GetIsArrayFunction() && pEval && pEval->GetOMEType()==OME_VARIABLE)
		{
			//replace variable with varArray.
			VarArray* pVa=new VarArray(*((Variable*)pEval));
			IndVec dims;
			pFunc->FindArrayDims(pFunc->GetUnitStr(),dims,mdls);
			pVa->OverrideDims(dims);
			m_objectIDMap[pVa->GetID()]=pVa;
			pFunc->SetOutObject(pVa);
			LinkRealignment(pEval,pVa);
			DrawDetails::DDForObject(pEval)->SetSrcObj(pVa);

			delete pEval;
		}
	}
}

/** Construct all Time Series objects associated with this model.*/
void PrologParser::ConstructTimeSeries()
{
	for (auto itr = m_objectIDMap.begin(); itr != m_objectIDMap.end(); ++itr)
	{
		if((*itr).second->GetOMEType()==OME_VARIABLE)
		{
			Variable* pCurr=(Variable*)(*itr).second;
			if(pCurr->GetExpr().empty() && !pCurr->GetExpectsFile())
			{
				DrawDetails* pDd=DrawDetails::DDForObject(pCurr);
				//ReplaceVariable will dealloc pCurr.
				TimeSeriesVar* pTs=TimeSeriesVar::ReplaceVariable(pCurr);
				m_objectIDMap[pTs->GetID()]=pTs;
				pDd->SetSrcObj(pTs);
				
			}
		}
	}
}

/** Extract source info (if present) and add it to model.
	@param mdl Pointer to model to modify
	@return true if source entry is found, false otherwise.
*/
bool PrologParser::ExtractSource(Model* mdl)
{
	//Populate fields from source entry, if present
	bool ret=false;
	NullSource* sourceInfo = (NullSource*)m_objectIDMap["source"];
	if(mdl && sourceInfo)
	{
		/// \todo put in non calc wrapper

		/*mdl->m_version=sourceInfo->m_version;
		mdl->m_lastModifiedDate=sourceInfo->m_lastModifiedDate;

		DBG_PRINT(STLString("Version: ")+STLString(mdl->m_version));
		DBG_PRINT(STLString("Last Modified: ")+STLString(mdl->m_lastModifiedDate));*/
		ret=true;
	}
	
	return ret;
}

/** Extract properties info (if present) and add it to model.
	@param mdl Pointer to model to modify
	@return true if properties entry is found, false otherwise.
*/
bool PrologParser::ExtractProperties(Model* mdl)
{
	//populate fields from properties, if present
	bool ret=false;
	NullProperties* props = (NullProperties*)m_objectIDMap["properties"];
	if(props)
	{
		if (!props->GetName().empty())
			mdl->SetName(props->GetName());
		//other props will be filled in here as necessary
		DBG_PRINT(STLString("Name: ")+STLString(mdl->GetName()));
		ret=true;
	}
	return ret;
}

/** Provides link objects with pointers to the named sources and targets
	@param removeList an array to populate with the ids of Links that are considered
		intermediate and need to be removed before objects are saved.
*/
void PrologParser::BindLinks(STLStringList & removeList)
{	
	Link* currLnk;
	/// \optimization this would be a good place to parallelize if there is a way to tease out Setting operations
	for(LinkIter it=m_linkMap.begin(); it!=m_linkMap.end(); it++)
	{
		currLnk=(*it).second;
		
		Evaluable* pEval=dynamic_cast<Evaluable*>(currLnk);

		if (pEval)
		{
			
			//only bind Eval-based links; SubmodelAssocs are bound elsewhere
			currLnk->SetPSource(m_objectIDMap[STLString(currLnk->GetSourceID())]);
			currLnk->SetPTarget(m_objectIDMap[STLString(currLnk->GetTargetID())]);

			
			//some links don't actually link to anything (null objects)
			//determine if we should keep or discard currLnk
			//(if VerifyRelationship returns false, any necessary merges/changes have
			// already been made)

			if (!VerifyRelationship(currLnk))
			{
				//don't remove in the loop, unless you want to break iterators
				removeList.push_back((*it).first);
			}

			//Evaluable* pEval=dynamic_cast<Evaluable*>(currLnk);
			if (pEval)
			{

				pEval->AddDownObject(currLnk->GetPEvalTarget());
				pEval->AddUpObject(currLnk->GetPEvalSource());
			}
			DBG_PRINT(STLString(currLnk->GetSourceID()) + "-->" + (*it).first + "-->" + STLString(currLnk->GetTargetID()));
		}
	}

	//ghosts should be properly configured at this point; run a verification on each ghost;
	//VerifyGhostLinks(removeList);

	//remove duplicates; will delete after mapping to submodel ports
	removeList.sort();
	removeList.unique();
}

/** Resolve Ghost aliases with their source objects.
	@param removeList On return, contains objects to be removed from the model.
*/
void PrologParser::VerifyGhostLinks(STLStringList & removeList)
{
	//for (auto itr = m_ghostArray.begin(); itr != m_ghostArray.end(); ++itr)
	//{				
	//	(*itr)->VerifyLinks(removeList);
	//}
}

/** Connect SubmodelPorts with associated Links.

*/
void PrologParser::BindPorts()
{
	Link* currFirst, *currSecond;
	NullArcLinkMap* currNALM;
	arcLinkList alList;
	arcLink currAL;
	OMEObject *inCheck,*outCheck;
	SubmodelPort *currPort, *dupPort;
	Evaluable* tempEval;

	DBG_PUSHLEVEL();
	//initial pass for mapping intermodel links through submodelPorts
	for (ALMMapIter almItr = m_interModelLinkMap.begin(); almItr != m_interModelLinkMap.end(); ++almItr)
	{
		currNALM=(*almItr).second;

		for (arcLinkIter allIter = currNALM->GetLinksBegin(); allIter != currNALM->GetLinksEnd(); ++allIter)
		{
			currAL=(*allIter);
		
			DBG_PRINT("Binding " + currAL.first + " and " + currAL.second);

			currFirst=m_linkMap[currAL.first];
			currSecond=m_linkMap[currAL.second];
			
			if (std::find(m_assocList.begin(), m_assocList.end(),currFirst)!=m_assocList.end())
			{
				//if link is in assocList, then both Links are SubmodelAssocs
				SubmodelAssoc* firstSa = (SubmodelAssoc*)currFirst;
				SubmodelAssoc* secondSa = (SubmodelAssoc*)currSecond;
				AssocPort* ap=ConvertToAssocPort(dynamic_cast<SubmodelPort*>(m_objectIDMap[secondSa->GetBaseId()]));
				if (ap)
				{
					auto spItr = find(m_subPortList.begin(), m_subPortList.end(), m_objectIDMap[secondSa->GetBaseId()]);
					m_subPortList.erase(spItr);

					//replace SubmodelPort with AssocPort
					delete m_objectIDMap[ap->GetID()];
					m_objectIDMap[ap->GetID()]=ap;
					m_assocPortMap[ap->GetID()] = ap;
					
					//ensure that assocs are pointing to port, not parent model
					firstSa->SetTargetID(ap->GetID());
					secondSa->SetSourceID(ap->GetID());

					//add port to known model
					((Model*)m_objectIDMap[currNALM->GetSubmodelID()])->AddAssocPort(ap);

					//assocPort binding will occur in AnchorAssocs()
				}
				else
				{
					DBG_PRINTC("Error creating AssocPort", DBG_RED);
				}
				continue;
			}
			//currFirst is upstream arc
			//	target will always be submodel or border
			//currSecond is downstream arc
			//	source will always be submodel or border
			//outside arc connects to submodel
			//inside arc connects to border
			//outbound influence connects to fn, which connects to visible arc

			inCheck=m_objectIDMap[currFirst->GetTargetID()];
			outCheck=m_objectIDMap[currSecond->GetSourceID()];
			if(inCheck && outCheck)
			{
				currPort=NULL;
				if(inCheck->GetOMEType()==OME_MODEL && outCheck->GetOMEType()== OME_SUBPORT)
				{
					//inbound toward submodel
					currPort=(SubmodelPort*)outCheck;
	
					currFirst->SetPTarget(currPort);
					tempEval=dynamic_cast<Evaluable*>(currFirst);

					currPort->AddOutmodelSource(tempEval);

					currSecond->SetPSource(currPort);
					tempEval=dynamic_cast<Evaluable*>(currSecond);
					currPort->AddSubmodelTarget(tempEval);
						
				}
				else if(inCheck->GetOMEType()==OME_SUBPORT && outCheck->GetOMEType()== OME_MODEL)
				{
					//outbound from submodel
					currPort=(SubmodelPort*)inCheck;

					currSecond->SetPSource(currPort);
					tempEval=dynamic_cast<Evaluable*>(currSecond);

					currPort->AddOutmodelTarget(tempEval);
					
					tempEval=dynamic_cast<Evaluable*>(currFirst);

					currPort->AddSubmodelSource(tempEval);
				}
				else if(inCheck->GetOMEType()==OME_SUBPORT && outCheck->GetOMEType()== OME_SUBPORT)
				{
					if(inCheck!=outCheck /*&& inCheck->m_x==outCheck->m_x && inCheck->m_y==outCheck->m_y*/)
					{
						//this is  a port with multiple connections. Merge the ports
						//assume only on submodel side for now

						currPort=(SubmodelPort*)inCheck;
						dupPort=(SubmodelPort*)outCheck;

						while(dupPort->DownObjectCount())
						{
							tempEval=dupPort->GetSubmodelTarget(0);
							dupPort->RemoveSubmodelTarget(tempEval);
							currPort->AddSubmodelTarget(tempEval);
						}

						while(dupPort->UpObjectCount())
						{
							tempEval=dupPort->GetSubmodelSource(0);
							dupPort->RemoveSubmodelSource(0);
							currPort->AddSubmodelSource(tempEval);
						}

						//remove dup after merge
						m_objectIDMap.erase(dupPort->GetID());
						m_subPortList.remove(dupPort);
						LinkRealignment(dupPort,currPort);
						delete dupPort;
					}
				}
				else
					DBG_PRINT(STLString("Port mismatch: ")+inCheck->GetID()+"-->"+outCheck->GetID());

			}
			else
			{
				DBG_PRINT(STLString("Port miss: ")+STLString(currFirst->GetTargetID())+"-->"+STLString(currSecond->GetSourceID()));
			}
		}
	}
	DBG_POPLEVEL();
}

/** Make sure that EvalAliases are connected to a SubmodelPort's source, not the port itself.*/
void PrologParser::FixAliasPortBinding()
{
	for (EvalAlias* pEa : m_ghostArray)
	{
		if (pEa->GetSrc() == NULL)
		{
			Influence* pInfl=NULL;
			for (Evaluable* pUp: pEa->GetUpObjects())
			{
				pInfl = (Influence*)pUp;
				if (pInfl->GetPEvalSource())
					break;
			}
			if (pInfl)
				pEa->SetSrc(pInfl->GetPEvalSource());
		}
		if (pEa->GetSrc()->IsOMEType(OME_SUBPORT) || pEa->GetSrc()->IsOMEType(OME_INFLUENCE))
		{
			SubmodelPort* pPort = dynamic_cast<SubmodelPort*>(pEa->GetSrc());
			Evaluable* pNewSrc = pPort->GetUpObject(0);
			while (pNewSrc->GetOMEType() == OME_INFLUENCE || pNewSrc->GetOMEType() == OME_SUBPORT)
				pNewSrc = pNewSrc->GetUpObject(0);
			pEa->SetSrc(pNewSrc);
			pEa->SetName(pNewSrc->GetName());
		}
	}
}

/** Move named objects to their respective submodels.
*/
void PrologParser::PopulateSubmodels()
{
	//populate submodels
	//NOTE: this will REMOVE OMEObjects from the main list
	OMEObject* subObj;
	Model* currSub;
	STLString tempID;

	for(ModelIter oit=m_submodelMap.begin(); oit!=m_submodelMap.end(); oit++)
	{
		currSub=(Model*)(*oit).second;

		subObj=NULL;

		DBG_PRINT(STLString("Populating Submodel: ")+STLString(currSub->GetName()));
		DBG_PUSHLEVEL();

		for(int i=0; i<m_modelSubIDs[currSub].size();i++)
		{
			tempID=STLString(m_modelSubIDs[currSub][i]);
			subObj=m_objectIDMap[tempID];

			//if spawners are present, then there are no default instances
			if(subObj->GetOMEType()==OME_SPAWNER && !((Spawner*)subObj)->IsConditional())
				currSub->SetInitNumInstances(0);

			//for some reason, null objs are creeping in. don't have time to investigate why; just block here
			if(subObj->GetOMEType()!=OME_NULL)
			currSub->AddObject(subObj);

	
			if (!subObj->IsOMEType(OME_ALIAS) && subObj->IsOMEType(OME_EVAL) && ((Evaluable*)subObj)->HasTable())
				currSub->AddEvalTable(((Evaluable*)subObj)->GetPEvalTable(0));
			
			//m_objectIDMap.erase(tempID);
			DBG_PRINT("Adding object "+tempID);
		}
		
		DBG_POPLEVEL();
	}

}

/** Takes any remaining objects in the objectMap and adds them to the provided model.
	@param mdl pointer to the model that will be the new home for any orphaned objects.
*/
void PrologParser::PopulateModel(Model* mdl)
{
	//map remaining nonlink items to root model
	OMETYPE type;
	OMEObject* subObj;
	for(OMEIter obit=m_objectIDMap.begin(); obit!=m_objectIDMap.end(); obit++)
	{
		subObj=(*obit).second;

		if(subObj && !subObj->GetParentModel())
		{
			type = subObj->GetOMEType();
			if (type != OME_SUBASSOC && type != OME_INFLUENCE && type != OME_FLOW && type != OME_NULL)
			{
				mdl->AddObject(subObj);
				if (!subObj->IsOMEType(OME_ALIAS) && subObj->IsOMEType(OME_EVAL) && ((Evaluable*)subObj)->HasTable())
					mdl->AddEvalTable(((Evaluable*)subObj)->GetPEvalTable(0));
				/// \todo add wiring step for submodel ports if they end up in root level.
			}
		}
	}
}

/** Assign Links connected to Submodel Ports to the appropriate model.
	Arc objects in Simile are not specified as belonging to any specific models, 
	so their parent models need to be inferred by their connections to Nodes.
*/
void PrologParser::AnchorPortLinks(STLStringList & removeList)
{
	SubmodelPort *currPort;
	//StrArray tempIDs;
	EvalArray tempObjs;
	Evaluable *in, *out;
	Model* pOuterModel;
	unsigned int i;
	if(m_submodelMap.size())
	{

		//adjust ports; move associated arcs to the appropriate models
		for (SubPortIter itr = m_subPortList.begin(); itr != m_subPortList.end(); ++itr)
		{
			currPort=(*itr);
			in=NULL;
			out=NULL;

			//sync expr
			switch(currPort->UpdateDirection())
			{
			case PD_INBOUND:
				in=currPort->GetOutmodelSource(0);
				out=currPort->GetSubmodelTarget(0);
				break;
			case PD_OUTBOUND:
				in=currPort->GetSubmodelSource(0);
				out=currPort->GetOutmodelTarget(0);
				break;
			default:
				
				if(currPort->GetSubSourceCount() && currPort->DownObjectCount())
				{
					for(i=0; i<currPort->DownObjectCount(); i++)
						currPort->AddOutmodelTarget(currPort->GetDownObject(i));
				}
				else if(currPort->GetSubTargetCount() && currPort->UpObjectCount())
				{
					for(i=0; i<currPort->UpObjectCount(); i++)
						currPort->AddOutmodelSource(currPort->GetUpObject(i));
				}
				else if(currPort->GetOutSourceCount() && currPort->DownObjectCount())
				{
					for(i=0; i<currPort->DownObjectCount(); i++)
						currPort->AddSubmodelTarget(currPort->GetDownObject(i));
				}
				else if(currPort->GetOutTargetCount() && currPort->UpObjectCount())
				{
					for(i=0; i<currPort->UpObjectCount(); i++)
						currPort->AddSubmodelSource(currPort->GetUpObject(i));
				}


				if(!currPort->UpdateDirection())
				{
					//likely left over from the removal of a ghost; delete
					DBG_PRINT(currPort->GetName()+" is directionless");
					currPort->GetParentModel()->RemoveSubPort(currPort);
					currPort->Detach();
				
					tempObjs.clear();
					currPort->FindNextDownObjects(tempObjs,false);
					currPort->FindNextUpObjects(tempObjs,false);

					//remove any straggling links from the linkMap
					for(i=0;i<tempObjs.size();i++)
					{
						if(tempObjs[i]->IsOMEType(OME_INFLUENCE))
							m_linkMap.erase(tempObjs[i]->GetID());
					}
				}
				else
				{
					--itr; //redo iter
					continue;
				}
			};

			//in the case of models loaded from Simile, port super models belong to the immediate parent of their current model
			currPort->SetOuterModel(currPort->GetParentModel()->GetParentModel());

			tempObjs.clear();
			//tempIDs.clear();
			//currPort->GetOutIDs(tempIDs);
			currPort->GetOutObjects(tempObjs);
			pOuterModel=currPort->GetOuterModel();
			for(i=0; i< tempObjs.size(); i++)
			{
				//some influences connect ports of two submodels, so they are on the outside of both ports.
				//prevent redundant entry.
				if(std::find(removeList.begin(),removeList.end(), tempObjs[i]->GetID())==removeList.end()
					&& tempObjs[i]->GetParentModel()!=pOuterModel /*&& tempObjs[i]->GetOMEType()!=OME_FLOW*/)
				{
					removeList.push_back(tempObjs[i]->GetID());
					pOuterModel->AddObject(tempObjs[i]);
				}
			}
			tempObjs.clear();
			currPort->GetSubObjects(tempObjs);

			for(i=0; i< tempObjs.size(); i++)
			{
				if (std::find(removeList.begin(), removeList.end(), tempObjs[i]->GetID()) == removeList.end()
					/*&& tempObjs[i]->GetOMEType() != OME_FLOW*/)
				{
					removeList.push_back(tempObjs[i]->GetID());
					currPort->GetParentModel()->AddObject(tempObjs[i]);
				}
			}

		/*	if(in && out)
				out->m_expr=in->m_expr;*/

			//flows leaving a port should just reference the port itself; the inbound flow
			//will carry the equation and the port will do any aggregation if necessary.
			if(out && out->GetOMEType()==OME_FLOW)
				out->SetExpr(currPort->GetExprName());
		}
	}
}

/** Anchor link to a specific model.
	@param pLinkObj Pointer to the Link object to anchor.
*/
void PrologParser::AnchorLinkObject(Link* pLinkObj)
{
	OMEObject* tempNode;
	OMEObject *tempSrc, *tempTrg;
	Evaluable* pEvalObj;
	pEvalObj=dynamic_cast<Evaluable*>(pLinkObj);
	
	if(pLinkObj && pEvalObj && !pEvalObj->GetParentModel())
	{
		tempSrc=pLinkObj->GetPOMESource();
		tempTrg=pLinkObj->GetPOMETarget();
		
		//find node to use for anchoring
		//TODO put in check here for dead port; remove link if dead port is found
		if (tempSrc && tempTrg && tempSrc->GetParentModel() != tempTrg->GetParentModel())
		{
			//flow must be anchored first
			if (tempSrc->IsOMEType(OME_FLOW))
				AnchorLinkObject((Flow*)tempSrc);
			tempNode = tempSrc->GetParentModel()->ContainsSubmodel(tempTrg->GetParentModel()) ? tempSrc : tempTrg;
		}
		else if(!tempSrc)
			tempNode=tempTrg;
		else
			tempNode=tempSrc;

		
		//if node used for anchoring is another link, anchor it first if necessary
		if (!tempNode->GetParentModel())
		{
			Link* tempLink=dynamic_cast<Link*>(tempNode);
			if(tempLink)
				AnchorLinkObject(tempLink);
		}

		//add link to tempNode's parent model and remove from master map.
		if(tempNode && tempNode->GetParentModel())
		{

			if(pLinkObj->GetLinkType()!=LN_INFLUENCE || !pLinkObj->GetPOMETarget()->IsOMEType(OME_NULL))
				tempNode->GetParentModel()->AddObject(pEvalObj);
			m_objectIDMap.erase(pEvalObj->GetID());
		}
	}
}

/** Assign Links to available models.
	Arc objects in Simile are not specified as belonging to any specific models, 
	so their parent models need to be inferred by their connections to Nodes.
*/
void PrologParser::AnchorLinks(STLStringList & removeList)
{
	//submodel list only hold nodes; need to assign arcs based on their connection's parents


	Link* tempLink;

	for(LinkIter it=m_linkMap.begin(); it!=m_linkMap.end(); it++)
	{
		if(std::find(removeList.begin(),removeList.end(),(*it).first)==removeList.end())
		{
			tempLink=(*it).second;
		
			AnchorLinkObject(tempLink);
		}
	}

}


/** Anchor any submodelAssoc that has a Model at either end.
*/
void PrologParser::AnchorAssocs()
{
	OMEObject* pBase;
	//find each assoc that starts at a model, and walk the chain.
	for (auto itr = m_assocList.begin(); itr != m_assocList.end(); ++itr)
	{
		pBase = m_objectIDMap[(*itr)->GetBaseId()];
		if (pBase->GetOMEType()==OME_MODEL)
		{
			OMEObject *pSub = NULL;
			if (m_objectIDMap.find((*itr)->GetSubId()) != m_objectIDMap.end())
				pSub= m_objectIDMap[(*itr)->GetSubId()];
			
			WalkAnchorAssoc((*itr),pBase,pSub);
		}
	}

}

//assumes 1-to-1 mapping of assocs on either side of port
/** Walk and anchor SubmodelAssocs to the appropriate submodels.
	@param pAssoc Pointer to the current SubmodelAssoc to anchro.
	@param pBase Pointer to SubmodelAssoc base object.
	@param pSub Pointer to SubmodelAssoc sub object.
*/
void PrologParser::WalkAnchorAssoc(SubmodelAssoc* pAssoc,OMEObject* pBase, OMEObject* pSub)
{
	//bind ends of assoc to current assoc.
	pAssoc->BindSubmodels(pBase, pSub);

	//model binding also adds the Assoc to the appropriate model

	AssocPort* pAp;
	//if assoc starts at model, anchor assoc to parent model
	if (pBase->GetOMEType() == OME_ASSOCPORT)
	{
		//if assoc starts at port, bind to port and anchor to submodel base on port's direction

		pAp = (AssocPort*)pBase;
		pAp->SetOuterModel(pAp->GetParentModel()->GetParentModel());
		//direction should be updated from previous step
		if (pAp->GetDirection() == PD_INBOUND)
		{
			pAp->AddSubOutAssoc(pAssoc, true);
			pAp->GetParentModel()->AddSubMCAssoc(pAssoc);
		}
		else
		{
			pAp->AddOutOutAssoc(pAssoc, true);
			pAp->GetOuterModel()->AddSubMCAssoc(pAssoc);
		}
	}

	if (pSub->GetOMEType() == OME_ASSOCPORT)
	{
		//if assoc ends at port, bind to port depending on relative parents
		pAp = (AssocPort*)pSub;
		if (pAp->GetParentModel() == pAssoc->GetParentModel())
			pAp->AddSubInAssoc(pAssoc, true);
		else
			pAp->AddOutInAssoc(pAssoc, true);

		//update direction of port
		pAp->UpdateDirection();

		//find next role that has current end port as start. Recursively walk it.
		SubmodelAssoc* pNext = *std::find_if(m_assocList.begin(), m_assocList.end(), [&pSub, &pAssoc](SubmodelAssoc* pSa){ return pSa != pAssoc && pSub->GetID() == pSa->GetBaseId(); });
		OMEObject* pNextObj = NULL;
		
		if (m_objectIDMap.find(pNext->GetSubId()) != m_objectIDMap.end())
			pNextObj = m_objectIDMap[pNext->GetSubId()];

		//recurse
		WalkAnchorAssoc(pNext, pSub, pNextObj);
	}
}

/** Replace all occurrances of one object with another across all Links in the linkmap
	@param oldObj the object to replace.
	@param newObj the object to use as a replacement.
*/
void PrologParser::LinkRealignment(OMEObject* oldObj, OMEObject* newObj)
{
	//brute force approach to fixing links after an object is replaced with another
	Link* currLink;
	std::list<std::pair<STLString,Link*> > rmList;
	for(LinkIter it=m_linkMap.begin(); it!=m_linkMap.end(); it++)
	{
		currLink=(*it).second;
		
		if(currLink->GetPOMESource()==oldObj)
			currLink->SetPSource(newObj);
		else if(currLink->GetPOMETarget()==oldObj)
			currLink->SetPTarget(newObj);

		//if source and target are now the same, the link is now obsolete and can be removed
		if(currLink->GetPOMESource()==currLink->GetPOMETarget())
			rmList.push_back((*it));
	}

	for (auto rmItr = rmList.begin(); rmItr != rmList.end(); ++rmItr)
	{
		STLString id=(*rmItr).first;
		m_linkMap.erase(id);
		Evaluable* pObj=dynamic_cast<Evaluable*>((*rmItr).second);
		if(pObj)
		{
			m_objectIDMap.erase(id);
			//remove links from submodel ports if necessary
			SubmodelPort* pPort;
			if((pPort=dynamic_cast<SubmodelPort*>((*rmItr).second->GetPEvalSource())))
				pPort->RemoveEval(pObj);
			if((pPort=dynamic_cast<SubmodelPort*>((*rmItr).second->GetPEvalTarget())))
				pPort->RemoveEval(pObj);
		}

	}
}

/** Checks an influence for an alias for a source's name, and 
	replaces any found with the proper name in the target's expression.
	@param pInfl the Influence to check.
*/
void PrologParser::FixAlias(Influence* pInfl)
{
	using namespace std;

	const static regex aliasReg(".*?,.*?,(?:usr\\()?\\[*'?(.*?)'?\\]*\\)?,.*?");
	smatch aliasMatch;
	string aliasStr, roleStr = m_roleMap[pInfl->GetID()];
	regex_match(roleStr,aliasMatch,aliasReg);
	aliasStr=aliasMatch[1];


	OMEObject* src=pInfl->GetPOMESource();

	while(src && src->GetOMEType()==OME_INFLUENCE)
		src=((Influence*)src)->GetPOMESource();

	OMEObject* trg=pInfl;
	while(trg && trg->GetOMEType()==OME_INFLUENCE)
		trg=((Influence*)trg)->GetPOMETarget();

	OMEArray visited; 
	SubmodelPort* pPort=NULL;
	while(src && std::find(visited.begin(),visited.end(),src)==visited.end() && src->GetOMEType()==OME_SUBPORT)
	{
		pPort=dynamic_cast<SubmodelPort*>(src);
		visited.push_back(src);
		src=pPort->GetOutmodelSource(0);
		if(!src)
			src=pPort->GetSubmodelSource(0);
		if(!src)
		{
			pPort=dynamic_cast<SubmodelPort*>(pInfl->GetPEvalSource());
			StrArray srcIDs=pPort->GetSubSourceIDs();
			AppendUnique<STLString>(srcIDs,pPort->GetOutSourceIDs());

			if(srcIDs.size())
			{
				//find first link that doesn't result in loop
				for(size_t i=0; i<srcIDs.size() && (i==0 || std::find(visited.begin(),visited.end(),src)!=visited.end()); i++)
					src=dynamic_cast<Evaluable*>(m_objectIDMap[srcIDs[i]]);		
			}
		}
		Link* currLnk=dynamic_cast<Link*>(src);
		if(currLnk)
			src=currLnk->GetPEvalSource();

		
	}

	if(src && src->GetOMEType()!=OME_SUBPORT)
	{

		string nameStr=src->GetName();
		string nameEStr = src->GetExprName();
		
		//check for illegal name
		//if no alias already exists, set original name to alias
		if(aliasStr.empty())
			aliasStr=nameEStr;

		//if name was scrubbed update object name;
		//downstream corrections will be handled in the same fashion as an alias
		src->SetName(nameStr);
	
		if(nameStr!=aliasStr)
		{
			STLString scrubstr;
			scrubstr.reserve(aliasStr.length());
			for(int i=0; i<aliasStr.length(); i++)
			{
				if(aliasStr[i]!='{' && aliasStr[i]!='}' && aliasStr[i]!=']' && aliasStr[i]!='[' && aliasStr[i]!='\'' && aliasStr[i]!='"')
					scrubstr.push_back(aliasStr[i]);
					//aliasStr.insert(i,"\\");
			}
			regex replaceReg("[\\{\\]']*\\b"+scrubstr+"\\b[\\[\\{']*");
            std::list<OMEObject*> exprVisited;
			ApplyExprMod(replaceReg,pInfl,nameEStr,exprVisited);
		}
		//if(trg && nameStr!=aliasStr)
		//{
		//	for(int i=aliasStr.length()-1; i>=0; i--)
		//	{
		//		if(aliasStr[i]=='{' || aliasStr[i]=='}' || aliasStr[i]==']' || aliasStr[i]=='[')
		//			aliasStr.insert(i,"\\");
		//	}

		//	if(!aliasStr.empty())
		//	{
		//		regex replaceReg("\\b"+aliasStr+"\\b");
		//	
		//		NullFunc* pNf;
		//		if(pNf=dynamic_cast<NullFunc*>(trg))
		//		{
		//			STLString exprStr(pNf->GetExpr());
		//			pNf->SetExpr(regex_replace(exprStr,replaceReg,nameStr));
		//		}
		//		else
		//		{
		//			EvalArray downEvals;
		//			Evaluable* evalTrg;

		//			//Null OMEObjects have not been resolved yet, so we cannot yet assume that 
		//			//variable names are only referenced one link down yet;
		//			pInfl->FindAllDownObjects(downEvals);
		//			//pInfl->FindNextDownObjects(downEvals);

		//			for(int i=0; i<downEvals.size(); i++)
		//			{
		//				evalTrg=downEvals[i];
		//				string exprStr(evalTrg->m_expr);
		//				evalTrg->m_expr=regex_replace(exprStr,replaceReg,nameStr);
		//			}
		//		}
		//	}
		//}
	}

	//scrub any names of terminal evaluable objects in heirarchy
	//if(trg && trg->GetOMEType()!=OME_SUBPORT && trg->IsOMEType(OME_EVAL))
	//{
		//scrub may happen repreatedly, but shouldn't be harmful. if it becomes an issue, a check can be added

		//Evaluable* pEval=dynamic_cast<Evaluable*>(trg);
		
	//}
}

/** Loop through each SubmodelAssoc and update any relevant references.
*/
void PrologParser::UpdateAssocReferences()
{
	SubmodelAssoc* pSa;
	StrList objNames;
	STLString exprName;
	for (auto itr = m_objectIDMap.begin(); itr != m_objectIDMap.end(); ++itr)
		objNames.push_back((*itr).second->GetExprName());

	for (auto itr = m_assocList.begin(); itr != m_assocList.end(); ++itr)
	{
		pSa=(*itr);

		try 
		{
			exprName = pSa->GetExprName();
			ReplaceAssocSyntaxForModel(exprName, objNames);
			ReplaceAssocSyntaxForModel(exprName, objNames);
		}
		catch (const std::out_of_range& oor)
		{
			DBG_PRINT(STLString("ERR IN UpdateAssocReferences(): ")+oor.what());
		}
	}
}

/** Loops through all evaluables and replaces any references to the named
	Role using the underline syntex with that using the dot syntax (there
	is likely a better way to accomplish this).
	@param aName name of the SubmodelAssoc to look for.
	@param objNames The name of all found objects.
*/
void PrologParser::ReplaceAssocSyntaxForModel(STLString aName,const StrList & objNames)
{
	STLRegex matchPreReg("\\b"+aName+"_(\\w*)\\b");
	STLRegex matchPostReg("\\b(\\w*)_"+aName+"\\b");
	
	STLString replacement(aName+".$1");
	STLString expr;

	Evaluable* eval;
	bool usePost;
	/// \optimization possible candidate loop for multithreading.
	for (auto itr = m_objectIDMap.begin(); itr != m_objectIDMap.end(); ++itr)
	{
		usePost=false;
		eval=dynamic_cast<Evaluable*>((*itr).second);
		if(eval)
		{
			STLsmatch match;
			
			expr=eval->GetExpr();
			if(!std::regex_search(expr,match,matchPreReg))
				usePost=std::regex_search(expr,match,matchPostReg);

			if(!match.empty() && find(objNames.begin(), objNames.end(),match[0].str())==objNames.end())
				eval->SetExpr(std::regex_replace(expr,(usePost ? matchPostReg : matchPreReg),replacement));
			
		}
	}
	
		
}

void PrologParser::AddScopeSyntax(const OMEMap & objMap)
{
	Evaluable* pE;

	/// \optimization potential multithread spot
	for (auto itr = objMap.begin(); itr != objMap.end(); ++itr)
	{

		pE=dynamic_cast<Evaluable*>((*itr).second);
		
		if(pE)
		{
			DBG_PARTIAL_PRINT(".");
			ApplyScope(pE);
		
		}
		
	}
	DBG_PRINT("\n");
}

/** Remove any specified objects from internal catalogs.
	@param remList List of IDs of OMEObjects to remove from internal catalogs.
*/
void PrologParser::RemoveMarkedIDs(STLStringList & remList)
{
	for (STLStringListIter sit = remList.begin(); sit != remList.end(); sit++)
	{

		STLString remKey = (*sit);
		Evaluable* pEval = dynamic_cast<Evaluable*>(m_objectIDMap[remKey]);
		if (pEval)
		{
			pEval->Detach();
			if (pEval->GetParentModel())
				pEval->GetParentModel()->RemoveObject(pEval);
		}
		delete m_objectIDMap[remKey];
		m_objectIDMap.erase(remKey);
		m_linkMap.erase(remKey);
	}
	remList.clear();
}

/** Convert a loaded SubmodelPort to an AssocPort.
	@param pPort Pointer to SubmodelPort to be used as template for new AssocPort.
	@return Pointer to newly constructed AssocPort.
*/
AssocPort* PrologParser::ConvertToAssocPort(SubmodelPort* pPort)
{
	AssocPort* ret = NULL;
	if (pPort)
	{
		ret = new AssocPort();
		ret->SetName(pPort->GetName());
		ret->SetID(pPort->GetID());
		//grab ids
		ret->MultiPort::operator=(*pPort);
	}
	return ret;
}

/** Apply scoping path to all ther expressions that reference pE.
	@param pE Pointer to the Evaluable object that is the target of the search query.
*/
void PrologParser::ApplyScope(Evaluable* pE)
{
	StrList pathParts;
	oSTLSStream buff;
	static std::list<OMEObject*> visited;
	if(pE->GetParentModel())
		((Model*)pE->GetParentModel())->GetUpPath(pathParts);
	else
		DBG_PRINT(STLString("Orphaned object: ")+pE->ToString());


	//for(auto itr=pathParts.begin(); itr!=pathParts.end(); ++itr)
	//	buff<<*itr<<'.';
	STLString evalName=pE->GetExprName();
	pathParts.push_back(evalName);
	//buff<<evalName;
	STLString path=ConcatStrings(pathParts.begin(),pathParts.end(),'.');//buff.str();

	if(!path.empty())
	{
		STLRegex varReg("((?:[-+/*^[:s:]<>=,({]|^)[[]*)("+evalName+")([\\]]*)\\b");
		//pE->FindTieredDownObjects(downObjects);
		////pE->FindNextDownObjects(downObjects);

		//orig="";
		//go=true;
		//for(auto elItr=downObjects.begin(); elItr!=downObjects.end() && go;++elItr)
		//{
		//	for(auto dItr=(*elItr).begin(); dItr!=(*elItr).end(); ++dItr)
		//	{
		//		orig=(*dItr)->m_expr;
		//		(*dItr)->m_expr=std::regex_replace((*dItr)->m_expr,varReg,"$1"+path);
		//		go=orig==(*dItr)->m_expr;
		//	}
		//}

		STLString rep="$1"+path+"$3";
		visited.clear();

		for (int i = 0; i < pE->DownObjectCount(); ++i)
			if (pE->GetDownObject(i)->GetOMEType()==OME_INFLUENCE)
				ApplyExprMod(varReg, (Influence*)pE->GetDownObject(i), rep, visited);
	}
	//downObjects.clear();
}

/** Apply modified expression to an object.
	@param varReg Regular expression used to identify sequences to replace.
	@param pInfl Pointer to the Influence to modify.
	@param pathRep Full model path to current object.
	@param visited List used to track previously visited objects.
*/
void PrologParser::ApplyExprMod(const STLRegex & varReg,Influence* pInfl, const STLString & pathRep, std::list<OMEObject*> & visited)
{
	//avoid loop

	Evaluable* pEval;
	
	pEval = pInfl->GetPEvalTarget();
	
		switch (pEval->GetOMEType())
		{
		case OME_VARIABLE:
		case OME_VARARRAY:
		case OME_FLOW:
		case OME_STATEVAR:
			pEval->SetExpr(std::regex_replace(pEval->GetExpr(), varReg, pathRep));
			break;
		case OME_ALIAS:
			//todo verify that this block of code works
			visited.push_back(pInfl);
			visited.push_back(pEval);
			for (int i = 0; i<pEval->GetDownObjects().size(); i++)
				ApplyExprMod(varReg, (Influence*)pEval->GetDownObject(i), pathRep, visited);
			break;
		default:
			visited.push_back(pInfl);
			visited.push_back(pEval);
			for (int i = 0; i < pEval->DownObjectCount(); i++)
			{
				if (pEval->GetDownObject(i)->GetOMEType() == OME_INFLUENCE)
				{
					ApplyExprMod(varReg, (Influence*)pEval->GetDownObject(i), pathRep, visited);
					visited.push_back(pEval->GetDownObject(i));
				}
			}
		}
	
	//bool first=visited.empty();
	//if(std::find(visited.begin(),visited.end(),pObj)==visited.end())
	//{
	//	visited.push_back(pObj);
	//	Evaluable* pEval=dynamic_cast<Evaluable*>(pObj);
	//	Link* pLnk;
	//	NullGhost* pGhst;
	//	STLString orig;
	//	if(pEval)
	//	{
	//		orig=pEval->GetExpr();
	//		//avoid self-reference
	//		if (!first)
	//			pEval->SetExpr(std::regex_replace(pEval->GetExpr(),varReg,pathRep));
	//		if(orig==pEval->GetExpr() || first) //keep going until replacement is found
	//		{
	//			if(pEval->GetOMEType()==OME_INFLUENCE)
	//				ApplyExprMod(varReg,((Influence*)pEval)->GetPOMETarget(),pathRep,visited);
	//			else
	//			{
	//				switch(pEval->GetOMEType())
	//				{
	//				case OME_VARIABLE:
	//				case OME_VARARRAY:
	//				case OME_FLOW:
	//				case OME_STATEVAR:
	//					if(!first)
	//						break;
	//				default:
	//					for(int i=0; i<pEval->DownObjectCount(); i++)
	//						ApplyExprMod(varReg,pEval->GetDownObject(i),pathRep,visited);
	//				}
	//			}
	//		}
	//	}
	//	else if(pGhst=dynamic_cast<NullGhost*>(pObj))
	//	{
	//		for(int i=0; i<pGhst->GetOutCount(); i++)
	//			ApplyExprMod(varReg,pGhst->GetPOMEOut(i),pathRep,visited);	
	//	}
	//}

}

/** Consolidate upgroup statements into a single upgroup call.
		@param objMap Map of all target objects.
		@param pRootMdl Pointer to root Model object.
*/
void PrologParser::DetermineUpGroups(const OMEMap & objMap,Model* pRootMdl)
{
	static const STLRegex fReg("^.*?upgroup\\(\\s*(.*?)\\s*,(@)\\)");
	STLsmatch res;
	STLString expr;
	STLString subExpr;
	Evaluable* pOther;
	Model* pCurrMdl,*pTrgMdl,*pEvalMdl;
	oSTLSStream tempBuff;
	size_t dist;
	size_t offset;
	size_t i;

		Evaluable* pE;

		for (auto itr = objMap.begin(); itr != objMap.end(); ++itr)
	{
		pE=dynamic_cast<Evaluable*>((*itr).second);

		if(pE && !pE->GetExpr().empty())
		{

			subExpr=pE->GetExpr();
			expr=STLString();
			while(std::regex_search(subExpr,res,fReg))
			{
				//find referenced object in upgroup using full model path.
				pOther=dynamic_cast<Evaluable*>(pRootMdl->NestedObjectForName(res[1]));
				if(pOther)
				{
					if(	pOther->GetParentModel())
					{
						//upgroup should only apply if pOther is super to pE; find how many models away.
						pCurrMdl=NULL;
						pTrgMdl=pOther->GetParentModel();
						pEvalMdl=pE->GetParentModel();
						offset=0;
						while(pTrgMdl && !pCurrMdl && pTrgMdl!=pEvalMdl)
						{
							pCurrMdl=pEvalMdl;
							//for(dist=offset;pCurrMdl!=NULL && pCurrMdl!=pTrgMdl;dist++)
							while(pCurrMdl!=NULL && pCurrMdl!=pTrgMdl)
								pCurrMdl=pCurrMdl->GetParentModel();

							pTrgMdl=pTrgMdl->GetParentModel();
							//if (!pCurrMdl)
								offset++;
						}

						if(pTrgMdl)
						{
							//find size, translate to string, make space in expr, and resize.
							tempBuff.clear();
							tempBuff.str(STLString());
							tempBuff<<offset;
							if(tempBuff.str().size()==1)
								subExpr[res.position(2)]=tempBuff.str()[0];
							else
							{
								i=expr.size()-1;
								dist=tempBuff.str().size()-1;
								subExpr.resize(expr.size()+tempBuff.str().size()-1);
								for(;i!=res.position(2);i--)
									subExpr[i+dist]=expr[i];
								for(size_t j=0;j<tempBuff.str().size();j++)
									subExpr[i+j]=tempBuff.str()[j];
							}

							/// \todo if @==0, remove specific upgroup call altogether.
						}
						else
						{
							//DBG_PRINTC(STLString("Objects ")+pE->GetName()+" and "+pOther->GetName()+" do not require an upgroup.",DBG_YELLOW);
							subExpr[res.position(2)]='0';
						}
					}
				}
				else
					DBG_PRINTC(STLString("Object not found at '")+res[1].str()+"' Was it omitted?",DBG_YELLOW);
				expr+=res[0].str();
				subExpr=res.suffix().str();
			}
			//add unmatched tail to expr
			expr+=subExpr;

			pE->SetExpr(expr);
		}
	}
}

/** Apply path scoping to enumerated value references.
	@param pMdl The source Model to evaluate.
*/
void PrologParser::ApplyScopeToEnums(Model* pMdl)
{
	EvalArray allEvals;
	if (pMdl->GetModelEnumCount())
	{
		pMdl->GetAllEvals(allEvals, DT_FULL);
		STLString upPath;
		pMdl->GetUpPath(upPath);
		STLRegex repReg;

		for (size_t i = 0; i < pMdl->GetModelEnumCount(); ++i)
		{
			//first check for enum type
			ModelEnum& currExprSet = pMdl->GetModelEnum(i);
			STLString enumPath = upPath + ":" + ScrubName(currExprSet.GetEnumClassName());
			
			repReg = STLRegex("\"\\s*" + currExprSet.GetEnumClassName() + "\\s*\"");
			std::for_each(allEvals.begin(), allEvals.end(), [&repReg, &enumPath](Evaluable* pE){ pE->SetExpr(std::regex_replace(pE->GetExpr(), repReg, enumPath)); });
			//then check for each val;
			for (size_t j = 0; j < currExprSet.GetEnumValueCount(); ++j)
			{
				STLString eValPath = enumPath + "." + ScrubName(currExprSet.GetEnumValue(j));
				repReg = STLRegex("\"\\s*" + currExprSet.GetEnumValue(j) + "\\s*\"");
				std::for_each(allEvals.begin(), allEvals.end(), [&repReg, &eValPath](Evaluable* pE){ pE->SetExpr(std::regex_replace(pE->GetExpr(), repReg, eValPath)); });
			}
		}
		
	}
	for (size_t i = 0; i < pMdl->GetSubmodelCount(); ++i)
		ApplyScopeToEnums(pMdl->GetSubmodel(i));
}
