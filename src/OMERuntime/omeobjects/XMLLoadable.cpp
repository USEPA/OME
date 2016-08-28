#include"omeobjects/XMLLoadable.h"
#include "BaseManager.h"
#include "ErrCollection.h"

#include<bitset>

XmlAttr XMLLoadable::m_sentinel=XMLLoadable::InitSentinel();

/** Initialize an XmlAttr to act as a sentinel value.
	@return XmlAttr to be used as a sentinel value.
*/
XmlAttr XMLLoadable::InitSentinel()
{
	XmlAttr sentinel;
	//added sentinel to end of list
	sentinel.name=NULL;
	sentinel.type=XML_NULL;
	sentinel.value=NULL;
	sentinel.isRequired=false;

	return sentinel;
}

/** Populate object from XML information.
		Object attributes are populated based on information defined in the pCurrElem pointer
			@param pCurrElem the current Tiny XML Element
			@param tag the name of the desired tag containing the info.
			@param filename the filename associated with the XML info.
			@return 1 on success, 0 on failure.
*/
int XMLLoadable::PopulateFromXML(TI2Element* pCurrElem, const STLString & tag, const STLString & filename)
{
		XMLAttrVector attrs;
	   this->GetXMLAttributeMap(attrs);

	   int ok=1;
	   if(!attrs.empty())
	   {
		   attrs.push_back(XMLLoadable::m_sentinel);

		   ok = ParseAttributes( pCurrElem, attrs, tag, filename);
	   }
	  //decription
	  if(ok)
	  {
		  ok=PopulateFromComplexElements(pCurrElem,tag,filename);
	  }

	  return ok;
}

/** Produce string with char codes for illegal xml characters
	@param str The string to convert to conforming xml.
	@param ignoreEdgeQuotes If true, ignore quotes at the beginning and end of the string if they both exist and are equal.
	@return a reference to the post xml-ized str. 
*/
STLString & XMLLoadable::XMLizeString(STLString & str,const bool ignoreEdgeQuotes)
{
	static SymMap sm=InitSymMap();

	if(!str.empty())
	{
		SymMap::iterator itr;
		int last=str.length()-1;	
		int first=0;

		if(ignoreEdgeQuotes && (str.front()=='\'' || str.back()=='"') && str.front()==str.back())
		{
			//ignore first and last chars
			last--;
			first++;
		}

		//step through each char, performing replacements as necessary
		for(int i=last; i>=first; i--)
		{
			itr=sm.find(str[i]);
			if(itr!=sm.end())
				str=str.replace(i,1,(*itr).second);
		}
	}	
	return str;
}

/** Initialize symbol map used for producing conforming xml.*/
SymMap XMLLoadable::InitSymMap()
{
	SymMap ret;
	ret['&']="&amp;";
	ret['<']="&lt;";
	ret['>']="&gt;";

	ret['\'']="&apos;";
	ret['"']="&quot;";

	ret['\n']="&#10;";
	ret['\r']="&#13;";
	ret['\t']="&#09;";

	return ret;
}

/** Extract Listed attributes from a TinyXml2 element.
	This function is directly derived from code found in the Envision code base. http://envision.bioe.orst.edu/
	@param pXml Pointer to a TinyXml2 element.
	@param attrs Vector of attributes to parse from pXml.
	@param element Optional; name or tag of element being parsed. Used for informing error messages.
	@param filename Optional; Path to originating xml file. Used for informing error messages.
	@return true if no errors were encountered; false otherwise.
*/
bool XMLLoadable::ParseAttributes(TI2Element *pXml, XMLAttrVector & attrs,const STLString & element, const STLString & filename)
{
	return ParseAttributes(pXml,&attrs[0],element,filename);
}

/** Extract Listed attributes from a TinyXml2 element.
	This function is directly derived from code found in the Envision code base. http://envision.bioe.orst.edu/
	@param pXml Pointer to a TinyXml2 element.
	@param attrs Array of attributes to parse from pXml.
	@param element Optional; name or tag of element being parsed. Used for informing error messages.
	@param filename Optional; Path to originating xml file. Used for informing error messages.
	@return true if no errors were encountered; false otherwise.
*/
bool XMLLoadable::ParseAttributes(TI2Element *pXml,XmlAttr attrs[],const STLString & element, const STLString & filename)
{
	//based on TiXmlGetAttributes() from the Envision Source Code http://envision.bioe.orst.edu/
	bool ret=true;
	const OMEChar *value;
	int intVal;
	float dVal;
	BaseManager* pBm=BaseManager::GetSingleton();
	ErrCollection errs;
	iSTLSStream inBuff;

	//find size
	size_t aSize;
	for(aSize=0; attrs[aSize].name!= NULL; aSize++);

	std::vector<bool> foundBits(aSize,false);

	const TI2Attribute *pAttr=pXml->FirstAttribute();

	while(pAttr!=NULL)
	{
		const OMEChar* name=pAttr->Name();

		for(size_t i=0; i<aSize; i++)
		{
			if(strcmp(name,attrs[i].name)==0)
			{
				foundBits[i]=true;

				switch(attrs[i].type)
				{
				case XML_BOOL:
					value = pAttr->Value();   // "true" or "false"

					if ( value == NULL )
						break;

					if ( value && ( *value == 't' || *value == 'T' || *value == '1' ))
						*((bool*)attrs[ i ].value) = true;
					else
						*((bool*)attrs[ i ].value) = false;
					break;
				case XML_CHAR:
					value =pAttr->Value();

					if ( value == NULL )
						break;

					*((OMEChar*)attrs[ i ].value) = *value;
					break;
				case XML_STR:
					value = (OMEChar*) pAttr->Value();   // ptr to string held by parser
					*((STLString*)(attrs[ i ].value))= value;
					break;
				case XML_INT:
					intVal=0;
					//if ( pAttr->QueryIntValue( &intVal ) != 0 && pAttr->Value()[0]!='\0')
					//{
					//	errs.AddErr(STLString(attrs[i].name)+" is not an int value.",EC_WARNING,dynamic_cast<OMEObject*>(this));
					//	break;
					//}
					intVal=pAttr->IntValue();		//if not int value, just returns 0.
					*((int*)(attrs[ i ].value)) = intVal;
					break;
				case XML_FLOAT:
				//	if ( pAttr->QueryDoubleValue( &dVal ) != 0 )
				//	{
				//		errs.AddErr(STLString(attrs[i].name)+" is not a float value.",EC_WARNING,dynamic_cast<OMEObject*>(this));
				//		break;
				//	}
					dVal=pAttr->DoubleValue();
					*((float*)(attrs[ i ].value)) = (float) dVal;
					break;
				case XML_DOUBLE:
					//if ( pAttr->QueryDoubleValue( &dVal ) != 0 )
					//{
					//	errs.AddErr(STLString(attrs[i].name)+" is not a float value.",EC_WARNING,dynamic_cast<OMEObject*>(this));
					//	break;
					//}
					dVal=pAttr->DoubleValue();
					*((double*)(attrs[ i ].value)) = dVal;
					break;
				case XML_POINT:		
					inBuff.str(STLString(pAttr->Value()));
					if(!inBuff>>((OMEPoint*)attrs[i].value)->x>>((OMEPoint*)attrs[i].value)->y)
						errs.AddErr(STLString(attrs[i].name)+" is a malformed point.",EC_WARNING,NULL);
					break;
				case XML_BOX:
					inBuff.str(STLString(pAttr->Value()));
					if(!inBuff>>((OMEBox*)attrs[i].value)->tl.x>>((OMEBox*)attrs[i].value)->tl.y>>((OMEBox*)attrs[i].value)->br.x>>((OMEBox*)attrs[i].value)->br.y)
						errs.AddErr(STLString(attrs[i].name)+" is a malformed box.",EC_WARNING,NULL);
					break;
				default:
					errs.AddErr(STLString(attrs[i].name)+" is of an unknown value type.",EC_WARNING,NULL);
					break;
				}

				//exit for loop
				break;
			}
		}


		pAttr=pAttr->Next();
	}

	StrList reqMissing;
	for(size_t i=0; i<aSize; i++)
	{
		if(!foundBits[i] && attrs[i].isRequired)
			reqMissing.push_back(attrs[i].name);
	}

	if(!reqMissing.empty())
	{
		errs.AddErr(STLString("The following required Attributes are missing: ")+ConcatStrings(reqMissing,','),
			EC_WARNING,NULL);
		
	}

	if(errs.GetErrCount())
	{
		ret=false;

		if(pBm)
		{
			oSTLSStream hdr;
			hdr<<"The following errors were found parsing XML";
			if(!element.empty())
				hdr<<" for element <"<<element<<'>';
			if(!filename.empty())
				hdr<<" in file '"<<filename<<'\'';
			hdr<<':';
			errs.AddErr(hdr.str(),EC_HEADER,NULL,true);

			pBm->ReportErrors(errs);
		}
	}

	return ret;
}

/** Retrieve a string representation of a tinyxml2 error.
	@param err The error to interpret.
	@return A string description of err.
*/
STLString XMLLoadable::ErrDescription(const tinyxml2::XMLError & err)
{
	using namespace tinyxml2;
	STLString ret;
	switch(err)
	{
	//case XML_NO_ERROR:  was causing error..
	case XML_SUCCESS:
		ret="No error";
		break;
	case XML_NO_ATTRIBUTE:
		ret="No attribute";
		break;
    case XML_WRONG_ATTRIBUTE_TYPE:
		ret="Wrong attribute type";
		break;
	case XML_ERROR_FILE_NOT_FOUND:
		ret="File not found";
		break;
	case XML_ERROR_FILE_COULD_NOT_BE_OPENED:
		ret="File could not be opened";
		break;
	case XML_ERROR_FILE_READ_ERROR:
		ret="File Read Error";
		break;
	case XML_ERROR_ELEMENT_MISMATCH:
		ret="Element mismatch";
		break;
	case XML_ERROR_PARSING_ELEMENT:
		ret="Error parsing element";
		break;
	case XML_ERROR_PARSING_ATTRIBUTE:
		ret="Error parsing attribute";
		break;
	case XML_ERROR_IDENTIFYING_TAG:
		ret="Error identifying tag";
		break;
	case XML_ERROR_PARSING_TEXT:
		ret="Error parsing text";
		break;
	case XML_ERROR_PARSING_CDATA:
		ret="Error parsing CDATA";
		break;
	case XML_ERROR_PARSING_COMMENT:
		ret="Error parsing comment";
		break;
	case XML_ERROR_PARSING_DECLARATION:
		ret="Error parsing declaration";
		break;
	case XML_ERROR_PARSING_UNKNOWN:
		ret="Error parsing unknown";
		break;
	case XML_ERROR_EMPTY_DOCUMENT:
		ret="Empty document";
		break;
	case XML_ERROR_MISMATCHED_ELEMENT:
		ret="Mismatched element";
		break;
	case XML_ERROR_PARSING:
		ret="Error parsing";
		break;
	case XML_CAN_NOT_CONVERT_TEXT:
		ret="Cannot convert text";
		break;
	case XML_NO_TEXT_NODE:
		ret="No text Node";
		break;
	};
	return ret;
}

//statics
/** Add A point-based Attribute to the StrArray specified in out.
	@param attr The name of the attribute to add.
	@param pt The OMEPoint to associate with the attribute.
	@param out The StrArray to store the new attribute entry.
*/
void XMLLoadable::AddAttributeValue(const OMEChar* attr, const OMEPoint & pt,StrArray & out)
{
	static oSTLSStream outStrm;
	outStrm.str(STLString());
	outStrm.clear();

	outStrm<<attr<<'='<<'\''<<pt.x<<','<<pt.y<<'\'';
	out.push_back(outStrm.str());
}

/** Add A box-based Attribute to the StrArray specified in out.
	@param attr The name of the attribute to add.
	@param bx The OMBox to associate with the attribute.
	@param out The StrArray to store the new attribute entry.
*/
void XMLLoadable::AddAttributeValue(const OMEChar* attr, const OMEBox & bx,StrArray & out)
{
	static oSTLSStream outStrm;
	outStrm.str(STLString());
	outStrm.clear();

	outStrm<<attr<<'='<<'\''<<bx.tl.x<<','<<bx.tl.y<<','<<bx.br.x<<','<<bx.br.y<<'\'';
	out.push_back(outStrm.str());
}

STLString XMLLoadable::GetXMLRepresentation(const unsigned int indent,const unsigned int inc)
{
	STLString baseIndent=STLString(indent,' ');
	StrArray attrs,subs;
	GetXMLAttributes(attrs);
	GetXMLSubNodes(subs,indent+inc,inc);

	oSTLSStream outStrm;
	outStrm<<baseIndent<<'<'<<GetXMLTag();
	if(!attrs.empty())
		outStrm<<' '<<ConcatStrings(attrs,' ');

	if(!subs.empty())
	{
		outStrm<<'>'<<endl;
		outStrm<<ConcatStrings(subs,'\n')<<endl;
		outStrm<<baseIndent<<"</"<<GetXMLTag()<<'>';
	}
	else
		outStrm<<"/>";
	return outStrm.str();
}