#pragma once
#include "PLParserDefines.h"
#include "PLToken.h"

/** Tokenizer for Simile's properties entry
*/
class PLTProperties : public PLToken
{
public:
	/** Indices of specific arguments returned during querying */
	enum PLTP_INDEX { PLTP_ALL=0, PLTP_COMPLETE, PLTP_FILENAME, PLTP_FILLCOLOR, PLTP_FIXMATH, PLTP_IMAGEPOS,
					  PLTP_MULTSPEC, PLTP_NAME, PLTP_SEPARATE, PLTP_STEP };

	PLTProperties();
	~PLTProperties();

	virtual int GetTokenType() { return NT_PROPERTIES; };
	virtual OMEObject* buildOMEObject(STLString args, PrologParser & parser, STLString & outLabel);
};
