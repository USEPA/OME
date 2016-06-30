#include "MDLHandlers/MDLEqnRecord.h"

MDLEqnRecord::MDLEqnRecord()
{
}

/** Detailed constructor.
	@param v The name of the associated variable.
	@param e The actual equation.
	@param u The associated units.
	@param c Any comments associated with the equation.
*/
MDLEqnRecord::MDLEqnRecord(const STLString & v, const STLString & e, const STLString & u, const STLString & c)
:m_comment(c), m_units(u), m_isTable(false), m_name(v), m_hasBeenApplied(false)
{
	//m_varID = FixIllegalChars(Dequote(ToLowerCase(v)));
	//m_eqn = FixIllegalChars(Dequote(ToLowerCase(e)));
	m_varID = FixIllegalChars(ToLowerCase(v));
	m_eqn = FixIllegalChars(ToLowerCase(e));
	m_eqn = DecimalizeNumbers(m_eqn); 
}

/** Detailed constructor for table entries.
@param v The name of the associated variable.
@param xV The X-values associated with the table.
@param yV The Y-values associated with the table.
@param u The associated units.
@param c Any comments assoicated with the equation.
*/
MDLEqnRecord::MDLEqnRecord(const STLString & v, const EvalTable::DataArray & xV, const EvalTable::DataArray & yV, const STLString & u, const STLString & c)
:MDLEqnRecord(v, "", u, c)
{
	m_isTable = true;
	m_xVals = xV;
	m_yVals = yV;
}

MDLEqnRecord::~MDLEqnRecord()
{
}

/** @return The ID of the associated varb able. */
STLString MDLEqnRecord::GetVariable() const
{
	return m_varID;
}

/** @return The stored expression. */
STLString MDLEqnRecord::GetEquation() const
{
	return m_eqn;
}

/** @return The equation's units. */
STLString MDLEqnRecord::GetUnits() const
{
	return m_units;
}

/** @return Any associated comments. */
STLString MDLEqnRecord::GetComment() const
{
	return m_comment;
}

/** @return The name of the associated variable.*/
STLString MDLEqnRecord::GetName() const
{
	return m_name;
}

/** Set if the equation has been applied to a variable model component.
	@param applied Whether or not the expression has been applied.
*/
void MDLEqnRecord::SetHasBeenApplied(const bool & applied)
{
	m_hasBeenApplied = applied;
}

/** @return true if the equation has been applied to a model object; false otherwise. */
bool MDLEqnRecord::GetHasBeenApplied() const
{
	return m_hasBeenApplied;
}

/** Generate a table from the stored table definition.
	@return A pointer to a newly allocated EvalXSetInterpTable, or NULL if table data is
	not stored in this object.
*/
EvalXSetInterpTable* MDLEqnRecord::GenTable()
{
	EvalXSetInterpTable* pTbl = NULL;
	if (m_isTable)
	{
		pTbl = new EvalXSetInterpTable(m_varID, m_xVals, m_yVals);
	}
	return pTbl;
}

/** Fix illegal chars and whitespace to all be the same legal expression character.
	@param inStr The string to fix.
	@return A fixed copy of the string.
*/
STLString MDLEqnRecord::FixWhitespace(const STLString & inStr)
{
	STLRegex wsReg("([^\\s=:<>+*/^\\(\\),-]|^)[\\\\\\s_]+(?=[^\\s=:<>+*/\\(\\)^-]|$)");

	return std::regex_replace(inStr, wsReg, STLString("$1_"));
}

/** Fix specific Illegal chars.
	@param inStr The string to fix.
	@return A fixed copy of the string.
*/
STLString MDLEqnRecord::FixIllegalChars(const STLString & inStr)
{

	const OMEChar* patterns[] = { 
        "([^\\s=:<>+*/^\\(\\),-])[\\\\\\s_]+(?=[^\\s=:<>+,*/\\(\\)^-])", "$1_",
        
        
        "[\"]", "_",
		"\\b([0-9]+_)", "_$1",
		"(\\s+)$", "",
		"^(\\s+)", "",
		"\0" };

	//only apply to regions that are not quoted
	/*bool inQuote = false;
	oSTLSStream outBuff;
	size_t lastPos = 0;
	for (size_t i = 0; i < inStr.size(); ++i)
	{
		if (inStr[i] == '"')
		{
			if (inQuote)
			{
				outBuff << inStr.substr(lastPos, i - lastPos + 1);
				lastPos = i + 1;
			}
			else
			{
				outBuff << BatchRegexReplace(inStr.substr(lastPos, i - lastPos), patterns);
				lastPos = i;
			}
			inQuote = !inQuote;
		}
	}
	outBuff << BatchRegexReplace(inStr.substr(lastPos), patterns);
	return outBuff.str();*/

	bool inQuote = false;
	STLString outStr = inStr;
	size_t lastPos = 0;
	for (size_t i = 0; i < inStr.size(); ++i)
	{
		if (inStr[i] == '"')
			inQuote = !inQuote;
		else if (inQuote)
		{
			switch (inStr[i])
			{
			case '%':
			case '+':
			case '-':
			case '*':
			case '/':
			case '$':
			case '#':
			case '!':
			case '^':
			case '|':
			case '&':
			case '.':
			case ':':
			case ',':

			//...
				outStr[i] = '_';
			}
		}


	}

	return BatchRegexReplace(outStr, patterns);
}

/** Dequote a string, replacing illegal chars with suitable replacements.
@param inStr The string to fix.
@return A fixed copy of the string.
*/
STLString MDLEqnRecord::Dequote(const STLString & inStr)
{
	std::map<char, const char*> subs{
		{ '%', "_pct_" },
		{ '+', "_pls_" },
		{ '-', "_min_" },
		{ '*', "_mul_" },
		{ '/', "_div_" },
		{ '$', "_dol_" },
		{ '#', "_pnd_" },
		{ '!', "_exc_" },
		{ '^', "_hat_" },
		{ '&', "_amp_" },
		{ '|', "_pip_" },
		{ ':', "_cln_" },
		{ '.', "_dot_" }
	};

	bool inQuote = false;
	oSTLSStream outBuff;

	for (const char & c : inStr)
	{
		switch (c)
		{
		case '"':
			inQuote = !inQuote;
			outBuff << '_';
			break;
		case '%':
		case '+':
		case '-':
		case '*':
		case '/':
		case '$':
		case '#':
		case '!':
		case '^':
		case '&':
		case '|':
		case ':':
		case '.':
			if (inQuote)
			{
				outBuff << '_';//subs[c];
				break;
			}
		default:
			outBuff << c;
		}
	}

	return outBuff.str();
}

/** Convert number string from scientific notation to decimal notation.
	@param inStr The string to parse for numeric values.
	@return A string with all numeric values represented as decimals.
*/
STLString MDLEqnRecord::DecimalizeNumbers(STLString inStr)
{
	//numReg
	STLRegex numReg("(?:[0-9]+(?:[.]?[0-9]*)|[0-9]*[.][0-9]+)(?:[Ee][+-]?[0-9]+)?");
	STLsmatch mtch;
	oSTLSStream outBuff;

	while (std::regex_search(inStr,mtch,numReg))
	{
		//add everything leading up to the number
		outBuff << mtch.prefix();

		//add number as captured.
		outBuff << mtch[0].str();
		size_t pos = mtch.position(0);
		size_t nextPos = pos+mtch.length(0);
		//add suffix only if we need it.
		if (mtch[0].str().find('.') == STLString::npos && mtch[0].str().find('e') == STLString::npos && mtch[0].str().find('E') == STLString::npos
			&& (pos == 0 || (!std::isalpha(inStr[pos - 1]) && inStr[pos - 1] != '_'))
			&& (nextPos >= inStr.length() || (!std::isalpha(inStr[nextPos]) && inStr[nextPos] != '_')))
			outBuff << ".0";

		inStr = mtch.suffix();
	}

	//add remainder
	outBuff << inStr;
	//OMEChar lastChar = '\0';
	//oSTLSStream outBuff;
	//for (size_t i = 0; i<inStr.size(); ++i)
	//{
	//	inStr[i];
	//	if (std::isdigit(inStr[i]) && !std::isalpha(lastChar) && lastChar != '_' && !std::isdigit(lastChar) && ((lastChar != '+' && lastChar != '-') || i <= 1 || inStr[i] != 'e'))
	//	{
	//		//run through digits until end is hit
	//		for (; i < inStr.size() && std::isdigit(inStr[i]); ++i)
	//			outBuff << inStr[i];
	//		if (i >= inStr.size() || (inStr[i] != '.' && !std::isalpha(inStr[i]) && inStr[i] != '_'))
	//			outBuff << ".0";
	//		else if (i < inStr.size() && inStr[i] == '.')
	//		{
	//			outBuff << inStr[i];
	//			//finish out number!
	//			for (++i; i < inStr.size() && std::isdigit(inStr[i]); ++i)
	//				outBuff << inStr[i];
	//		}
	//	}
	//	
	//	if (i < inStr.size())
	//	{
	//		outBuff << inStr[i];
	//		lastChar = inStr[i];
	//	}
	//}

	return outBuff.str();
}
