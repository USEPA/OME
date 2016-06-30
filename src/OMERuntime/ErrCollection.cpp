#include "ErrCollection.h"
#include "omeobjects/OMEObject.h"
#include <sstream>

/**Default constructor. */
ErrCollection::ErrCollection(void)
{
	
}

/** Destructor.*/
ErrCollection::~ErrCollection(void)
{
}

/** Query the collection for the total number of errors.
	@return number of errors in list.
*/
int ErrCollection::GetErrCount() const
{ 
		int count=0;

	for(auto itr=m_errMsgs.begin(); itr!=m_errMsgs.end(); ++itr)
	{
		if((*itr).m_errType==EC_CRITICAL || (*itr).m_errType==EC_WARNING)
			count++;
	}

	return count;
}

/** Query the collection for all entries in msg list.
	@return number of entries in the msg list.
*/
int ErrCollection::GetCount() const
{
	return (int)m_errMsgs.size(); 
}

/** Query the collection for the total number of critical errors.
	@return number of critical errors in list.
*/
int ErrCollection::GetCriticalCount() const
{
	int count=0;

	for(auto itr=m_errMsgs.begin(); itr!=m_errMsgs.end(); ++itr)
	{
		if((*itr).m_errType==EC_CRITICAL)
			count++;
	}

	return count; 
}

/** Add an error notification with a given classification.
	@param rhs c-string message.
	@param eType The type of error string being added.
	@param pObj Optional pointer to object that reported the error.
	@param addFront If true, the error is added to the front of the error list; otherwise added to the end of the list.
*/
void ErrCollection::AddErr(const OMEChar* rhs, EC_ERR_TYPE eType,OMEObject* pObj,bool addFront)
{
	AddErr(std::string(rhs),eType,pObj,addFront);
}

/** Add an error notification with a given classification.
	@param rhs STL string message.
	@param eType The type of error string being added.
	@param pObj Optional pointer to object that reported the error.
	@param addFront If true, the error is added to the front of the error list; otherwise added to the end of the list.
*/
void ErrCollection::AddErr(const std::string & rhs, EC_ERR_TYPE eType,OMEObject* pObj,bool addFront)
{
	ErrEntry entry(rhs,eType,pObj);
	addFront ? m_errMsgs.push_front(entry) : m_errMsgs.push_back(entry);
}

/** Clear all error messages. */
void ErrCollection::ClearErrs()
{
	m_errMsgs.clear();
}

/** Append error notifications from another ErrCollection.
	@param rhs ErrCollection whose content will be appended.
*/
void ErrCollection::AddErr(ErrCollection& rhs)
{
	m_errMsgs.insert(m_errMsgs.end(),rhs.m_errMsgs.begin(),rhs.m_errMsgs.end());
}

/** Duplicate another ErrCollection.
	@param inErr The ErrCollection to duplicate.
*/
void ErrCollection::Duplicate(const ErrCollection & inErr)
{
	m_errMsgs.assign(inErr.m_errMsgs.begin(),inErr.m_errMsgs.end());
}

const char* ErrCollection::ToString(const char tabChar, const unsigned int indent, const unsigned int inc) const
{
	using namespace std;

	string front(indent,tabChar);
	string headerfront(indent+inc,tabChar);
	string errfront(indent+inc*2,tabChar);

	ostringstream oss;

	int errCount=GetErrCount();
	if(errCount)
	{
		oss<<front<<"The following ";
		if(errCount==1)
			oss<<"error was ";
		else
			oss<<errCount<<" errors were ";
		oss<<"found:"<<endl;

		for(auto itr=m_errMsgs.begin(); itr!=m_errMsgs.end(); ++itr)
		{
			switch((*itr).m_errType)
			{
			case EC_HEADER:
			case EC_GROUP:
				oss<<headerfront;
				break;
			
			default:
				oss<<errfront;
			};
			if((*itr).m_pErrObj)
				oss<<(*itr).m_pErrObj->GetName()<<": ";
			oss<<(*itr).m_errMsg<<endl;
		}
	}
	else
		oss<<"No errors found."<<endl;

	int outSize=(int)oss.str().size();
	char* buff= new char[outSize+1];
	buff[outSize]='\0';
	oss.str().copy(buff,outSize);

	return buff;
}

/** Concat operator.
	Appends errs from rhs to this.
	@param rhs The ErrCollection containing The errors to append.
	@return a reference to this with new errors appended to its error list.
*/
ErrCollection& ErrCollection::operator+=(const ErrCollection & rhs)
{
	m_errMsgs.insert(m_errMsgs.end(),rhs.m_errMsgs.begin(),rhs.m_errMsgs.end());
	return *this;
}