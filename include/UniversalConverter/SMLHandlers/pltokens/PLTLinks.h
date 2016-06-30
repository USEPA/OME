#pragma once
#include "PLParserDefines.h"
#include "PLToken.h"

/** Tokenizer for Simile's links entry
*/
class PLTLinks : public PLToken
{
public:
	PLTLinks();
	~PLTLinks();

	virtual int GetTokenType() { return NT_LINKS; };
	virtual OMEObject* buildOMEObject(STLString args, PrologParser & parser, STLString & outLabel);
};
