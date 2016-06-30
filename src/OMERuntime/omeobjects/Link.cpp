#include "omeobjects/Link.h"
#include "omeobjects/Evaluable.h"
#include "omeobjects/SimObjs.h"

/** Default Constructor */
Link::Link()
	:m_source(""), m_target(""), m_pSource(NULL), m_pTarget(NULL)
{}
/** Simple Constructor
	Takes unique ids for source and target; assumes that pointers will be supplied later.
	@param src unique id of source object.
	@param trg unique id of target object.
*/
Link::Link(STLString src, STLString trg)
	:m_source(src), m_target(trg), m_pSource(NULL), m_pTarget(NULL)
{}

/** Full Constructor
	Takes unique ids and pointers for source and target objects.
	@param src unique id of source object.
	@param trg unique id of target object.
	@param pSrc pointer to source object.
	@param pTrg pointer to target object.
*/
Link::Link(STLString src, STLString trg, OMEObject* pSrc, OMEObject* pTrg)
	:m_source(src), m_target(trg), m_pSource(pSrc), m_pTarget(pTrg)
{}

/** Copies values from another link.
	@param lnk the link to be copied.
*/
 void Link::Duplicate(const Link & lnk)
 {
	 m_source=lnk.m_source;	
	 m_target=lnk.m_target;

     //m_ctrlPoint0=lnk.m_ctrlPoint0;
     //m_ctrlPoint1=lnk.m_ctrlPoint1;

	 m_pSource=lnk.m_pSource;
	 m_pTarget=lnk.m_pTarget;

 }

/** Destructor */
Link::~Link()
{

}

/** Join Operator
	Creates a new link with source entries from lhs and target entries from rhs; control points averaged.
	@param lhs Left Hand Side argument.
	@param rhs Right Hand Side argument.
	@return new link object.
	*/	
Link operator+ (const Link & lhs, const Link & rhs)
{
	Link out=Link(lhs.m_source,rhs.m_target,lhs.m_pSource,rhs.m_pTarget);
	
	//for now, just average control points
	//out.m_ctrlPoint0.x=(lhs.m_ctrlPoint0.x+rhs.m_ctrlPoint0.x)/2;
	//out.m_ctrlPoint0.y=(lhs.m_ctrlPoint0.y+rhs.m_ctrlPoint0.y)/2;

	//out.m_ctrlPoint1.x=(lhs.m_ctrlPoint1.x+rhs.m_ctrlPoint1.x)/2;
	//out.m_ctrlPoint1.y=(lhs.m_ctrlPoint1.y+rhs.m_ctrlPoint1.y)/2;

	return out;
}

/** Concantenation operator.
	Replaces target info with that of rhs; averages control points.
	@param rhs right hand side argument
*/
Link& Link::operator+=(const Link & rhs)
{
	this->m_target=rhs.m_target;
	this->m_pTarget=rhs.m_pTarget;

	//for now, just average control points
	//this->m_ctrlPoint0.x=(this->m_ctrlPoint0.x+rhs.m_ctrlPoint0.x)/2;
	//this->m_ctrlPoint0.y=(this->m_ctrlPoint0.y+rhs.m_ctrlPoint0.y)/2;

	//this->m_ctrlPoint1.x=(this->m_ctrlPoint1.x+rhs.m_ctrlPoint1.x)/2;
	//this->m_ctrlPoint1.y=(this->m_ctrlPoint1.y+rhs.m_ctrlPoint1.y)/2;

	return *this;
}

/** Setter for source pointer.
	@param pSrc new source pointer.
*/
void Link::SetPSource(OMEObject* pSrc)
{
	m_pSource=pSrc;
	m_source=pSrc ? pSrc->GetID() : "";

}

/** Setter for target pointer.
	@param pTrg new target pointer.
*/
void Link::SetPTarget(OMEObject* pTrg)
{
	m_pTarget=pTrg;
	m_target= pTrg ? pTrg->GetID() : "";

}
/** Getter for source pointer.
	@return pointer to source object.
*/
Evaluable* Link::GetPEvalSource() const
{
	return dynamic_cast<Evaluable*>(m_pSource);
}

/** Getter for target pointer.
	@return pointer to target object.
*/
Evaluable* Link::GetPEvalTarget() const
{
	return dynamic_cast<Evaluable*>(m_pTarget);
}

	/** Generates a list of attributes.
		Generates a list of strings, one for each attribute entry for the top-tier XML tag
			@param out  the array to be populated by attribute strings.

*/
void Link::GetXMLAttributes(StrArray & out) const
{

	if(m_pSource)
		AddAttributeValue<STLString>("source",m_pSource->GetID(),out);
	else if(m_source.length())
		AddAttributeValue<STLString>("source",m_source,out);

	//const Evaluable* pEval = dynamic_cast<const Evaluable*>(this);
	//if (pEval)
	//	DBG_PRINTC(pEval->GetID(), DBG_RED);
	if(m_pTarget)
		AddAttributeValue<STLString>("target",m_pTarget->GetID(),out);
	else if(m_target.length())
		AddAttributeValue<STLString>("target",m_target,out);

   //AddAttributeValue<int>("ctrl0x",m_ctrlPoint0.x,out);
   //AddAttributeValue<int>("ctrl0y",m_ctrlPoint0.y,out);
   //AddAttributeValue<int>("ctrl1x",m_ctrlPoint1.x,out);
   //AddAttributeValue<int>("ctrl1y",m_ctrlPoint1.y,out);

}


void Link::GetXMLAttributeMap(XMLAttrVector & out)
{
	XmlAttr attrs[] = {
	      // attr				type		      address                       isReq
	  { "source",			XML_STR,	&(this->m_source),				false },
	  { "target",			XML_STR,	&(this->m_target),				false },
	  //{ "ctrl0x",			XML_INT,		   &(this->m_ctrlPoint0.x),		true,		0 },
	  //{ "ctrl0y",			XML_INT,		   &(this->m_ctrlPoint0.y),		true,		0 },
	  //{ "ctrl1x",			XML_INT,		   &(this->m_ctrlPoint1.x),		true,		0 },
	  //{ "ctrl1y",			XML_INT,		   &(this->m_ctrlPoint1.y),		true,		0 },
      { NULL,				XML_NULL,		NULL,							false } };

	  //drop null
	 int count;
	 for(count=0; attrs[count].name!=NULL; count++)
		  out.push_back(attrs[count]);
}

/** Check to see if source and target are in the same model.
	Check to see if source and target share the same root; the root can be restricted to the current level, or checked all the way up the heirarchy.
	@param pSrc pointer to the source object.
	@param pTrg pointer to the target object.
	@param recurse if true, all parents in the hierarchy are compared; if false, only the immediate parents are compared.
	@return true if a common root is found, false otherwise.
*/
bool Link::HasCommonRoot(OMEObject* pSrc, OMEObject* pTrg, bool recurse)
{
	
	if(pSrc && pTrg) 
	{
		//if !recursing, just return base equality
		//otherwise return true if any sources match any targets in each hierarchy
		if(!recurse)
			return (pSrc->GetParentModel() == pTrg->GetParentModel());
		else
			return	(pSrc->GetParentModel() == pTrg->GetParentModel()) ||					//check for base equality
					(HasCommonRoot((OMEObject*)pSrc->GetParentModel(), pTrg, recurse)) || //compare all sources to target
					(HasCommonRoot(pSrc,(OMEObject*)pTrg->GetParentModel(), recurse));	//compare all targets to source
	}
	
	return false; //both rootless or no match; false by definition
}

/** Synchronize source and target pointers with their name string using the provided map.
	@param omap the map to use for name lookup.
	@param forceAssign overwrites existing pointer address if true; otherwise does not modify existing pointers
*/
void Link::SyncWithMap(ObjMap & omap, const bool forceAssign)
{
	OMEObject* entry=NULL;
	if((!m_pSource || forceAssign) && !m_source.empty())
	{
		entry=omap[m_source];
		SetPSource(entry);
	}

	if((!m_pTarget || forceAssign) && !m_target.empty())
	{
		if(omap.find(m_target)!=omap.end())
		{
			//sometimes spawners are linked, but they shouldn't exist in each instance
			entry=omap[m_target];
			SetPTarget(entry);
		}
	}
}

/** Equivalence operator.
	@param rhs The Link object to compare against.
	@return true if this and rhs have equivalent values; false otherwise.
*/
bool Link::operator==(const Link & rhs)
{
	return this==&rhs ||
		(
		m_source==rhs.m_source &&
		m_target==rhs.m_target
		);
}