#include "omeobjects/Model.h"
#include "events/CentralEventDispatcher.h"
#include "DebugOutput.h"
#include "SimManager.h"
#include "omeobjects/SubmodelAssoc.h"
#include "omeobjects/ModelInstance.h"
#include "EvalXSetInterpTable.h"

#include <stack>

bool Model::m_tagsInitialized=false;
const OMEChar* Model::m_xmlTags[Model::TAG_COUNT];

/** Default constructor */
Model::Model()
	: OMEObject(), 
	m_isRunning(false),
	m_pEvalParser(NULL),
	m_preferredIntMethod(OMEIntegrationBlock::IM_EULER),
	m_reportInterval(1.0),
	m_pSimManager(NULL),
	m_isInitialized(false),
	m_prioritized(false),
	m_initialNumInstances(1), 
	m_preSpawned(false), 
	m_expectsSpatial(false),
	m_totInstCount(0),
	m_instMode(STATIC),
	m_pValArchive(NULL),
	m_isUnPacked(false)
{
	for (size_t i = 0; i < OME_THREAD_COUNT; ++i)
	{
		m_currentInstance[i] = 0;
		m_indMultiplier[i] = 0;
	}
	if(!m_tagsInitialized) 
		InitXMLTags(); 
}

/** Simple Constructor
@param id The Unique ID to assign to the model.
@param n name of model.
*/
Model::Model(const STLString & id, const STLString & n)
	: OMEObject(id,n), 
	m_isRunning(false),
	m_pEvalParser(NULL),
	m_preferredIntMethod(OMEIntegrationBlock::IM_EULER),
	m_reportInterval(1.0),
	m_pSimManager(NULL),
	m_isInitialized(false),
	m_prioritized(false),
	m_initialNumInstances(1), 
	m_preSpawned(false), 
	m_expectsSpatial(false),
	m_totInstCount(0),
	m_instMode(STATIC),
	m_pValArchive(NULL),
	m_isUnPacked(false)
{ 
	if (m_name.empty())
		m_name = m_id;

	for (size_t i = 0; i < OME_THREAD_COUNT; ++i)
	{
		m_currentInstance[i] = 0;
		m_indMultiplier[i] = 0;
	}
	if(!m_tagsInitialized)  
		InitXMLTags();
}

Model::Model(const Model & mdl)
{ 
	Duplicate(mdl); 
}

/**	XML Constructor.
This constructor instantiates the object based on the arguments passed in.
@param pCurrElem  the current Tiny XML Element
@param tag   the name of the desired tag containing the info.
@param filename  the filename associated with the XML info.
@param parent Optional pointer to parent model.
@see	GetXMLForObject()
*/
Model::Model(TI2Element* pCurrElem,const STLString & tag, const STLString & filename,Model* parent) 
	: m_isRunning(false),
	m_pEvalParser(NULL),
	m_preferredIntMethod(OMEIntegrationBlock::IM_EULER),
	m_reportInterval(1.0),
	m_pSimManager(NULL),
	m_isInitialized(false),
	m_prioritized(false),
	m_initialNumInstances(1), 
	m_preSpawned(false), 
	m_expectsSpatial(false),
	m_totInstCount(0),
	m_instMode(STATIC),
	m_pValArchive(NULL),
	m_isUnPacked(false)

{
	for (size_t i = 0; i < OME_THREAD_COUNT; ++i)
	{
		m_currentInstance[i] = 0;
		m_indMultiplier[i] = 0;
	}

	if(!m_tagsInitialized)
		InitXMLTags();
	m_pParentModel=parent;
	OMEObject::PopulateFromXML(pCurrElem,tag,filename);
}

/** Destructor. */ 
Model::~Model(void)
{
	DeallocPtrArray(m_influenceArray);
	DeallocPtrArray(m_varArray);
	DeallocPtrArray(m_stateVarArray);
	DeallocPtrArray(m_flowArray);
	DeallocPtrArray(m_portArray);
	DeallocPtrArray(m_assocPortArray);
	DeallocPtrArray(m_iterCondArray);
	DeallocPtrArray(m_labelArray);
	DeallocPtrArray(m_subAssocArray);
	DeallocPtrArray(m_instances);
	DeallocPtrArray(m_spawnerArray); 
	for (auto mItr = m_tables.begin(); mItr != m_tables.end(); ++mItr)
		delete (*mItr).second;
	
	//for (auto ecItr = m_evalColls.begin(); ecItr != m_evalColls.end(); ++ecItr)
	//	delete (*ecItr).second;
	DeallocPtrArray(m_submodelArray);
}

/** Copies values from another object.
All Values are copied except up and down object arrays.
m_name and m_id are append with underscores to maintain uniqueness (is there a better way to handle this?).
All objects contained within the model are copied as well.
@param mdl the object to be copied.
@param copyPorts if true, the submodelPorts are duplicated as well; if false, the submodelPorts are linked into this model.
*/
void Model::Duplicate(const Model & mdl,bool copyPorts)
{
	//copy primitive members
	m_stepSize=mdl.m_stepSize;
	//m_minStepSize=mdl.m_minStepSize;
	//m_maxStepSize=mdl.m_maxStepSize;
	m_preferredIntMethod=mdl.m_preferredIntMethod;
	m_reportInterval=mdl.m_reportInterval;
	m_prioritized=mdl.m_prioritized;

	//iterate throug ptrArrays and make copies of enclosed objects
	std::for_each(mdl.m_paramArray.begin(),mdl.m_paramArray.end(),[this](const Param & pP){ AddParameter(pP); });
	std::for_each(mdl.m_varArray.begin(),mdl.m_varArray.end(),[this](Variable* pV){ AddVariable(new Variable(*pV)); });
	std::for_each(mdl.m_stateVarArray.begin(),mdl.m_stateVarArray.end(),[this](StateVar* pSv){ AddStateVar(new StateVar(*pSv)); });
	std::for_each(mdl.m_flowArray.begin(),mdl.m_flowArray.end(),[this](Flow* pF){ AddFlow(new Flow(*pF)); });
	std::for_each(mdl.m_influenceArray.begin(),mdl.m_influenceArray.end(),[this](Influence* pI){ AddInfluence(new Influence(*pI)); });
	std::for_each(mdl.m_submodelArray.begin(),mdl.m_submodelArray.end(),[this](Model* pM){ AddSubmodel(new Model(*pM)); });

	//int i;

	//for(i=0;i<mdl.m_paramArray.size();i++)
	//	AddParameter((Param*)mdl.m_paramArray[i]->Clone());
	//for(i=0;i<mdl.m_varArray.size();i++)
	//	AddVariable((Variable*)mdl.m_varArray[i]->Clone());
	//for(i=0;i<mdl.m_stateVarArray.size();i++)
	//	AddStateVar((StateVar*)mdl.m_stateVarArray[i]->Clone());
	//for(i=0;i<mdl.m_flowArray.size();i++)
	//	AddFlow((Flow*)mdl.m_flowArray[i]->Clone());
	//for(i=0;i<mdl.m_influenceArray.size();i++)
	//	AddInfluence((Influence*)mdl.m_influenceArray[i]->Clone());
	//for(i=0;i<mdl.m_submodelArray.size();i++)
	//	AddSubmodel((Model*)mdl.m_submodelArray[i]->Clone());
	
	std::for_each(mdl.m_spawnerArray.begin(),mdl.m_spawnerArray.end(),[this](Spawner* pSpwn){ AddSpawner(new Spawner(*pSpwn)); });
	std::for_each(mdl.m_instances.begin(),mdl.m_instances.end(),[this](ModelInstance* pMi){ m_instances.push_back(pMi->Clone()); });

	std::for_each(mdl.m_iterCondArray.begin(),mdl.m_iterCondArray.end(),[this](IterConditional* pIc){ AddIterConditional(new IterConditional(*pIc)); });

	if(copyPorts)
	{
		//create copies of the ports
		std::for_each(mdl.m_portArray.begin(),mdl.m_portArray.end(),[this](SubmodelPort* pSp){AddSubPort(new SubmodelPort(*pSp),true); });
		std::for_each(mdl.m_assocPortArray.begin(), mdl.m_assocPortArray.end(), [this](AssocPort* pSp){AddAssocPort(new AssocPort(*pSp), true); });
	}
	else
	{
		//copy ports into model.
		std::for_each(mdl.m_portArray.begin(),mdl.m_portArray.end(),[this](SubmodelPort* pSp){AddSubPort(pSp,false);});
		std::for_each(mdl.m_assocPortArray.begin(), mdl.m_assocPortArray.end(), [this](AssocPort* pSp){AddAssocPort(pSp, false); });

	}

	m_modelEnums.assign(mdl.m_modelEnums.begin(),mdl.m_modelEnums.end());

	//build new object map
	RefreshObjectMaps();

	//force refresh to ensure that all duplicate links are pointing to duplicates
	JoinLinks(true);

	m_initialNumInstances=mdl.m_initialNumInstances;
	m_preSpawned=mdl.m_preSpawned;
	m_expectsSpatial = mdl.m_expectsSpatial;
	memcpy(m_currentInstance,mdl.m_currentInstance,sizeof(m_currentInstance));
	m_pValArchive = mdl.m_pValArchive;
	m_isUnPacked = mdl.m_isUnPacked;
	DisperseMultiplier();

}

/** Retrieve a single value for a specific index at a specific instance.
	@param inst The present instance to query.
	@param ind Index of the value to lookup.
	@param out variable to hold the requested value, if found.
	@return true if a value was found (both inst and ind were valid indices); false otherwise.
*/
bool Model::GetValForInstIndex(const size_t & inst, const size_t & ind, OME_SCALAR & out)
{
	OME_SCALAR* temp;
	bool ret=GetValAddrForInstIndex(inst,ind,temp);
	if (ret)
	{
		out = m_instances[inst + m_indMultiplier[OME_CURR_THREAD]]->IsActive() ? *temp : 0.0;
	}
	return ret;
}

/** Retrieve a pointer to a single value for a specific index at a specific instance.
	@param inst The present instance to query.
	@param ind Index of the value to lookup.
	@param out pointer to hold the requested address, if found.
	@return true if a value was found (both inst and ind were valid indices); false otherwise.
*/
bool Model::GetValAddrForInstIndex(const size_t & inst, const size_t & ind, OME_SCALAR* & out)
{
	bool ret=false;
	if(m_totInstCount>inst)
	{
		out = &(*m_pValArchive)[ind + (inst+ m_indMultiplier[OME_CURR_THREAD])];
		ret=true;
	}
	return ret;
}

/** Retrieve a single value for a specific index at a specific instance.
@param inst The raw index to the instance to query.
@param ind Index of the value to lookup.
@param out variable to hold the requested value, if found.
@return true if a value was found (both inst and ind were valid indices); false otherwise.
*/
bool Model::GetValForRawInstIndex(const size_t & inst, const size_t & ind, OME_SCALAR & out)
{
	OME_SCALAR* temp;
	bool ret = GetValAddrForRawInstIndex(inst, ind, temp);
	if (ret)
	{
		out = m_instances[inst]->IsActive() ? *temp : 0.0;
	}
	return ret;
}

/** Retrieve a pointer to a single value for a specific index at a specific instance.
@param inst The raw index to the instance to query.
@param ind Index of the value to lookup.
@param out pointer to hold the requested address, if found.
@return true if a value was found (both inst and ind were valid indices); false otherwise.
*/
bool Model::GetValAddrForRawInstIndex(const size_t & inst, const size_t & ind, OME_SCALAR* & out)
{
	bool ret = false;
	if (m_instances.size()>inst)
	{
		out = &(*m_pValArchive)[ind + inst];
		ret = true;
	}
	return ret;
}

/** Retrieve a single value for a specific index at a specific instance with a specific stride.
@param inst The index to the instance to query.
@param ind Index of the value to lookup.
@param step The step to take between relevant values.
@param out variable to hold the requested value, if found.
@return true if a value was found (both inst and ind were valid indices); false otherwise.
*/
bool Model::GetValForInstIndex(const size_t & inst, const size_t  & ind, const size_t & step, OME_SCALAR & out)
{
	OME_SCALAR* temp;
	bool ret = GetValAddrForInstIndex(inst, ind,step, temp);
	if (ret)
	{
		out = m_instances[inst + m_indMultiplier[OME_CURR_THREAD]]->IsActive() ? *temp : 0.0;
	}
	return ret;
}

/** Retrieve the address of single value for a specific index at a specific instance with a specific stride.
@param inst The index to the instance to query.
@param ind Index of the value to lookup.
@param step The step to take between relevant values.
@param out The pointer to hold for the requested value, if found.
@return true if a value was found (both inst and ind were valid indices); false otherwise.
*/
bool Model::GetValAddrForInstIndex(const size_t & inst, const size_t  & ind, const size_t & step, OME_SCALAR* & out)
{
	bool ret = false;
	if (m_totInstCount>inst)
	{
		out = &(*m_pValArchive)[ind + (inst + m_indMultiplier[OME_CURR_THREAD])*step];
		ret = true;
	}
	return ret;
}

/** Retrieve a single value for a specific raw index at a specific instance with a specific stride.
@param inst The raw index to the instance to query.
@param ind Index of the value to lookup.
@param step The step to take between relevant values.
@param out variable to hold the requested value, if found.
@return true if a value was found (both inst and ind were valid indices); false otherwise.
*/
bool Model::GetValForRawInstIndex(const size_t & inst, const size_t  & ind, const size_t & step, OME_SCALAR & out)
{
	OME_SCALAR* temp;
	bool ret = GetValAddrForRawInstIndex(inst, ind, step,temp);
	if (ret)
	{
		out = m_instances[inst]->IsActive() ? *temp : 0.0;
	}
	return ret;
}

/** Retrieve the address of single value for a specific raw index at a specific instance with a specific stride.
@param inst The raw index of the instance to query.
@param ind Index of the value to lookup.
@param step The step to take between relevant values.
@param out The pointer to hold for the requested value, if found.
@return true if a value was found (both inst and ind were valid indices); false otherwise.
*/
bool Model::GetValAddrForRawInstIndex(const size_t & inst, const size_t  & ind, const size_t & step, OME_SCALAR* & out)
{
	bool ret = false;
	if (m_instances.size()>inst)
	{
		out = &(*m_pValArchive)[ind + inst*step];
		ret = true;
	}
	return ret;
}

/** Copies values from another object.
All Values are copied except up and down object arrays.
m_name and m_id are append with underscores to maintain uniqueness (is there a better way to handle this?).
All objects contained within the model are copied as well.
@param mdl the object to be copied.
*/
void Model::Duplicate(const Model & mdl)
{
	//dup parent
	OMEObject::Duplicate(mdl);
	Duplicate(mdl,true);

   std::for_each(mdl.m_labelArray.begin(),mdl.m_labelArray.end(),[this](SimpleLabel* pSl){ m_labelArray.push_back((SimpleLabel*)pSl->Clone()); });
   std::for_each(mdl.m_subAssocArray.begin(),mdl.m_subAssocArray.end(),[this](SubmodelAssoc* pSa){ m_subAssocArray.push_back((SubmodelAssoc*)pSa->Clone()); });

	//make links point to objects within model
	JoinLinks(true);
	//GenerateInstance();
}



//XML stuff
STLString Model::GetXMLRepresentation(const unsigned int indent, const unsigned int inc)
{
	return GetXMLForObject(indent,inc);
}

void Model::GetXMLAttributes(StrArray & out) const
{
	AddAttributeValue<int>("init_instances",m_initialNumInstances,out);
	OMEObject::GetXMLAttributes(out);

	//model attrs

	AddAttributeValue<OME_SCALAR>("step_size",m_stepSize,out);
  // AddAttributeValue<float>("min_step_size",m_minStepSize,out);
  // AddAttributeValue<float>("max_step_size",m_maxStepSize,out);

   AddAttributeValue<int>("int_method",m_preferredIntMethod,out);
   if (m_expectsSpatial)
	AddAttributeValue<int>("expects_spatial", m_expectsSpatial, out);
   //AddAttributeValue<STLString>("data_source",m_dataFilePath,out);

}

/** Get Subnodes that do not contain draw data. This function will be called directly by DrawDetails.
	@param out An array to store each attribute entry.
	@param indent The current indentation level.
	@param inc The increment step for each additional level of indentation.
*/
void Model::GetXMLNonDrawableSubNodes(StrArray & out, const unsigned int indent, const unsigned int inc)
{
	oSTLSStream temp;
	const STLString baseIndent(indent,' ');
	const STLString sub1Indent(baseIndent.length()+inc,' ');
	const STLString sub2Indent(sub1Indent.length()+inc,' ');
	size_t i;

		//record Data sources
	if(!m_dataFilePaths.empty())
	{
		temp<<baseIndent<<"<data_sources>\n"<<std::endl;
		for(auto itr=m_dataFilePaths.begin(); itr!=m_dataFilePaths.end();++itr)
		{
			temp<<sub1Indent<<"<data_source path=\""<<(*itr).second.m_path<<"\" ";
			/// \todo reimplement for new dataSourcesEntry
			//if((*itr).second.m_colMappings.size())
			//{
			//	temp<<">"<<std::endl;
			//	for(auto mapItr=(*itr).second.m_colMappings.begin(); mapItr!=(*itr).second.m_colMappings.end(); ++mapItr)
			//		temp<<sub2Indent<<"<field_map column=\""<<(*mapItr).second<<"\" field=\""<<(*mapItr).first<<"\" />"<<endl;
			//}
			//else
				temp<<"/>"<<std::endl;
		}
		temp<<baseIndent<<"</data_sources>";
		out.push_back(temp.str());
	   temp.str(STLString());
	  temp.clear();
	}

	if(!m_tables.empty())
	{
		temp<<baseIndent<<"<tables>"<<std::endl;
		for (auto mItr = m_tables.begin(); mItr != m_tables.end(); ++mItr)
			temp<<((*mItr).second)->GetXMLRepresentation(indent+inc,inc)<<std::endl;
		temp<<baseIndent<<"</tables>";
		out.push_back(temp.str());
	   temp.str(STLString());
	  temp.clear();
	}

		//enumerated types
	if(!m_modelEnums.empty())
	{
		temp<<baseIndent<<"<enumerations>"<<std::endl;
		for(i=0; i<m_modelEnums.size();i++)
		{
			temp << sub1Indent + "<enum name=\"" << m_modelEnums[i].GetEnumClassName() << "\">" << std::endl;
			for (int j = 0; j<m_modelEnums[i].GetEnumValueCount(); j++)
				temp<<sub2Indent<<"<val name=\""<<m_modelEnums[i].GetEnumValue(j)<<"\"/>"<<std::endl;

			temp<<sub1Indent<<"</enum>"<<std::endl;
		}
		temp<<baseIndent<<"</enumerations>";
		out.push_back(temp.str());
	   temp.str(STLString());
	  temp.clear();
	}
	
}

/** Get Subnodes that contain draw data. This function will be overridden as needed by DrawDetails.
	@param out An array to store each attribute entry.
	@param indent The current indentation level.
	@param inc The increment step for each additional level of indentation.
*/
void Model::GetXMLDrawableSubNodes(StrArray & out, const unsigned int indent, const unsigned int inc)
{
	//grab standard collections
	//variables
	CollectionXMLRepresentation("variables",m_varArray,out,indent,inc);
	//states
	CollectionXMLRepresentation("states",m_stateVarArray,out,indent,inc);
	//flows
	CollectionXMLRepresentation("flows",m_flowArray,out,indent,inc);
	//influences
	CollectionXMLRepresentation("influences",m_influenceArray,out,indent,inc);
	//labels
	CollectionXMLRepresentation("labels",m_labelArray,out,indent,inc);
	//iterConditionals
	CollectionXMLRepresentation("iterators",m_iterCondArray,out,indent,inc);
	//ports
	CollectionXMLRepresentation("modelports",m_portArray,out,indent,inc,true);
	CollectionXMLRepresentation("assocports", m_assocPortArray, out, indent, inc, true);
	//SubmodelAssocs
	CollectionXMLRepresentation("assocs",m_subAssocArray,out,indent,inc/*,true*/);
	//Spawners
	CollectionXMLRepresentation("spawners",m_spawnerArray,out,indent,inc);
	//submodels
	CollectionXMLRepresentation("submodels",m_submodelArray,out,indent,inc);

	//aliases
	CollectionXMLRepresentation("aliases", m_aliasArray, out, indent, inc);
}

void Model::GetXMLSubNodes(StrArray & out, const unsigned int indent, const unsigned int inc)
{
	OMEObject::GetXMLSubNodes(out,indent,inc);

	GetXMLNonDrawableSubNodes(out,indent,inc);
	GetXMLDrawableSubNodes(out,indent,inc);

}


void Model::GetXMLAttributeMap(XMLAttrVector & out)
{

	XmlAttr attrs[] = {
		// attr				type		   address                          isReq  checkCol
		{ "step_size",		XML_FLOAT,		&(this->m_stepSize),			true },
		//{ "min_step_size",	XML_FLOAT,		&(this->m_minStepSize),			true },
		//{ "max_step_size",	XML_FLOAT,		&(this->m_maxStepSize),			true },
		{ "int_method",		XML_INT,		&(this->m_preferredIntMethod),	false },
		{ "init_instances",	XML_INT,		&(this->m_initialNumInstances),	false },
		{ "expects_spatial", XML_BOOL,      &(this->m_expectsSpatial),      false },
		{ NULL,				XML_NULL,		NULL,							false } };
	
		//drop null
		int count;
		for(count=0; attrs[count].name!=NULL; count++)
			out.push_back(attrs[count]);

		OMEObject::GetXMLAttributeMap(out);
}

int Model::PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
{
	//data sources
	STLString subTag="data_source";
	TI2Element* pXmlSub = pCurrElem->FirstChildElement( "data_sources" );

	// \todo reimplement for new datasourceentry
	/*if ( pXmlSub )
	{
		STLString pathTemp;
		TI2Element* pXmlFM;

		pXmlSub=pXmlSub->FirstChildElement(subTag.c_str());
		while(pXmlSub!=NULL)
		{
			DataSourceEntry dsEntry;
			dsEntry.m_path=pXmlSub->Attribute("path");

			pXmlFM=pXmlSub->FirstChildElement("field_map");
			while(pXmlFM!=NULL)
			{
				dsEntry.m_colMappings[pXmlFM->Attribute("column")]=pXmlFM->Attribute("field");
				pXmlFM=pXmlFM->NextSiblingElement("field_map");
			}
			m_dataFilePaths[dsEntry.m_path]=dsEntry;
			pXmlSub=pXmlSub->NextSiblingElement(subTag.c_str());
		}
	}
*/
	//tables
	subTag=m_xmlTags[TAG_TBL];
	TI2Element* pTblSub = pCurrElem->FirstChildElement( "tables" );
	if (pTblSub)
	{
		//regular table
		pXmlSub = pTblSub->FirstChildElement(subTag.c_str());
		while(pXmlSub!=NULL)
		{
			EvalTable* et=new EvalTable(pXmlSub,subTag,filename);
			AddEvalTable(et);
			pXmlSub=pXmlSub->NextSiblingElement(subTag.c_str());
		}

		//interp tables
		subTag = m_xmlTags[TAG_INTTBL];
		pXmlSub = pTblSub->FirstChildElement(subTag.c_str());
		while (pXmlSub != NULL)
		{
			EvalInterpTable* et = new EvalInterpTable(pXmlSub, subTag, filename);
			AddEvalTable(et);
			pXmlSub = pXmlSub->NextSiblingElement(subTag.c_str());
		}

		//Xpt-based interp tables
		subTag = m_xmlTags[TAG_XINTTBL];
		pXmlSub = pTblSub->FirstChildElement(subTag.c_str());
		while (pXmlSub != NULL)
		{
			EvalXSetInterpTable* et = new EvalXSetInterpTable(pXmlSub, subTag, filename);
			AddEvalTable(et);
			pXmlSub = pXmlSub->NextSiblingElement(subTag.c_str());
		}
	}


	//variables and vararrays
	LoadCollectionFromXML(pCurrElem,filename,m_xmlTags[TAG_VAR],"variables",m_varArray);
	LoadCollectionFromXML<VarArray,Variable>(pCurrElem,filename,m_xmlTags[TAG_VA],"variables",m_varArray);
	LoadCollectionFromXML<TimeSeriesVar,Variable>(pCurrElem,filename,m_xmlTags[TAG_TS],"variables",m_varArray);

	//iterators
	LoadCollectionFromXML(pCurrElem,filename,m_xmlTags[TAG_ITR],"iterators",m_iterCondArray);

	//states
	LoadCollectionFromXML(pCurrElem,filename,m_xmlTags[TAG_STATE],"states",m_stateVarArray);
	LoadCollectionFromXML<Conveyor,StateVar>(pCurrElem, filename, m_xmlTags[TAG_CONVEYOR], "states", m_stateVarArray);

	//influences
	LoadCollectionFromXML(pCurrElem,filename,m_xmlTags[TAG_INFL],"influences",m_influenceArray);

	//flows
	LoadCollectionFromXML(pCurrElem,filename,m_xmlTags[TAG_FLOW],"flows",m_flowArray);
	
	//labels
	LoadCollectionFromXML(pCurrElem,filename,m_xmlTags[TAG_LBL],"labels",m_labelArray);
	
	//submodel associations
	LoadCollectionFromXML(pCurrElem,filename,m_xmlTags[TAG_ASSC],"assocs",m_subAssocArray);

	//spawners
	LoadCollectionFromXML(pCurrElem,filename,m_xmlTags[TAG_SPWN],"spawners",m_spawnerArray);

	//aliases
	LoadCollectionFromXML(pCurrElem, filename, m_xmlTags[TAG_ALIAS], "aliases", m_aliasArray);
//Other loads that aren't handled by LoadCollectionFromXML()

	//submodels
	subTag=GetXMLTag();
	pXmlSub = pCurrElem->FirstChildElement( "submodels" );
	if ( pXmlSub )
	{
		pXmlSub=pXmlSub->FirstChildElement(subTag.c_str());
		while(pXmlSub!=NULL)
		{
			Model* model=new Model(pXmlSub,subTag,filename,this);
			model->SetParentModel(this);
			m_submodelArray.push_back(model);
			pXmlSub=pXmlSub->NextSiblingElement(subTag.c_str());
		}
	}

	//ports

	Model* pBm=this->GetParentModel();
	subTag=m_xmlTags[TAG_PORT];
	pXmlSub = pCurrElem->FirstChildElement( "modelports" );
	if ( pXmlSub )
	{
		pXmlSub=pXmlSub->FirstChildElement(subTag.c_str());
		while(pXmlSub!=NULL)
		{
			SubmodelPort* smp=new SubmodelPort(pXmlSub,subTag,filename);
			smp->SetParentModel(this);
			if(pBm)
			{
				pBm->AddSubPort(smp,false);
				smp->SetOuterModel(pBm);
				
			}
			//DBG_PRINT(smp->ToString());
			m_portArray.push_back(smp);
			pXmlSub=pXmlSub->NextSiblingElement(subTag.c_str());
		}
	}

	subTag = m_xmlTags[TAG_ASSOCPORT];
	pXmlSub = pCurrElem->FirstChildElement("assocports");
	if (pXmlSub)
	{
		pXmlSub = pXmlSub->FirstChildElement(subTag.c_str());
		while (pXmlSub != NULL)
		{
			AssocPort* ap = new AssocPort(pXmlSub, subTag, filename);
			ap->SetParentModel(this);
			if (pBm)
			{
				pBm->AddAssocPort(ap, false);
				ap->SetOuterModel(pBm);

			}

			m_assocPortArray.push_back(ap);
			pXmlSub = pXmlSub->NextSiblingElement(subTag.c_str());
		}
	}
	//enumerations
	subTag="enum";
	pXmlSub = pCurrElem->FirstChildElement("enumerations");
	if( pXmlSub )
	{
		STLString enumTemp;
		TI2Element *pEnumSub;
		XmlAttr enumAttrs[] = {
			// attr				type		   address                          isReq  checkCol
			{ "name",			XML_STR,	&enumTemp,						true },
			{ NULL,				XML_NULL,		   NULL,							false } };
			
		pXmlSub=pXmlSub->FirstChildElement(subTag.c_str());
		while(pXmlSub!=NULL)
		{
			ParseAttributes(pXmlSub, enumAttrs, subTag, filename);
			ModelEnum newEnum(enumTemp);
		 

			pEnumSub= pXmlSub->FirstChildElement("val");
			while(pEnumSub!=NULL)
			{
				ParseAttributes( pEnumSub, enumAttrs, "val", filename );
				newEnum.AddEnumValue(enumTemp);

				pEnumSub=pEnumSub->NextSiblingElement("val");
			}

			m_modelEnums.push_back(newEnum);
			pXmlSub=pXmlSub->NextSiblingElement(subTag.c_str());
		}
	}

	//wire all link-based objects
	JoinLinks();

	//wire associations
	JoinAssocs();

	if (isRoot())
		JoinAliases();
	return 1;
}

/** Spawns a new instance of this model using the present state.
	@return The unique instance identifier.
*/
 int Model::SpawnInstance(const int useIndex, const int instParent, Spawner* pSpwn)
 {
	 /// \question do we need to update for multiplier?
	 DBG_PRINTELAPSED("Spawn instance");
	 
	 //append to end of each section list
	 size_t secCount=GetTotParentInstances();
	 size_t oldEnd=m_instances.size();
	 int index;
	 if(useIndex >= 0)
		 index = useIndex;
	 else
	 {

		 index=m_totInstCount;
		 m_instances.resize(index+secCount);
	 }

	 //walk backwards down list, shifting old values into correct spots,
	 //and inserting new instances in the correct places
	 size_t dist=secCount;
	 for(size_t i=oldEnd; i>0 && dist; i--)
	 {
		 if(!((i-index) % secCount))
		 {
			 m_instances[i+dist]=GenerateInstance(i+dist);
			 dist--;
		 }
		 if(i+dist<m_instances.size())
			m_instances[i+dist]=m_instances[i];
		
	 }


	// if(instParent>=0)
	// {
	////	 newInst->m_instanceParent=instParent;
	//	 *newInst=*m_instances[instParent];
	// }

	// OMEParser* pParser=m_pSimManager->GetPEvalParser();
	// if(pParser)
	//	pParser->UpdateSubmodelEntry(this,index);

	 m_totInstCount++;

	 return index;
 }

 /** Spawn a number of instances of the representative model.
	@param num the number of model instances to spawn.
	@param instParent The identifier of the instance responsible for this call.
	@param pSpwn Optional Pointer to Spawner responsible for creating the new instances.
 */
 void Model::SpawnInstances(const int num,const int instParent, Spawner* pSpwn)
 {
	 //pre-size CArrays to prepare for parallel assignment
	 DBG_PRINTELAPSED("Spawn instances");
	

	 int start=m_instances.size();
	 
	 //DBG_PRINTC(std::to_string(GetTotParentInstances()),DBG_GREEN);

	 m_instances.resize(start+num*GetTotParentInstances());

	 //new method
	 for (size_t i = start; i < m_instances.size(); i++)
	 {
		 m_instances[i] = instParent<0 ? GenerateInstance(i) : GenerateInstance(i,instParent,pSpwn);
		 if (pSpwn)
			 pSpwn->AddSpwnInd(i);
	 }
	 m_totInstCount+=num;

 }

 /** Kill a specified instance, rendering it inactive.
	The model still exists in the instance list; it just no longer updates.
	@param inst The instance number of the model instance to kill.
*/
 void Model::KillInstance(int inst)
 {
	 //don't resize array; just set instance to NULL (we don't want to desync instance indexes)
	 if(inst>=0 && inst<m_totInstCount)
		m_instances[inst]->Kill();
 }

 /** Dealloc and cleanup any instances marked as "dead".*/
 void Model::CollectDeadInstances()
 {
	 //wipe dead
	 MdlInstanceArray::iterator endItr=std::remove_if(m_instances.begin(),m_instances.end(),[](ModelInstance* pMi){ return pMi->IsDead();});
	 //resize container
	 m_instances.resize(endItr-m_instances.begin());

	 //resize to nearest multiple of parent instances
	 size_t totParent=GetTotParentInstances();
	 if(m_instances.size() % totParent)
		 m_instances.resize(m_instances.size()+(m_instances.size() % totParent));
	 m_totInstCount=m_instances.size()/totParent;
 }

/** Kill all instances marked for death for current set of Model Instances.
*/
void Model::KillMarked()
{
	if (IsDynamic() || IsConditional())
	{
		size_t start = GetParentOffset();
		for (size_t i = start; i < start + m_totInstCount; i++)
			m_instances[i]->ApplyMortality();
	}
}

/** Kill all instances marked for death for all Model Instances.
*/
void Model::KillAllMarked()
{
	if (IsDynamic()|| IsConditional())
	{
		size_t start = GetParentOffset();
		for (size_t i = start; i < start + m_totInstCount; i++)
			m_instances[i]->ApplyMortality();
	}
}


/** Determine if a submodel instance is marked for death by a specific spawner.
	@param inst The instance to check.
	@param pSpwn The Spawner to check against.
	@return true if pSpwn has marked inst for death, false otherwise.
*/
bool Model::IsMarkedBySpawner(int inst, Spawner* pSpwn)
{
	return pSpwn->DidSpawnInst(inst);
}


/** Join any disjointed links.
Pointers are added to any links that contain only source/target ids. A populated object map is required for this process, so the object map is populated with the current model objects if it is empty.
@param forceAssign If true, any redundant entries in the objectmap will be overwritten with the most recently added
object with the conflicted key.
@param recurse if true, call JoinLinks on all submodels.
@return 1 If there are no problems joining lakes.
*/
int Model::JoinLinks(const bool & forceAssign, const bool & recurse)
{
	int ret=1;
	//if(m_objectIDMap.empty())
	RefreshObjectMaps();

	if(!m_objectIDMap.empty())
	{
		//flows
		for (Flow* pFlow: m_flowArray)
		{
			if (pFlow)
				pFlow->SyncWithMap(m_objectIDMap, forceAssign);
		}

		//ports
		for(SubmodelPort* pPort:m_portArray)
		{
			if(pPort)
				pPort->SyncWithParents();
		}

		//influences
		for(Influence* pInfl:m_influenceArray)
		{
			if(pInfl)
				pInfl->SyncWithMap(m_objectIDMap,forceAssign);
		}

		//associations
		//ports
		for (AssocPort* pAssoc : m_assocPortArray)
		{
			if (pAssoc)
				pAssoc->SyncWithParents();
		}

		if(recurse)
			std::for_each(m_submodelArray.begin(),m_submodelArray.end(),[&forceAssign](Model* pMdl){pMdl->JoinLinks(forceAssign,true);});
		ret=1;
	}
	else
		DBG_PRINT("Object Map is empty");

	return ret;
}

/** Join all loaded aliases within the model heirarchy with their source objects.*/
void Model::JoinAliases()
{
	JoinAliases(GetRootModel());
}

/** Join all loaded aliases for a given Model with their source objects.
	@param pRootMdl Pointer to the Model whose EvalAliases will be joined.
*/
void Model::JoinAliases(Model* pRootMdl)
{
	//aliases
	for (EvalAlias* pAlias : m_aliasArray)
	{
		pAlias->SetSrc((Evaluable*)pRootMdl->ObjectForKey(pAlias->GetSrcID()));
	}

	for (Model* pSubMdl : m_submodelArray)
	{
		pSubMdl->JoinAliases(pRootMdl);
	}

}

/** Call the resFunc function if available, and make the same call recursively to any available submodels
*/
void Model::Report(ModelReportFunction resFunc, OME_SCALAR reportTime)
{
	if(resFunc)
		resFunc(reportTime,this);

	for(int i=0; i<m_submodelArray.size(); i++)
		m_submodelArray[i]->Report(resFunc,reportTime);
}

/** Register current ModelEnums with an expression parser
@param evalParser the parser to register the ModelEnums with.
*/
void Model::RegisterEnums(OMEParser* evalParser)
{
	for(int i=0; i<m_modelEnums.size(); i++)
		evalParser->DefineEnumVals(m_modelEnums[i]);
}

/** Add an enumerated model class by decomposing a pattern.
	@param enumPattern Pattern used to define an enumerated type.
		The pattern template would be "class" : "value1" | "value2" | "value3" ...
*/
void Model::AddModelEnum(const STLString & enumPattern)
{
	StrArray subs = SplitString(enumPattern, ':');

	if (subs.size() == 2)
	{
		ModelEnum newEnum(subs[0]);

		StrArray vals = SplitString(subs[1], '|');
		for (unsigned int i = 0; i<vals.size(); ++i)
			newEnum.AddEnumValue(vals[i]);

		m_modelEnums.push_back(newEnum);
	}
}

/** Collect all Evaluable objects from this point in the model heirarchy.
@param out The array to append found Evaluables to.
@param depth Mode of traversal.

*/
void Model::GetAllEvals(EvalArray & out, const DT_TYPE & depth)
{
	int i;
	if(m_allEvals.empty())
		FindEvaluables();
	for(auto itr=m_allEvals.begin(); itr!=m_allEvals.end(); ++itr)
	{
		if ((*itr)->GetOMEType() != OME_SUBPORT && (*itr)->GetParentModel() == this &&
			(!(depth & DT_ASK) || (*itr)->GetAskVal())
			&& (!(depth & DT_NO_AUTOGEN) || !(*itr)->GetAutoGenerated())
			&& (!(depth & DT_CONSTS_ONLY) || (*itr)->GetIsConstant()))
			AppendIfUnique(out, (*itr));
		else if ((*itr)->GetOMEType() == OME_SUBPORT && ((SubmodelPort*)(*itr))->LinkedToFlow())
			AppendIfUnique(out, (*itr));
	}

	if (depth & DT_ALIAS)
	{
		for (Evaluable* pAlias : m_aliasArray)
		{
			out.push_back(pAlias);
		}
	}

	if (depth & DT_FULL)
	{
		for(i=0; i< m_submodelArray.size(); i++)
			m_submodelArray[i]->GetAllEvals(out,depth);
	}
}

/** Collect all Evaluable objects from this point in the model heirarchy.
@param out The array to append found Evaluables to.
@param depth The behavior of the search.

*/
void Model::GetAllEvals(EvalSet & out, const DT_TYPE & depth)
{
	int i;
	if(m_allEvals.empty())
		FindEvaluables();


	for (auto itr = m_allEvals.begin(); itr != m_allEvals.end(); ++itr)
	{
		if ((*itr)->GetOMEType() != OME_SUBPORT && (*itr)->GetParentModel() == this &&
			(!(depth & DT_ASK) || (*itr)->GetAskVal())
			&& (!(depth & DT_NO_AUTOGEN) || !(*itr)->GetAutoGenerated())
			&& (!(depth & DT_CONSTS_ONLY) || (*itr)->GetIsConstant()))
			out.insert(*itr);
		else if ((*itr)->GetOMEType() == OME_SUBPORT && ((SubmodelPort*)(*itr))->LinkedToFlow())
			out.insert(*itr);
	}
	

	if (depth & DT_ALIAS)
	{
		for (Evaluable* pAlias : m_aliasArray)
		{
			out.insert(pAlias);
		}
	}

	if (depth & DT_FULL)
	{
		for(i=0; i< m_submodelArray.size(); i++)
			m_submodelArray[i]->GetAllEvals(out,depth);
	}
}

/** Get a list of details for each accessible eval.
@param out The EvalRepArray to populate with EvalReps.
@param flags Designates any filters to apply upon fetching reps.
*/
void Model::GetEvalReps(EvalRepArray & out, const DT_TYPE & flags)
{
	if(m_allEvals.empty())
		FindEvaluables();

	bool constOnly = flags & DT_CONSTS_ONLY;
	bool includeAliases = flags & DT_ALIAS;
	bool noAutogen = flags & DT_NO_AUTOGEN;
	for(Evaluable* pE : m_allEvals)
	{
		if(!pE->GetInternal() && pE->GetOMEType()!=OME_INFLUENCE && pE->GetOMEType()!=OME_SUBPORT && (!constOnly || pE->GetIsConstant())
			&& (!noAutogen || !pE->GetAutoGenerated()))
			out.push_back(EvalRep(pE));
	}

	if (includeAliases)
	{
		for (EvalAlias* pEa : m_aliasArray)
		{
			if (!pEa->GetInternal()  && (!constOnly || pEa->GetSrc()->GetIsConstant()))
				out.push_back(EvalRep(pEa));
		}
	}
	if (flags & DT_FULL)
	{
		for (Model* pMdl : m_submodelArray)
			pMdl->GetEvalReps(out, flags);
	}

   /// \todo determine if the following comment block is needed for eval rep
   //if(out.size())
   //{
	  // for(auto itr=out.begin(); itr!=out.end(); ++itr)
	  // {
		 //  /// \todo update for dynamic counts
		 //  //add dot syntax to represent submodel
		 //  (*itr).name=m_name+"."+(*itr).name;
		 //  (*itr).id=m_id+"."+(*itr).id;
		 //  (*itr).isArray=true;
		 //  (*itr).count= m_instances.size();
	  // }
	  // out.insert(out.end(),out.begin(),out.end());
   //}
}

/** Assign evaluation priorities to all model objects.
@param basePriority The initial priority to begin with.
@param includeInitial Generate initialization priorities as well.
*/
unsigned int Model::PrioritizeObjects(unsigned int basePriority, const bool includeInitial)
{
	for (int m=0; m<m_submodelArray.size();m++)
		m_submodelArray[m]->PrioritizeObjects(basePriority,includeInitial);

	unsigned int maxEvalPriority=0, maxInitPriority=0, currPriority;

#ifdef OME_DBG_PRIORITIZE
	oSTLSStream dbg;
#endif

	for(auto itr=m_objectIDMap.begin(); itr!=m_objectIDMap.end(); ++itr)
	{
		OMEObject *curr=(*itr).second;
		if(curr->GetParentModel()==this && !curr->IsOMEType(OME_STATEVAR) && curr->GetOMEType()!=OME_SPAWNER)
		{
			currPriority= curr->Prioritize(basePriority);

#ifdef OME_DBG_PRIORITIZE
			dbg<<curr->GetName()<<", "<<curr->m_evalPriority<<"::"<<curr->m_initPriority<<std::endl;
#endif
			maxEvalPriority= currPriority > maxEvalPriority ? currPriority : maxEvalPriority;

			if(includeInitial)
			{
				currPriority= curr->Prioritize(basePriority, true);
				maxInitPriority= currPriority > maxInitPriority ? currPriority : maxInitPriority;
			}
		}
	}

	
	//make spawners the highest priorities, so that they are the first to be evaluated in submodels.
	for(auto itr=m_spawnerArray.begin(); itr!=m_spawnerArray.end(); ++itr)
	{
		(*itr)->Prioritize(maxEvalPriority);
		if (includeInitial)
		{
			(*itr)->Prioritize(maxInitPriority, true);
		}
	}
	
#ifdef OME_DBG_PRIORITIZE
	dbg<<maxEvalPriority<<endl;
	DBG_PRINT(dbg.str());
#endif


	m_prioritized=true;
	return maxEvalPriority;
}

/** Set all model object priorities to 0
*/
void Model::ClearPriorities()
{
	for(auto itr=m_objectIDMap.begin(); itr!=m_objectIDMap.end(); ++itr)
	{
		// ostringstream strm;
		// strm<<"ID: "<<(*itr).second->m_id;
		//DBG_PRINT(strm.str());
		(*itr).second->SetEvalPriority(0);
		(*itr).second->SetInitPriority(0);
	}

	std::for_each(m_submodelArray.begin(),m_submodelArray.end(),[](Model *pMc){pMc->ClearPriorities();});

}

void Model::EventAction(const CentralEvent & ev, void* extra)
{
	STLString eType=ev.GetEventType();
	if(eType==EV_MD_INIT)		//Initialization event
	{
		//grab start time, pointer to manager, and initialize model.
		OME_SCALAR startTime=ev.GetEventTime();
		SimManager* pSm=(SimManager*)ev.GetPayload();

		if(InitRun(startTime,pSm))//,true);
		{
			if(m_submodelArray.size())
			{
				for(auto itr=m_submodelArray.begin(); itr!=m_submodelArray.end(); ++itr)
				{
					(*itr)->EventAction(ev,extra);
				}
			}
		}
	}
	//else if(eType==EV_MD_RECORD)			//Record state event; save state to record stack
	//{
	//	RecordState(ev.GetEventTime());
	//}
	else if(eType==CED_ERR)
	{
		ErrCollection* errMsgs=(ErrCollection*)ev.GetPayload();
		if(errMsgs)
		{
			ErrCollection tempCollection;
			oSTLSStream msg;
			msg<<"Instance "<<"An instance of Model "<<m_name<<" has reported the following Errors:";
			tempCollection.AddErr(msg.str(),EC_HEADER);

			*errMsgs=tempCollection;

			POST_EVENT(CentralEvent(this,m_pSimManager,CED_ERR,m_pSimManager->GetPresentTime(), 0.0,CE_TOP_PRIORITY,errMsgs));
		}
	}
	else
	{
		OMEObject::EventAction(ev,extra);
	}
}

/** Initialize relevant model objects for a new set of simulation runs.
	@param startTime The initial time of the simulation run.
	@param sm Pointer to the parent Manager.
	@return true if all objects pass validation, false otherwise.
*/
bool Model::InitRun(const OME_SCALAR startTime, BaseManager* sm)
{
	bool ret=true;
	size_t i;
	if(!m_isInitialized)
	{
		//ClearInstances();
		//populate ind lists
		//PopulateIndLists();

		m_pSimManager=sm;
		//m_pSimManager->RegisterModel(this);
		m_iterations=0;

		//register listeners
		EVENT_DISPATCHER.RegisterEventListener(EV_MD_RECORD,(OMEObject*)this);

		//Link to manager's eval parser
		m_pEvalParser= m_pSimManager->GetPEvalParser();
		if (!m_pEvalParser->RequiresRuntimePrioritization())
			m_prioritized = true;
		//register any predefined enums.
		//	if(!m_modelEnums.empty())
		//		RegisterEnums(m_pEvalParser);

		//populate objectMap if it is empty
		if(m_objectIDMap.empty())
			RefreshObjectMaps();
		
		InitializeAccessPermissions();

		//initialize submodels, if necessary

		for(i=0; i<m_submodelArray.size() && ret; i++)
		{
			m_submodelArray[i]->m_stepSize=m_stepSize;
			ret=m_submodelArray[i]->InitRun(startTime,m_pSimManager);

		}

		m_isInitialized=true;

		if(m_stepSize<=m_reportInterval)
		{
			if(!m_preSpawned && !m_dataFilePaths.empty())
			{
				//ensure we have enough dataobjs, clear, and repopulate.
				m_dataSources.resize(m_dataFilePaths.size());
				i=0;
				for(auto itr=m_dataFilePaths.begin(); itr!=m_dataFilePaths.end();++itr,i++)
				{
					m_dataSources[i].Clear();
					m_dataSources[i].ReadCSV((*itr).second.m_path);
				}
			}

			int numInstances= CalcBaseNumInstances();
			EvalArray spawnEvals;

			if(m_pEvalParser)
			{
				for(i=0; i< m_spawnerArray.size(); i++)
				{
					m_spawnerArray[i]->Reset(m_pSimManager);
					EVENT_DISPATCHER.RegisterEventListener(CED_INCREMENT,m_spawnerArray[i]);
					spawnEvals.push_back(m_spawnerArray[i]);
					//numInstances+=m_spawnerArray[i]->Initialize(0.0);
				}
				//register for spawners?

				//assign priorities for initialization and runtime
				if(!m_prioritized)
					PrioritizeObjects(1,true);

				if(numInstances && !m_preSpawned)
				{
					//create record template
					SpawnInstances(numInstances);
					//InitializeFromDataSource();
					//InitializeFromSpatialSource();
					if(!m_subAssocArray.empty())
						DistributeSubAssocs();
				}

				RegisterWithManager();

			}
		}
		else
		{
			//return error here
			ret = false;
			ReportErrToMgr("Step Size is greater than report interval.",this);
		}
	}

	return ret;
}


/** Add a parameter with a scalar value.
Parameter's parent model set to this.
@param name Name of target object.
@param val The value to assign.
@return index of added object.
*/
int Model::AddParameter(const STLString & name,const OME_SCALAR & val) 
{ 
	int ret = m_paramArray.size();
	m_paramArray.emplace_back(name, val);
	return ret;
}

/** Add a parameter associated with a spatial data provider.
Parameter's parent model set to this.
@param name Name of target object.
@param pME Pointer to the SDPMapEntry to associate with Param.

@return index of added object.
*/
int Model::AddParameter(const STLString & name, SDPMapEntry* pME)
{
	int ret = m_paramArray.size();
	m_paramArray.emplace_back(name, pME);
	return ret;
}

/** Initialize and add a Param object referencing an external data source.
	@param name The name of the data source.
	@return internal index of the added Param. 
*/
int Model::AddDSParameter(const STLString & name)
{
	int ret = m_paramArray.size();
	m_paramArray.emplace_back(name, PARAM_DATA);
	return ret;
}

/** Add a previously defined Param object.
	@param param The Param to add.
	@return internal index of the added Param.
*/
int Model::AddParameter(const Param & param)
{
	int ret = m_paramArray.size();
	m_paramArray.emplace_back(param);
	return ret;
}

/** Add a variable.
Variables's parent model set to this.
@param pVar the variable to add.
@return index of added object.
*/
int Model::AddVariable( Variable *pVar )
{ 
	return AddItem<Variable*>(pVar,m_varArray);
}

/** Add a state variable.
State variable's parent model set to this.
@param pStateVar the state variable to add.
@return index of added object.
*/
int Model::AddStateVar( StateVar *pStateVar ) 
{ 
	return AddItem<StateVar*>(pStateVar,m_stateVarArray);
}

/** Add a flow.
Flow's parent model set to this.
@param pFlow the flow to add.
@return index of added object.
*/
int Model::AddFlow( Flow *pFlow )
{
	return AddItem<Flow*>(pFlow,m_flowArray);
}

/** Add an influence.
Influence's parent model set to this.
@param pInfluence the influence to add.
@return index of added object.
*/
int Model::AddInfluence( Influence *pInfluence ) 
{
	return AddItem<Influence*>(pInfluence,m_influenceArray);
}

/** Add a submodel.
Submodel's parent model set to this.
@param pSubmodel the submodel to add.
@return index of added object.
*/
int Model::AddSubmodel( Model *pSubmodel ) 
{
	return AddItem(pSubmodel,m_submodelArray);
}

/** Add a SubmodelPort.
@param pPort the SubmodelPort to add.
@param setParent parent set to this if true, unmodified if false.
@return index of added object.
*/
int Model::AddSubPort( SubmodelPort *pPort, bool setParent )
{
	m_portArray.push_back(pPort);
	int ret=m_portArray.size()-1;

	if(ret>=0 && setParent)
		pPort->SetParentModel(this);
	return ret;
}

/** Add an AssocPort.
@param pPort the AssocPort to add.
@param setParent parent set to this if true, unmodified if false.
@return index of added object.
*/
int Model::AddAssocPort(AssocPort *pPort, bool setParent)
{
	m_assocPortArray.push_back(pPort);
	int ret = m_assocPortArray.size() - 1;

	if (ret >= 0 && setParent)
		pPort->SetParentModel(this);
	return ret;
}

/** Add an IterConditional.
@param pIc the IterConditional to add.
@return index of added object.
*/
int Model::AddIterConditional( IterConditional* pIc ) 
{
	return AddItem<IterConditional*>(pIc,m_iterCondArray);
}

/** Add a SimpleLabel.
	@param pLbl the SimpleLabel to add.
	@return index of added object.
*/
int Model::AddLabel(SimpleLabel* pLbl)
{
	return AddItem<SimpleLabel*>(pLbl,m_labelArray);
}

/** Add Model to the tail end of an association.
	@param pSma Pointer to the submodel Association with the tail.
	@return Count of associated SubmodelAssoc tails.
*/
int Model::AddBaseMCAssoc(SubmodelAssoc* pSma)
{
	if(ObjectPresent<SubmodelAssoc*>(pSma, m_baseAssocArray)<0)
		m_baseAssocArray.push_back(pSma);
	
	return m_baseAssocArray.size();
}

/** Add Model to the head end of an association.
	@param pSma Pointer to the submodel Association with the head.
	@return Count of associated SubmodelAssoc heads.
*/
int Model::AddSubMCAssoc(SubmodelAssoc* pSma)
{
	if (ObjectPresent<SubmodelAssoc*>(pSma, m_subAssocArray) < 0)
	{
		pSma->SetParentModel(this);
		m_subAssocArray.push_back(pSma);
	}
	return m_subAssocArray.size();
}

/** Add Spawner to Model.
	@param pSpwn Pointer to Spawner to add.
	@return Internal index of Spawner.
*/
int Model::AddSpawner(Spawner *pSpwn)
{
	return AddItem<Spawner*>(pSpwn,m_spawnerArray);
}

/** Add Alias to Model.
@param pA Pointer to the EvalAlias to add.
@return Internal index of EvalAlias.
*/
int Model::AddAlias(EvalAlias *pA)
{
	return AddItem<EvalAlias*>(pA, m_aliasArray);
}

/** Remove a Param from the model.
@param pParam the Param to remove.
*/
void Model::RemoveParameter(Param* pParam)
{
	for (auto itr = m_paramArray.begin(); itr != m_paramArray.end(); ++itr)
	{
		if (&(*itr) == pParam)
		{
			m_paramArray.erase(itr);
			break;
		}
	}
}

/** Remove a Param from the model.
@param index The index of the param to remove.
*/
void Model::RemoveParameter( const int & index )
{
	m_paramArray.erase(m_paramArray.begin() + index);
}

/** Remove a Variable from the model
@param pVar the Variable to remove.
*/
void Model::RemoveVariable( Variable *pVar )
{
	RemoveItem<Variable*>(pVar,m_varArray);
}

/** Remove a StateVar from the model
@param pStateVar the StateVar to remove.
*/
void Model::RemoveStateVar( StateVar *pStateVar )
{
	RemoveItem<StateVar*>(pStateVar,m_stateVarArray);
}

/** Remove a Flow from the model
@param pFlow the Flow to remove.
*/
void Model::RemoveFlow( Flow *pFlow )
{	
	RemoveItem<Flow*>(pFlow,m_flowArray);
}

/** Remove an Influence from the model
@param pInfluence the Influence to remove.
*/
void Model::RemoveInfluence( Influence *pInfluence )
{
	RemoveItem<Influence*>(pInfluence,m_influenceArray);
}

/** Remove a Submodel from the model
@param pModel the submodel to remove.
*/
void Model::RemoveSubmodel( Model *pModel )
{
	RemoveItem(pModel,m_submodelArray);
}

/** Remove an IterConditional from the model
@param pIc the IterConditional to remove.
*/
void Model::RemoveIterConditional( IterConditional *pIc )
{
	RemoveItem<IterConditional*>(pIc,m_iterCondArray);
}

/** Remove a SubmodelPort from the model
@param pPort the SubmodelPort to remove.
*/
void Model::RemoveSubPort( SubmodelPort *pPort)
{
	RemoveItem<SubmodelPort*>(pPort,m_portArray);
}

/** Remove an AssocPort from the model
@param pPort the AssocPort to remove.
*/
void Model::RemoveAssocPort(SubmodelPort *pPort)
{
	RemoveItem<AssocPort*>(pPort, m_assocPortArray);
}

/** Remove a SimpleLabel from the model
	@param pLbl the SimpleLabel to remove.
*/
void Model::RemoveLabel(SimpleLabel* pLbl)
{
	RemoveItem<SimpleLabel*>(pLbl,m_labelArray);
}

/** UnAssociate from the tail end of a SubmodelAssoc.
	@param pSma Pointer to the SubmodelAssoc to disassociate from.
*/
void Model::RemoveBaseMCAssoc(SubmodelAssoc* pSma)
{
	RemoveItem<SubmodelAssoc*>(pSma,m_baseAssocArray);
}

/** UnAssociate from the head end of a SubmodelAssoc.
	@param pSma Pointer to the SubmodelAssoc to disassociate from.
*/
void Model::RemoveSubMCAssoc(SubmodelAssoc* pSma)
{
	RemoveItem<SubmodelAssoc*>(pSma,m_subAssocArray);
}

/** Remove a specific Spawner from a model.
	@param pSpwn Pointer to Spawner to remove.
*/
void Model::RemoveSpawner( Spawner* pSpwn )
{
	RemoveItem<Spawner*>(pSpwn,m_spawnerArray);
}

/** Remove a specific Alias from a model.r
@param pA Pointer to the Alias to remove.
*/
void Model::RemoveAlias(EvalAlias* pA)
{
	RemoveItem<EvalAlias*>(pA, m_aliasArray);
}

/** Add an OMEObject-derived object.
Object is added to appropriate list base on GetOMEType. Object's parent model set to this.
@param pObj the object to add.
@return index of added object.
*/
int Model::AddObject( OMEObject* pObj	)
{
	int ret=-1;
	oSTLSStream errMsg;
	switch(pObj->GetOMEType())
	{
	case OME_STATEVAR:
		ret=AddItem(pObj,m_stateVarArray);
		break;
	case OME_VARIABLE:
	case OME_VARARRAY:
	case OME_TSVAR:
		ret=AddItem(pObj,m_varArray);
		break;
	
	case OME_FLOW:
		ret=AddItem(pObj,m_flowArray);
		break;
	case OME_INFLUENCE:
		ret=AddItem(pObj,m_influenceArray);
		break;
	case OME_MODEL:
		ret=AddItem(pObj,m_submodelArray);
		break;
	case OME_SUBPORT:
		m_portArray.push_back( (SubmodelPort*)pObj );
		ret=m_portArray.size();

		//only assign parent model if there isn't one already
		if(!pObj->GetParentModel())
			pObj->SetParentModel(this);
		break;
	case OME_ASSOCPORT:
		m_assocPortArray.push_back((AssocPort*)pObj);
		ret = m_assocPortArray.size();

		//only assign parent model if there isn't one already
		if (!pObj->GetParentModel())
			pObj->SetParentModel(this);
		break;
	case OME_ITERCOND:
		ret=AddItem(pObj,m_iterCondArray);
		break;
	case OME_LABEL:
		ret=AddItem(pObj,m_labelArray);
		break;
	case OME_SUBASSOC:
		ret=AddItem(pObj,m_subAssocArray);
		break;
	case OME_SPAWNER:
		ret=AddItem(pObj,m_spawnerArray);
		break;
	case OME_ALIAS:
		ret = AddItem(pObj, m_aliasArray);
		break;
	default:
		errMsg<<"Unknown OME Type: "<<pObj->GetID()<<". Insertion Failed"<<endl;
			cerr<<errMsg.str();
	};
	return ret;
}

/** Remove an object from the model.
@param pObj the object to remove.
*/
void Model::RemoveObject( OMEObject* pObj	)
{
	if(pObj)
	{
		oSTLSStream errMsg;
		switch(pObj->GetOMEType())
		{
		case OME_STATEVAR:
			RemoveItem<StateVar*>(pObj,m_stateVarArray);
			break;
		case OME_VARIABLE:
		case OME_VARARRAY:
		case OME_TSVAR:
			RemoveItem<Variable*>(pObj,m_varArray);
			break;

		case OME_FLOW:
			RemoveItem<Flow*>(pObj,m_flowArray);
			break;
		case OME_INFLUENCE:
			RemoveItem<Influence*>(pObj,m_influenceArray);
			break;
		case OME_MODEL:
			RemoveItem<Model*>(pObj,m_submodelArray);
			break;
		case OME_SUBPORT:
			RemoveItem<SubmodelPort*>(pObj,m_portArray);
			break;
		case OME_ASSOCPORT:
			RemoveItem<AssocPort*>(pObj, m_assocPortArray);
			break;
		case OME_ITERCOND:
			RemoveItem<IterConditional*>(pObj,m_iterCondArray);
			break;
		case OME_LABEL:
			RemoveItem<SimpleLabel*>(pObj,m_labelArray);
			break;
		case OME_SUBASSOC:
			RemoveItem<SubmodelAssoc*>(pObj,m_subAssocArray);
		case OME_SPAWNER:
			RemoveItem<Spawner*>(pObj,m_spawnerArray);
		break;
		case OME_ALIAS:
			RemoveItem<EvalAlias*>(pObj, m_aliasArray);
			break;
		default:
			errMsg<<"Unkown OME Type: "<<pObj->GetID()<<". Removal Failed";
				cerr<<errMsg.str();
		};
		if(pObj->GetParentModel()==this)
			pObj->SetParentModel(NULL);
	}
}

/** Reconstruct the internal objectMap.
The object map is reconstructed based on the contents of the various element type arrays in the model.
*/
void Model::RefreshObjectMaps(const bool & recurse)
{
	m_objectIDMap.clear();
	m_objectNameMap.clear();
	int i;

	for(i=0; i < m_varArray.size(); i++)
	{
		AddToObjectIDMap(m_varArray[i]);
		AddToObjectNameMap(m_varArray[i]);
	}

	for(i=0; i < m_stateVarArray.size(); i++)
	{
		AddToObjectIDMap(m_stateVarArray[i]);
		AddToObjectNameMap(m_stateVarArray[i]);
	}

	for(i=0; i < m_flowArray.size(); i++)
	{
		AddToObjectIDMap(m_flowArray[i]);
		AddToObjectNameMap(m_flowArray[i]);
	}

	for(i=0; i < m_influenceArray.size(); i++)
		AddToObjectIDMap(m_influenceArray[i]);

	for(i=0; i < m_submodelArray.size(); i++)
	{
		AddToObjectIDMap(m_submodelArray[i]);
		//search for models explicitly using SubmodelForName() to avoid potential name collisions.
		//AddToObjectNameMap(m_submodelArray[i]);
	}

	for(i=0; i< m_portArray.size(); i++)
	{
		AddToObjectIDMap(m_portArray[i]);
		AddToObjectNameMap(m_portArray[i]);
	}

	for (i = 0; i< m_assocPortArray.size(); i++)
	{
		AddToObjectIDMap(m_assocPortArray[i]);
		AddToObjectNameMap(m_assocPortArray[i]);
	}

	for(i=0; i< m_iterCondArray.size(); i++)
	{
		AddToObjectIDMap(m_iterCondArray[i]);
		AddToObjectNameMap(m_iterCondArray[i]);
	}

	std::for_each(m_labelArray.begin(),m_labelArray.end(),[this](SimpleLabel* pSl){ AddToObjectIDMap(pSl);} );

	for(i=0; i<m_subAssocArray.size(); i++)
	{
		AddToObjectIDMap(m_subAssocArray[i]);
		AddToObjectNameMap(m_subAssocArray[i]);
	}

	for(i=0; i<m_spawnerArray.size(); i++)
	{
		AddToObjectIDMap(m_spawnerArray[i]);
		AddToObjectNameMap(m_spawnerArray[i]);
	}

	for (i = 0; i < m_aliasArray.size(); i++)
	{
		AddToObjectIDMap(m_aliasArray[i]);
		AddToObjectNameMap(m_aliasArray[i]);
	}

	//refresh eval cache
	FindEvaluables(true);

	if(recurse)
		std::for_each(m_submodelArray.begin(),m_submodelArray.end(),[](Model* pMdl){ pMdl->RefreshObjectMaps(true);});
}

/** Use all objects in the object map to populate the model.
During the construction process, objects can be stored in the object map, and then inserted in the proper places at a later step.
@param clearMap Clear the object map after the import process.
*/
void Model::ImportFromObjectMaps(bool clearMap)
{
	for(auto itr=m_objectIDMap.begin(); itr!=m_objectIDMap.end(); ++itr)
		AddObject((*itr).second);

	if(clearMap)
		m_objectIDMap.clear();
}

/** Register object with object ID map.
@param pVal object to register
*/
void Model::AddToObjectIDMap(OMEObject* pVal)
{
	m_objectIDMap[pVal->GetID()]=pVal;
}

/** Register object with object Name map.
@param pVal object to register
*/
void Model::AddToObjectNameMap(OMEObject* pVal)
{
	m_objectNameMap[pVal->GetExprName()]=pVal;
}

/** Find a pointer to an object within the model by id. Deep search.
@param key the id of the object to search for.
@return a pointer to the object if found; otherwise NULL.
*/
OMEObject* Model::ObjectForKey(const STLString key)
{
	if(m_objectIDMap.empty())
		RefreshObjectMaps();

	size_t scope=key.find('.');
	OMEObject* res=NULL;

	if(!key.empty())
	{
		if(scope==STLString::npos)
		{
			
			auto itr = m_objectIDMap.find(key);

			res = (itr != m_objectIDMap.end() ? (*itr).second : NULL);

			//recursively search submodels
			if (!res)
			{
				//res = ObjectForExprName(key);

				for (auto modItr = m_submodelArray.begin(); !res && modItr != m_submodelArray.end(); ++modItr)
					res = (*modItr)->ObjectForKey(key);
			}


			if (!res && GetID() == key)
				res = this;
		}
		else
		{
			STLString model=key.substr(0,scope);
			STLString subQuery=key.substr(scope+1);
			
			//sometimes root model path with start with itself; if so, restart search minus first component
			Model* pM;
			
			pM= dynamic_cast<Model*>(ObjectForKey(model));
			if (!pM && model == GetID())
				pM = this;
			if(pM)
				res=pM->ObjectForKey(subQuery);
		}
	}
	return res;
}

/** Find object that resides somewhere in the nested model hierarchy.
	@param path Delimited path to where object resides in heirarchy.
	@param delim The delimiting character to use to separate path components.
	@return A pointer to the object if found; NULL otherwise.
*/
OMEObject* Model::NestedObjectForName(const STLString path, const OMEChar delim)
{
	StrArray parts=SplitString(path,delim);
	StrArray::const_iterator sItr=parts.begin();

	//only increment if there is more than 1 part; otherwise might miss search
	//that is not actually a path.
	if(parts.size() > 1 && *sItr==GetExprName())
		++sItr;
	return NestedObjectForName(sItr,parts.end());
}

/** Find object that resides somewhere in the nested model hierarchy using a collection of path components.
	@param sItr pointer to the start of the path StrArray.
	@param eItr pointer the the end of the path StrAray.
	@return A pointer to the object if found; NULL otherwise.
*/
OMEObject* Model::NestedObjectForName(StrArray::const_iterator sItr,StrArray::const_iterator eItr)
{
	//discard this model's name
	OMEObject* out= NULL;
	if(sItr!=eItr)
	{
		if (sItr + 1 != eItr)
		{
			//intermediate entry; must be model
			Model* pSub = SubmodelForExprName(*sItr, false);
			++sItr;
			out = pSub->NestedObjectForName(sItr, eItr);
		}
		else
		{
			//tail entry; must be object
			out=ObjectForExprName(ScrubName(*sItr),false);
		}
	}
	return out;
}

/** Find a pointer to an object within the model by name.
@param name The name of the object to search for.
@param recurse If true, search will continue into submodels; if false, only this submodel is searched.
@return a pointer to the object if found; otherwise NULL.
*/
OMEObject* Model::ObjectForName(const STLString name,const bool & recurse)
{
	return ObjectForExprName(ScrubName(name),recurse);
}

/** Find a pointer to an object of a given type within the model by name.
@param name The name of the object to search for.
@param oType The type of object to search for.
@param recurse If true, search will continue into submodels; if false, only this submodel is searched.
@return a pointer to the object if found; otherwise NULL.
*/
OMEObject* Model::ObjectForName(const STLString name, const OMETYPE & oType, const bool & recurse)
{
	return ObjectForExprName(ScrubName(name),oType, recurse);
}

/** Find a pointer to an object within the model by expression-friendly name.
@param name The name of the object to search for.
@param recurse If true, search will continue into submodels; if false, only this submodel is searched.
@return a pointer to the object if found; otherwise NULL.
*/
OMEObject* Model::ObjectForExprName(const STLString name, const bool & recurse)
{
	if (m_objectIDMap.empty())
		RefreshObjectMaps();

	OMEObject* res = NULL;

	if (!name.empty())
	{
		auto itr = m_objectNameMap.find(name);
		res = (itr != m_objectNameMap.end() ? (*itr).second : NULL);
		if (!res && recurse)
		{
			for (int i = 0; !res && i<m_submodelArray.size(); i++)
				res = m_submodelArray[i]->ObjectForExprName(name, recurse);
		}
	}

	return res;
}

/** Find a pointer to an object of a given type within the model by expression-friendly name.
@param name The name of the object to search for.
@param oType The type of object to search for.
@param recurse If true, search will continue into submodels; if false, only this submodel is searched.
@return a pointer to the object if found; otherwise NULL.
*/
OMEObject* Model::ObjectForExprName(const STLString name, const OMETYPE & oType, const bool & recurse)
{
	if (m_objectIDMap.empty())
		RefreshObjectMaps();

	OMEObject* res = NULL;

	if (!name.empty())
	{
		auto itr = m_objectNameMap.find(name);
		res = (itr != m_objectNameMap.end() && (*itr).second->IsOMEType(oType) ? (*itr).second : NULL);

		if (!res && recurse)
		{
			for (int i = 0; !res && i<m_submodelArray.size(); i++)
				res = m_submodelArray[i]->ObjectForExprName(name, recurse);
		}
	}

	return res;
}

/** Find a pointer to an object within the model by case-insensitive name.
	Comparison is slower than ObjectForName(), so only use if case insensitive comparison is needed.
@param name The name of the object to search for.
@param recurse If true, search will continue into submodels; if false, only this submodel is searched.
@return a pointer to the object if found; otherwise NULL.
*/
OMEObject* Model::ObjectForInsensitiveName(const STLString name, const bool & recurse)
{
	if (m_objectIDMap.empty())
		RefreshObjectMaps();

	OMEObject* res = NULL;

	if (!name.empty())
	{
		STLString lName = ToLowerCase(name);
		auto itr = m_objectNameMap.begin();
		for (; itr != m_objectNameMap.end(); ++itr)
		{
			STLString key = ToLowerCase((*itr).first);

			if (key == lName)
				break;
		}

		res = (itr != m_objectNameMap.end() ? (*itr).second : NULL);
		if (!res && recurse)
		{
			for (int i = 0; !res && i<m_submodelArray.size(); i++)
				res = m_submodelArray[i]->ObjectForInsensitiveName(name, recurse);
		}
	}

	return res;
}

/** Find the submodel with the common name.
	@param name The common name to search for.
	@param recurse If true, search all submodels until a match is found or all models are exhausted.
	@return A pointer to the Model matching name, or NULL if no match is found.
*/
Model* Model::SubmodelForName(const STLString & name, bool recurse)
{
	return SubmodelForExprName(ScrubName(name), recurse);
}

/** Find the submodel with the expression-friendly name.
	@param name The expression-friendly name to search for.
	@param recurse If true, search all submodels until a match is found or all models are exhausted.
	@return A pointer to the Model matching name, or NULL if no match is found.
*/
Model* Model::SubmodelForExprName(const STLString & name, bool recurse)
{
	Model* ret = NULL;

	for (auto itr = m_submodelArray.begin(); !ret && itr != m_submodelArray.end(); ++itr)
	{
		if ((*itr)->GetExprName() == name)
			ret = (*itr);
	}

	if (!ret && recurse)
	{
		for (auto itr = m_submodelArray.begin(); !ret && itr != m_submodelArray.end(); ++itr)
		{
			ret = (*itr)->SubmodelForExprName(name, recurse);
		}
	}

	return ret;
}

/**
Update name of an OMEObject in the Name lookup map.
@param oldName The original name of the object.
@param newName The new name of the object.
*/
void Model::UpdateObjectName(const STLString oldName, const STLString newName)
{
	auto itr= m_objectNameMap.find(oldName);
	if(itr!= m_objectNameMap.end())
	{
		m_objectNameMap[newName]=m_objectNameMap[oldName];
		m_objectNameMap.erase(oldName);
	}
}

/** Clears all entries from the object map */
void Model::ClearObjectMaps()
{
	m_objectIDMap.clear();
	m_objectNameMap.clear();
}

/** 
	Binds all SubmodelAssocs in model to their named models.
*/
void Model::JoinAssocs()
{
	std::for_each(m_subAssocArray.begin(),m_subAssocArray.end(),[this](SubmodelAssoc* pSa){ pSa->JoinFromReferences(this); });
}

/** 
	Collect all model objects that inherit from the Evaluable class, with the exception of influences.
	Influences are volatile (may be removed from the model at a later time) and only describe prioritization.
	@param noRebuild boolean value that prevents the object cache from rebuilding if it is empty. This
		is needed to avoid looping under certain conditions.
*/
void Model::FindEvaluables(const bool & noRebuild)
{
	m_allEvals.clear();
	if (m_objectIDMap.empty())
	{
		if (noRebuild)
			return;
		RefreshObjectMaps();
	}

	for(auto itr=m_objectIDMap.begin(); itr!= m_objectIDMap.end(); ++itr)
	{
		// don't count influences; they are only important for prioritization.
		if ((*itr).second->GetOMEType() != OME_INFLUENCE && (*itr).second->GetOMEType() != OME_ALIAS)
		{
			Evaluable* evalCast=dynamic_cast<Evaluable*>((*itr).second);
			if(evalCast)
				m_allEvals.push_back(evalCast);
		}
	}
}

/** Provide a list of Evaluables to be registered with an expression parser.
@param out The Evaluables array to populate.
@param includePorts If true, include ports in out. Since ports are shared,
it isn't always necessary to register them with every model.
*/
void Model::GetVarsToRegister(EvalArray & out, bool includePorts)
{

	out.insert(out.end(),m_stateVarArray.begin(),m_stateVarArray.end());
	out.insert(out.end(),m_flowArray.begin(),m_flowArray.end());
	out.insert(out.end(),m_varArray.begin(),m_varArray.end());
	out.insert(out.end(),m_iterCondArray.begin(),m_iterCondArray.end());

	//spawners - should these go here?
	out.insert(out.end(), m_spawnerArray.begin(), m_spawnerArray.end());

	if(includePorts)
		out.insert(out.end(),m_portArray.begin(),m_portArray.end());

}

/** Performs some model-specific postrun cleanup.

*/
void Model::Cleanup()
{

	std::for_each(m_dataSources.begin(),m_dataSources.end(),[](DataSrc & ds){ ds.Clear(); });

	for(int i=0; i<m_submodelArray.size(); i++)
		m_submodelArray[i]->Cleanup();
}

/** Remove influence objects and some submodel ports from models to reduce size.
	Influences are only used to prioritize evaluations; they can be removed one prioritization
	has been accomplished to reduce memory imprint. This change permanently alters models; if 
	influences are needed again, the model must be reloaded from source. SubmodelPorts associated with 
	influences will be removed as well, unless they are marked to be retained.
*/
void Model::PurgeInfluences(const bool recurse)
{

	//reconfig Submodel ports
	for(auto itr=m_portArray.begin(); itr!=m_portArray.end(); ++itr)
	{
		if((*itr)->GetParentModel()==this)
			(*itr)->PurgeInfluences();
	}

	//recurse here so as to avoid a submodel port removing an influence that has already been deleted.
	if(recurse)
		std::for_each(m_submodelArray.begin(),m_submodelArray.end(),[](Model* pMc){ pMc->PurgeInfluences(); });

	//maintain up/down object heirarchy for parsing purposes
	Influence* pInfl;
	Evaluable* pSrc,*pTrg;
	for(auto itr=m_influenceArray.begin(); itr!=m_influenceArray.end(); ++itr)
	{
		pInfl=*itr;

		pSrc=pInfl->GetPEvalSource();
		if (pSrc->IsOMEType(OME_ALIAS))
			pSrc = ((EvalAlias*)pSrc)->GetSrc();
		pTrg=pInfl->GetPEvalTarget();
		if (pTrg->IsOMEType(OME_ALIAS))
			pTrg = ((EvalAlias*)pSrc)->GetSrc();

		if(pSrc && pTrg)
			pSrc->AddDownObject(pTrg);
	}

	//remove influences
	DeallocPtrArray(m_influenceArray);
	
	//remove any SubmodelPorts that aren't flagged for retention
	//PortArray removePorts;
	//for (auto itr = m_portArray.begin(); itr != m_portArray.end(); ++itr)
	//{
	//	if (!(*itr)->GetIsEvaluated())
	//	{
	//		//bypass the port by binding all upstream objects with all downstream objects
	//		for (size_t i = 0; i < (*itr)->GetAllSourceCount(); ++i)
	//		{
	//			for (size_t j = 0; j < (*itr)->GetAllTargetCount(); ++j)
	//				(*itr)->GetAllSource(i)->AddDownObject((*itr)->GetAllTarget(j));
	//		}
	//		(*itr)->Detach();
	//		removePorts.push_back(*itr);
	//	}
	//}

	//SubtractObjects(m_portArray, removePorts);
	//std::for_each(removePorts.begin(), removePorts.end(), [](SubmodelPort* pPort){delete pPort; });

	//blow away model cache to force reconstitution on next call to GetAllEvals.
	m_allEvals.clear();

	//rebuild object map
	RefreshObjectMaps();
}

/** Purge any ports that are not used in model calculations.
	@param recurse If true all submodels will purge their unused ports.
*/
void Model::PurgeUnusedPorts(const bool recurse)
{
	//recurse here so as to avoid a submodel port removing an influence that has already been deleted.
	if (recurse)
		std::for_each(m_submodelArray.begin(), m_submodelArray.end(), [](Model* pMc){ pMc->PurgeUnusedPorts(); });

	//remove any SubmodelPorts that aren't flagged for retention
	PortArray removePorts;
	for (auto itr = m_portArray.begin(); itr != m_portArray.end(); ++itr)
	{
		if (!(*itr)->LinkedToFlow())
		{
			//bypass the port by binding all upstream objects with all downstream objects
			for (size_t i = 0; i < (*itr)->GetAllSourceCount(); ++i)
			{
				for (size_t j = 0; j < (*itr)->GetAllTargetCount(); ++j)
					(*itr)->GetAllSource(i)->AddDownObject((*itr)->GetAllTarget(j));
			}
			(*itr)->Detach();
			removePorts.push_back(*itr);
		}
	}

	SubtractObjects(m_portArray, removePorts);
	std::for_each(removePorts.begin(), removePorts.end(), [this](SubmodelPort* pPort){
		if(pPort->GetParentModel()==this)
		delete pPort; });

	//blow away model cache to force reconstitution on next call to GetAllEvals.
	m_allEvals.clear();

	//rebuild object map
	RefreshObjectMaps();
}

/** Retrieve a pointer to an EvalTable based on id.
	@param et Id of table to retreive.
	@return Pointer to EvalTable with id et, or NULL if no matching table is found.
*/
EvalTable* Model::GetEvalTable(const STLString & et)
{
	auto mapItr= m_tables.find(et);
	return mapItr!=m_tables.end() ? (*mapItr).second : NULL;
}

/** @return A StrArray containing the ids of all EvalTables managed by this Model object.*/
StrArray Model::GetIDsForTables() const
{
	StrArray ret;
	for(auto mItr=m_tables.begin(); mItr!=m_tables.end();++mItr)
		ret.push_back((*mItr).first);
	return ret;
}

/** Register all Evaluable Model Components for this model with the parent SimManager.

*/
void Model::RegisterWithManager() 
{ 
	if(m_pSimManager) 
		m_pSimManager->RegisterModel(this);
}

/** Tells the SimManager to perform an update operation.
@param time The time of the Update
*/
void Model::UpdateSupports(OME_SCALAR time)
{
	if(m_pSimManager) 
		m_pSimManager->UpdateSupportObjects(time);
}

/** Check to see if associated SimManager is in the middle of an integration step.
@return true if SimManager is in integration step; false otherwise.
*/
bool Model::IsInIntegrate()
{
	bool ret=false;
	if(m_pSimManager && m_pSimManager->GetHasStartedIntegrate())
		ret=true;

	return ret;
}

/** Determine if this model is marked for destruction by a given Spawner
@param pSpwn Pointer to the Spawner to check against.
@return true if Spawner has this model marked; false otherwise.
*/
bool Model::IsMarkedBySpawner(Spawner* pSpwn)
{
	bool ret=false;

	/// \todo update for new instance scheme

	//if(m_pSpawningParent && m_pSpawningParent->GetModelType()==MD_COLL)
	//	ret=((ModelCollection*)m_pSpawningParent)->IsMarkedBySpawner(m_currentInstance,pSpwn);
	return ret;
}

/** Loads submodels prior to the full initialization process. 
This is necessary for registration with Envision.
*/
void Model::PreloadInstances()
{
	DBG_PRINTELAPSED("Preload instances");

	int spawnCount = 0;

	if (IsDynamic())
	{
		//determine if any initial instances are needed.
		//for now, just add an instance for conditional spawner
		for (size_t i = 0; i < m_spawnerArray.size() && !spawnCount; i++)
		{
			if (m_spawnerArray[i]->IsConditional())
				spawnCount = 1;
		}
	}
	if((!IsDynamic() || spawnCount ) && !m_preSpawned)
	{
		int i;
		if(!m_dataFilePaths.empty())
		{
			//ensure we have enough dataobjs, clear, and repopulate.
			m_dataSources.resize(m_dataFilePaths.size());
			i=0;
			for(auto itr=m_dataFilePaths.begin(); itr!=m_dataFilePaths.end();++itr,i++)
			{
				m_dataSources[i].Clear();
				m_dataSources[i].ReadCSV((*itr).second.m_path);
			}
		}
		if (!spawnCount)
			spawnCount= CalcBaseNumInstances();
		//DBG_COLOR(std::cout<<m_name<<": "<<spawnCount<<" instances"<<std::endl,DBG_MAGENTA);
		//sim manager needs to be set for spawning to work

		//create record template
		SpawnInstances(spawnCount);
		//preload submodel instances for each sub instance
		//InitializeFromDataSource();
		//InitializeFromSpatialSource();
		if(!m_subAssocArray.empty())
			DistributeSubAssocs();

		//DBG_COLOR(cout<<m_name<<"::"<<spawnCount<<endl,DBG_MAGENTA);
	}

	m_preSpawned=true;
	for(auto itr=m_submodelArray.begin(); itr!=m_submodelArray.end(); ++itr)
		(*itr)->PreloadInstances();
}

/** Retrieve ModelEnum with the provided name.
	@param name The name of the ModelEnum class to search for.
*/
ModelEnum* Model::ModelEnumForName(STLString name)
{
	ModelEnum* ret = NULL;
	name = ScrubName(name);
	for (size_t i = 0; !ret && i < m_modelEnums.size(); ++i)
	{
		if (ScrubName(m_modelEnums[i].GetEnumClassName()) == name)
			ret = &m_modelEnums[i];
	}
	return ret;
}

/** Retrieve integer equivalant value for the named ModelEnm Enetry.
	@param name The name of the enumerated value to search for.
	@return the value of the enum pointed to by name, or -1 if the enumerated value is not found.
*/
short Model::ModelEnumValueForName(const STLString & name)
{
	short ret = -1;
	for (auto itr = m_modelEnums.begin(); itr != m_modelEnums.end() && ret == -1; ++itr)
		ret = (*itr).ValForLabel(name);

	return ret;
}

/** Bind a SubmodelAssoc to this model.
@param pSa A pointer to the SubmodelAssoc to be bound to.
@param instVal Number indicating the corresponding instance that the submodel will be associated
with in the opposite model.
*/
void Model::BindSubmodelAssoc(SubmodelAssoc* pSa, const int instVal)
{
	m_parentAssocMap[pSa]=instVal;
}

/**	Get the Association index for a given SubmodelAssoc if available.
@param pSa A pointer to the SubmodelAssoc for which to retrieve the Assoc index for.
@return An index that represents the corresponding instance in the opposing model, or -1 if no value is present.
*/
int Model::GetAssocIndex(const SubmodelAssoc* pSa) 
{
	auto ptr=m_parentAssocMap.find(pSa);
	int ret=-1;
	if(ptr!=m_parentAssocMap.end())
		ret=(*ptr).second;
	return ret; 
}

/** Determines if provided model is nested within this model.
@param pBm Pointer to the model to look for.
@param found Optional pointer to ModelList to populate with models in path to found between this and pBm
@return true if the model is nested in the current models sub-heirarchy; false otherwise.
*/
bool Model::IsNested(const Model* pBm, ModelList* found) const
{
	bool ret=false;

	if(this==pBm)
		ret=true;
	//search nested models
	for(auto itr=m_submodelArray.begin(); !ret && itr!=m_submodelArray.end(); ++itr)
	{
		ret=(*itr)->IsNested(pBm,found);
		if(ret && found)
			found->push_front(*itr);
	}

	return ret;
}

/** Recursively construct a nested path to pBm.
@param pBm Pointer to the target model.
@return A dot-delimited submodel path to pBm, if available.
*/
STLString Model::DoNestedPath(const Model* pBm) const
{
	//depth first

	STLString ret;

	STLString currName;
	currName=((Model*)this)->GetName();

	if(pBm==this)
		ret=currName;

	for(auto itr=m_submodelArray.begin(); ret.empty() && itr!=m_submodelArray.end(); ++itr)
	{
		ret=(*itr)->DoNestedPath(pBm);
		if(!ret.empty())
			ret=currName+"."+ret;
	}
	return ret;
}

/** Find path from this model to another submodel in the same heirarchy.
@param pBm The Model to look for.
@return A STLString with a dot-delimited path to pBm.
*/
STLString Model::GetNestedPath(const Model* pBm) const
{
	STLString ret;

	for(auto itr=m_submodelArray.begin(); ret.empty() && itr!=m_submodelArray.end(); ++itr)
		ret=(*itr)->DoNestedPath(pBm);
	return ret;
}


/** Get list of submodels from the root model down to this submodel.
@param outPath A list to store an ordered list of model names from root to this.
*/
void Model::GetUpPath(StrList & outPath)
{
	outPath.push_front(GetExprName());
	if(GetParentModel())
		GetParentModel()->GetUpPath(outPath);
}

/** Retrieve path as a period-delimited string.
	@param outPath The variable to hold the path on return.
*/
void Model::GetUpPath(STLString & outPath)
{
	StrList parts;
	GetUpPath(parts);
	outPath = ConcatStrings(parts, '.');
}

/** Clear initialization flag for this model and all submodels.
*/
void Model::ClearInitialization()
{
	m_isInitialized=false;
	m_prioritized=false;
	for(int i=0; i<m_submodelArray.size(); i++)
		m_submodelArray[i]->ClearInitialization();
}

/** Equivalence operator.
	@param rhs Model to compare values against.
	@return true if models are equivalent; false otherwise.
*/
bool Model::operator==(const Model & rhs)
{
	return this==&rhs ||
		(
		OMEObject::operator==(rhs) &&
		std::equal(m_paramArray.begin(),m_paramArray.end(),rhs.m_paramArray.begin()) &&
		std::equal(m_varArray.begin(),m_varArray.end(),rhs.m_varArray.begin()) &&
		std::equal(m_stateVarArray.begin(),m_stateVarArray.end(),rhs.m_stateVarArray.begin()) &&
		std::equal(m_flowArray.begin(),m_flowArray.end(),rhs.m_flowArray.begin()) &&
		std::equal(m_influenceArray.begin(),m_influenceArray.end(),rhs.m_influenceArray.begin()) &&
		std::equal(m_portArray.begin(),m_portArray.end(),rhs.m_portArray.begin()) &&
		std::equal(m_iterCondArray.begin(),m_iterCondArray.end(),rhs.m_iterCondArray.begin()) &&
		m_stepSize==rhs.m_stepSize &&
		//m_minStepSize==rhs.m_minStepSize &&
		//m_maxStepSize==rhs.m_maxStepSize &&
		m_reportInterval==rhs.m_reportInterval &&
		m_initTime==rhs.m_initTime &&
		m_preferredIntMethod==rhs.m_preferredIntMethod &&
		m_currentInstance[OME_CURR_THREAD]==rhs.m_currentInstance[OME_CURR_THREAD] &&
		m_isInitialized==rhs.m_isInitialized &&
		m_pSimManager==rhs.m_pSimManager &&
		std::equal(m_labelArray.begin(),m_labelArray.end(),rhs.m_labelArray.begin()) &&
		std::equal(m_subAssocArray.begin(),m_subAssocArray.end(),rhs.m_subAssocArray.begin()) &&
		std::equal(m_spawnerArray.begin(),m_spawnerArray.end(),rhs.m_spawnerArray.begin()) &&
		std::equal(m_instances.begin(),m_instances.end(),rhs.m_instances.begin())
		);
}

/** Allocate a new model instance based on this model.
	@return Pointer to the newly allocated model instance.
*/
ModelInstance* Model::GenerateInstance(const size_t & ind)
{
	if (m_pValArchive)
		m_pValArchive->AddSpaceForModel(this, 1);
	ModelInstance *pInst=new ModelInstance(this,ind,true);
	return pInst;
}

/** Allocate a new model instance based on a specific instance of this model.
	@param ind The index to associate with the generated instance.
	@param parent Index of the ModelInstance whose evaluation resulted in the creation of the new ModelInstance.
	@param pSpwn Optional pointer to Spawner that is responsible for generating the instance.
	@return Pointer to the newly allocated model instance.
*/
ModelInstance* Model::GenerateInstance(const size_t & ind,const size_t & parent, Spawner* pSpwn)
{
	if (m_pValArchive)
		m_pValArchive->AddSpaceForModel(this, 1);
	ModelInstance *pInst = new ModelInstance(*m_instances[parent],ind,parent,pSpwn);
	return pInst;
}
/** Retrieve pointer to a specific model instance.
	@param i index of the instance to retrieve.
	@return Pointer to specified model instance if index i is valid; NULL otherwise.
*/
ModelInstance* Model::GetInstance(size_t i)
{
	i+=GetParentOffset();
	return i< m_instances.size() ? m_instances[i] : NULL;
}

/** Get a list of all object keys that can be used to reference model objects
	@param depth The type of key search to perform.
	Available Options:
- __DT_SHALLOW:__ Only collect keys from this model level.
- __DT_NOINST:__ Not Used (defaults to DT_FULL).
- __DT_FULL:__ Collect keys from all submodels in addition to the calling model.
	@return A StrArray containing all valid keys for the model(s).
*/
StrArray Model::GetObjectKeys(const DT_TYPE & depth)
{
	StrArray out;
	if(m_objectIDMap.empty())
		RefreshObjectMaps();
	std::for_each(m_objectIDMap.begin(),m_objectIDMap.end(),[&out,&depth](const std::pair<std::string,OMEObject*> & p)
	{if(!(depth & DT_ASK) || !p.second->IsOMEType(OME_EVAL) || ((Evaluable*)p.second)->GetAskVal())
		out.push_back(p.first);});
	if(depth & DT_FULL)
	{
		StrArray temp;
		for(int i=0; i<m_submodelArray.size(); i++)
		{
			temp=m_submodelArray[i]->GetObjectKeys(depth);
			out.insert(out.end(),temp.begin(),temp.end());
		}
	}
	return out;
}

/** Retrieve all models included in the heirarchy starting with this Model.
	@param bma The ModelArray to populate.
*/
void Model::GetAllModels(ModelArray & bma)
{
	bma.push_back(this);
	std::for_each(m_submodelArray.begin(),m_submodelArray.end(),[&bma](Model* pMc){pMc->GetAllModels(bma);});
}

/** Update the number of reported Instances. 
	This is used to prevent newly spawned instances from being manipulated by successive spawners.
*/
void Model::UpdateNumInstances()
{
	OME_SCALAR newInstCount=0.0;

	std::for_each(m_spawnerArray.begin(), m_spawnerArray.end(),[&newInstCount](Spawner* pS){ newInstCount+=pS->GetValue(); });

	if(newInstCount>0.0)
		SpawnInstances((int)newInstCount);
	else
	{
		//kill
	}
}

/** Checks to see if two Models share a common SubmodelAssoc.
	@param pMc Pointer to the Model to query against.
	@return true if an association is shared; false otherwise.
*/
bool Model::IsAssociated(Model* pMc) const
{
	bool ret=false;

	if(pMc)
	{
		for(auto itr=m_subAssocArray.begin(); !ret && itr!=m_subAssocArray.end(); ++itr)
				ret=(*itr)->IsAssociated(pMc);
		if(!ret)
		{
			for(auto itr=m_baseAssocArray.begin(); !ret && itr!=m_baseAssocArray.end(); ++itr)
				ret=(*itr)->IsAssociated(pMc);
		}
	}

	return ret;
}



/** Determine the initial number of instances that will be produced at the beginning of a simulation run.
	@return the number of instances that will be initially spawned.
*/
int Model::CalcBaseNumInstances()
{
	int ret=m_instMode==RECORD ? GetMaxDataSourceRowCount() : m_initialNumInstances;
		
		oSTLSStream out;
		//out<<m_name<<": "<<ret<<" instances";
		//DBG_PRINT(out.str());

	/// \todo add second layer of indexing
	if(!m_subAssocArray.empty())
		std::for_each(m_subAssocArray.begin(),m_subAssocArray.end(),[&ret](SubmodelAssoc* pSa){ret*=pSa->GetBaseNumInstances(); });
	
	//ret*=GetTotParentInstances();

	return ret;
}

/** Bind instances to the appropriate relationships in all referencing SubmodelAssocs. */
void Model::DistributeSubAssocs()
{
	unsigned int a,b,i;
	unsigned int currInd,sizeA,sizeB;

	/// \todo update for new instances

	//switch(m_subAssocArray.size())
	//{
	//case 0: //do nothing
	//	break;
	//case 1:
	//	//single role
	//	for(a=0; a<m_subAssocArray[0]->GetBaseNumInstances(); a++)
	//	{
	//		for(i=0; i<m_initialNumInstances; i++)
	//		{
	//			currInd=a+i*m_initialNumInstances;
	//			m_instances[currInd]->BindSubmodelAssoc(m_subAssocArray[0],a);
	//		}
	//	}
	//	break;
	//case 2:
	//	//two roles
	//	sizeA=m_subAssocArray[0]->GetBaseNumInstances();
	//	sizeB=m_subAssocArray[1]->GetBaseNumInstances();

	//	for(a=0; a<sizeA; a++)
	//	{
	//		for(b=0; b<sizeB; b++)
	//		{
	//			for(i=0; i<m_initialNumInstances; i++)
	//			{
	//				currInd=a*sizeB+b+i*m_initialNumInstances;
	//				m_instances[currInd]->BindSubmodelAssoc(m_subAssocArray[0],a);
	//				m_instances[currInd]->BindSubmodelAssoc(m_subAssocArray[1],b);
	//			}
	//		}
	//	}

	//	break;
	//default:
	//	DBG_PRINT_FLAGGED("MORE THAN 2 assocs pointing to submodel",DBG_ERR);
	//	
	//}

	
}

/** Get instances for SubmodelAssoc and sort by Assoc Index.
	@param pSa Pointer to the SubmodelAssoc to consider.
	@return A MdlInstanceArray containing the relevant instances sorted by AssocIndex.
*/
ModelArray Model::GetInstancesForAssoc(SubmodelAssoc* pSa)
{
	/// \todo rework for new instance setup.
	ModelArray ret;

	//for(int i=0; i< m_instances.size(); i++)
	//{
	//	if(m_instances[i]->BoundToAssoc(pSa))
	//		ret.push_back(m_instances[i]);
	//}

	//std::sort(ret.begin(),ret.end(),[pSa](BaseModel* lhs, BaseModel* rhs){return lhs->GetAssocIndex(pSa)<rhs->GetAssocIndex(pSa);});
	return ret;
}

/** Attempt to initialize a specific Evaluable object from available Data sources.
	@param pEval Pointer to the Evaluable object to initialize.
	@return true if an initialization value is found; false otherwise.
*/
bool Model::InitializeEvalFromDataSource(Evaluable* pEval)
{
	bool ret = false;

	int indCol = -1;
	int indCol2;
	int dSInd = -1;

	DataSourceEntry* pDsEntry = NULL;

	//check to see if Eval has listing in any of the data sources
	for (int i = 0; i < m_dataSources.size() && dSInd == -1; i++)
	{
		pDsEntry = &m_dataFilePaths[m_dataSources[i].GetSrcPath().c_str()];
		if (std::find(pDsEntry->m_varNames.begin(), pDsEntry->m_varNames.end(), pEval->GetName()) != pDsEntry->m_varNames.end())
			dSInd = i;
	}

	if (dSInd != -1)
	{
	
		//entry found; determine what type of listing
		if (pDsEntry->m_colMappings.empty())
		{
			indCol = m_dataSources[dSInd].GetFieldCol(pEval->GetName().c_str());
			if (indCol != -1)
			{
				pEval->SetExpr(STLString());
				if (pEval->GetOMEType() != OME_TSVAR)
				{
					FindColValForEval(dSInd, indCol, pEval);
				}
				else
					((TimeSeriesVar*)pEval)->SetSeries(m_dataSources[dSInd].GetNumValsForCol(indCol));

				ret = true;
			}
		}
		else
		{
			//m_colMappings is a multimap, meaning multiple entries can have the same key.
			for (auto itr = pDsEntry->m_colMappings.begin(); itr != pDsEntry->m_colMappings.end(); ++itr)
			{
				if ((*itr).second.m_label == pEval->GetName())
				{
					indCol = m_dataSources[dSInd].GetFieldCol((*itr).first.c_str());
					switch (pEval->GetOMEType())
					{
					case OME_TSVAR:
					{
						TimeSeriesVar* pTSCast = (TimeSeriesVar*)pEval;
						//set interval (can be zero)
						pTSCast->SetInterval((*itr).second.m_interval);
						//set non-default interp mode
						if ((*itr).second.m_interp)
							pTSCast->SetInterpolation(static_cast<TimeSeriesVar::INTERPMODE>((*itr).second.m_interp));
						//assign values
						if ((*itr).second.m_indCol.empty())
							pTSCast->SetSeries(m_dataSources[dSInd].GetNumValsForCol(indCol));
						else if ((indCol2 = m_dataSources[dSInd].GetFieldCol((*itr).second.m_indCol.c_str()) >= 0))
						{
							//vals in ind_col are times for value in col e for a given row/record
							OME_SCALAR time, val;
							for (int r = 0; r < m_dataSources[dSInd].GetRowCount(); r++)
							{
								m_dataSources[dSInd].GetNumVal(indCol2, r, time);
								m_dataSources[dSInd].GetNumVal(indCol, r, val);
								pTSCast->AddTimeVal(time, val);
							}
						}
						ret = true;
					}
						break;
					case OME_VARARRAY:
					{
						VarArray* pVa = (VarArray*)pEval;
						size_t vOffset = GetParentOffset()*pVa->GetSize();
						OME_SCALAR tmp;
						if ((*itr).second.m_inds.empty())
						{
							for (int r = 0; r < pVa->GetSize(); r++)
							{
								m_dataSources[dSInd].GetNumVal(indCol, r + vOffset, tmp);
								pVa->SetValue(r, tmp);
							}
						}
						else
							FindDSIndexedVal(m_dataSources[dSInd], indCol, (*itr).second, pVa);
						ret = true;
					}
						break;
					default:
					{
						if ((*itr).second.m_inds.empty())
							FindColValForEval(dSInd, indCol, pEval);
						else
							FindDSIndexedVal(m_dataSources[dSInd], indCol, (*itr).second, pEval);
						ret = true;
					}
					}
				}
			}
		}
	}
	//make sure any variables marked to be filled in are cleared
	if (ret == true)
		pEval->SetAskVal(false);
	else if (m_pParentModel)		//try eval in parent (sometimes heirarchical
		ret=m_pParentModel->InitializeEvalFromDataSource(pEval);

	return ret;
}

/** Unpack all model component expressions which have compressed expression functions.*/
void Model::UnpackModelComponentExpressions()
{
	if (!m_isUnPacked)
	{
		EvalArray evals;
		GetAllEvals(evals, DT_SHALLOW);
		for (Evaluable* pE : evals)
			OMEParser::PerformUnPack(pE);
		for (Model* pSubMdl : m_submodelArray)
			pSubMdl->UnpackModelComponentExpressions();

		m_isUnPacked = true;
	}
}

/** Pack all model component expressions which have compressed expression functions.*/
void Model::PackModelComponentExpressions()
{
	if (m_isUnPacked)
	{
		EvalArray evals;
		GetAllEvals(evals, DT_SHALLOW);
		for (Evaluable* pE : evals)
			OMEParser::PerformPack(pE);
		for (Model* pSubMdl : m_submodelArray)
			pSubMdl->PackModelComponentExpressions();

		m_isUnPacked = false;
	}
}

/** Retrieve a column value from a given data source for the current instance of a given Evaluable.
	@param dSInd Index of the DataSource to retrieve the value from.
	@param indCol Index of the column where the value will reside.
	@param pEval Pointer to the Evaluable object to retrieve the value for.
*/
void Model::FindColValForEval(const int & dSInd,const int & indCol,Evaluable* pEval)
{
	size_t indStep = 0;
	Model* pCurrMdl=this;
	Model* pParentMdl = GetParentModel();

	for (; pParentMdl; pCurrMdl = pParentMdl, pParentMdl = pParentMdl->GetParentModel())
		indStep += pCurrMdl->m_currentInstance[OME_CURR_THREAD] * pParentMdl->GetNumInstances();
	//mod is for taking in cross references of subAssocs
	int currRecord = indStep % GetMaxDataSourceRowCount();
	OME_SCALAR& currValRef = GetCurrentInstanceVal(pEval->GetModelIndex());
	m_dataSources[dSInd].GetNumVal(indCol, currRecord, currValRef);
	if (pEval->IsAsInt())
		currValRef = (int)currValRef;
}

/** Determine if instance count can change over duration of run.
@return true If there can be a variable number of instances, false otherwise.
This function assumes that if there are any spawners associated with this model, then it
is dynamic.
*/
bool Model::IsDynamic() const
{
	bool ret = m_pParentModel && m_pParentModel->IsDynamic();
	if (!ret && !m_spawnerArray.empty())
	{
		for (auto itr = m_spawnerArray.begin(); itr != m_spawnerArray.end() && !ret; ++itr)
			ret = !(*itr)->IsConditional();
	}
	return ret;
}

/** 
@return true if Model contains conditional components; false otherwise.
*/
bool Model::IsConditional() const
{
	bool ret = IsDynamic() || (m_pParentModel && m_pParentModel->IsConditional());
	if (!ret && !m_spawnerArray.empty())
	{
		for (auto itr = m_spawnerArray.begin(); itr != m_spawnerArray.end() && !ret; ++itr)
			ret = (*itr)->IsConditional();
	}
	return ret;
}

/** Walk Model up and f find appropriate sub IndexNode.
	@param pMdl Pointer to starting Model.
	@param pRootNode Pointer to root IndexNode.
	@param indCount The total number of indices.
	@return Pointer to located IndexNode.
*/
IndexNode* Model::WalkIndexPath(Model* pMdl, IndexNode* pRootNode,const size_t & indCount)
{
	//model indices work from outside in, so push_front
	std::list<size_t> indList;
	for (Model* pCurrModel =pMdl; pCurrModel; pCurrModel = pCurrModel->GetParentModel())
	{
		if (pCurrModel->GetNumInstances() != 1 || pCurrModel->IsDynamic())
			indList.push_front(pCurrModel->m_currentInstance[OME_CURR_THREAD] + 1);
	}
	IndexNode* pCurrNode = pRootNode;

	auto itr = indList.begin();
	for (unsigned short i = 0; i < indCount && itr != indList.end(); ++i, ++itr)
	{
		auto fItr = pCurrNode->m_subNodes.find(*itr);
		if (fItr != pCurrNode->m_subNodes.end())
		{
			pCurrNode = &(*fItr).second;
		}
		else
		{
			throw OMEFuncOpException("WalkIndexPath", "Missing path component.");
		}
	}
	return pCurrNode;
}

/** Retrieve an n-indexed value from a given DataSrc for a given Evaluable.
	@param dSrc The DataSrc to query.
	@param indCol Index of the column that will contain the desired value.
	@param dsVal DataSourceValue that contains the relavent columns and values used to lookup the desired value for pEval.
	@param pEval Pointer to Evaluable object that will be assigned the discovered value.
*/
void Model::FindDSIndexedVal(const DataSrc & dSrc, const int & indCol, DataSourceValue & dsVal, Evaluable* pEval)
{
	if (!dsVal.m_inds.empty() && !dsVal.m_indexTreeRoot.m_ind)
		dSrc.BuildIndexTree(dsVal.m_indexTreeRoot, dsVal.m_inds, indCol);

	IndexNode* pCurrNode= WalkIndexPath(pEval->GetParentModel(), &dsVal.m_indexTreeRoot, dsVal.m_inds.size());
	pEval->SetValue(*pCurrNode->m_pVal);
}

/** Populate a varArray using an n-based index.
@param dSrc The DataSrc to query.
@param indCol Index of the column that will contain the desired value.
@param dsVal DataSourceValue that contains the relavent columns and values used to lookup the desired value for pEval.
@param pVa Pointer to a VarArray to populate.
*/
void Model::FindDSIndexedVal(const DataSrc & dSrc, const int & indCol, DataSourceValue & dsVal, VarArray* pVa)
{
	if (!dsVal.m_inds.empty() && !dsVal.m_indexTreeRoot.m_ind)
		dSrc.BuildIndexTree(dsVal.m_indexTreeRoot, dsVal.m_inds, indCol);

	IndexNode* pCurrNode = WalkIndexPath(pVa->GetParentModel(), &dsVal.m_indexTreeRoot, dsVal.m_inds.size());
	//start here to populate each spot in VarArray
	IndexNode* pVaStartNode = pCurrNode;
	IndVec vaInds;
	for (size_t v = 0; v < pVa->GetSize(); v++)
	{
		pCurrNode=pVaStartNode;
		pVa->GetDimsForRawIndex(v, vaInds);
		//again, we walk from outside-in
		for (size_t u = 0; u < vaInds.size(); ++u)
		{
			auto fItr = pCurrNode->m_subNodes.find(vaInds[vaInds.size()-u-1] + 1);
			if (fItr != pCurrNode->m_subNodes.end())
			{
				pCurrNode = &(*fItr).second;
			}
			else
			{
				throw OMEFuncOpException("FindDSIndexedVal", "Index not recognized");
			}
		}
		pVa->SetValue(v, *pCurrNode->m_pVal);
	}
}

/** Initialize records of which values are visible to outside tools*/
void Model::InitializeAccessPermissions()
{
	if (m_allEvals.empty())
		FindEvaluables();

	m_valRecAccess.assign(m_allEvals.size(), false);
	STLString externalVisRule = m_pSimManager->GetVisRule();

	for (size_t i = 0; i < m_valRecAccess.size(); ++i)
	{
		switch (m_allEvals[i]->GetOMEType())
		{
		case OME_STATEVAR:
			if (externalVisRule == "statevars")
			{
				m_valRecAccess[i] = !m_allEvals[i]->GetInternal();
				break;
			}
		default:
			if (externalVisRule == "all")
				m_valRecAccess[i] = !m_allEvals[i]->GetInternal();
		}
	}

	/// @todo add other vis rules as needed
}

/** Generate Record representation for model.
	@param recs Collection of records to append to.
	@sa CollectReadableRecords
*/
void Model::GenRecordRep(RecordRepArray & recs)
{
	/// @todo generate new record info here
}


/** Colects readable record data for this and all submodels.
	@param reps The RecordRepArray to append to.
*/
void Model::CollectReadableRecords(RecordRepArray & reps)
{
	GenRecordRep(reps);

	for (auto itr = m_submodelArray.begin(); itr != m_submodelArray.end(); ++itr)
		(*itr)->CollectReadableRecords(reps);
}

void Model::GetReadableValues(RecordRep & obj)
{
	/// @todo fill in
}


/** Get the largest number of rows utilized by an included data source.
	@return The maximum number of rows found in a data source.
*/
int Model::GetMaxDataSourceRowCount()
{
	int ret=0;
	int currCount;
	
	if (!m_expectsSpatial)
	{
		for (int i = 0; i < m_dataSources.size(); i++)
		{
			currCount = m_dataSources[i].GetRowCount();
			if (currCount > ret)
				ret = currCount;
		}
	}
	else
		ret = m_initialNumInstances; //assumes that SDP has been applied.
	return ret;
}

/** Propagate Model index multiplier down the model heirarchy.__May not be needed__*/
void Model::DisperseMultiplier(const size_t & tot)
{

	for(auto itr=m_submodelArray.begin(); itr!=m_submodelArray.end(); ++itr)
	{
		//multiplier is current multiplier times the number of current instances
		//(*itr)->m_indMultiplier=m_indMultiplier+m_currentInstance*(*itr)->GetNumInstances();
		//submodel must perpetuate changes downstream.
		(*itr)->m_indMultiplier[OME_CURR_THREAD] = tot*(*itr)->GetNumInstances();
		(*itr)->DisperseMultiplier((*itr)->m_indMultiplier[OME_CURR_THREAD] + m_currentInstance[OME_CURR_THREAD]);
	}
	
}

/** Find current instance index offset for the parent model.
	@return The offset used when looking up the current set of instances.
*/
size_t Model::GetParentOffset() const
{
	size_t out = 0;
	Model* pMdl = m_pParentModel;

	//build stack up to root.
	while (pMdl)
	{
		out *= pMdl->GetNumInstances();
		out += pMdl->GetCurrentInstance();
		pMdl = pMdl->m_pParentModel;
	}

	return out;
}

/** Get index offset based on current parent instances.
	@return Current parent instances offset.
*/
size_t Model::GetTotParentInstances() const
{
	size_t prod=1;
	Model* pMdl=m_pParentModel;

	while(pMdl)
	{
		prod*=pMdl->GetNumInstances();
		pMdl=pMdl->m_pParentModel;
	}
	return prod;
}

/** @return the number of active instances.
*/
size_t Model::GetActiveInstanceCount() const
{ 
	if (!IsDynamic() && !IsConditional())
		return m_totInstCount;

	size_t tot=0;
	size_t start=GetParentOffset();
	for(size_t i=start; i<start+m_totInstCount; i++)
	{
		if(m_instances[i]->IsActive())
			tot++;
	}
	return tot;
};

/** Send an error to the parent simulation manager.
	@param rhs The message detailing the error.
	@param pObj Optional pointer to an object associated with the error.
	@param eType The type of error.
	@param addFront If true, add message to the front of the error queue; otherwise, add to the end.
*/
void Model::ReportErrToMgr(const OMEChar* rhs, OMEObject* pObj,EC_ERR_TYPE eType,const bool addFront)
{
	if(m_pSimManager)
		m_pSimManager->ReportError(rhs,pObj,eType,addFront);
}

/** Send an error to the parent simulation manager.
	@param rhs The message detailing the error.
	@param pObj Optional pointer to an object associated with the error.
	@param eType The type of error.
	@param addFront If true, add message to the front of the error queue; otherwise, add to the end.
*/
void Model::ReportErrToMgr(const STLString & rhs, OMEObject* pObj,EC_ERR_TYPE eType,const bool addFront)
{
	if(m_pSimManager)
		m_pSimManager->ReportError(rhs,pObj,eType,addFront);
}

/** Retrieve a list of indices which can be accessed externally.
	@param srcs The source objects (used to retrieve labels).
	@param inds Upon return, array will be populated with indices for accessible values.
	@param access Boolean list of accessible indices.
	@param labels Upon return, array containing names of accessible objects.
*/
void Model::AllocReadableInds(const OMEArray & srcs, ARRAY_TYPE<size_t> & inds, ARRAY_TYPE<bool> & access, STLString* & labels)
{
	/// @todo update for ValueArchive
	
	//inds.resize(std::count_if(access.begin(),access.end(),[](bool b) { return b;}));
	//labels= new STLString[inds.size()];
	//for(size_t offset=0, i=0; i<m_singleValInds.size(); i++)
	//{
	//	if(access[i])
	//	{
	//		labels[offset]=srcs[i]->GetName();
	//		inds[offset++]=i;
	//	}
	//}
}

const char* Model::ToString(const char tabChar, const unsigned int indent, const unsigned int inc) const
{
	oSTLSStream temp;
	temp<<OMEObject::ToString(tabChar,indent,inc);
	//temp<<"Current Instance: "<<m_currentInstance;
	temp<<std::endl;

	OMEChar* out=new OMEChar[temp.str().size()+1];
	size_t i;
	for(i=0; i<temp.str().size(); i++)
		out[i]=temp.str()[i];
	out[i]='\0';

	return out;
}

/** Initialize lookup tables for xml tags.*/
void Model::InitXMLTags()
{
	//in order to keep tags consistant, model wants to get tags from objects.
	//since GetXMLTag() is virtual, the tags must be retrieved from instances, even
	//though the value is essentially static. Therefore, the model class will keep a static
	//list of tags in memory to be retrieved as necessary.

	m_xmlTags[TAG_TBL]=EvalTable().GetXMLTag();
	m_xmlTags[TAG_INTTBL] = EvalInterpTable().GetXMLTag();
	m_xmlTags[TAG_XINTTBL] = EvalXSetInterpTable().GetXMLTag();
	m_xmlTags[TAG_VAR]=Variable().GetXMLTag();
	m_xmlTags[TAG_VA]=VarArray().GetXMLTag();
	m_xmlTags[TAG_TS]=TimeSeriesVar().GetXMLTag();
	m_xmlTags[TAG_ITR]=IterConditional().GetXMLTag();
	m_xmlTags[TAG_STATE]=StateVar().GetXMLTag();
	m_xmlTags[TAG_INFL]=Influence().GetXMLTag();
	m_xmlTags[TAG_FLOW]=Flow().GetXMLTag();
	m_xmlTags[TAG_LBL]=SimpleLabel().GetXMLTag();
	m_xmlTags[TAG_ASSC]=SubmodelAssoc().GetXMLTag();
	m_xmlTags[TAG_SPWN]=Spawner().GetXMLTag();
	m_xmlTags[TAG_PORT]=SubmodelPort().GetXMLTag();
	m_xmlTags[TAG_ASSOCPORT] = AssocPort().GetXMLTag();
	m_xmlTags[TAG_ALIAS] = EvalAlias().GetXMLTag();
	m_xmlTags[TAG_CONVEYOR] = Conveyor().GetXMLTag();
	m_tagsInitialized=true;
}

/** Retrieve A list of all contained Evaluable objects properly sorted for serialization into a set of update statements.
	@param evals An EvalArray that will contain a list of Evaluable objects sorted in the proper order for serialization.
	@param useInit Set to true if the Evaluable objects are being sorted for Initialization; false otherwise.
*/
void Model::GetOrderedEvalsForUpdates(EvalArray & evals, const bool & useInit)
{
	GetAllEvals(evals);
	std::sort(evals.begin(), evals.end(), useInit ? OMEObject::CompareInitPriority : OMEObject::CompareEvalPriority);

	//group to the left as much as possible

	Model* pCurrModel;
	bool go;
	unsigned int steps;
	int evalInd, newInd;
	std::set<Model*> hitMdls;

	/*
	for each eval in list, shift up/left as much as possible
	- if first time model is encountered, do nothing
	- if model has been encountered, shift up until
	+ next left neighbor is in same model.
	+ next object is directly upstream
	- adjust ports?
	*/

	//1st pass: bubble toward front of list until like object hit
	for (int i = 0; i < evals.size(); i++)
	{
		pCurrModel = evals[i]->GetParentModel();
		if (hitMdls.find(pCurrModel) != hitMdls.end())
		{
			go = true;
			evalInd = i;
			newInd = evalInd;
			steps = 0;
			while (evalInd > 0 && go)
			{
				go = false;
				--newInd;
				if (!evals[evalInd]->IsUpStream(evals[newInd])) //check to see if we can bubble up without violating causality
				{
					if (evals[evalInd]->GetOMEType() != OME_SUBPORT || ((SubmodelPort*)evals[evalInd])->GetDirection() == PD_OUTBOUND)
					{
						//if not inbound port
						if (evals[newInd]->GetParentModel() != pCurrModel)  //move until we hit our model cluster
							go = true;
						else if ((evals[newInd]->GetOMEType() == OME_SUBPORT && ((SubmodelPort*)evals[newInd])->GetDirection() == PD_OUTBOUND)) //skip past outbound ports!
							go = true;
					}
					else if (evals[newInd]->GetParentModel() == pCurrModel) //inbound port - move until we are about to leave cluster
						go = true;
				}

				//delay swap step until 
				if (go)
					++steps;


			}

			//if eval did not make it to cluster, move back down until in former position or until it hits a dependency
			if (evals[evalInd]->IsUpStream(evals[newInd]))
			{
				++newInd;
				while (steps && !evals[evalInd]->IsDownStream(evals[newInd]))
				{
					++newInd;
					--steps;
				}
			}
			if (steps)
				DoSwapMarch(evals, evalInd, -steps);
		}
		else
			hitMdls.insert(pCurrModel);
	}
	hitMdls.clear();

	//2nd pass: collect clusters.
	///@question can this be combined with previous loop?
	///@todo test with flow submodelPorts.

	int clusterInd;
	for (int i = 0; i < evals.size(); i++)
	{
		pCurrModel = evals[i]->GetParentModel();
		if (hitMdls.find(pCurrModel) != hitMdls.end())
		{
			go = true;
			clusterInd = i;
			evalInd = i;
			newInd = i - 1;
			if (!evals[evalInd]->IsEvalRoot())
			{
				for (; evalInd > 0 && newInd > 0 && !evals[evalInd]->IsUpStream(evals[newInd]); --newInd)
				{
					if (evals[evalInd]->GetParentModel() == evals[newInd]->GetParentModel())
						clusterInd = newInd;
				}

				while (evalInd != clusterInd)
					DoSwapStep(evals, evalInd, -1);
			}
		}
		else
			hitMdls.insert(pCurrModel);
	}

	//3rd pass: sort on execution groups (if any)
	for (int i = 1; i < evals.size(); i++)
	{
		if (evals[i]->GetExecGroup() != OME_NO_EXEC_GRP)
		{
			pCurrModel = evals[i]->GetParentModel();
			evalInd = i;
			newInd = i - 1;

			for (; newInd > 0 && evals[evalInd]->GetParentModel() == evals[newInd]->GetParentModel() &&
				!evals[evalInd]->IsUpStream(evals[newInd]) &&
				(evals[newInd]->GetExecGroup() == OME_NO_EXEC_GRP || evals[evalInd]->GetExecGroup() < evals[newInd]->GetExecGroup()); --newInd);

			while (evalInd != newInd + 1)
				DoSwapStep(evals, evalInd, -1);
		}
	}


	//4th pass: ensure that spawners are placed at the earliest possible positions
	for (int i = 0; i < evals.size(); i++)
	{
		if (evals[i]->GetOMEType() == OME_SPAWNER)
		{
			//walk up.
			evalInd = i;
			go = true;
			while (evalInd > 0 && go == true)
			{
				go = false;
				newInd = evalInd - 1;
				if (!evals[evalInd]->IsUpStream(evals[newInd])
					//&& (!evals[evalInd]->IsEvalRoot() || !evals[evalInd]->GetIsConstant())
					&& evals[newInd]->GetParentModel() == evals[evalInd]->GetParentModel()
					&& evals[newInd]->GetOMEType() != OME_SPAWNER
					&& evals[newInd]->GetOMEType() != OME_SUBPORT)
				{
					DoSwapStep(evals, evalInd, -1);
					go = true;
				}
			}
		}
	}


}

/** Get First value for provided index.
	@param ind The index to lookup.
	@param step The stride to use during lookup.
	@return The first value assigned to the index or 0.0 if no value has previously been assigned.
*/
OME_SCALAR Model::GetCurrentInstanceFirstVal(const size_t & ind, const size_t & step) 
{ 
	if (m_pSimManager->GetPresentTime() == m_pSimManager->GetStartTime())
		return 0.0;
	return m_pValArchive->GetFirstValue(GetArchiveIndex(ind, step)); 
}

/** Get the most recent value for provided index.
	@param ind The index to lookup.
	@param step The stride to use during lookup.
	@return The most recent value assigned to the index or 0.0 if no value has previously been assigned.
*/
OME_SCALAR Model::GetCurrentInstanceLastVal(const size_t & ind, const size_t & step)
{ 
	if (m_pSimManager->GetPresentTime()-m_pSimManager->GetInterval() < m_pSimManager->GetStartTime())
		return 0.0;
	return m_pValArchive->GetPreviousValue(GetArchiveIndex(ind, step)); 
}

/** Get the value for an index for a specific timestep.
	@param ind The index to lookup.	
	@param time The timestep to lookup.
	@param step The stride to use during lookup.
	@return The value at timestep time, or 0.0 if time is not a valid value.
*/
OME_SCALAR Model::GetCurrentInstancePreviousVal(const size_t & ind, const OME_SCALAR & time, const size_t & step)
{ 
	if (time < m_pSimManager->GetStartTime() || time>= m_pSimManager->GetPresentTime())
		return 0.0;
	return m_pValArchive->GetValueForTime(GetArchiveIndex(ind, step), time); 
}

/** Get the value for an index for a number of previous timesteps ago.
	@param ind The index to lookup.
	@param offset The amount of time to look back.
	@param step The stride to use during lookup.
	@return The value at offset time ago, or 0.0 if offset is too big.
*/
OME_SCALAR Model::GetCurrentInstanceBackVal(const size_t & ind, const OME_SCALAR & offset, const size_t & step)
{ 
	if (offset > m_pSimManager->GetPresentTime() - m_pSimManager->GetStartTime())
		return 0.0;
	return m_pValArchive->GetValueForTime(GetArchiveIndex(ind, step), m_pSimManager->GetPresentTime() - offset); 
}

/** Move an Evaluable in an Array to a new position, shifting all values along the way.
	@param evals EvalArray containing order to modify.
	@param ind The index of the target Evaluable.
	@param stepsDir The number of steps and direction to move. Steps are represented by absolute magnitude, sign indicates direction.
*/
void Model::DoSwapMarch(EvalArray & evals, int & ind, const int & stepsDir)
{
	int dir = stepsDir >= 0 ? 1 : -1;
	unsigned int numSteps = abs(stepsDir);

	for (unsigned int i = 0; i < numSteps; ++i)
		DoSwapStep(evals, ind, dir);
}

/** Find the common root model between two other Models.
	@param pMdl1 Pointer to the first Model to include in the comparison.
	@param pMdl2 Pointer to the second Model to include in the comparison.
	@return Pointer to the common root model shared by pMdl1 and pMdl2.
*/
Model* Model::FindCommonRoot(Model* pMdl1, Model* pMdl2)
{
	//these are the same model
	if (pMdl1 == pMdl2)
		return pMdl1;

	//model 2 is nested in model 1
	if (pMdl1->IsNested(pMdl2))
		return pMdl1;

	//model 1 is nested in model 2
	if (pMdl2->IsNested(pMdl1))
		return pMdl2;

	//climb model heirarchy looking for common model
	for (Model* pCurrModel = pMdl1->GetParentModel(); pCurrModel != NULL; pCurrModel = pCurrModel->GetParentModel())
	{
		if (pCurrModel->IsNested(pMdl2))
			return pCurrModel;
	}

	//if we reach here, there is no common root
	return NULL;
}