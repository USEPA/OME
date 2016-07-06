#pragma once
#if defined WIN32 || defined _WIN64
#include "..\include\Windows\stdafx.h"
//#include<vld.h>
//disable sign mismatch warnings;
#pragma warning(push)
#pragma warning(disable : 4018 4800)

//windows stupidly defines generic all lower case min and max functions as preprocessor macros, which breaks
//all other min/max usages, even when scoped (outside of ::max() and ::min()) when referenced in header files.
// undef is needed to prevent conflict.
#undef min
#undef max

#include <direct.h>

#define MAXPATHLEN MAX_PATH

#else
//include other platform headers here
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "CharType.h"
#include "ValTypes.h"
#include "DebugOutput.h"

#include <list>
#include <string>
#include <iosfwd>    //for char_traits
#include <deque>
#include <vector>
#include <map>
#include <regex>
#include <iostream>
#include <sstream>
#include <set>
#include <valarray>
#include <functional>

#ifndef OME_MDL_BUILD
#include "tinyxml2.h"

//typedefs for simplification
//tinyxml2 typedefs
typedef tinyxml2::XMLElement		TI2Element;
typedef tinyxml2::XMLAttribute	TI2Attribute;
typedef tinyxml2::XMLDocument		TI2Document;
typedef tinyxml2::XMLError      TI2Error;

#else
//disable tinyxml attributes, since headers shouldn't need them
typedef void*		TI2Element;
typedef void*	TI2Attribute;
typedef void*		TI2Document;
typedef void*      TI2Error;

#endif

#define OME_WS_PATTERN "([^\\s=:<>+*/^\\(\\),-]|^)[\\\\\\s_]+(?=[^\\s=:<>+*/\\(\\)^-])"

const size_t OME_NOIND = std::numeric_limits<size_t>::max();
const OME_SCALAR OME_NOLIM = std::numeric_limits<OME_SCALAR>::infinity();
const unsigned int OME_NO_EXEC_GRP = 0;

typedef std::basic_istringstream<OMEChar, std::char_traits<OMEChar>,std::allocator<OMEChar> > iSTLSStream;
typedef std::basic_ostringstream<OMEChar, std::char_traits<OMEChar>,std::allocator<OMEChar> > oSTLSStream;
typedef std::basic_regex<OMEChar> STLRegex;
typedef std::regex_iterator<STLString::iterator> STLRegexItr;
typedef std::match_results<STLString::const_iterator> STLsmatch;

typedef std::valarray<OME_SCALAR> SclrArgs;
typedef std::vector<OME_SCALAR> SclrResults;
typedef SclrArgs SclrVArray;
typedef std::valarray<int> IntVArray;

typedef std::vector<bool> BreakSet;

typedef unsigned char DT_TYPE;

class LuaOMEParser;
typedef LuaOMEParser EVAL_PARSER;

//forward declarations
class OMEObject;
class Evaluable;
class Model;
class StateVar;
class Variable;
class SubmodelPort;
class IterConditional;
class BaseManager;
class Link;

class Param;
class Flow;
class Influence;
class Spawner;
class SubmodelAssoc;
class ModelInstance;
class EvalAlias;
struct EvalRep;

using namespace std;
//useful typedefs
typedef ARRAY_TYPE<Evaluable*> EvalArray;
typedef void (*ModelReportFunction)(const OME_SCALAR &, Model* );
typedef void (*ManagerReportFunction)(const OME_SCALAR &, BaseManager*,void* );
typedef void(*ManagerInitVarParamFunction)(EvalArray & evals, void*);


typedef std::function<OME_SCALAR(const OME_SCALAR & ,const OME_SCALAR &)> OpFunc;
typedef std::vector<unsigned int> IndVec; ///< Type that contains a list of indices.

typedef int ORDTYPE;

//lists are linked-lists; best for lots of resizing, random access is not needed;
typedef std::list<string> StrList;

//sets are containers which only permit unique values. Best for quick lookup/cataloguing.
typedef std::set<string> StrSet;

/** Simple 2D point structure. */
struct __EXPORT__ OMEPoint
{
	ORDTYPE x;		///< X-ordinate.
	ORDTYPE y;		///< Y-ordinate.

	OMEPoint();
	OMEPoint(const ORDTYPE & a, const ORDTYPE & b);
	OMEPoint(const OMEPoint & pt);
	OMEPoint(ORDTYPE* pts, const size_t & offset=0);

	ORDTYPE& operator[](const size_t & i);
	ORDTYPE  operator[](const size_t & i) const;
	OMEPoint& operator=(const OMEPoint & rhs);
	OMEPoint  operator+(const OMEPoint & rhs) const;
	OMEPoint& operator+=(const OMEPoint & rhs);
	OMEPoint  operator-(const OMEPoint & rhs) const;
	OMEPoint& operator-=(const OMEPoint & rhs);
};

/** Simple corner-based Box structure.*/
struct __EXPORT__ OMEBox
{
	OMEPoint tl;	///< Top-left corner of box.
	OMEPoint br;	///< Bottom-right corner of box.
	
	OMEBox();
	OMEBox(const OMEPoint & a, const OMEPoint & b);
	OMEBox(const OMEBox & bx);
	OMEBox(const ORDTYPE & l,const ORDTYPE & t,const ORDTYPE & r,const ORDTYPE & b);

	ORDTYPE& operator[](const size_t & i);
	ORDTYPE  operator[](const size_t & i) const;
	OMEBox& operator=(const OMEBox & rhs);
	OMEBox  operator+(const OMEBox & rhs) const;
	OMEBox& operator+=(const OMEBox & rhs);
	OMEBox  operator-(const OMEBox & rhs) const;
	OMEBox& operator-=(const OMEBox & rhs);
};

/** Simple RGB color structure.*/
/// @cond DOX_NO_DOC
struct __EXPORT__ OMEColor
{
	union
	{
		struct
		{
			float r,g,b;
		};
		float rgb[3];
	};

	OMEColor();
	OMEColor(const OMEColor & c);
	OMEColor& operator=(const OMEColor & c);
};
///@endcond

/** Identifiers for xml fields*/
enum XML_TYPE {XML_NULL,XML_BOOL,XML_CHAR,XML_STR,XML_INT,XML_FLOAT,XML_DOUBLE,XML_POINT,XML_BOX };

/** Structure used for collecting data from XML tags. Derived from Envision source code. http://envision.bioe.orst.edu/ */
struct XmlAttr
{
   const OMEChar* name;	///< Name to search for.
   XML_TYPE type;		///< Type of value.
   void   *value;		///< Address of variable which will hold parsed value.
   bool    isRequired;	///< If true, an error will be thrown if the tag designated by name is not found.
 };

STLString __EXPORT__ ScrubName(const STLString & in);
STLString __EXPORT__ DequoteName(const STLString & in);


/** struct for recording model values at a specific time. */
struct  __EXPORT__  ModelRecord
{
	
	//SclrVArray m_portVals;						///< Values of submodelPorts.
	ARRAY_TYPE<ModelInstance*> m_insts;		///< Values from all extant ModelInstances.
	STLString m_fileName;					///< Name of cache file storing this record. 


	ModelRecord();
	~ModelRecord();
};

typedef std::map<OME_SCALAR,ModelRecord> RecordMap;


/** Simple index-label pair.*/
typedef pair<unsigned short, STLString> IndPair;

/** Comparator for IndPair.*/
struct IndPairComp
{
	/**	@param lhs The first IndPair in the comparison.
		@param rhs The second IndPair in the comparison.
		@return true if the index of lhs is less than the index of rhs
	*/
	bool operator()(const IndPair & lhs, const IndPair & rhs){ return lhs.first<rhs.first; }
};

/** Index node for DataSourceValue. */
struct IndexNode
{
	size_t m_ind;							///< Index of node.
	std::map<size_t,IndexNode> m_subNodes;	///< Map of subnodes entered by index.
	const OME_SCALAR* m_pVal;				///< Pointer to represented value.

	IndexNode() :m_ind(0), m_pVal(NULL){}
	/** Detailed Constructor.
		@param i index of new node.
		@param ptr Pointer to represented value.
	*/
	IndexNode(const size_t & i, OME_SCALAR* ptr=NULL) :m_ind(i), m_pVal(ptr){}

	/** Copy constructor.
		@param node The IndexNode to copy.
	*/
	IndexNode(const IndexNode & node)
	{
		m_ind = node.m_ind;
		m_pVal = node.m_pVal;
		m_subNodes = node.m_subNodes;
	}
	//for sorting
	//bool operator<(const IndexNode & in) const { return m_ind < in.m_ind; }
};

/** Information that can be associated with a data source cell*/
struct DataSourceValue
{
	STLString m_label;			///< Label of variable.
	size_t m_interp;			///< Interpolation value used with time series.
	STLString m_indCol;			///< Name of column that contains time indices.
	OME_SCALAR m_interval;		///< Repeat interval for time series.

	std::set < IndPair,IndPairComp > m_inds;	///< List of Indices used for lookup.
	IndexNode m_indexTreeRoot;		///< Root node for index tree (populated as needed).

	/** Default Constructor. */
	DataSourceValue() :m_interp(0), m_interval(0.0),m_indexTreeRoot(0) {}
};

typedef std::multimap<STLString,DataSourceValue> ColMMap;
typedef pair<ColMMap::iterator,ColMMap::iterator> ColMMapRange;

/** Entry for a file that provides initialization values */
struct DataSourceEntry
{
	STLString m_path;										///< Path to file.
	StrList m_varNames;		///< List of variable names present in this DataSource
	ColMMap m_colMappings;	///< Mappings from file columns to model variables.
	size_t m_stepSize;		///< Nested values are loaded from outside-in; m_stepSize determines how far apart individual records are.
};

typedef std::map<STLString,DataSourceEntry> DSEntryMap;

typedef map<std::string, OMEObject*> ObjMap;
typedef list<OMEObject*> ObjList;

//ARRAY_TYPE should either be vector or deque
typedef ARRAY_TYPE<string> StrArray;
typedef ARRAY_TYPE<OMEObject*> OMEArray;
typedef ARRAY_TYPE<Link*> LinkArray;

typedef std::list<Evaluable*> EvalList;
typedef ARRAY_TYPE<Param> ParamArray;
typedef ARRAY_TYPE<Variable*> VariableArray;
typedef ARRAY_TYPE<Flow*> FlowArray;
typedef ARRAY_TYPE<Influence*> InflArray;
typedef ARRAY_TYPE< IterConditional* > ICArray;
typedef ARRAY_TYPE<SubmodelPort*> PortArray;
typedef ARRAY_TYPE<StateVar*> SVArray;
typedef vector<XmlAttr> XMLAttrVector;
typedef ARRAY_TYPE<EvalRep> EvalRepArray;
typedef ARRAY_TYPE<Spawner*> SpawnerArray;
typedef ARRAY_TYPE<SubmodelAssoc*> AssocArray;
typedef ARRAY_TYPE<EvalAlias*> AliasArray;

typedef ARRAY_TYPE<Model*> ModelArray;
typedef std::list<Model*> ModelList;
typedef std::vector<ModelInstance*> MdlInstanceArray;
typedef std::list<ModelInstance*> MdlInstanceList;

typedef ARRAY_TYPE<OME_SCALAR> OMESclrArray;
typedef ARRAY_TYPE<OME_SCALAR*> SclrPtrArray;
typedef ARRAY_TYPE<size_t> OMESizeArray;
typedef std::valarray<size_t> IndexArray;


typedef std::set<Evaluable*,bool(*)(const OMEObject*,const OMEObject*)> EvalSet; ///< Used in situations where unique values need to be enforced, and no random access


/** Structure for handing Timestep records back and forth.

*/
struct __EXPORT__ RecordRep
{
	/// \todo finish updating for new instance and record keeping scheme.

	STLString m_modelName;		///< Name of originating model.
	size_t m_instCount;		///< number of instances represented.
	STLString* m_valLabels;				///< labels for returned values.

	OMESclrArray m_values;		///< current values.

	Model* m_pSrcMdl;		///< Pointer to originating model.

	/** Default Constructor. */
	RecordRep() :m_pSrcMdl(NULL), m_valLabels(NULL) {}

	void Clear();
	~RecordRep();

};
typedef ARRAY_TYPE<RecordRep> RecordRepArray;

//functions that are useful globally. May want to refactor into a class (or classes) later


//Utility defines
/**	Determines if pObj exists in objList
	@param pObj The object to search for.
	@param objList The list to search in.
	@return The index of the object present, -1 otherwise.
*/
template<class T,class C>
int ObjectPresent(const T pObj, const C & objList)
{
	int x=0;
	//iterate through the list, only breaking if we hit the end or the match is found.
	for (typename C::const_iterator itr = objList.begin(); itr != objList.end() && pObj != (*itr); ++itr, x++);

	//if the end of the list is reached, then pObj was not found
	if(x==objList.size())
		x=-1;
	return x;
}

template<class T,class C>
void AppendIfUnique(C & dest, const T & obj)
{
		if(ObjectPresent<T,C>(obj,dest)<0)
			dest.push_back(obj);
}


template<class T,class C>
void AppendUnique(C & dest, const C & src)
{
	C unique;
	for(const T & obj : src)
	{
		if(ObjectPresent<T,C>(obj,dest)<0)
			unique.push_back(obj);
	}
	dest.insert(dest.end(),unique.begin(),unique.end());
}

template<class T>
void InPlaceSort(ARRAY_TYPE<T> & arr)
{
	std::sort(arr.begin(),arr.end());
}

template<class T>
ARRAY_TYPE<T> Intersect(ARRAY_TYPE<T> & a1, ARRAY_TYPE<T> & a2)
{
	ARRAY_TYPE<T> out;

	for(auto itr1=a1.begin(); itr1!=a1.end(); itr1++)
	{
		if(ObjectPresent(*itr1,a2)>=0)
		{
			out.push_back(*itr1);
			//continue;
		}
	}
	return out;
}

template<class T>
ARRAY_TYPE<T> Difference(ARRAY_TYPE<T> & a1, ARRAY_TYPE<T> & a2)
{
	ARRAY_TYPE<T> out;

	for(auto itr1=a1.begin(); itr1!=a1.end(); itr1++)
	{
		if(ObjectPresent(*itr1,a2)<0)
		{
			out.push_back(*itr1);
			//continue;
		}
	}
	//sloppy way to do this, but will work
	for(auto itr2=a2.begin(); itr2!=a2.end(); itr2++)
	{
		if(ObjectPresent(*itr2,a1)<0)
		{
			out.push_back(*itr2);
			//continue;
		}
	}
	return out;
}

template<class T>
void SubtractObjects(ARRAY_TYPE<T> & a1, ARRAY_TYPE<T> & a2)
{
	ARRAY_TYPE<T> out;

	for(auto itr1=a1.begin(); itr1!=a1.end(); itr1++)
	{
		if(ObjectPresent(*itr1,a2)<0)
		{
			out.push_back(*itr1);
			//continue;
		}
	}

	a1=out;
}


/** Derive 1-dimensional index from n-dimensional set. Assumes CheckBounds() would return true. 
	@param inds Pointer to an array of valid indices.
	@indCount The number of indices in inds.
	@return the equivalent 1-dimensional index used to directly access m_vals.
*/
static inline unsigned int DeriveIndex(const unsigned int* inds, const unsigned int indCount,const IndVec & dims)
{
	//assume CheckBounds()==true

	/// \todo check with 3 dimensions
	unsigned int ret=inds[0];
	unsigned int sub;
	for(unsigned int i=1; i<dims.size() && i<indCount;i++)
	{
		sub=1;
		for(unsigned int j=0;j<i;j++)
			sub*=dims[j];
		
		ret+=inds[i]*sub;
	}

	return ret;
}

static inline unsigned int DeriveIndex(const IndVec & inds, const IndVec & dims)
{
	//assume CheckBounds()==true

	/// \todo check with 3 dimensions
	unsigned int ret=inds[0];
	unsigned int sub;
	for(unsigned int i=1; i<dims.size();i++)
	{
		sub=1;
		for(unsigned int j=0;j<i;j++)
			sub*=dims[j];
		
		ret+=inds[i]*sub;
	}

	return ret;
}

static STLString GetExtension(const STLString & filepath)
{
	STLString out="";
	size_t loc=filepath.find_last_of('.');
	if(string::npos!=loc)
		out=filepath.substr(loc+1);

	return out;
}

/** Apply a series of short regex expressions. This approach is generally faster for large
	Regular expressions since it reduces backtracks.

	@param str The String to evaluate;
	@param patterns A simple array of C-strings used for regular expressions. the list must be terminated with "\0".
	@param sub the grouping index to use for searching. Defaults to 1. 
	@param useSearch If true, call regex_search instead of regex_match.

	@return A StringArray that mimics the structure of a regular expression match object.
			The first index is the queried string; each subsequent string are matches of the desired field.
*/
static StrArray BatchRegex(const STLString & str, const OMEChar** patterns,const int sub=1, const bool useSearch=false)
{
	ARRAY_TYPE<STLRegex> regexes;
	
	int i=0;

	//parse until null char hit
	while (patterns[i][0]!='\0')
		regexes.push_back(STLRegex(patterns[i++]));

	//allocate regexes, set first string
	StrArray regMatches(regexes.size()+1);
	regMatches[0]=str;
	
	//run all regexes.
	size_t k;
	try
	{
		for (k = 1; k <= regexes.size(); k++)
		{
			STLsmatch subMatch;
			useSearch ? std::regex_search(str, subMatch, regexes[k - 1]) : std::regex_match(str, subMatch, regexes[k - 1]);
			regMatches[k] = subMatch[sub];

		}
	}
	catch (regex_error & err)
	{
		DBG_PRINTC("Batch Regex error (Iteraton " + std::to_string(k) + "): " + err.what(), DBG_RED);
		throw err;
	}
	return regMatches;
}

static StrArray BatchRegex(const STLString & str, const StrArray & patterns, const int sub = 1, const bool useSearch = false)
{
	ARRAY_TYPE<STLRegex> regexes;

	//parse until null char hit
	for (auto itr = patterns.begin(); itr != patterns.end(); ++itr)
		regexes.push_back(STLRegex(*itr));

	//allocate regexes, set first string
	StrArray regMatches(regexes.size() + 1);
	regMatches[0] = str;

	//run all regexes.
	size_t k;
	try
	{
		for (k = 1; k <= regexes.size(); k++)
		{
			STLsmatch subMatch;
			useSearch ? std::regex_search(str, subMatch, regexes[k - 1]) : std::regex_match(str, subMatch, regexes[k - 1]);
			regMatches[k] = subMatch[sub];

		}
	}
	catch (regex_error & err)
	{
		DBG_PRINTC("Batch Regex error (Iteraton " + std::to_string(k) + "): " + err.what(), DBG_RED);
		throw err;
	}
	return regMatches;
}


static bool BatchRegexMatchAny(const STLString & str, const OMEChar** patterns,const int sub=1, bool useSearch=false)
{
	bool ret=false;
	StrArray results=BatchRegex(str,patterns,sub,useSearch);
	if(results.size()==1)
		ret=true;
	else if(results.size()>1)
		ret=std::any_of(results.begin()+1,results.end(),[](STLString & res){return !res.empty();});
	return ret;
}

static bool BatchRegexMatchAny(const STLString & str, const StrArray & patterns, const int sub = 1, bool useSearch = false)
{
	bool ret = false;
	StrArray results = BatchRegex(str, patterns, sub, useSearch);
	if (results.size() == 1)
		ret = true;
	else if (results.size()>1)
		ret = std::any_of(results.begin() + 1, results.end(), [](STLString & res){return !res.empty(); });
	return ret;
}

static bool BatchRegexMatchAll(const STLString & str, const OMEChar** patterns,const int sub=1, bool useSearch=false)
{
	bool ret=false;
	StrArray results=BatchRegex(str,patterns,sub,useSearch);
	if(results.size()==1)
		ret=true;
	else if(results.size()>1)
		ret=std::all_of(results.begin()+1,results.end(),[](STLString & res){return !res.empty();});
	return ret;
}


static bool BatchRegexMatchAll(const STLString & str, const StrArray & patterns, const int sub = 1, bool useSearch = false)
{
	bool ret = false;
	StrArray results = BatchRegex(str, patterns, sub, useSearch);
	if (results.size() == 1)
		ret = true;
	else if (results.size()>1)
		ret = std::all_of(results.begin() + 1, results.end(), [](STLString & res){return !res.empty(); });
	return ret;
}

static STLString BatchRegexReplace(const STLString & str, const OMEChar** patterns)
{
	ARRAY_TYPE<STLRegex> searchRegs;
	StrArray replacePatterns;
	
	int i=0;

	//parse until null char hit
	while (patterns[i][0]!='\0')
	{
		searchRegs.push_back(STLRegex(patterns[i++]));
		replacePatterns.push_back(patterns[i++]);
	}
	//allocate regexes
	STLString ret=str;
	
	//run all regexes.
	for(int k=0; k<searchRegs.size(); k++)
	{
		STLsmatch subMatch;
		ret=std::regex_replace(ret,searchRegs[k],replacePatterns[k]);
		
	}

	return ret;
}

static STLString BatchRegexReplace(const STLString & str, const StrArray & patterns)
{
	ARRAY_TYPE<STLRegex> searchRegs;
	StrArray replacePatterns;

	//parse until null char hit
	for (size_t i = 0; i < patterns.size();)
	{
		searchRegs.push_back(STLRegex(patterns[i++]));
		replacePatterns.push_back(patterns[i++]);
	}
	//allocate regexes
	STLString ret = str;

	//run all regexes.
	for (int k = 0; k<searchRegs.size(); k++)
	{
		STLsmatch subMatch;
		ret = std::regex_replace(ret, searchRegs[k], replacePatterns[k]);

	}

	return ret;
}

//Utility functions
__EXPORT__ size_t FindMatching(const STLString & expr, const size_t & start, const OMEChar & oChar, const OMEChar & cChar, const bool & reverse = false);
__EXPORT__ size_t FindMatching(const STLString & expr, const size_t & start, const STLString & oStr, const STLString & cStr);
__EXPORT__ size_t FindNth(const STLString & expr, const OMEChar & trg, const size_t & n);
__EXPORT__ size_t RFindNth(const STLString & expr, const OMEChar & trg, const size_t & n);

static STLString PrepPath(const STLString & relPath,const STLString mdlPath=STLString())
{
	//stupid windows reverse slash path thingy...
	char divider= mdlPath.find('/') !=STLString::npos ? '/' : '\\';
	
	size_t tailInd = mdlPath.find_last_of(divider);
	tailInd = tailInd!=STLString::npos ? tailInd+1 : 0;
	
	//if relPath is absolute, just return
	//cull current directory indicator
	STLString out=relPath;
	if (relPath.empty() || (relPath[0] != '/' && relPath[0]!='\\' && (relPath.size() <= 1 || relPath[1] != ':')))
	{
		if (relPath.substr(0, 2) == ".\\" || relPath.substr(0, 2) == "./")
			out = relPath.substr(2, STLString::npos);
		out = mdlPath.substr(0, tailInd) + out;
		//ensure consistant path dividers
		for (size_t i = 0; i < out.size(); i++)
		{
			if (out[i] == '\\')
				out[i] = '/';
		}

	}

	return out;
}

template<class _RanIt,class _De>
inline STLString ConcatStrings(_RanIt begin, _RanIt end, const _De delim)
{
	_RanIt itr=begin;
	oSTLSStream out;
	if(itr!=end)
	{
		out<<(*itr);
		++itr;

		for(;itr!=end;++itr)
			out<<delim<<*itr;
	}

	return out.str();
}


template<class _Cont, class _De>
inline STLString ConcatStrings(_Cont & cont, const _De delim)
{
	return ConcatStrings(cont.begin(), cont.end(), delim);
}


template<class _RanIt, class _De>
inline STLString ConcatStrings(_RanIt begin, _RanIt end, const _De delim,STLString(*ConvFunc)(_RanIt &))
{
	_RanIt itr = begin;
	oSTLSStream out;
	if (itr != end)
	{
		out << ConvFunc(itr);
		++itr;

		for (; itr != end; ++itr)
			out << delim << ConvFunc(itr);
	}

	return out.str();
}


inline StrArray SplitString(const STLString & inStr, const OMEChar & delim='\n')
{
	StrArray outArray;
	
	if(!inStr.empty())
	{
		iSTLSStream inStrm(inStr);
		STLString buff;
		while(inStrm.good())
		{
			std::getline(inStrm,buff,delim);
			outArray.push_back(buff);
		}
	}

	return outArray;
}

inline StrArray SplitString(const STLString & inStr, const STLString & deStr)
{
	StrArray outArray;

	if (!inStr.empty())
	{
		size_t startPos = 0;
		size_t hit;
		do
		{
			hit = inStr.find(deStr,startPos);
			if (hit == STLString::npos)
				outArray.push_back(inStr.substr(startPos));
			else
				outArray.push_back(inStr.substr(startPos, hit - startPos));

			startPos = hit + deStr.length();

		} while (hit != STLString::npos);
	}

	return outArray;
}

inline STLString LTrimString(const STLString & str)
{
	static const STLRegex ltr("^[[:space:]]*");
	return std::regex_replace(str,ltr,STLString());
}

inline STLString RTrimString(const STLString & str)
{
	static const STLRegex ltr("[[:space:]]*$");
	return std::regex_replace(str,ltr,STLString());
}

inline STLString FullTrimString(const STLString & str)
{
	static const STLRegex ltr("^[\\s]*(.*?)[\\s]*$");
	return std::regex_replace(str, ltr, STLString("$1"));
}

inline STLString ToLowerCase(STLString str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

void TrimRange(const STLString & str, size_t & start, size_t & len);
size_t CountChar(const STLString & str, const OMEChar & trg);

inline STLString OMEGetWorkingDirectory()
{
	char buff[MAXPATHLEN];
#if defined WIN32 || defined _WIN64
	_getcwd(buff, MAXPATHLEN);
#else
	getcwd(buff,MAXPATHLEN);
#endif
	STLString ret(buff);

	return ret;

}

inline bool OMECreateDirectory(const OMEChar* path)
{
#if defined WIN32 || defined _WIN64
	//unicode workaround
	CString temp(path);
	return CreateDirectoryW(temp.AllocSysString(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
#else
    struct stat st = {0};
    
    bool ret=stat(path, &st) != -1;
    if (!ret)
        ret=mkdir(path,0700)!=-1;
    return ret;
#endif
}
