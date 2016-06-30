#include "ResultsDetailsMgr.h"
#include "TableResultsDetails.h"
#include "TreeResultsDetails.h"
#include "MultiValResultsDetails.h"

/** Default constructor.*/
ResultsDetailsMgr::ResultsDetailsMgr()
	:m_pActiveDetails(NULL)
{
}

/** Destructor. */
ResultsDetailsMgr::~ResultsDetailsMgr()
{
	std::for_each(m_details.begin(),m_details.end(),[](BaseResultsDetails* pRd) { delete pRd; });
}

/** Construct and store a new BaseResultsDetails of a specified type.
	@param rtype The type of BaseResultsDetails to construct.
	@param name The name to apply to the new BaseResultsDetails.
	@return A pointer to the newly allocated BaseResultsDetails.
*/
BaseResultsDetails* ResultsDetailsMgr::NewRD(const RV_TYPE & rtype,const STLString & name)
{
	BaseResultsDetails* pOut=NULL;
	switch(rtype)
	{
	case RVT_TABLE:
		pOut=new TableResultsDetails();
		break;
	case RVT_TREE:
		pOut=new TreeResultsDetails();
		break;
	case RVT_MULTVAL:
		pOut = new MultiValResultsDetails();
		break;
	}

	pOut->SetViewName(name);
	m_details.push_back(pOut);
	return pOut;
}

/** Retrieve the index for the BaseResultsDetails with the provided name.
	@param name The name of the BaseResultsDetails to search for.
	@return the index of the BaseResultsDetails with name, or -1 if it no BaseResultsDetails is found.
*/
int ResultsDetailsMgr::IndexForView(const STLString & name)
{
	int ret=-1;
	for(size_t i=0; i<m_details.size() && ret==-1; i++)
	{
		if(name==m_details[i]->GetViewName())
			ret=i;
	}
	return ret;
}

/** Retrieve the index for the BaseResultsDetails with the provided address.
@param pRd pointer to the BaseResultsDetails to search for.
@return the index of the BaseResultsDetails with the address pRd, or -1 if it no such address is found within the ResultsDetailsMgr.
*/
int ResultsDetailsMgr::IndexForView(BaseResultsDetails* pRd)
{
	int ret=-1;

	for(size_t i=0; i<m_details.size() && ret==-1; i++)
	{
		if(pRd==m_details[i])
			ret=i;
	}
	return ret;
}

/** Add a new BaseResultsDetails to the ResultsDetailsMgr. 
	@param pRd Pointer to the BaseResultsDetails to add.
	@return true if the BaseResultsDetails pointed to by pRd has a unique name and is successfully added; false otherwise.
*/
bool ResultsDetailsMgr::AddRD(BaseResultsDetails* pRd)
{
	bool ret=true;
	for(auto itr=m_details.begin(); itr!=m_details.end() && ret; ++itr)
		ret=((*itr)!=pRd) && ((*itr)->GetViewName()!=pRd->GetViewName());

	return ret;
}

/** Remove an existing BaseResultsDetails from the ResultsDetailsMgr.
	@param pRd Pointer to the BaseResultsDetails to be removed.
*/
void ResultsDetailsMgr::RemoveRD(BaseResultsDetails* pRd)
{
	for(auto itr=m_details.begin(); itr!=m_details.end(); ++itr)
	{
		if(*itr==pRd)
		{
			m_details.erase(itr);
			break;
		}
	}
}

/** Retrieve a specific BaseResultsDetails.
	@param i Index of BaseResultsDetails to retrieve.
	@return Pointer to BaseResultsDetails at index i, or NULL if i is an invalid index.
*/
BaseResultsDetails* ResultsDetailsMgr::GetDetails(size_t i)
{
	BaseResultsDetails* ret=NULL;
	if(i<m_details.size())
		ret=m_details[i];

	return ret;
}

/** Retrieve a specific BaseResultsDetails and mark it as the active details.
	@param i Index of BaseResultsDetails to retrieve.
	@return Pointer to BaseResultsDetails at index i, or NULL if i is an invalid index.
*/
BaseResultsDetails* ResultsDetailsMgr::SetActiveDetails(size_t i)
{
	m_pActiveDetails=NULL;
	if(i<m_details.size())
		m_pActiveDetails=m_details[i];
	return m_pActiveDetails;
}

/** Retrieve a specific BaseResultsDetails and mark it as the active details.
@param pRD Address of the BaseResultsDetails to retrieve.
@return Address pRD, or NULL if the BaseResultsDetails pointed to by pRD are not present in the RecordDetailsMgr.
*/
BaseResultsDetails* ResultsDetailsMgr::SetActiveDetails(BaseResultsDetails* pRD)
{
	m_pActiveDetails=NULL;
	for(auto itr=m_details.begin(); itr!=m_details.end() && !m_pActiveDetails; ++itr)
	{
		if(pRD==(*itr))
			m_pActiveDetails=pRD;
	}
	return m_pActiveDetails;
}

/** Retrieve a specific BaseResultsDetails and mark it as the active details.
@param name Name of BaseResultsDetails to retrieve.
@return Pointer to BaseResultsDetails with the provide name, or NULL if no entry with the provided name is found.
*/
BaseResultsDetails* ResultsDetailsMgr::SetActiveDetails(const STLString & name )
{
	m_pActiveDetails=NULL;
	for(auto itr=m_details.begin(); itr!=m_details.end() && !m_pActiveDetails; ++itr)
	{
		if(name==(*itr)->GetViewName())
			m_pActiveDetails=*itr;
	}
	return m_pActiveDetails;
}

/** Check to see if any other BaseResultsDetails in the RecordDetailsMgr has the same name.
	@param name The name to search for.
	@param pRD Pointer to the BaseResultsDetails with the specified name.
	@return true if only the BaseResultsDetails pointed to by pRD has the specified name; false otherwise.
*/
bool ResultsDetailsMgr::NameIsUnique(const STLString & name, BaseResultsDetails* pRD)
{
	bool ret=true;
	for(auto itr=m_details.begin(); itr!=m_details.end() && ret; ++itr)
	{
		if((*itr)!=pRD && (*itr)->GetViewName()==name)
			ret=false;
	}

	return ret;
}

/** @return true if a change has occurred which will require a save operation; false otherwise.*/
bool ResultsDetailsMgr::SaveIsNeeded() const
{
	return std::any_of(m_details.begin(),m_details.end(),[](BaseResultsDetails* pBrd){ return pBrd->GetSaveNeeded(); });
}

/** Clear the "save required" flags from all stored BaseResultsDetails. */
void ResultsDetailsMgr::ClearSaveFlags()
{
	std::for_each(m_details.begin(),m_details.end(),[](BaseResultsDetails* pBrd){ pBrd->ClearSaveFlag(); });
}

/** Remove all BaseResultsDetails from the RecordDetailsMgr.*/
void ResultsDetailsMgr::ClearViews()
{
	m_details.clear();
	m_pActiveDetails=NULL;
}

void ResultsDetailsMgr::GetXMLAttributes(StrArray & out) const
{
	if(m_pActiveDetails)
	{
		size_t i;
		for(i=0;i<m_details.size() && m_pActiveDetails!=m_details[i];i++);

		if(i<m_details.size())
			XMLLoadable::AddAttributeValue("selected",i,out);
	}
}

void ResultsDetailsMgr::GetXMLSubNodes(StrArray & out,const unsigned int indent,const unsigned int inc)
{
	for(size_t i=0; i<m_details.size(); i++)
		out.push_back(m_details[i]->GetXMLRepresentation(indent,inc));
}

void ResultsDetailsMgr::GetXMLAttributeMap(XMLAttrVector & out)
{
	//no attributes for now
}

int ResultsDetailsMgr::PopulateFromComplexElements(TI2Element* pCurrElem, const STLString & tag,const STLString & filename)
{
	//load views here
	TI2Element* pViewElem=pCurrElem->FirstChildElement();
	STLString subName;
	BaseResultsDetails* pDetails;
	while(pViewElem)
	{
		pDetails=NULL;
		subName=pViewElem->Name();

		if(subName=="tree_view")
			pDetails=new TreeResultsDetails();
		else if (subName == "table_view")
			pDetails = new TableResultsDetails();
		else if (subName == "multival_view")
			pDetails = new MultiValResultsDetails();

		if(pDetails)
		{
			pDetails->PopulateFromXML(pViewElem,subName.c_str(),filename);
			m_details.push_back(pDetails);
		}
		pViewElem=pViewElem->NextSiblingElement();
	}

	//select currentView
	int selected=-1;
	if(pCurrElem->QueryIntAttribute("selected",&selected)==tinyxml2::XML_SUCCESS)
		m_pActiveDetails=m_details[selected];

	return 1;
}

/** Load BaseResultsDetails entries from a OME Control file (.omec).
	@param path A path to the control file to read.
	@return true if the loading operation was successful; false otherwise.
*/
bool ResultsDetailsMgr::LoadFromControlFile(const STLString & path)
{
	TI2Document doc;
	bool ok = doc.LoadFile( path.c_str() )==tinyxml2::XML_SUCCESS;
	if(ok)
		LoadFromXMLTree(doc.RootElement(),path);

	return ok;
}

/** Load BaseResultsDetails entries from an XML heirarchy.
	@param pCurrElem Pointer to the root xml element.
	@param path A path to the originating file.
*/
void ResultsDetailsMgr::LoadFromXMLTree(TI2Element* pCurrElem,const STLString & path)
{
	bool ret=false;
	TI2Element* pViewElem=pCurrElem->FirstChildElement(GetXMLTag());
	if(pViewElem)
		PopulateFromXML(pViewElem,GetXMLTag(),path.c_str());
}