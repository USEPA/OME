#pragma once
#include "PLParserDefines.h"
#include <stack>

enum PL_TOKEN_TYPE {NT_BASE=0, NT_NODE, NT_SOURCE, NT_ROOTS, NT_PROPERTIES, NT_LINKS, NT_ARC, NT_COUNT };

/** Base class for Prolog tokenizers.
	Each subclass is intended to handle a specific line identifier in a simile-generated prolog file.
*/
class PLToken
{
public:
	PLToken();
	virtual ~PLToken() {};
    
	/** Unique identifier for PLToken child classes
		@return unique identifier.
	*/
	virtual int GetTokenType() { return NT_BASE; };

	/** Constructs an OMEObject from information passed in.
		@param args The string of arguments defining the entry.
		@param parser The calling parser.
		@param outLabel String for capturing returned label/ID.
		@return A pointer to a newly instantiated OMEObject
	*/
	virtual OMEObject* buildOMEObject(STLString args, PrologParser & parser, STLString & outLabel)=0;

	static STLString ConvertExpression(const STLString & in,OMEObject* pUnused);
	static STLString ConvertEscapes(const STLString & in);
	static STLString ScrubNewlines(const STLString & in);
	static STLString ConvertCommaAnds(const STLString & in); 

private:
	/** Insert string representing the end of an if statement.
		@param str The string to modify with end statement.
		@param ind The initial index of end statement of insertion point. On return, ind will hold the index of the position
			of the first character after the end of the end statement.
		@param stk Optional stack of indices to pop.
	*/
	inline static void InsertIfEnd(STLString & str, unsigned int & ind,std::stack<unsigned int>* stk=NULL)
	{
		str[ind++]=' ';
		str[ind++]='e';
		str[ind++]='n';
		str[ind++]='d';
		if(stk)
			stk->pop();
	}

};

