#include "SMLHandlers/pltokens/PLToken.h"
#include <regex>

PLToken::PLToken() 
{}

/**
	Converts a simile expression to an OME expression.
	@param in String with the expression to convert.
	@param pUnused Not used.
	@return The converted expression.
*/
STLString PLToken::ConvertExpression(const STLString & in,OMEObject* pUnused)
{
	//Assumes ASCII; may want to update at some point to support UTF-8
	STLString ret=ConvertEscapes(ConvertCommaAnds(in));
	
	std::stack<std::pair<size_t,bool> > depthStack;
	std::stack<size_t> parenStack;
	std::list<size_t> ugoList;
	std::list<size_t> ugcList;
	std::list<size_t> cAndList;
	//std::list<size_t> gatolist;
	//std::list<size_t> gatclist;

	//find array references, replace with upgroup or GetAsTable()
	//unless its a makearray() statement.
	//if (in.find("makearray") == STLString::npos)
	//{
	//	for (size_t i = 0; i < in.size(); i++)
	//	{
	//		switch (ret[i])
	//		{
	//		case '[':
	//		case '{':
	//			depthStack.push(std::pair<size_t, bool>(i, false));
	//			//ensure single char
	//			ret[i] = '{';
	//			break;
	//		case ']':
	//		case '}':
	//			if (!depthStack.top().second)
	//			{
	//				ugoList.push_back(depthStack.top().first);
	//				ugcList.push_back(i);
	//			}
	//			//else
	//			//{
	//			//	gatoList.push_back(depthStack.top().first);
	//			//	gatcList.push_back(i);
	//			//}

	//			depthStack.pop();
	//			//ensure single char
	//			ret[i] = '}';
	//			break;
	//		case ',':
	//			//if commas are located between square brackets, then this would imply the construction of an inline table.
	//			if (!depthStack.empty() && (parenStack.empty() || depthStack.top().first > parenStack.top()))
	//				depthStack.top().second = true;
	//			break;
	//		case '(':
	//			parenStack.push(i);
	//			break;
	//		case ')':
	//			parenStack.pop();
	//			break;
	//		}
	//	}

	//	oSTLSStream buff;
	//	size_t ugCount = 0;
	//	for (size_t i = 0; i < ret.size(); i++)
	//	{
	//		if (ret[i] == '{')
	//		{
	//			if (std::find(ugoList.begin(), ugoList.end(), i) != ugoList.end())
	//			{
	//				buff << "upgroup(";
	//				ugCount = 0;
	//				//count the number of brackets until end;
	//				for (; ret[i] == '{' || std::isspace(ret[i]); i++)
	//					if (ret[i] == '{')
	//						ugCount++;

	//				//account for outer loop increment
	//				i -= 1;
	//			}
	//			else //if(gatoList.find(i)!=ugoList.end())
	//				buff << "getAsTable(";
	//		}
	//		else if (ret[i] == '}')
	//		{
	//			if (std::find(ugcList.begin(), ugcList.end(), i) != ugcList.end())
	//			{
	//				buff << "," << ugCount << ")";
	//				//skip additional brackets
	//				i += ugCount - 1;
	//				ugCount = 0;
	//			}
	//			else //if(gatcList.find(i)!=ugoList.end())
	//				buff << ')';
	//		}
	//		else
	//			buff << ret[i];
	//	}
	//	ret = buff.str();
	//}

	//regex approach
	const OMEChar* patterns[] = {
								//Deal with quotes (replace with space since they may be used as separators)
								//Keep double quotes, which can signify enums when found in equations
								"[']"," ",
								//replace && with and
								"&&"," and ",
								//replace ; with or
								";"," or ",
								//replace ints with doubles
								"((?:[^._a-zA-Z]|^)\\b[0-9]+)\\b(?![._a-zA-Z])", "$1.0",
								"[{]", "[",
								"[}]", "]","\0"};

								//commas in boolean statements also need to be replaced, but that will be handled by a separate func
								//since it is a bit more trickier than just a find/replace


	ret=BatchRegexReplace(ret,patterns);
//	DBG_PRINTC(ret, DBG_CYAN);
	//place ends on if statements (for easier processing by expr engines)
	static STLRegex ifReg("\\bif\\b");

	STLRegexItr rItr(ret.begin(),ret.end(),ifReg);
	static STLRegexItr rEnd; //points to end itr by default	
	std::list<unsigned int> posList;
	for (; rItr != rEnd; ++rItr)
		posList.push_back(rItr->position());


	if(!posList.empty())
	{
		//ifs found; append ends
		std::stack<unsigned int> ifLevel;
		unsigned int level=0;
		STLString dup((ret.size()+(posList.size()*4)),'\0');
		unsigned int i=0;
		for(unsigned int c=0; c<ret.size(); c++,i++)
		{
			if(!posList.empty() && c==posList.front())
			{
				ifLevel.push(level);
				posList.pop_front();
			}
			else
			{
				switch(ret[c])
				{
				case '[':
				case '{':
				case '(':
					level++;
					break;
				case ']':
				case '}':
				case ')':
					if(!ifLevel.empty() && level==ifLevel.top())
						InsertIfEnd(dup,i,&ifLevel);
					level--;
					break;
				case ',':
					if (!ifLevel.empty() && level == ifLevel.top())
						InsertIfEnd(dup,i,&ifLevel);
					break;
				};
			}
			dup[i]=ret[c];
		}

		while(ifLevel.size())
			InsertIfEnd(dup,i,&ifLevel);

		ret=dup;
	}
	return ret;
}

/** Convert discovered escape sequences into their actual values.
	@param in The String to evaluate.
	@return A copy of in with the relevant escapes properly converted.
*/
STLString PLToken::ConvertEscapes(const STLString & in)
{
	//for now, just worry about newline and tab
	const OMEChar* patterns[]={
						"\\\\n","\n",
						"\\\\r","\r",
						"\\\\t","\t",
						"\0"};

	return BatchRegexReplace(in,patterns);
}

/** Remove illegal characters from a name string.
	@param in The STLString to analyze.
	@return A modified version of in with all illegal characters replaced with underscores.
*/
STLString PLToken::ScrubNewlines(const STLString & in)
{
	//list illegal characters here.
	static STLRegex matchReg("[\\n\\r\\v]+");
	
	STLString tmp,out=in;
	static STLString rep(" ");
	while(true)
	{
		tmp=std::regex_replace(out,matchReg,rep);
		if(out!=tmp)
			out=tmp;
		else
			break;
	}
	return out;
}

/** Convert comma syntax to ands in boolean statements.
	@param in The string to modify.
	@return The modified string.
*/
STLString PLToken::ConvertCommaAnds(const STLString & in)
{
	//find start/stop of conditional statement.
	//   since entire statement may be conditional, walk entire statement looking for orphaned commas.
	//   orphaned comma = comma that is not in argument list or table list.
	//   process before table/grouping syntax is removed

	oSTLSStream buff;

	std::stack<bool> inValidList;
	inValidList.push(false); //top level is not list situation
	bool pushChar; 
	for (size_t i = 0; i < in.length(); i++)
	{
		pushChar = true;
		switch (in[i])
		{
		case ',':
			if (!inValidList.top())
			{
				pushChar = false;
				if (!std::isspace(in[i - 1]))
					buff << ' ';
				buff << "and";
				if (!std::isspace(in[i + 1]))
					buff << ' ';
			}
			break;
		case '(':
			if (i<=0 || !std::isalnum(in[i - 1]))
			{
				//if open paren is first char, or not directly preceded by an alphanum, then it is not a function arg list
				inValidList.push(false);
				break;
			}
		case '{':
		case '[':
			inValidList.push(true);
			break;
		case ')':
		case '}':
		case ']':
			inValidList.pop();
			break;
		}

		if (pushChar)
			buff << in[i];
	}

	return buff.str();
}
