#pragma once
#include "Summarizable.h"
#include <list>
#include<string>

class OMEObject;

/** Flag denoting what type of error is associated with a message. */
enum EC_ERR_TYPE { EC_HEADER, EC_GROUP, EC_WARNING, EC_CRITICAL };

/** Wrapper for error-related information. */
struct ErrEntry
{
	EC_ERR_TYPE m_errType;		///< The type of error; informs parent program how to respond.
	std::string m_errMsg;		///< A message describing the error.
	OMEObject* m_pErrObj;		///< Pointer to the OMEObject that generated the error (optional).

	/** Convenience Constructor.
		@param msg The string describing the error.
		@param eType Severity flag.
		@param pObj Optional pointer to originating object.
	*/
	ErrEntry(const std::string msg=std::string(),const EC_ERR_TYPE eType=EC_CRITICAL,OMEObject* pObj=NULL) \
		:m_errMsg(msg),m_errType(eType),m_pErrObj(pObj) {}
};

typedef std::list<ErrEntry>::const_iterator ErrItr;

/** Simple class for collecting, handling, and displaying errors. 

	Error entries in this class consist of an error description and an
	error-type flag. The following flags are available:

	- __EC_CRITICAL:__ Error type that represents a problem that the program cannot recover from.
	- __EC_WARNING:__ An issue that may cause unintentional behavior, but will not break the program.
	- __EC_GROUP:__ Informational; inserts a subheader into the list to display when ToString() is called.
	- __EC_HEADER:__ Same as EC_GROUP, but inserted at the head of the list.
*/
class  __EXPORT__ ErrCollection : public Summarizable
{
public:
	ErrCollection(void);
	~ErrCollection(void);

	/** Copy Constructor.
		@param inErr The ErrCollection to copy.
	*/
	ErrCollection(const ErrCollection & inErr) { Duplicate(inErr); };
	/** Assignment Operator.
		@param inErr The right-hand side argument for the operator.
	*/
	ErrCollection& operator=(const ErrCollection & inErr) { Duplicate(inErr); return *this; };

	ErrCollection& operator+=(const ErrCollection & rhs);
	//ToString
	int GetErrCount() const;
	int GetCount() const;
	int GetCriticalCount() const;

	void AddErr(const OMEChar* rhs, EC_ERR_TYPE eType=EC_CRITICAL,OMEObject* pObj=NULL,const bool addFront=false);
	void AddErr(const std::string & rhs, EC_ERR_TYPE eType=EC_CRITICAL,OMEObject* pObj=NULL,const bool addFront=false); 
	void AddErr(ErrCollection& rhs);

	void ClearErrs();

	virtual const OMEChar* ToString(const OMEChar tabChar=' ', const unsigned int indent=0, const unsigned int inc=4) const;

	/** @return ErrItr that points to the last ErrEntry. */
	inline ErrItr GetEnd() const {return m_errMsgs.cend(); }
	/** @return ErrItr that points to the first ErrEntry. */
	inline ErrItr GetStart() const {return m_errMsgs.cbegin(); };

private:
	//must be pointer to avoid dll issues
	std::list<ErrEntry> m_errMsgs;		///< List of present error messages.

	void Duplicate(const ErrCollection & inErr);
};

