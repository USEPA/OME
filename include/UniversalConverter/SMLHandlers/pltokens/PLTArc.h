#pragma once
#include "PLParserDefines.h"
#include "PLToken.h"

/** Tokenizer for Simile's arc entry
*/
class PLTArc : public PLToken
{
public:
	/** Indices of specific arguments returned during querying */
	enum PLTA_INDEX {PLTA_ID=1,PLTA_SOURCE,PLTA_TARGET,PLTA_TYPE,PLTA_ATTR,PLTA_LOC};

	PLTArc();
	~PLTArc();

	virtual int GetTokenType() { return NT_ARC; };
	virtual OMEObject* buildOMEObject(STLString args, PrologParser & parser, STLString & outLabel);
};
