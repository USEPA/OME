#pragma once

#include <string>

#include "OMEDefines.h"

typedef std::map<OMEChar,STLString> SymMap;

/** Virtual class defining a standard interface for loading and storing using a standardized XML interface */
class __EXPORT__  XMLLoadable
{
public:
	/** Default constructor. */
	XMLLoadable() {}
	virtual ~XMLLoadable(){}
	/** Get an XML Representation of the object.
		@param indent the current indentation level.
		@param inc the increment step for each additional level of indentation.
		@return a string containing an xml representation of the object.
			*/
	virtual STLString GetXMLRepresentation(const unsigned int indent=0,const unsigned int inc=4);
	/** Get a list of attribute flags and values.
		@param out an array to store each attribute entry.
	*/
	virtual void GetXMLAttributes(StrArray & out) const=0;
   /** Get a list of subnode entries.
    @param out An array to store each attribute entry.
		@param indent The current indentation level.
		@param inc The increment step for each additional level of indentation.
		@return A string containing an xml representation of the object.
			*/
   virtual void GetXMLSubNodes(StrArray & out,const unsigned int indent=0,const unsigned int inc=4)=0;

   int PopulateFromXML(TI2Element* pCurrElem,const STLString & tag, const STLString & filename);

     //void GetXMLAttrs(CXMLAttrVector & out) { return GetXMLAttributeMap(out); };

   /** @return xml tag identifier for object. */
   virtual const OMEChar* GetXMLTag() const=0;

 
   template<class T>
   static void AddAttributeValue(const OMEChar* attr, const T & val,StrArray & out);

	static void AddAttributeValue(const OMEChar* attr, const OMEPoint & pt,StrArray & out);
	static void AddAttributeValue(const OMEChar* attr, const OMEBox & bx,StrArray & out);
	static bool ParseAttributes(TI2Element *pXml, XMLAttrVector & attrs,const STLString & element=STLString(), const STLString & filename=STLString());
	static bool ParseAttributes(TI2Element *pXml,XmlAttr attrs[],const STLString & element=STLString(), const STLString & filename=STLString());

	static STLString ErrDescription(const TI2Error & err);

protected:

	/** Collect a list of Tinyxml-compatible attribute maps. The null terminator is not included
		@param out the list to populate.
	*/
	virtual void GetXMLAttributeMap(XMLAttrVector & out)=0;
	/** Grab data from child elements from XML information
		Object attributes are populated based on information defined in the pCurrElem pointer
			@param pCurrElem the current Tiny XML Element
			@param tag the name of the desired tag containing the info.
			@param filename the filename associated with the XML info.
			@return 1 on success, 0 on failure.
	*/
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem, const STLString & tag,const STLString & filename)=0;

	static STLString & XMLizeString(STLString & str,const bool ignoreEdgeQuotes=true);

private:
	static XmlAttr m_sentinel;			///<Sentinel value used for terminating arrays
	static XmlAttr InitSentinel();
	static SymMap InitSymMap();
};

/** Add an Attribute to the StrArray specified in out.
@param attr The name of the attribute to add.
@param val The value to associate with the attribute.
@param out The StrArray to store the new attribute entry.
*/
template<class T> void XMLLoadable::AddAttributeValue(const OMEChar* attr, const T & val,StrArray & out)
{
	static oSTLSStream outStrm;
	outStrm.str(STLString());
	outStrm.clear();
	outStrm<<val;
	char qt='\'';

    STLString temp=outStrm.str();
	XMLizeString(temp);

	//if single quote is present in xmlized string, bound with doublequotes.
	if(temp.find_first_of('\'')!=STLString::npos)
		qt='"';
	outStrm.str(STLString());
	outStrm.clear();

	outStrm<<attr<<'='<<qt<<temp<<qt;
	out.push_back(outStrm.str());
}
