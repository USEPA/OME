#pragma once
#include <string.h>

#include "OMEObject.h"
#include "Evaluable.h"

#include "tinyxml2.h"
#include "OMEDefines.h"

enum LINKTYPE {LN_NULL=0,LN_FLOW,LN_INFLUENCE};

/** Base class for link objects.

	This class represents a source-target relationship, and is used to augment OMEObjects that require such a representation.	
*/
class __EXPORT__ Link : public XMLLoadable
{
public:
	
	Link();
	Link(STLString src, STLString trg);
	Link(STLString src, STLString trg, OMEObject* pSrc, OMEObject* pTrg);

	/** Copy constructor
		@param lnk the Link to copy
	*/
	Link(const Link & lnk) { Duplicate(lnk); };
	/** Assignment Operator.
		@param lnk The Link to use in the assignment.
		@return A reference to the current Link object.
	*/
	Link& operator=(const Link & lnk) { Duplicate(lnk); return *this;};

	~Link();

	//member concantenation overload
	//global/static contantenator overload
	//may want to override in subclasses

	Link& operator+=(const Link & rhs);
	friend Link operator+ (const Link & lhs, const Link & rhs);

	bool operator==(const Link & rhs);
   
	/** @return String containing ID of source object. */
	inline STLString GetSourceID() const {return m_source;}
	/** @return String containing ID of target object. */
	inline STLString GetTargetID() const {return m_target;}

	/** Set ID for associated source.
		@param src ID of the Source Object.
		@return true if there does not already exist a pointer to a source object; false otherwise.
	*/
	inline bool SetSourceID(const STLString & src) { if(!m_pSource) m_source=src; return !m_pSource; }
	/** Set ID for associated source.
	@param src ID of the Source Object as raw C-String.
	@return true if there does not already exist a pointer to a source object; false otherwise.
	*/
	inline bool SetSourceID(const OMEChar* src) { return SetSourceID(STLString(src)); }
	/** Set ID for associated target.
	@param trg ID of the Target Object.
	@return true if there does not already exist a pointer to a target object; false otherwise.
	*/
	inline bool SetTargetID(const STLString & trg) { if(!m_pTarget) m_target=trg; return !m_pTarget; }
	/** Set ID for associated target.
	@param trg ID of the Target Object as raw C-String.
	@return true if there does not already exist a pointer to a target object; false otherwise.
	*/
	inline bool SetTargetID(const OMEChar* trg) { return SetTargetID(STLString(trg)); }

	virtual void SetPSource(OMEObject* src);
	virtual void SetPTarget(OMEObject* trg);

	virtual Evaluable* GetPEvalSource() const;
	virtual Evaluable* GetPEvalTarget() const;

	/** Get source pointer as OMEObject. 
		@return pointer to source object. 
	*/
	virtual OMEObject* GetPOMESource() const { return m_pSource; }; 
	/** Get target pointer as OMEObject.
		@return pointer to target object. 
	*/
	virtual OMEObject* GetPOMETarget() const { return m_pTarget; }; 

	/** Unique Link class identifier
		@return unique identifier for Link subclass.
		*/
	virtual LINKTYPE GetLinkType( void ) {return LN_NULL;};

	void SyncWithMap(ObjMap & map, const bool forceAssign=false);


	virtual void GetXMLAttributes(StrArray & out) const;
	virtual STLString GetXMLRepresentation(const unsigned int indent=0,const unsigned int inc=4) { return STLString(""); };
	virtual const OMEChar* GetXMLTag() const { return "link"; }

	virtual void GetXMLSubNodes(StrArray & out,const unsigned int indent=0,const unsigned int inc=4) {};

protected:

   OMEObject *m_pSource;		///< Pointer to source object.
   OMEObject *m_pTarget;		///< Pointer to target object.
   STLString    m_source;	///< unique id of source object.
   STLString    m_target; ///< unique id of target object.

   void Duplicate(const Link & lnk);
   bool static HasCommonRoot(OMEObject* src, OMEObject* trg, bool recurse=false);
   
   virtual void GetXMLAttributeMap(XMLAttrVector & out);
   virtual int PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename) { return 1; };
   
};

Link operator+ (const Link & lhs, const Link & rhs);