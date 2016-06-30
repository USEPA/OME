#pragma once
#include "PLParserDefines.h"
#include "PLToken.h"

/** Tokenizer for Simile's source entry
*/
class PLTSource : public PLToken
{
public:

	/** Indices of specific arguments returned during querying */
	enum PLTS_INDEX {PLTS_ALL, PLTS_PROGRAM,PLTS_VERSION,PLTS_EDITION,PLTS_DATE}; 

	PLTSource();
	virtual int GetTokenType() {return NT_SOURCE;}
	virtual OMEObject* buildOMEObject(STLString args, PrologParser & parser, STLString & outLabel);
};