#pragma once
#include "OMEObject.h"

/** Class that represents basic user-provided labels used in the visual environment.
*/
class __EXPORT__ SimpleLabel : public OMEObject
{
public:
	/** Default Constructor */
	SimpleLabel(void) : OMEObject() {};

	/** Simple Constructor.
		@param name Name of the label.
		@param text Text of the label.
	*/
	SimpleLabel(STLString name, STLString text) : OMEObject(name,""), m_text(text) {}

	/** Copy Constructor.
		@param sl The SimpleLabel to copy.
	*/
	SimpleLabel(const SimpleLabel & sl) { Duplicate(sl); }
	/** Assignment Operator.
		@param rhs The SimpleLabel to copy.
		@return Reference to the SimpleLabel.
	*/
	SimpleLabel& operator=(const SimpleLabel & rhs) { Duplicate(rhs); return *this; };
	/** Comparison operator.
		@param rhs The SimpleLabel to compare values against.
		@return true If SimpleLabel values are equal; false otherwise.
	*/
	bool operator==(const SimpleLabel & rhs);
	/** XML constructor.
		@param pCurrElem The current TinyXML element.
		@param tag The tag used in the Object's XML representation.
		@param filename Path to originating file.
	*/
	SimpleLabel(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
	{ OMEObject::PopulateFromXML(pCurrElem,tag,filename); }
	
	/** Destructor */
	virtual ~SimpleLabel(void){};

	/** @return The display text value. */
	virtual STLString GetText() const { return m_text; };
	/** Set the Displayed Text Value.
		@param txt The text to display.
	*/
	virtual void SetText(const STLString & txt) { m_text=txt; };

   virtual STLString GetXMLRepresentation(const unsigned int indent=0, const unsigned int inc=4);
   virtual void GetXMLAttributes(StrArray & out) const;
   virtual const OMEChar* GetXMLTag() const { return "label"; }

   virtual OMETYPE GetOMEType() const { return OME_LABEL; }
   virtual SimpleLabel* Clone() { return new SimpleLabel(*this); }

protected:
	STLString m_text;	///< The text that is intended to be displayed during drawing.

	/** Copy another SimpleLabel.
		@param sl The SimpleLabel to copy.
	*/
	void Duplicate(const SimpleLabel & sl) { m_text= sl.m_text; OMEObject::Duplicate(sl);};
	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename);
};

