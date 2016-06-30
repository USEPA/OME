#include "DrawDetails.h"
#include "omeobjects/SimObjs.h"

namespace DDCache
{
	std::map<OMEObject*,DrawDetails*> m_ddMap;
};

/** Simple Constructor.
	@param pObj Pointer to object to represent.
*/
DrawDetails::DrawDetails(OMEObject* pObj)
	:m_pSrcObj(pObj)
{ 
	if(m_pSrcObj) 
		DDCache::m_ddMap[m_pSrcObj]=this; 
}

/** Destructor. */
DrawDetails::~DrawDetails()
{
	DDCache::m_ddMap.erase(m_pSrcObj);
}

/** Sets source object and sets association in DrawDetails map.
	@param pObj Pointer to object to represent.
*/
void DrawDetails::SetSrcObj(OMEObject* pObj) 
{ 
	if(m_pSrcObj)
		DDCache::m_ddMap.erase(m_pSrcObj);

	m_pSrcObj=pObj;

	DDCache::m_ddMap[m_pSrcObj]=this;
}

STLString DrawDetails::GetXMLRepresentation(const unsigned int indent,const unsigned int inc)
{
	STLString tag=GetXMLTag();
	oSTLSStream outStrm; 

	StrArray attrs, subs;		
	GetXMLAttributes(attrs);
	GetXMLSubNodes(subs,indent+inc,inc);

	outStrm<<STLString(indent,' ')<<"<"<<tag<<" ";

	for(auto itr=attrs.begin(); itr!=attrs.end(); ++itr)
		outStrm<<(*itr)<<" ";

	if(subs.size())
	{
		outStrm<<">"<<std::endl;

		for(auto itr=subs.begin(); itr!=subs.end(); ++itr)
			outStrm<<(*itr)<<std::endl;

		outStrm<<STLString(indent,' ')<<"</"<<tag<<">";
	}
	else
		outStrm<<"/>";

	return outStrm.str();
}

void DrawDetails::GetXMLAttributes(StrArray & out) const
{
	if(m_pSrcObj)
	{
		m_pSrcObj->GetXMLAttributes(out);

		if(m_lastModifiedDate.length())
			AddAttributeValue<STLString>("last_modified","\""+m_lastModifiedDate+"\"",out);
		if(m_version.length())
			AddAttributeValue<STLString>("version","\""+m_version+"\"",out);
		if(m_unitStr.length())
			AddAttributeValue<STLString>("units","\""+m_unitStr+"\"",out);

		if(m_lblOrigin.x!=0 || m_lblOrigin.y!=0)
			AddAttributeValue("label_origin",m_lblOrigin,out);

		//use a different set of coordinate tags for link types
		if(!dynamic_cast<Link*>(m_pSrcObj))
		{
			AddAttributeValue("origin",m_origin,out);
			AddAttributeValue("extents",m_dims,out);
		}
		else
		{
			AddAttributeValue("ctrl_pt1",m_ctrlPoint1,out);
			AddAttributeValue("ctrl_pt2",m_ctrlPoint2,out);
		}

		if(m_pSrcObj->GetOMEType()==OME_MODEL)
		{
			AddAttributeValue("inner_box",m_internalExts,out);
		}
		
	}
	else
	{
		throw OMEFuncOpException("DrawDetails::GetXMLAttributes", "No associated model object");
	}
}

void DrawDetails::GetXMLSubNodes(StrArray & out,const unsigned int indent,const unsigned int inc)
{
	STLString baseIndent(indent,' ');
	if(m_description.length())
		out.push_back(baseIndent+"<description>\n"+STLString(indent+inc,' ')+m_description+"\n"+baseIndent+"</description>");

	if(m_pSrcObj)
	{
		//special handling for model since Models call their children xml details and would bypass associated DrawDetails
		if(m_pSrcObj->GetOMEType()!=OME_MODEL)
		{
			m_pSrcObj->GetXMLSubNodes(out,indent,inc);

		}
		else
		{
			//grab none drawables as normal...
			Model* pMdl=((Model*)m_pSrcObj);
			pMdl->GetXMLNonDrawableSubNodes(out,indent,inc);

			//and override drawables.
			//variables
			CollectionXMLDrawables("variables",pMdl->m_varArray,out,indent,inc,pMdl);
			//states
			CollectionXMLDrawables("states",pMdl->m_stateVarArray,out,indent,inc,pMdl);
			//flows
			CollectionXMLDrawables("flows",pMdl->m_flowArray,out,indent,inc,pMdl);
			//influences
			//for (auto itr = pMdl->m_influenceArray.begin(); itr != pMdl->m_influenceArray.end(); ++itr)
			//	DBG_PRINTC((*itr)->GetID(), DBG_GREEN);
			//DBG_PRINTC(std::to_string(pMdl->m_influenceArray.size()), DBG_MAGENTA);
			CollectionXMLDrawables("influences",pMdl->m_influenceArray,out,indent,inc,pMdl);
			//labels
			CollectionXMLDrawables("labels",pMdl->m_labelArray,out,indent,inc,pMdl);
			//iterConditionals
			CollectionXMLDrawables("iterators",pMdl->m_iterCondArray,out,indent,inc,pMdl);
			//ports
			CollectionXMLDrawables("modelports",pMdl->m_portArray,out,indent,inc,pMdl,true);
			//SubmodelAssocs
			CollectionXMLDrawables("assocs",pMdl->m_subAssocArray,out,indent,inc,pMdl/*,true*/);
			//Spawners
			CollectionXMLDrawables("spawners",pMdl->m_spawnerArray,out,indent,inc,pMdl);

			CollectionXMLDrawables("aliases", pMdl->m_aliasArray, out, indent, inc, pMdl);
			//submodels
			CollectionXMLDrawables("submodels",pMdl->m_submodelArray,out,indent,inc,pMdl);
		}
	}
}

const OMEChar* DrawDetails::ToString(const OMEChar tabChar, const unsigned int indent, const unsigned int inc) const
{
	if(m_pSrcObj)
	{
		//append drawing details
		return m_pSrcObj->ToString(tabChar,indent,inc);
	}
	else
	{
		return "DrawDetails is not associated with OMEObject; this is likely an error.";
	}
}

void DrawDetails::GetXMLAttributeMap(XMLAttrVector & out)
{

	XmlAttr attrs[] = {
		// attr					type		   address                          isReq 
		{ "last_modified",	XML_STR,		&(this->m_lastModifiedDate),		false	},
		{ "version",			XML_STR,		&(this->m_version),					false	},
		{ "units",				XML_STR,		&(this->m_unitStr),					false	},
		{ "origin",				XML_POINT,	&(this->m_origin),					false	},
		{ "label_origin",		XML_POINT,	&(this->m_origin),					false	},
		{ "extents",			XML_POINT,	&(this->m_dims),						false	},
		{ "ctrl_pt1",			XML_POINT,	&(this->m_ctrlPoint1),				false	},
		{ "ctrl_pt2",			XML_POINT,	&(this->m_ctrlPoint2),				false	},
		{ "inner_box",			XML_BOX,	&(this->m_internalExts),			false	},

		{ NULL,					XML_NULL,	NULL,										false	}};

	//drop null
	int count;
	for(count=0; attrs[count].name!=NULL; count++)
		out.push_back(attrs[count]);

}

int DrawDetails::PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag,const STLString & filename)
{
	TI2Element *pXmlDesc = pCurrElem->FirstChildElement( "description" );
	if ( pXmlDesc )
		this->m_description = pXmlDesc->GetText();     // note: doesn't handle nested xml
	else
		this->m_description="";
	return 1;
}

//statics
/** Retrieve a DrawDetails object for a specific object.
	@param pObj Pointer to OMEObject to use for query.
	@return Pointer to a DrawDetails object that is associated with pObj, or NULL if none is found.
*/
DrawDetails* DrawDetails::DDForObject(OMEObject* pObj)
{
	auto fItr=DDCache::m_ddMap.find(pObj);
	return fItr!=DDCache::m_ddMap.end() ? (*fItr).second : NULL;
}

/** Replace object in cache with another object in cache.
	@param pOld Pointer to object to be replaced.
	@param pNew Pointer to object to be inserted.
*/
void DrawDetails::ReplaceObjectForDD(OMEObject* pOld, OMEObject* pNew)
{
	auto fItr = DDCache::m_ddMap.find(pOld);
	if (fItr != DDCache::m_ddMap.end())
	{
		//erase first just in case pOld==pNew
		DrawDetails* pDD = (*fItr).second;
		pDD->SetSrcObj(pNew);
		DDCache::m_ddMap.erase(pOld);
		DDCache::m_ddMap[pNew] =pDD;
		
	}
}

/** Deallocates and removes any DrawDetails from the global/static map.*/
void DrawDetails::CleanupAllDDs()
{
	//copy dds to a temporary list, then dealloc all of them
	//(deallocation automatically removes them from map.

	std::list<DrawDetails*> tempList;
	std::for_each(DDCache::m_ddMap.begin(),DDCache::m_ddMap.end(),[&tempList](pair<OMEObject*,DrawDetails*> p){ tempList.push_back(p.second);});
	std::for_each(tempList.begin(),tempList.end(),[](DrawDetails* pDD){ delete pDD; });
}

/** Retrieve reference to global cache object.
	@return a reference to global map object which maps an OMEObject to its associated DrawDetail object.
*/
std::map<OMEObject*,DrawDetails*>& DrawDetails::GetDrawObjectCache()
{
	return DDCache::m_ddMap;
}

/** Set all point ordinates to 0.*/
void DrawDetails::ZeroOutPts()
{
    m_ctrlPoint1[0]=0;
    m_ctrlPoint1[1]=0;
    m_ctrlPoint2[0]=0;
    m_ctrlPoint2[1]=0;
}
