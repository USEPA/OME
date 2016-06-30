#include "InitializerSet.h"
#include "DebugOutput.h"
#include "omeobjects/SimObjs.h"

/** Default Constructor */
InitializerSet::InitializerSet()
	:m_isPopulated(false)
{}

/** Simple Constructor 
	@param filepath Path to file to be used to populate the InitializerSet.
*/
InitializerSet::InitializerSet(const STLString filepath)
{
	LoadFromFile(filepath);
}

/** Populate InitializerSet from a given parameter file. Currently, only Simile's .spf files are supported.
	@param filepath Path to file to use for population.
	@return true if population process is successful; false otherwise.

*/
bool InitializerSet::LoadFromFile(const STLString filepath)
{
	STLString ext=filepath.substr(filepath.find_last_of('.')+1);
	size_t subEnd = filepath.find_last_of('/');
	if (subEnd == STLString::npos)
		subEnd = filepath.find_last_of('\\');

	if (subEnd!=STLString::npos)
		subEnd++;
	m_dir = filepath.substr(0, subEnd);
	m_mappings.clear();
	m_isPopulated=false;

	if(ext=="spf")
	{
		m_isPopulated=ProcessSPF(filepath)==tinyxml2::XML_SUCCESS;
	}
	else
		DBG_PRINT("Initializer file extension not recognized");

	return m_isPopulated;
}

/** Process the SPF file found at a given filepath.
	@param filepath Path to the .spf to be processed.
	@return tinyxml2 result flag.
*/
int InitializerSet::ProcessSPF(const STLString filepath)
{
	TI2Document doc;
	int result= doc.LoadFile( filepath.c_str() );

	if(result==tinyxml2::XML_SUCCESS)
	{
		TI2Element *pXmlRoot = doc.RootElement();
		ProcessSPFLevel(pXmlRoot, m_mappings);
	}

	return result;
}

/** Process specific depth of SPF XML tree.
	@param pCurrElem Pointer to current subnode.
	@param currMap Map to place process results into.
*/
void InitializerSet::ProcessSPFLevel(TI2Element *pCurrElem, ISModelMap & currMap)
{
	TI2Element* pSub=pCurrElem->FirstChildElement("submodel");
	TI2Element* pSubNode=NULL;
	TI2Element* pVars=NULL;
	TI2Element* pCurrVar=NULL;
	TI2Element* pSubVal=NULL;
	STLString label;
	STLsmatch matchRes;

	while(pSub)
	{
		if(!pSub->IntAttribute("ome_omit"))
		{
			ISModelEntry currEntry;
			currEntry.m_modelName=pSub->Attribute("label");
			if(currEntry.m_modelName=="top")
				currEntry.m_modelName="root";

			pVars=pSub->FirstChildElement("variables");
			if(pVars)
			{
				pCurrVar=pVars->FirstChildElement("single_value");
				while(pCurrVar)
				{
					OME_SCALAR val=0.0;
					if(!pCurrVar->IntAttribute("ome_omit"))
					{
						XMLVALQUERY(pCurrVar,"val",&val);
					
						label=ScrubLabel(pCurrVar->Attribute("label"));
						
						currEntry.m_values[label]=val;
					}
				
					pCurrVar=pCurrVar->NextSiblingElement("single_value");
				}

				pCurrVar=pVars->FirstChildElement("csv_columns");
				while(pCurrVar)
				{
					if(!pCurrVar->IntAttribute("ome_omit"))
					{
						//csv entries are distinguished by filepath
						//each entry contains

						const STLString filename=PrepPath(STLString(pCurrVar->Attribute("filename")),m_dir);
						const OMEChar* colName=pCurrVar->Attribute("data_column");
						
						currEntry.m_dataSources[filename].m_path = filename;
						DataSourceValue newVal;
						newVal.m_label=pCurrVar->Attribute("label");
						//replace spaces with underscores
						/*for (size_t i = 0; i < newVal.m_label.size(); i++)
						{
							if (std::isspace(newVal.m_label[i]))
								newVal.m_label[i] = '_';
						}*/
						//check for time series  and/or index stuff
						pSubVal=pCurrVar->FirstChildElement("value");
						while(pSubVal)
						{
							//check for time series pattern
							const static STLRegex valReg("^(.*?)(?:,(.*?):(.*?))?$");
							if(!pSubVal->IntAttribute("ome_omit"))
							{
								STLString valStr(pSubVal->Attribute("val"));
								if (std::regex_match(valStr, matchRes, valReg))
								{
									enum { INDCOL=1, FLAG, VAL };
									//grab time interval column (if it exists)
									newVal.m_indCol=matchRes[INDCOL].str();
									//look for interval
									if(matchRes[FLAG]=="wrap")
										newVal.m_interval=OME_SCLR_READSTR(matchRes[VAL]);
									else if(matchRes[FLAG]=="others")
									{
										//look for interpolation type
										const OMEChar* modes[] = {"use_last","use_closest","interpolate"};
										newVal.m_interp=0;
										for(size_t i=0; i<3 && !newVal.m_interp; i++)
										{
											if(matchRes[VAL]==modes[i])
												newVal.m_interp=i+1;
										}
									}

								}
								//check for index lookup
								int ind = 0;
								pSubVal->QueryIntAttribute("index", &ind);
								if (ind && !newVal.m_indCol.empty())
									newVal.m_inds.emplace((unsigned short)ind, newVal.m_indCol);
								
								pSubVal=pSubVal->NextSiblingElement("value");
							}
						}
					//replace spaces with underscores for colName key
						/*STLString key = colName;
						for (size_t i = 0; i<key.length(); i++)
							if (std::isspace(key[i]))
								key[i] = '_';*/
					currEntry.m_dataSources[filename].m_colMappings.insert(pair<STLString,DataSourceValue>(colName,newVal));
					currEntry.m_dataSources[filename].m_varNames.push_back(newVal.m_label);
					}
					pCurrVar=pCurrVar->NextSiblingElement("csv_columns");
				}
			}

			//run for submodels
			pSubNode=pSub->FirstChildElement("submodels");
			if(pSubNode)
				ProcessSPFLevel(pSubNode,currEntry.m_subEntries);

			//add entry
			currMap[currEntry.m_modelName]=currEntry;
		}
		pSub=pSub->NextSiblingElement("submodel");
	}
}

/** Apply initializer values to a specific model.
	@param pModel The model to populate.
*/
void InitializerSet::ApplySinglesToModel(Model* pModel)
{
	for(auto itr=m_mappings.begin(); itr!=m_mappings.end();++itr)
		ApplyISLevel((*itr).second,pModel,false);

}

/** Apply values from columns to a specific model.
	@param pModel The model to populate.
*/
void InitializerSet::ApplyColumnsToModel(Model* pModel)
{
	for(auto itr=m_mappings.begin(); itr!=m_mappings.end();++itr)
		ApplyISLevel((*itr).second,pModel,true);
}

/** Apply values from a specific Initializer Level to Model.
	@param entry Current entry to extract values from.
	@param pModel The Model to apply values to.
	@param colPass true if evaluating initializer columns; false otherwise.
*/
void InitializerSet::ApplyISLevel(const ISModelEntry & entry, Model* pModel, const bool colPass)
{

	//assign simple values first - ignore values that aren't in the model
	Evaluable* pEval;
	SimpleValMap missedEntries;

	if(!colPass)
	{
		for(auto itr=entry.m_values.begin(); itr!=entry.m_values.end(); ++itr)
		{
			//rely on name, not id.
			pEval=dynamic_cast<Evaluable*>(pModel->ObjectForName((*itr).first,true));

			if(pEval)
			{
				int ind = pModel->AddParameter((*itr).first, (*itr).second);
				pEval->SetInitializer(&pModel->GetParam(ind));
			}
			else //store missed value to apply to submodels
			{
				missedEntries[(*itr).first]=(*itr).second;
			}
		}
	}
	else
	{
		//some columns will go to models, others will go to time series.
		//grab data source values
		pModel->SetDataFileSources(entry.m_dataSources);

		for (auto deItr = entry.m_dataSources.begin(); deItr != entry.m_dataSources.end(); ++deItr)
		{
			size_t i = 0;
			for (auto vItr = (*deItr).second.m_varNames.begin(); vItr != (*deItr).second.m_varNames.end(); ++vItr,++i)
			{
				pEval = dynamic_cast<Evaluable*>(pModel->ObjectForName((*vItr), true));
				/// @todo check if eval not found
				
				int ind = pModel->AddDSParameter((*vItr));
				pEval->SetInitializer(&pModel->GetParam(ind));
			}
		}
	}

	//apply sub entries
	Model *pSubModel;
	ISModelEntry subEntry;
	for(auto itr=entry.m_subEntries.begin(); itr!=entry.m_subEntries.end(); ++itr)
	{
		pSubModel=pModel->SubmodelForName((*itr).first,true);

		if(pSubModel)
		{
			//apply any fields that weren't found here
			subEntry=(*itr).second;
			//this should not overwrite existing entries in case of collision.
			subEntry.m_values.insert(missedEntries.begin(),missedEntries.end());

			ApplyISLevel(subEntry,pSubModel,colPass);
		}
	}
}

/** Initialize an Evaluable object if an entry exists for it.
	@param pEval Pointer to the Evaluable object to attempt to initialize.
	@return true If Initial value was found and assigned; false otherwise.
*/
bool InitializerSet::InitializeEval(Evaluable* pEval)
{
	bool ret = false;
	if (pEval)
	{
		//first find record
		StrList upPath;
		pEval->GetParentModel()->GetUpPath(upPath);
		//remove parent model name
		upPath.pop_back();
		ISModelMap* pCurr=&m_mappings;

		ISModelMap::iterator fItr;
		for (auto itr = upPath.begin(); itr != upPath.end() && pCurr; ++itr)
		{
			fItr = pCurr->find(*itr);
			pCurr = fItr != pCurr->end() ? &(*fItr).second.m_subEntries : NULL;
		}

		ISModelEntry* pEntry = NULL;
		if (pCurr)
		{
			fItr = pCurr->find(pEval->GetParentModel()->GetName());
			if (fItr != pCurr->end())
				pEntry = &(*fItr).second;
		}
		if (pEntry)
		{
			//next, find if the value is present
			auto svItr = pEntry->m_values.find(pEval->GetName());
			if (svItr != pEntry->m_values.end())
			{
				pEval->SetValue((*svItr).second);
				ret = true;
			}
			if (!ret)
			{
				throw OMEObjException("Initialization failed", pEval);
			}
		}
		
	}
	return ret;
}