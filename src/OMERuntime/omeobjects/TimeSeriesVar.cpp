#include "omeobjects/TimeSeriesVar.h"
#include "omeobjects/Flow.h"
#include "omeobjects/Influence.h"
#include "omeobjects/SubmodelPort.h"


#define TIME_TOLERANCE 0.00000001
/** Default constructor. */
TimeSeriesVar::TimeSeriesVar()
	:Variable(),m_mode(LAST),m_interval(0.0)
{
}

/** Simple Constructor.
	@param n The name to apply to the TimeSeriesVar.
	@param d Desciption of the time series var.
*/
TimeSeriesVar::TimeSeriesVar(const STLString & n, const STLString & d)
	:Variable(n,d,0.0),m_mode(LAST),m_interval(0.0)
{
}

/** Copy Constructor.
	@param tsv The TimeSeriesVar to copy.
*/
TimeSeriesVar::TimeSeriesVar(const TimeSeriesVar & tsv)
{
	Duplicate(tsv);
}

/** Copy constructor.
	@param var The variable to copy.
*/
TimeSeriesVar::TimeSeriesVar(const Variable & var)
	:m_mode(LAST),m_interval(0.0)
{
	Variable::Duplicate(var);
}

/** XML constructor.
	@param pCurrElem Pointer to the current XML element.
	@param tag The current xml tag.
	@param filename The name of the originating file.
*/
TimeSeriesVar::TimeSeriesVar(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
	:m_mode(LAST),m_interval(0.0)
{
	 OMEObject::PopulateFromXML(pCurrElem,tag,filename);
}

/** Destructor*/
TimeSeriesVar::~TimeSeriesVar()
{
}

OME_SCALAR TimeSeriesVar::Evaluate(const OME_SCALAR time,BaseManager* pBm)
{
	OME_SCALAR ret=GetValueForTime((OME_SCALAR)time);
	SetValue(ret);
	return ret;
}

OME_SCALAR TimeSeriesVar::Initialize(const OME_SCALAR time,BaseManager* pBm)
{
	OME_SCALAR ret=GetValueForTime((OME_SCALAR)time);
	//set initval instead?
	SetValue(ret);
	return ret;
}

void TimeSeriesVar::Reset(BaseManager* pBm)
{
	Evaluate(0.0,NULL);
}

STLString TimeSeriesVar::GetXMLRepresentation(const unsigned int indent,const unsigned int inc)
{
	return GetXMLForObject(indent,inc);
}

void TimeSeriesVar::GetXMLAttributes(StrArray & out) const
{
	Variable::GetXMLAttributes(out);
	AddAttributeValue("interval",m_interval,out);
	
	STLString mode;
	switch(m_mode)
	{
	case LAST:
		mode="last";
		break;
	case NEAREST:
		mode="nearest";
		break;
	case INTERPOLATE:
		mode="interpolate";
		break;
	case FIXED:
		mode = "fixed";
		AddAttributeValue("fixed_val", GetValue(), out);
		break;
	}

	out.push_back(STLString("mode=\"")+mode+'"');
}
 
void TimeSeriesVar::GetXMLSubNodes(StrArray & out,const unsigned int indent,const unsigned int inc)
{
	Variable::GetXMLSubNodes(out,indent,inc);
}

/** Assign time-value pairs.
	@param times A list of times to assign.
	@param values A list of values to assign.
*/
 void TimeSeriesVar::SetSeries(const ARRAY_TYPE<OME_SCALAR> & times, const ARRAY_TYPE<OME_SCALAR> & values)
{
	m_timeEntries.clear();
	for(size_t i=0; i<times.size() && i<values.size(); i++)
		m_timeEntries[(OME_SCALAR)times[i]]=(OME_SCALAR)values[i];
}

/** Assign a series of values. Times are assumed to increment by one starting with 0 (0.0,1.0,2.0...).
	@param values A list of values to assign.
*/
void TimeSeriesVar::SetSeries(const OMESclrArray & values)
{
	m_timeEntries.clear();
	//assume indexing starts at 0.0 and increments by 1.0
	for(size_t i=0; i<values.size(); i++)
		m_timeEntries[(OME_SCALAR)i]=values[i];
}

/** Set the fixed value for the TimeSeriesVar.
	@param value The value to fix to the TimeSeriesVar.
*/
void TimeSeriesVar::SetFixed(const OME_SCALAR & value)
{
	m_mode = FIXED;
	SetValue(value);
}

/** Retrieve a value for a specific time. If a specific value for time isn't designated, then the value returned depends on 
	The interpolation mode set by GetInterval().
	@param time The simulation time to query for a specific value.
	@return The stored or calculated value for time.
	@sa GetInterpolation,SetInterpolation
*/
OME_SCALAR TimeSeriesVar::GetValueForTime(OME_SCALAR time)
{
	OME_SCALAR out=0.0;
	if(m_interval)
		time=fmod(time,m_interval);

	if(!m_timeEntries.empty())
	{
		TSMap::iterator lItr, fItr=m_timeEntries.lower_bound(time);
		lItr=TSMap::iterator(fItr);
		--lItr;

		//if there is an exact entry or if the time preceeds any entries, return the found entry
		//if time is beyond bounds of entry, return the last value
		//otherwise, derive value based on m_mode

		if(fItr==m_timeEntries.end())
			out=(*lItr).second;
		else if((time-TIME_TOLERANCE <=(*fItr).first && time+TIME_TOLERANCE>=(*fItr).first) || fItr==m_timeEntries.begin())
			out=(*fItr).second;
		else
		{
			switch(m_mode)
			{
			case LAST:
				--fItr;
				out=(*fItr).second;
				break;
			case NEAREST:
				out= (time-(*lItr).first) < ((*fItr).first-time) ? (*lItr).second : (*fItr).second;
				break;
			case INTERPOLATE:
				//find temporal fraction
				out=(time-(*lItr).first)/((*fItr).first-(*lItr).first);
				//use fraction to find midrange value
				out=(*lItr).second+(out*((*fItr).second-(*lItr).second)); 
				break;
			case FIXED:
				out = GetValue();
			}
		}
	}
	else if (m_mode==FIXED)
		out = GetValue();
	return out;
}

void TimeSeriesVar::GetXMLAttributeMap(XMLAttrVector & out)
{
		
	 XmlAttr attrs[] = {
      // attr				type		   address                          isReq  
	  { "interval",		XML_SCALAR,	&(this->m_interval),			false },
      { NULL,				XML_NULL,		NULL,							false } };
	  
	  //drop null
	 int count;
	 for(count=0; attrs[count].name!=NULL; count++)
		  out.push_back(attrs[count]);

	Variable::GetXMLAttributeMap(out);
}

int TimeSeriesVar::PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
{
	m_mode=LAST;

	STLString mode=pCurrElem->Attribute("mode");
	
	if(mode=="nearest")
		m_mode=NEAREST;
	else if (mode == "interpolate")
		m_mode = INTERPOLATE;
	else if (mode == "fixed")
	{
		m_mode = FIXED;
		if (pCurrElem->Attribute("fixed_val"))
		{
			OME_SCALAR tmp;
			XMLVALQUERY(pCurrElem, "fixed_val", &tmp);
			SetValue(tmp);
		}
	}
	//else if(mode=="last")
	//	m_mode=LAST;



	return Variable::PopulateFromComplexElements(pCurrElem,tag,filename);
}

/** @param var The TimeSeriesVar to duplicate.*/
void TimeSeriesVar::Duplicate(const TimeSeriesVar & var)
{
	m_interval=var.m_interval;
	m_mode=var.m_mode;
	m_timeEntries=var.m_timeEntries;
}

/** Replace a variable in the model hierarchy with the equivalent TimeSeriesVar.
	@param pVar Pointer to the Variable to replace and dispose of.
	@return The newly creaded TimeSeriesVar.
*/
TimeSeriesVar* TimeSeriesVar::ReplaceVariable(Variable* pVar)
{
	TimeSeriesVar* pTsv=new TimeSeriesVar(*pVar);
	Evaluable* pEval;
	EvalArray objsArray=pVar->GetUpObjects();
	for(size_t i=0; i<objsArray.size(); i++)
	{
		pEval=objsArray[i];
		switch(pEval->GetOMEType())
		{
		case OME_FLOW:
			if(((Flow*)pEval)->GetPEvalTarget()==pVar)
				((Flow*)pEval)->SetPTarget(pTsv);
			else
			{
				pEval->RemoveDownObject(pVar);
				pEval->AddDownObject(pTsv);
			}
			break;
		case OME_INFLUENCE:
			((Influence*)pEval)->SetPTarget(pTsv);
			break;
		case OME_SUBPORT:
			if(((SubmodelPort*)pEval)->IsSubEval(pVar))
			{
				((SubmodelPort*)pEval)->RemoveSubmodelTarget(pVar);
				((SubmodelPort*)pEval)->AddSubmodelTarget(pTsv);
			}
			else
			{
				((SubmodelPort*)pEval)->RemoveOutmodelTarget(pVar);
				((SubmodelPort*)pEval)->AddOutmodelTarget(pTsv);
			}
			break;
		default:
				pEval->RemoveDownObject(pVar);
				pEval->AddDownObject(pTsv);
		}
	}
	
	objsArray=pVar->GetDownObjects();
	for(size_t i=0; i<objsArray.size(); i++)
	{
		pEval=objsArray[i];
		switch(pEval->GetOMEType())
		{
		case OME_FLOW:
			if(((Flow*)pEval)->GetPEvalSource()==pVar)
				((Flow*)pEval)->SetPSource(pTsv);
			else
			{
				pEval->RemoveUpObject(pVar);
				pEval->AddUpObject(pTsv);
			}
			break;
		case OME_INFLUENCE:
			((Influence*)pEval)->SetPSource(pTsv);
			break;
		case OME_SUBPORT:
			if(((SubmodelPort*)pEval)->IsSubEval(pVar))
			{
				((SubmodelPort*)pEval)->RemoveSubmodelSource(pVar);
				((SubmodelPort*)pEval)->AddSubmodelSource(pTsv);
			}
			else
			{
				((SubmodelPort*)pEval)->RemoveOutmodelSource(pVar);
				((SubmodelPort*)pEval)->AddOutmodelSource(pTsv);
			}
			break;
		default:
				pEval->RemoveUpObject(pVar);
				pEval->AddUpObject(pTsv);
		}
	}


	Model* pMdl=pVar->GetParentModel();
	if(pMdl)
	{
		pMdl->RemoveVariable(pVar);
		pMdl->AddVariable(pTsv);
	}
	delete pVar;

	return pTsv;
}

const char* TimeSeriesVar::ToString(const char tabChar, const unsigned int indent, const unsigned int inc) const
 {
	 oSTLSStream out;
	 StrList parts;
	 parts.push_back(OMEObject::ToString(tabChar,indent,inc));

	 if(m_pParentModel)
	{	
		out<<"Reference Index: "<<m_mdlIndex<<endl;
		
		 
		out<<"Instance:"<<endl;
		out<<"  Value: "<<GetValue()<<endl;
		
	 }
	 parts.push_back(out.str());
	 STLString combined=ConcatStrings(parts,'\n');
	 size_t outSize=combined.size();
	 char* outBuff=new char[outSize+1];
	 outBuff[outSize]='\0';
	 SAFE_STRCPY(combined.c_str(), outBuff, outSize+1);
	 return outBuff;
 }

/** @return OMESclrArray containing all explicitly defined times.*/
OMESclrArray TimeSeriesVar::GetTimes()
{
	OMESclrArray ret;
	for(auto itr=m_timeEntries.begin(); itr!=m_timeEntries.end(); ++itr)
		ret.push_back((*itr).first);

	return ret;
}