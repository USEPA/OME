#include "OMEDefines.h"
#include "omeobjects/ModelInstance.h"


/** Remove expression-illegal characters from a name string.
@param in The STLString to analyze.
@return A modified version of in with all illegal characters replaced with underscores.
*/
STLString ScrubName(const STLString & in)
{
	//list illegal characters here.
	static STLRegex matchReg("[\\-+*.<>=/!^%&$#^|:\"[:s:]\\n\\r\\v,]");

	STLString tmp, out = in;

	static STLString rep("_");
	while (true)
	{
		tmp = std::regex_replace(out, matchReg, rep);
		if (out != tmp)
			out = tmp;
		else
			break;
	}

	//Simile allows for labels to start with numbers. This is bad since most parsers hate this. prepend string to fix.
	const OMEChar* patterns[] = {
		"([^\\s=:<>+*/^\\(\\),-]|^)[\\\\\\s_]+(?=[^\\s=:<>+*/\\(\\)^-]|$)", "$1_",

		"(?:^|[.])([0-9].*$)", "_$1",
		"(\\s+)$", "",
		"^(\\s+)", "",

		"\0" };

	out = BatchRegexReplace(out, patterns);


	return out;
}

STLString DequoteName(const STLString & in)
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

	for (const char & c : in)
	{
		switch (c)
		{
		case '"':
			inQuote = !inQuote;
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
		case ',':
			if (inQuote)
			{
				outBuff << subs[c];
				break;
			}
		default:
			outBuff << c;
		}
	}

	return outBuff.str();
}

#pragma region ModelRecord Definitions

ModelRecord::ModelRecord()
{
}

/** Destructor. */
ModelRecord::~ModelRecord()
{
	for(size_t i=0; i<m_insts.size(); i++)
		delete m_insts[i];
}
#pragma endregion
#pragma region RecordRep Definitions
/** Clear all values in a record rep. */
void RecordRep::Clear()
{
	delete[] m_valLabels;
	m_valLabels=NULL;
	m_pSrcMdl=NULL;
	m_instCount = 0;
	m_values.clear();
}

/** Destructor. */
RecordRep::~RecordRep() 
{
	Clear();
};
#pragma endregion
#pragma region OMEPoint Definitions

/** Default constructor. */
OMEPoint::OMEPoint() : x(0),y(0) {}

/** Simple constructor. 
	@param a The x-ordinate.
	@param b The y-ordinate.
*/
OMEPoint::OMEPoint(const ORDTYPE & a, const ORDTYPE & b)
	:x(a),y(b) {}

/** Copy Constructor.
	@param pt The OMEPoint to copy.
*/
OMEPoint::OMEPoint(const OMEPoint & pt)
	:x(pt.x),y(pt.y) {}

/** Array Based constructor.
	Extracts two contigous ordinates in an array.
	@param pts Pointer to array of ordinate values.
	@param offset index of x-ordinate (y-ordinate is assumed to be offset+1).
*/
OMEPoint::OMEPoint(ORDTYPE* pts, const size_t & offset)
	:x(pts[offset]),y(pts[offset+1]) {}

/** Random access operator. 
	Index 0 refers to x ordinate; y Index 1 refers to y ordinate.
	@param i index to access.
	@return Reference to value of ordinate at index i.
	@throws OMEOOBException Thrown if index is outside of [0,1].
*/
ORDTYPE& OMEPoint::operator[](const size_t & i)
{
	if(i==0)
		return x;
	if(i==1)
		return y;
	throw OMEOOBException("OMEPoint only accepts index values of 0 or 1");
}

/** Constant random access operator. 
	Index 0 refers to x ordinate; y Index 1 refers to y ordinate.
	@param i index to access.
	@return Value of ordinate at index i.
	@throws OMEOOBException Thrown if index is outside of [0,1].
*/
ORDTYPE OMEPoint::operator[](const size_t & i) const
{
	if(i==0)
		return x;
	if(i==1)
		return y;
	throw OMEOOBException("OMEPoint only accepts index values of 0 or 1");
}

/** Assignment operator.
	@param rhs The object which contains values to copy.
	@return Reference to this.
*/
OMEPoint& OMEPoint::operator=(const OMEPoint & rhs)
{
	x=rhs.x;
	y=rhs.y;
	return *this;
}

/** Addition operator. 
	@param rhs The right-hand addition operand.
	@return OMEPoint that is equal to *this+rhs.
*/
OMEPoint OMEPoint::operator+(const OMEPoint & rhs) const
{
	return OMEPoint(x+rhs.x,y+rhs.y);
}

/** Addition assignment operator. 
	@param rhs The OMEPoint to add to this object.
	@return reference to this.
*/
OMEPoint& OMEPoint::operator+=(const OMEPoint & rhs)
{
	x+=rhs.x;
	y+=rhs.y;
	return *this;
}

/** Subtraction operator. 
	@param rhs The right-hand subtraction operand.
	@return OMEPoint that is equal to *this-rhs.
*/
OMEPoint OMEPoint::operator-(const OMEPoint & rhs) const
{
	return OMEPoint(x-rhs.x,y-rhs.y);
}

/** Subtraction assignment operator. 
	@param rhs The OMEPoint to subtract from this object.
	@return reference to this.
*/
OMEPoint& OMEPoint::operator-=(const OMEPoint & rhs)
{
	x-=rhs.x;
	y-=rhs.y;
	return *this;
}
#pragma endregion
#pragma region OMEBox Definitions

/** Default Constructor. */
OMEBox::OMEBox() : tl(OMEPoint()),br(OMEPoint()) {}

/** Simple Constructor.
	@param a The top-left corner of the box.
	@param b The bottom-right corner of the box.
	@throws OMEOOBException Thrown if top-left coordinates are greater than bottom-right coordinates, 
		which would invert internal box coordinates.
*/
OMEBox::OMEBox(const OMEPoint & a, const OMEPoint & b)
	:tl(a),br(b) 
{
	if(tl.x > br.x || tl.y> br.y)
		throw OMEOOBException("Top left coordinates cannot be greater than bottom-right coordinates.");
}

/** Copy constructor.
	@param bx The OMEBox to copy.
*/
OMEBox::OMEBox(const OMEBox & bx)
	:tl(bx.tl),br(bx.br) {}

/** Edge-based Constructor.
	@param l Ordinate for left edge.
	@param t Ordinate for top edge.
	@param r Ordinate for right edge.
	@param b Ordinate for bottom edge.
*/
OMEBox::OMEBox(const ORDTYPE & l,const ORDTYPE & t,const ORDTYPE & r,const ORDTYPE & b)
{
	tl=OMEPoint(l,t);
	br=OMEPoint(r,b);
}

/** Random access operator. 
	- Index 0 refers to left ordinate.
	- Index 1 refers to top ordinate.
	- Index 2 refers to right ordinate.
	- Index 3 refers to bottom ordinate.
	@param i index to access.
	@return Reference to value of ordinate at index i.
	@throws OMEOOBException Thrown if index is outside of [0,3].
*/
ORDTYPE& OMEBox::operator[](const size_t & i)
{
	switch(i)
	{
	case 0:
		return tl.x;
	case 1:
		return tl.y;
	case 2:
		return br.x;
	case 3: 
		return br.y;
	default:
		throw OMEOOBException("OMEBox only accepts index values from 0 to 3");
	}
}

/** Costant random access operator. 
	- Index 0 refers to left ordinate.
	- Index 1 refers to top ordinate.
	- Index 2 refers to right ordinate.
	- Index 3 refers to bottom ordinate.
	@param i index to access.
	@return Value of ordinate at index i.
	@throws OMEOOBException Thrown if index is outside of [0,3].
*/
ORDTYPE OMEBox::operator[](const size_t & i) const
{
	switch(i)
	{
	case 0:
		return tl.x;
	case 1:
		return tl.y;
	case 2:
		return br.x;
	case 3: 
		return br.y;
	default:
		throw OMEOOBException("OMEBox only accepts index values from 0 to 3");
	}
}

/** Assignment operator.
	@param rhs The object which contains values to copy.
	@return Reference to this.
*/
OMEBox& OMEBox::operator=(const OMEBox & rhs)
{
	tl=rhs.tl;
	br=rhs.br;
	return *this;
}

/** Addition operator. 
	@param rhs The right-hand addition operand.
	@return OMEBox that is equal to *this+rhs.
*/
OMEBox OMEBox::operator+(const OMEBox & rhs) const
{
	return OMEBox(tl+rhs.tl,br+rhs.br);
}

/** Addition assignment operator. 
	@param rhs The OMEBox to add to this object.
	@return reference to this.
*/
OMEBox& OMEBox::operator+=(const OMEBox & rhs)
{
	tl+=rhs.tl;
	br+=rhs.br;
	return *this;
}

/** Subtraction operator. 
	@param rhs The right-hand subtraction operand.
	@return OMEBox that is equal to *this-rhs.
*/
OMEBox OMEBox::operator-(const OMEBox & rhs) const
{
	return OMEBox(tl-rhs.tl,br-rhs.br);
}

/** Subtraction assignment operator. 
	@param rhs The OMEBox to subtract from this object.
	@return reference to this.
*/
OMEBox& OMEBox::operator-=(const OMEBox & rhs)
{
	tl-=rhs.tl;
	br-=rhs.br;
	return *this;
}

#pragma endregion
#pragma region OMEColor Definitions
///@cond DOX_NO_DOC
OMEColor::OMEColor() :r(0.0),g(0.0),b(0.0) {}
OMEColor::OMEColor(const OMEColor & c):r(c.r),g(c.g),b(c.b) {}
OMEColor& OMEColor::operator=(const OMEColor & c) 
{ 
	r=c.r;
	g=c.g;
	b=c.b;
	return *this;
}
///@endcond
#pragma endregion

/** Utility function for finding characters that donote the opening and closing of the same level block.
@param expr The expression to search.
@param start Index of the first character of the search. Seacch should start on character that matches oChar.
@param oChar The character that represents the beginning of a block.
@param cChar The character that represents the ending of a block.
@param reverse If true, search goes in reverse order, opening a block with cChar and closing a block with oChar.
@return index of matching closing character, or std::string::npos if no match is found (ie the open and close characters are not balanced).
*/
size_t FindMatching(const STLString & expr, const size_t & start, const OMEChar & oChar, const OMEChar & cChar, const bool & reverse)
{
	int inc = reverse ? -1 : 1;
	int limit = reverse ? -1 : (int)expr.size();
	size_t lvl = 0;
	size_t ret = STLString::npos;
	int i = (int)start;

	//skip opening char if it is the first entry in the start of the search
	//if (expr[i] == oChar)
	//	i+=inc;

	for (; i != limit && ret == STLString::npos; i += inc)
	{
		if (expr[i] == cChar)
		{
			lvl--;
			if (!lvl)
				ret = i;
		}
		else if (expr[i] == oChar)
			lvl++;
	}

	return ret;
}

/** Utility function for finding substrings that donote the opening and closing of the same level block.
@param expr The expression to search.
@param start Index of the first character of the search.
@param oStr The substring that represents the beginning of a block.
@param cStr The substring that represents the ending of a block.
@return index of the first character of the matching closing substring, or std::string::npos if no match is found (ie the open and close characters are not balanced).
*/
size_t FindMatching(const STLString & expr, const size_t & start, const STLString & oStr, const STLString & cStr)
{
	const STLRegex oReg("[^_]?\\b(" + oStr + ")\\b[^_]?");
	const STLRegex cReg("[^_]?\\b(" + cStr + ")\\b[^_]?");

	std::list<size_t> oList, cList;
	STLsmatch match;
	//find indexes for oStrs
	STLString sub = expr.substr(start);
	size_t offs = 0;
	while (std::regex_search(sub, match, oReg))
	{
		oList.push_back(match.position(1) + offs);
		//use outer match for offset, since that is what match.suffix() operates on.
		offs += match.position(0) + match.length(0);
		sub = match.suffix();
	}

	//find indexes for cStrs
	sub = expr.substr(start);
	offs = 0;
	while (std::regex_search(sub, match, cReg))
	{
		cList.push_back(match.position(1) + offs);
		offs += match.position(0) + match.length(0);
		sub = match.suffix();
	}

	//walk levels
	size_t ret = 0;
	size_t lvl = 1;
	while (lvl && (!oList.empty() || !cList.empty()))
	{
		if (cList.empty() || (!oList.empty() && oList.front() < cList.front()))
		{
			lvl++;
			oList.pop_front();
		}
		else
		{
			lvl--;
			if (!lvl)
				ret = cList.front();
			cList.pop_front();
		}
	}

	if (lvl && oList.empty() && cList.empty())
	{
		DBG_PRINTC("FindMatching: Could not find balanced '" + oStr + "' and '" + cStr + "' statements.", DBG_RED);
	}
	return ret;
}

size_t FindNth(const STLString & expr, const OMEChar & trg, const size_t & n)
{
	size_t startPos=0;
	size_t ret = STLString::npos;
	for (size_t i = 0; i < n; ++i, startPos = ret + 1)
	{
		ret = expr.find(trg, startPos);
		if (ret == STLString::npos)
			break;
	}

	return ret;
}

size_t RFindNth(const STLString & expr, const OMEChar & trg, const size_t & n)
{
	size_t endPos = STLString::npos;
	size_t ret = STLString::npos;
	for (size_t i = 0; i < n; ++i, endPos = ret - 1)
	{
		ret = expr.rfind(trg, endPos);
		if (ret == 0 && i < n - 1)
			ret = STLString::npos;
		if (ret == STLString::npos)
			break;
	}

	return ret;
}

void TrimRange(const STLString & str, size_t & start, size_t & len)
{
	//skip whitespace on left side
	while (std::isspace(str[start]) && len)
	{
		++start;
		--len;
	}

	//skip white space on right side.
	while (std::isspace(str[start + len]) && len)
		--len;

}

size_t CountChar(const STLString & str, const OMEChar & trg)
{
	size_t total = 0;
	for (const OMEChar& c : str)
	{
		if (c == trg)
			++total;
	}
	return total;
}
