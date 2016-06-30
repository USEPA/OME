#include "omeobjects/Evaluable.h"
#include "DebugOutput.h"
#include "omeobjects/Influence.h"
#include "omeobjects/Model.h"
#include "BaseManager.h"
#include "omeobjects/SubmodelPort.h"
#include "omeobjects/EvalInterpTable.h"
#include "OMEException.h"

#include <regex>

/// \optimization modify conditionals to bundle index call as part of evaluation.
#define GETMVAL()		 m_pParentModel->GetCurrentInstanceVal(m_mdlIndex)
#define GETMVALADDR()	m_pParentModel->GetCurrentInstanceValAddr(m_mdlIndex)
#define SETMVAL(x)		m_pParentModel->SetCurrentInstanceVal(x,m_mdlIndex)
/** Detaches Evaluables from all associated objects.*/
void Evaluable::Detach()
{
	//Remove..Object will shrink lists as objects are removed
	while(!m_upObjects.empty())
		m_upObjects[0]->RemoveDownObject(this);
	while(!m_downObjects.empty())
		m_downObjects[0]->RemoveUpObject(this);
}

/** Add object to upObject list if not already present.
	Current object is added to pObject's downObject list as well

	@param pObject the object to add
	@return 1 on success, 0 on failure
*/
int Evaluable::AddUpObject( Evaluable *pObject ) 
{ 
	int ret=ObjectPresent<Evaluable*>(pObject,m_upObjects);
	if(pObject && ret<0)
	{
		m_upObjects.push_back( pObject );
		ret=m_upObjects.size();
		pObject->AddDownObject(this); 
	}
	return ret;
} 

/** Add object to downObject list if not already present.
	Current object is added to pObject's upObject list as well

	@param pObject the object to add
	@return 1 on success, 0 on failure
*/
int Evaluable::AddDownObject( Evaluable *pObject )
{
	int ret=ObjectPresent<Evaluable*>(pObject,m_downObjects);
	if(pObject && ret<0)
	{
		m_downObjects.push_back( pObject );
		ret=m_downObjects.size();
		pObject->AddUpObject(this); 
	}
	return ret;
}

/** Remove object from upObject list if present.
	Current object is removed from pObject's downObject list as well

	@param pObject the object to remove
*/
void Evaluable::RemoveUpObject( Evaluable *pObject ) 
{
	int ind=ObjectPresent<Evaluable*>(pObject,m_upObjects);
	
	if(ind >=0)
		m_upObjects.erase(m_upObjects.begin()+ind);

	ind=ObjectPresent<Evaluable*>(this,pObject->m_downObjects);

	if(ind>=0)
	  pObject->m_downObjects.erase(pObject->m_downObjects.begin()+ind);

}

/** Remove object from downObject list if present.
	Current object is removed from pObject's upObject list as well

	@param pObject the object to remove
*/
void Evaluable::RemoveDownObject( Evaluable *pObject ) 
{
	int ind=ObjectPresent<Evaluable*>(pObject,m_downObjects);
	
	if(ind >=0)
	  m_downObjects.erase(m_downObjects.begin()+ind);

	ind=ObjectPresent<Evaluable*>(this,pObject->m_upObjects);

	if(ind>=0)
	  pObject->m_upObjects.erase(pObject->m_upObjects.begin()+ind);

}

/** returns all up objects accessible to this Evaluable object.
	@param out an EvalArray to be populated
	@param exclude Optional argument for specific type of OMEObject to exclude.
*/
void Evaluable::FindAllUpObjects(EvalArray & out, int exclude) 
{ 
	//first object should be excluded, so do first iteration here
	for(int x=0; x < UpObjectCount(); x++)
			DescendTree(GetUpObject(x),out,exclude);
	//Evaluable::DescendTree(this,out,exclude); 
};

/** returns all down objects accessible to this Evaluable object.
	@param out an EvalArray to be populated
	@param exclude Optional argument for specific type of OMEObject to exclude.
*/
void Evaluable::FindAllDownObjects(EvalArray & out, int exclude) 
{ 
	//first object should be excluded, so do first iteration here
	for(int x=0; x < DownObjectCount(); x++)
		AscendTree(GetDownObject(x),out,exclude);
	//Evaluable::AscendTree(this,out,exclude);
}

//void Evaluable::FindTieredDownObjects(ARRAY_TYPE<EvalArray> & out, int exclude)
//{
//	//ignore this level
//	AscendTree(this,out,exclude);
//}

void Evaluable::Reset(BaseManager* pBm)
{
	m_lastTime=-1.0; 
	//m_value=0;
	SetInitialized(false);
	//m_evalPriority=0;
}

/** Recursively parse and collect all downstream objects
	@param currObj the current Evaluable object being evaluated
	@param outArray the list to store results.
	@param exclude Optional argument for specific type of OMEObject to exclude.
	@return true if no duplicates found, false otherwise
*/
bool Evaluable::DescendTree(Evaluable* currObj, EvalArray & outArray, int exclude)
{	
	bool ret=true;
	if(ObjectPresent<Evaluable*>(currObj,outArray)<0 && currObj->GetOMEType()!=exclude)
	{
		outArray.push_back(currObj);
	
		for(int x=0; x < currObj->UpObjectCount(); x++)
			ret= DescendTree(currObj->GetUpObject(x),outArray,exclude) && ret;
	}
	else
		ret=false;
	return ret;
}

/** Recursively parse and collect all downstream objects down to pTrg.
@param currObj the current Evaluable object being evaluated.
@param outArray the list to store results.
@param pTrg Pointer at which to terminate crawl at.
@return true if pTrg is found; false otherwise.
*/
bool Evaluable::DescendTree(Evaluable* currObj, EvalArray & outArray, Evaluable* pTrg)
{
	bool ret=currObj==pTrg;
	if(!ret && ObjectPresent<Evaluable*>(currObj,outArray)<0)
	{
		outArray.push_back(currObj);
	
		for(int x=0; x < currObj->UpObjectCount() && !ret; x++)
			ret= DescendTree(currObj->GetUpObject(x),outArray,pTrg);
	}
	return ret;
}

/** Recursively parse and collect all upstream objects
	@param currObj the current Evaluable object being evaluated
	@param outArray the list to store results
	@param exclude Optional argument for specific type of OMEObject to exclude.
	@return true if no duplicates found, false otherwise
*/
bool Evaluable::AscendTree(Evaluable* currObj, EvalArray & outArray, int exclude)
{
	bool ret=true;
	if(ObjectPresent<Evaluable*>(currObj,outArray)<0 && currObj->GetOMEType()!=exclude)
	{
		outArray.push_back(currObj);

		for(int x=0; x < currObj->DownObjectCount(); x++)
			ret=AscendTree(currObj->GetDownObject(x),outArray,exclude) && ret;
	}
	else
		ret=false;
	return ret;
}

/** Recursively parse and collect all upstream objects
	@param currObj the current Evaluable object being evaluated
	@param outArrays the list to store results
	@param exclude Optional argument for specific type of OMEObject to exclude.
	@return true if no duplicates found, false otherwise
*/
bool Evaluable::AscendTree(Evaluable* currObj, ARRAY_TYPE<EvalArray> & outArrays, int exclude)
{
	bool ret=true;
	EvalArray currLevel;

	for(int x=0; x < currObj->DownObjectCount(); x++)
	{
		currLevel.push_back(currObj);
	}
	outArrays.push_back(currLevel);

	for(int x=0; x < currObj->DownObjectCount(); x++)
		ret=AscendTree(currObj->GetDownObject(x),outArrays,exclude) && ret;

	//if(currObj->GetOMEType()==OME_SUBPORT)
	//{
	//	Evaluable* pEval=((SubmodelPort*)currObj)->GetPEvalTarget();
	//	if(pEval)
	//		ret=AscendTree(pEval,outArray,exclude) && ret;
	//	else
	//	{
	//		EvalArray subs;
	//		((SubmodelPort*)currObj)->GetSubObjects(subs);
	//		for(int i=0; i<subs.size();i++)
	//			ret=AscendTree(subs[i],outArray,exclude) && ret;
	//	}
	//}
	
	return ret;
}

/** Recursively parse and collect all upstream objects up to pTrg.
@param currObj the current Evaluable object being evaluated.
@param outArray the list to store results.
@param pTrg Pointer at which to terminate crawl at.
@return true if pTrg is found; false otherwise.
*/
bool Evaluable::AscendTree(Evaluable* currObj, EvalArray & outArray, Evaluable* pTrg)
{
	bool ret=currObj==pTrg;
	if(!ret && ObjectPresent<Evaluable*>(currObj,outArray)<0)
	{
		outArray.push_back(currObj);

		for(int x=0; x < currObj->DownObjectCount() && !ret; x++)
			ret=AscendTree(currObj->GetDownObject(x),outArray,pTrg);
	}

	return ret;
}

/** Collect all evaluable objects that are immediately downstream from the Evaluable object.
	@param out The EvalArray that will store the found evaluables.
	@param excludeInfl If true, any encountered Influences or SubmodelPorts will be be skipped and their downstream Evaluables will be returned.
	@param bypassPorts If true, skip any SubmodelPorts when collecting down objects.
*/
void Evaluable::FindNextDownObjects(EvalArray & out, bool excludeInfl,bool bypassPorts)
{
	Evaluable* pCurr;
	OMETYPE type;
	for(int i=0; i<m_downObjects.size();i++)
	{
		pCurr=m_downObjects[i];
		type=pCurr->GetOMEType();
	
		if(type==OME_INFLUENCE && excludeInfl)
		{
			pCurr=((Influence*)pCurr)->GetPEvalTarget();
			if(bypassPorts && pCurr && pCurr->GetOMEType()==OME_SUBPORT && ObjectPresent(pCurr,out)<0)
				((SubmodelPort*)pCurr)->FindNextDownObjects(out,excludeInfl);

		}
		//else if(type==OME_FLOW && ((Flow*)pCurr)->GetPEvalSource()==this)
		//{
		//	//if object is both src/trg and attached via influence, then object should show up twice if influnces have been purged
		//	unsigned int count=0;
		//	for(unsigned int j=0; j<pCurr->DownObjectCount() && count<2;j++,count++)
		//	{
		//		if(pCurr->GetDownObject(j)==this)
		//			count++;
		//	}
		//	if(count<2)
		//		pCurr=NULL;
		//}
		if(pCurr)
			out.push_back(pCurr);
	}
}

/** Collect all evaluable objects that are immediately upstream from the Evaluable object.
	@param out The EvalArray that will store the found evaluables.
	@param excludeInfl If true, any encountered Influences or SubmodelPorts will be be skipped and their downstream Evaluables will be returned.
	@param bypassPorts If true, skip any SubmodelPorts when collecting up objects.
*/
void Evaluable::FindNextUpObjects(EvalArray & out, bool excludeInfl,bool bypassPorts)
{
	Evaluable* pCurr;
	OMETYPE type;
	for(int i=0; i<m_upObjects.size();i++)
	{
		pCurr=m_upObjects[i];
		type=pCurr->GetOMEType();
		if(type==OME_INFLUENCE && excludeInfl)
		{
			pCurr=((Influence*)pCurr)->GetPEvalTarget();
			if(bypassPorts && pCurr && pCurr->GetOMEType()==OME_SUBPORT && ObjectPresent(pCurr,out)<0)
				((SubmodelPort*)pCurr)->FindNextUpObjects(out,excludeInfl);
			
		}
		else if(type==OME_FLOW && ((Flow*)pCurr)->GetPEvalTarget()==this)
		{
			//if object is both src/trg and attached via influence, then object should show up twice if influnces have been purged
			unsigned int count=0;
			for(unsigned int j=0; j<pCurr->UpObjectCount() && count<2;j++)
			{
				if(pCurr->GetUpObject(j)==this)
					count++;
			}
			if(count<2)
				pCurr=NULL;
		}
		if(pCurr)
			out.push_back(pCurr);
	}
}

/** Test to see if object is downstream from this Evaluable.
	@param pTrg Pointer to target to search for.
	@param useShallow If true, only evaluate the next level of down objects.
	@return true if object is downstream from this; false otherwise.
	@sa IsUpStream
*/
bool Evaluable::IsDownStream(Evaluable* pTrg, const bool & useShallow)
{
	EvalArray cache;
	bool ret=this==pTrg;
	if(!ret)
	{
		if (!useShallow)
		{
			for (int x = 0; x < DownObjectCount() && !ret; x++)
				ret = AscendTree(GetDownObject(x), cache, pTrg);
		}
		else
			ret = std::find(m_downObjects.begin(), m_downObjects.end(), pTrg) != m_downObjects.end();
	}
	return ret;
}

/** Test to see if object is upstream from this Evaluable.
@param pTrg Pointer to target to search for.
@param useShallow If true, only evaluate the next level of up objects.
@return true if object is upstream from this; false otherwise.
@sa IsDownStream
*/
bool Evaluable::IsUpStream(Evaluable* pTrg, const bool & useShallow)
{
	EvalArray cache;
	bool ret=this==pTrg;
	if(!ret)
	{
		if (!useShallow)
		{
			for (int x = 0; x < UpObjectCount() && !ret; x++)
				ret = DescendTree(GetUpObject(x), cache, pTrg);
		}
		else
			ret = std::find(m_upObjects.begin(), m_upObjects.end(), pTrg) != m_upObjects.end();
	}
	return ret;
}

/** Perform upstream search for specific Evaluable with specified name.
	@param name The name of the Evaluable to search for.
*/
Evaluable* Evaluable::UpObjectForName(const STLString & name)
{
	Evaluable* ret=NULL;
	if (!m_upObjects.empty())
	{
		Evaluable* pCurr;
		//search for name first, as the name is likely to be on the next level
		auto itr = m_upObjects.begin();
		for (; itr != m_upObjects.end() && !ret; ++itr)
		{
			pCurr = *itr;
			if (pCurr->GetOMEType() == OME_INFLUENCE)
				pCurr = ((Influence*)pCurr)->GetPEvalSource();

			if (pCurr->GetName() == name)
				ret = pCurr;
		}

		if (!ret)
		{
			//continue search; SubmodelPorts should be the only ones upstream whose references continue to be passed upward.
			for (itr=m_upObjects.begin(); itr != m_upObjects.end() && !ret; ++itr)
			{
				pCurr = *itr;
				if (pCurr->GetOMEType() == OME_INFLUENCE)
					pCurr = ((Influence*)pCurr)->GetPEvalSource();

				if (pCurr->IsOMEType(OME_SUBPORT))
					ret = pCurr->UpObjectForName(name);
			}
		}
	}
	return ret;
}

/** Generates a list of attributes.
	Generates a list of strings, one for each attribute entry for the top-tier XML tag
		@param out  the array to be populated by attribute strings.
*/
void Evaluable::GetXMLAttributes(StrArray & out) const
{
	OMEObject::GetXMLAttributes(out);

	
	AddAttributeValue("expr",m_expr,out);
	if (!m_initExpr.empty())
		AddAttributeValue("init_expr", m_initExpr, out);
	if (IsAsInt())
		AddAttributeValue("as_int", 1, out);
	if (GetAskVal())
		AddAttributeValue("ask_val", 1, out);
	if (GetExecGroup())
		AddAttributeValue("exec_group", m_execGroup, out);
   //AddAttributeValue("value",m_value,out);
}

void Evaluable::GetXMLSubNodes(StrArray & out, const unsigned int indent, const unsigned int inc)
{
	if (!m_tableIDs.empty())
	{
		oSTLSStream outBuff;
		outBuff << "<tables>" << endl;
		for (STLString & tId : m_tableIDs)
			outBuff<<"<table id='"<<tId<<"' />"<<endl;
		outBuff << "</tables>" << endl;
		out.push_back(outBuff.str());
	}
	OMEObject::GetXMLSubNodes(out,indent,inc);
}

void Evaluable::GetXMLAttributeMap(XMLAttrVector & out)
{
	OMEObject::GetXMLAttributeMap(out);
	XmlAttr attrs[] = {
	  // attr				type		   address                          isReq 
	  { "expr",				XML_STR,	&(this->m_expr),				true },
	  { "exec_group",       XML_INT,    &(this->m_execGroup),           false },
	  { "init_expr",		XML_STR,	&(this->m_initExpr),			false },
	  //{ "value",			XML_SCALAR,	&(this->m_value),				false,		0 },
	  { NULL,				XML_NULL,		NULL,							false } };

	  //drop null
	 int count;
	 for(count=0; attrs[count].name!=NULL; count++)
		  out.push_back(attrs[count]);

}

int Evaluable::PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
{
	if (pCurrElem->BoolAttribute("as_int"))
		SetAsInt(true);
	if (pCurrElem->BoolAttribute("ask_val"))
		SetAskVal(true);

	//compatibility with older version
	const char* tID = pCurrElem->Attribute("table");
	if (tID)
		m_tableIDs.push_back(tID);

	TI2Element* pTblElem = pCurrElem->FirstChildElement("tables");
	if (pTblElem)
	{
		TI2Element* pSubTblElem = pTblElem->FirstChildElement("table");
		while (pSubTblElem)
		{
			m_tableIDs.push_back(pSubTblElem->Attribute("id"));
			pSubTblElem = pSubTblElem->NextSiblingElement("table");
		}
		m_pTables.resize(m_tableIDs.size(), NULL);
	}

	OMEObject::PopulateFromComplexElements(pCurrElem,tag,filename);

	return 1;
}


/** @param pEt Pointer to EvalTable to associate with Evaluable.
	@param ind The index of the table to set.
*/
void Evaluable::SetPEvalTable(EvalTable* pEt,const size_t & ind) 
{
	if (ind >= m_pTables.size())
		m_pTables.resize(ind+1, NULL);
	if (ind >= m_tableIDs.size())
		m_tableIDs.resize(ind+1, STLString());
	m_pTables[ind]=pEt; 
	if(pEt)
		m_tableIDs[ind]=pEt->GetID();
	else
		m_tableIDs[ind].clear();
}


/** Copies values from another function.
	@param func the function to be copied.
*/
void Evaluable::Duplicate(const Evaluable & func)
{
	OMEObject::Duplicate(func);
	m_expr=func.m_expr;
	//m_value=func.m_value;
	m_evalFlags=func.m_evalFlags;
	m_pTables=func.m_pTables;
	m_tableIDs=func.m_tableIDs;
	m_pEnumParent = func.m_pEnumParent;
}


//================= Eval Stuff ===================


/** Evaluates current expression, stores result
	@param time the current time step.
	@param pBm Pointer to BaseManager that contains the OMEParser to evaluate expression.
	@return The new OME_SCALAR value.
*/
OME_SCALAR Evaluable::Evaluate(const OME_SCALAR time,BaseManager* pBm)
{
	if(m_expr.length() && (!GetIsConstant() || !IsInitialized()))
	{
		m_lastTime=time;
		OMEParser* pPrsr=pBm->GetPEvalParser();

	  pPrsr->CompileExpression(m_expr.c_str(),this);
	  
	  pPrsr->Evaluate(GETMVAL());

#ifdef OME_EVAL_DUMP
		oSTLSStream strm;
		strm<<time<<": "<<m_name<<" "<<m_value<<" "<<m_expr;
		DBG_PRINT(strm.str());
#endif
	}
	return GETMVAL();

}

/** Special Evaluation step that is intended to initialize the object.
	Can only be called once; subsequent calls will do nothing until after a call to Reset().
	@param time Time of initialization.
	@param pBm Pointer to BaseManager that contains the OMEParser to evaluate expression.
	@return The stored value.
*/
OME_SCALAR Evaluable::Initialize(const OME_SCALAR time,BaseManager* pBm)
{
	if(!IsInitialized())
	{
		m_mdlIndex=GetModelIndex();
		OME_SCALAR out=Evaluate(time,pBm);
		SETMVAL(!IsAsInt() ? out : (int)out);
		SetInitialized(true);
	}

	return GETMVAL();
}

/** This function should be called as part of the batch initialization process after the expression
	returned by BatchInitExpr has been evaluated.
	@param time The start time of the simulation.
	@param pBm A pointer to the parent manager of the simulation.
	@return The initial value of the object.
*/
void Evaluable::BatchInitialize(const OME_SCALAR time,BaseManager* pBm)
{
//	if(!IsInitialized())
//	{
		m_mdlIndex=GetModelIndex();
		//try files first, then load
		if (m_pInitializer)
			m_pInitializer->InitializeEval(this);
		
		SetInitialized(true);
//	}
}

unsigned int Evaluable::Prioritize(unsigned int currPriority, const bool isInitial)
{
	unsigned int maxPriority=0, tempPriority;
	unsigned int& rCompPriority = isInitial ? m_initPriority : m_evalPriority;
	
	if (IsEvalRoot())
		rCompPriority = std::numeric_limits<unsigned int>::max();
	
	if(currPriority>rCompPriority)
	{
		rCompPriority=currPriority; 

		Evaluable* curr;
		int i;
		for(i=0; i< m_upObjects.size(); i++)
		{
			curr=m_upObjects[i];
			if(curr->GetOMEType()==OME_INFLUENCE && !(((Influence*)curr)->GetUseCurrentVal()))//(!(((Influence*)curr)->m_useCurrentVal) || isInitial))
			{
				tempPriority=curr->Prioritize(rCompPriority+1, isInitial);
				maxPriority= tempPriority > maxPriority ? tempPriority : maxPriority;
			}
		}

		//only apply use current vals if it is post initialization
		if(!isInitial)
		{
			for(i=0; i< m_downObjects.size(); i++)
			{
				curr=m_downObjects[i];
				if (curr->GetOMEType() == OME_INFLUENCE && ((Influence*)curr)->GetUseCurrentVal())
				{
					tempPriority=curr->Prioritize(rCompPriority,isInitial);
					maxPriority= tempPriority > maxPriority ? tempPriority : maxPriority;
				}
			}
		}
	}

	/// @todo attempt to collapse submodel clusters even further. (see seagrass)
	/*if (IsTerminal())
	{
		rCompPriority = 0;
		maxPriority = 0;
	}*/
	return maxPriority > rCompPriority ? maxPriority : rCompPriority;
}

void Evaluable::SetName(const STLString & name)
{
	//if(m_pEvalParser)
	//{

		//use regex to ensure exact match(not some subset of a larger word)
		std::regex matchReg("\\b"+m_name+"\\b");
		std::string tempName;
		std::string tempExpr;

		//update any expressions that rely on this object.
		//assumes all referencing expressions are downstream in heirarchy

		EvalArray downs;
		FindAllDownObjects(downs);

		for(int i=0; i< downs.size(); i++)
		{
			tempName.assign(name);
			tempExpr.assign(downs[i]->m_expr);
			downs[i]->m_expr=std::regex_replace(tempExpr,matchReg,tempName).c_str();
		}

	//}
	if(m_pParentModel)
		m_pParentModel->UpdateObjectName(m_name,name);
	m_name=name;

}

void Evaluable::SetParentModel(Model* pMdl)
{
	OMEObject::SetParentModel(pMdl);

	if (pMdl)
	{
		if (m_tableIDs.size() > m_pTables.size())
			m_pTables.resize(m_tableIDs.size(), NULL);
		for (size_t i = 0; i < m_tableIDs.size(); ++i)
		{
			if (!m_tableIDs[i].empty() && !m_pTables[i])
				m_pTables[i] = pMdl->GetEvalTable(m_tableIDs[i]);
		}
	}
	
}

/** Records whether or not the internal expression is composed of only a numeric constant.
	This would make the variable a lever candidate.
*/
 void Evaluable::CheckConstant()
 {
	  static const STLRegex numReg("^\\s*[-+]?\\s*[0-9]+(?:[.][0-9]*)?\\s*$");
	  SetIsConstant((!m_pParentModel || !m_pParentModel->IsDynamic()) && (m_expr.empty() || std::regex_match(m_expr, numReg)));

	  //the following may be taken care of elsewhere (such as initialization), but move value to m_value to be sure
	  //if(m_isConstant && !m_expr.empty())
	  //   m_initValue=std::stod(m_expr);
 }

 const char* Evaluable::ToString(const char tabChar, const unsigned int indent, const unsigned int inc) const
 {
	 oSTLSStream out;
	 StrList parts;
	 parts.push_back(OMEObject::ToString(tabChar,indent,inc));

	 if(m_pParentModel)
	{	
		out<<"Reference Index: "<<m_mdlIndex<<endl;

		 if(GetOMEType()!=OME_VARARRAY)
		 {
		 
			 out<<"Instance:"<<endl;
			 out<<"  Value: "<<GetValue()<<endl;
		 }
		 else
		 {
			 ///@todo add var array dump.
		 }
	 }
	 parts.push_back(out.str());
	 STLString combined=ConcatStrings(parts,'\n');
	 size_t outSize=combined.size();
	 char* outBuff=new char[outSize+1];
	 outBuff[outSize]='\0';
	 SAFE_STRCPY(combined.c_str(),outBuff,outSize+1);
	 return outBuff;
 }

 ostream& operator<<(ostream& outStrm, const EvalRep & rhs)
{
   using namespace std;
   outStrm<<"Name: "<<rhs.name<<endl;
   outStrm<<"Type ID: "<<rhs.type<<endl;
   outStrm<<"Constant? "<<(rhs.isConstant ? "Yes" : "No")<<endl;
   outStrm<<"Array? "<<(rhs.isArray ? "Yes" : "No")<<endl;
   if(rhs.isArray)
	  outStrm<<"Array Size: "<<rhs.count<<endl;
   outStrm<<endl;

   return outStrm;
}


/** Find the total number of instances in which the Evaluable is present
	@return The total number instances associated with this Evaluable.
*/
size_t Evaluable::GetInstValCount() const
{
	size_t currCount = 0;
	
	Model* pMdl = NULL;
	if (pMdl = m_pParentModel)
	{
		currCount = 1;

		for (; pMdl && currCount; pMdl = pMdl->GetParentModel())
			currCount *= pMdl->GetNumInstances();
	}
	return currCount;
}

/** Retrieve value from associated EvalInterpTable, if any exists.
	@param inVal The value used for the lookup.
	@param outVal The variable to hold any returned value.
	@param ind The index of the Table to retrieve.
	@return true if a value was successfully located and returned, false otherwise.
*/
bool Evaluable::GetInterpTableValue(const OME_SCALAR & inVal, OME_SCALAR & outVal, const size_t & ind)
{
	bool ret = false;

	EvalInterpTable* pEIT=dynamic_cast<EvalInterpTable*>(GrabTData(ind));
	if (pEIT)
	{
		outVal = pEIT->GetInterpVal(inVal);
		ret = true;
	}

	return ret;
}

/** @return index used to access the Evaluable's value in its parent Model.*/
size_t Evaluable::GetModelIndex() const
{ 
	return m_mdlIndex;
};

/** Safe accessor.
	@param out The variable to hold the Evaluable's index to its value in the parent Model.
	@return true if the Evaluable has an index to a parent Model; false otherwise.
*/
bool Evaluable::GetModelIndex(size_t & out) const
{ 
	return m_pParentModel && m_pParentModel->GetValueArchive() && m_pParentModel->GetValueArchive()->IndexForID(m_id, out);
};

/** access the currently stored value 
		@return the currently stored value.
*/
OME_SCALAR Evaluable::GetValue() const
{
	OME_SCALAR ret=0.0;
	if (m_pParentModel->GetCurrentInstanceRef()->IsActive())
		ret=GETMVAL();
	
	return ret;
}

/** Access value for specific relative instance.
	@param inst The relative instance to access.
	@return The value for inst.
	@throws OMEObjException if inst is invalid.
*/
OME_SCALAR Evaluable::GetValue(const size_t & inst) const
{
	OME_SCALAR out;
	if (!(IsInitialized() ? m_pParentModel->GetValForInstIndex(inst, m_mdlIndex, out) : \
		m_pParentModel->GetValForInstIndex(inst, GetModelIndex(), out)))
		throw OMEObjException("Target instance is invalid.",this);

	return out;
}

/** Access value for specific absolute instance.
@param inst The absolute instance to access.
@return The value for inst.
@throws OMEObjException if inst is invalid.
*/
OME_SCALAR Evaluable::GetValueForRawInst(const size_t & inst) const
{
	OME_SCALAR out;
	if (!(IsInitialized() ? m_pParentModel->GetValForRawInstIndex(inst, m_mdlIndex, out) : \
		m_pParentModel->GetValForRawInstIndex(inst, GetModelIndex(), out)))
		throw OMEObjException("Target instance is invalid.", this);

	return out;
}

/** @return Class of enum value, or NULL if no value is enumerated value is assigned. */
ModelEnum* Evaluable::GetEnumClass()
{
	return m_pEnumParent;
}

/** @return Assigned enum value, or NULL if no value is enumerated value is assigned. */
EnumEntry* Evaluable::GetEnumEntry()
{
	EnumEntry* ret = NULL;

	if (m_pEnumParent)
	{
		ret = m_pEnumParent->AddrForIndex(GetValue());
	}
	return ret;
}

/** @return true if value represents an enumerated class; false otherwise.*/
bool Evaluable::IsEnumClass() const
{
	return m_pEnumParent && (m_evalFlags & ENCLASS_FLAG);
}

/** @return true if value represents an enumerated value; false otherwise.*/
bool Evaluable::IsEnumValue() const
{
	return m_pEnumParent && (m_evalFlags & ~ENCLASS_FLAG);
}


/** Retrieve address of currently selected value.
	@return Pointer to base value for current instance.
*/
const OME_SCALAR* Evaluable::GetValueAddr()
{
	return GETMVALADDR();
}

/** Retrieve address of specified instance.
	@param inst The instance to retrieve address from.
	@return Pointer to value at inst.
*/
const OME_SCALAR* Evaluable::GetValueAddr(const size_t & inst)
{
	return &(*m_pParentModel->GetValueArchive())[inst+m_mdlIndex];
}

/** Set internal value for the currently active instance.
		@param val the desired current value
*/
void Evaluable::SetValue(const OME_SCALAR val)
{
	//parameter assignment may have set constant flag
	if (!(m_evalFlags & CONST_FLAG))
		SETMVAL(!IsAsInt() ? val : (int)val);
}

bool Evaluable::GetIsConstant() const
{ 
	return (!m_pParentModel || !m_pParentModel->IsDynamic()) && (m_evalFlags & CONST_FLAG); 
}

/** Set internal value for the specified instance.
@param val the desired current value.
@param inst The instance to target with the new value.
*/
void Evaluable::SetValue(const OME_SCALAR val, const size_t & inst)
{
	//parameter assignment may have set constant flag
	if (!(m_evalFlags & CONST_FLAG))
		(*m_pParentModel->GetValueArchive())[m_mdlIndex + inst] = !IsAsInt() ? val : (int)val;
}

/** Set internal value for the current instance to specified Enumerated Class.
@param me Pointer to the desired enumerated class.
*/
void Evaluable::SetValue(ModelEnum* me)
{
	m_pEnumParent = me;
	SetIsEnumClass(true);

	SetValue(me->GetEnumValueCount());
}

/** Set internal value for the current instance to specified Enumerated Value.
@param ee Pointer to the desired enumerated value.
*/
void Evaluable::SetValue(EnumEntry* ee)
{
	m_pEnumParent = ee->pCollection;
	SetIsEnumClass(false);

	SetValue(ee->ind);
}

//================= Rep Stuff ====================

EvalRep::EvalRep() 
	: name(STLString())
	,id(STLString())
	,desc(STLString())
	,type(OME_NULL)
	,isConstant(false)
	,isArray(false)
	,count(0)
	,color(OMEColor())
{}

/** Convenience constructor.
@param eval Pointer to Evaluable to represent.
@param ia If true, EvalRep is considered to be representing an array.
@param c Number of objects in represented array, if applicable.
*/
EvalRep::EvalRep(Evaluable *eval,bool ia,unsigned int c)
	: name(eval->GetName()), id(eval->GetID()),/*desc(eval->m_description),*/type(eval->GetOMEType()),isConstant(eval->GetIsConstant()),isArray(ia),count(c) 
{
	if(eval && eval->GetParentModel())
	{
		parentModel=eval->GetParentModel()->GetExprName();
		StrList parts;
		eval->GetParentModel()->GetUpPath(parts);
		modelPath=ConcatStrings(parts,'.');
	}
}

/** Copy Constructor.
	@param er The EvalRep to copy.
*/
EvalRep::EvalRep(const EvalRep & er)
	: name(er.name)
	, id(er.id)
	, desc(er.desc)
	, type(er.type)
	, isConstant(er.isConstant)
	, isArray(er.isArray)
	, count(er.count)
	, color(er.color)
	, selected(er.selected)
	, parentModel(er.parentModel)
	, modelPath(er.modelPath)
{}

OME_SCALAR operator+(const OME_SCALAR & lhs, Evaluable & rhs)
{
	return lhs+rhs.GetValue();
}
OME_SCALAR operator-(const OME_SCALAR & lhs, Evaluable & rhs)
{
	return lhs-rhs.GetValue();
}

OME_SCALAR operator*(const OME_SCALAR & lhs, Evaluable & rhs)
{
	return lhs*rhs.GetValue();
}
OME_SCALAR operator/(const OME_SCALAR & lhs, Evaluable & rhs)
{
	return lhs/rhs.GetValue();
}

bool operator==(const OME_SCALAR & lhs, Evaluable & rhs)
{
	return lhs == rhs.GetValue();
}

bool operator!=(const OME_SCALAR & lhs, Evaluable & rhs)
{
	return lhs != rhs.GetValue();
}

bool operator<=(const OME_SCALAR & lhs, Evaluable & rhs)
{
	return lhs <= rhs.GetValue();
}

bool operator>=(const OME_SCALAR & lhs, Evaluable & rhs)
{
	return lhs >= rhs.GetValue();
}

bool operator<(const OME_SCALAR & lhs, Evaluable & rhs)
{
	return lhs < rhs.GetValue();
}

bool operator>(const OME_SCALAR & lhs, Evaluable & rhs)
{
	return lhs > rhs.GetValue();
}

OME_SCALAR operator+(Evaluable & lhs, const OME_SCALAR & rhs)
{
	return lhs.GetValue() + rhs;
}

OME_SCALAR operator-(Evaluable & lhs, const OME_SCALAR & rhs)
{
	return lhs.GetValue() - rhs;
}

OME_SCALAR operator*(Evaluable & lhs, const OME_SCALAR & rhs)
{
	return lhs.GetValue() * rhs;
}
OME_SCALAR operator/(Evaluable & lhs, const OME_SCALAR & rhs)
{
	return lhs.GetValue() / rhs;
}

bool operator==(Evaluable & lhs, const OME_SCALAR & rhs)
{
	return lhs.GetValue() == rhs;
}

bool operator!=(Evaluable & lhs, const OME_SCALAR & rhs)
{
	return lhs.GetValue() != rhs;
}
bool operator<=(Evaluable & lhs, const OME_SCALAR & rhs)
{
	return lhs.GetValue() <= rhs;
}

bool operator>=(Evaluable & lhs, const OME_SCALAR & rhs)
{
	return lhs.GetValue() >= rhs;
}

bool operator<(Evaluable & lhs, const OME_SCALAR & rhs)
{
	return lhs.GetValue() < rhs;
}

bool operator>(Evaluable & lhs, const OME_SCALAR & rhs)
{
	return lhs.GetValue() > rhs;
}

/** Retrieve a table associated with Evaluable (if any), and cache its pointer if needed.
	@return Pointer to associated EvalTable, or NULL if none exists.
*/
EvalTable* Evaluable::GrabTData(const size_t & ind)
{ 
	if (!m_pTables[ind] && m_pParentModel && !m_tableIDs[ind].empty()) 
		m_pTables[ind] = m_pParentModel->GetEvalTable(m_tableIDs[ind]); 
	return m_pTables[ind]; 
}

/** Retrieve a table associated with Evaluable (if any).
	@return Pointer to associated EvalTable, or NULL if none exists.
*/
EvalTable* Evaluable::GrabTData(const size_t & ind) const
{
	if (!m_pTables[ind] && m_pParentModel && !m_tableIDs[ind].empty())
		return m_pParentModel->GetEvalTable(m_tableIDs[ind]);
	return m_pTables[ind];
}