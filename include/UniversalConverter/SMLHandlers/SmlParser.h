#pragma once
#include "PrologParser.h"
#include "MIMEParser.h"

/** Parser for Simile's .sml files.
	Currently only parses out the model's prolog information.
*/
class SmlParser :public PrologParser, public MIMEParser
{
public:
	SmlParser(void) : PrologParser(), MIMEParser() {};
	/** Copy Constructor
		@param smlp the parser to copy
	*/
	SmlParser(const SmlParser & smlp) { Duplicate(smlp); };
	/** Assignment Operator.
		@param smlp The SmlParser to copy.
		@return Reference to the SmlParser.
	*/
	SmlParser& operator=(const SmlParser & smlp) { Duplicate(smlp); return *this; };

	~SmlParser(void) {};

	virtual Model* ParseModelFromFile(const STLString & path);

protected:


	/** Duplicate Prolog parser fields
	@param smlp the parser to copy
*/
	void Duplicate(const SmlParser & smlp) { PrologParser::Duplicate(smlp); MIMEParser::Duplicate(smlp); };

	/** take runtime info from a specific MIME section and use it to flesh out the model
		@param mdl the model to modify
	*/
	void PopulateRuntimeForModel(Model* mdl);

};

