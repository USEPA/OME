#pragma once
#include "OMEObject.h"

enum NULLTYPE {NULL_BASE=0, NULL_ALM, NULL_FUNC, NULL_SOURCE, NULL_PROPERTIES, NULL_CLOUD, NULL_GHOST};

/** Non-modelable object 
	Object used to represent an object aspect that isn't actually modelled; used during model construction and importation.
*/
class OMENull : public OMEObject
{   
public:
	/**Default Constructor */
	OMENull() :OMEObject() {}
	/**Simple Constructor
		@param n name of null object
		@param d description of null object
	*/
	OMENull(STLString n, STLString d) : OMEObject(n,d) {}

	/** Copy Constructor
		@param omn the object to copy
	*/
	OMENull(const OMENull & omn) { Duplicate(omn); };

	virtual ~OMENull() {};

	/** Assignment Operator.
		@param omn The OMENull to copy.
		@return Reference to the OMENull.
	*/
	OMENull& operator=(const OMENull & omn) { Duplicate(omn); return *this; };

   virtual OMETYPE GetOMEType( void ) const { return OME_NULL; }
   virtual bool IsOMEType(OMETYPE type) const { return type==OME_NULL || OMEObject::IsOMEType(type); }
   /** Returns OMENull object class identifier */
   virtual NULLTYPE GetNullType( void ) { return NULL_BASE; }

   STLString GetXMLRepresentation(const unsigned int indent=0, const unsigned int inc=4) { return STLString(""); };
   void GetXMLAttributes(StrArray & out) const {};
   void GetXMLSubNodes(StrArray &out, const unsigned int indent=0, const unsigned int inc=4) {};

   virtual OMENull* Clone() { return NULL;}; //disable cloning for null objects
protected:
	virtual void GetXMLAttributeMap(XMLAttrVector & out) { };
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename) { return 1; };

	/** Copy values from another OMENull object
	@param omn the OMENull object to copy
*/
	void Duplicate(const OMENull & omn) { OMEObject::Duplicate(omn); };
}; 
