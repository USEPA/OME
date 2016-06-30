#pragma once
#include "PLParserDefines.h"
#include "PLToken.h"

//typedefs for function map
typedef OMEObject* (*buildFunction)(const STLString &, const STLStringList &, const StrArray &,PrologParser & prsr);
typedef std::map<STLString, buildFunction> buildFuncMap;
typedef std::map<STLString, OMETYPE> typeMap;

/** Tokenizer for Simile's node entry
*/
class PLTNode : public PLToken
{
public:

	///match indices for first regex pass
	enum PLTN_INDEX {PLTN_ALL=0, PLTN_ID,PLTN_TYPE,PLTN_SUB,PLTN_ATTR,PLTN_LOC};

	///match indices for attributes regex pass
	enum PLTN_ATTR {ATTR_ALL=0,		ATTR_COMMENT,	ATTR_COMPLETE,	ATTR_DESC,	ATTR_ENUM,	ATTR_EXTCODE,
					ATTR_FILLCOLOR, ATTR_FIXMATH,	ATTR_IMPOS,		ATTR_MAX,	ATTR_MIN,	ATTR_MULTSPEC,	ATTR_NAME,
					ATTR_PARAM,		ATTR_SEPARATE,	ATTR_QSPEC,		ATTR_SPEC,	ATTR_TABLE,	ATTR_UNITS,		ATTR_VALUE, 

					ATTR_COUNT};

	///match drawing details-may be used later.
	enum PLTN_GRAPH {GRAPH_ALL=0, GRAPH_BB,GRAPH_CAPT,GRAPH_CENTER,GRAPH_INTEXT};

	PLTNode();
	~PLTNode();

	virtual int GetTokenType() { return NT_NODE; };
	virtual OMEObject* buildOMEObject(STLString args, PrologParser & parser, STLString & outLabel);


private:

	buildFuncMap funcMap; ///< function map / jump table
	typeMap tMap;			///< used for type lookup
	
	//function targets for funcMap
	static OMEObject* buildAlarmObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr);
	static OMEObject* buildCloudObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr);
	static OMEObject* buildCompartmentObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr);
	static OMEObject* buildConditionObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr);
	static OMEObject* buildFunctionObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr);
	static OMEObject* buildVariableObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr);
	static OMEObject* buildSubmodelObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr);
	static OMEObject* buildCreationObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr);
	static OMEObject* buildImmigrationObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr);
	static OMEObject* buildReproductionObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr);
	static OMEObject* buildLossObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr);
	static OMEObject* buildBorderObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr);
	static OMEObject* buildTextObject(const STLString & name, const STLStringList & subs, const StrArray & attrs,PrologParser & prsr);

	//utility for building NullFunc's tableData struct
	static NullFunc::TableData buildTableDataEntry(STLString args);

};


