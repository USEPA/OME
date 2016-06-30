#pragma once
//#include "PLParserDefines.h"
#include "OMEDefines.h"

class Model;
/** Static class for processing import modifiers. */
class ModifierProcessor
{
	/** Function pointer type for use in determining an object's scope.*/
	using ScopeFunc= void(*)(Evaluable*);
	/** Function pointer type for use in expression conversion.*/
	using ExprConvFunc = STLString(*)(const STLString &,OMEObject*);

	/** Result flags for modification processing.
	*/
	enum RESULT {	NO_ERR=0,		///< No error occurred. 
						BAD_SYNTAX,		///< Statement contained improper syntax.
						OBJECT_NF,		///< Targeted object was not found.
						MISSING_ARG,	///< Required argument is missing.
						BAD_TYPE			///< Incorrect type for statement.
					};

public:
	static RESULT ProcessObject(OMEObject*& pObj,const STLString & expr,OMEArray & omitted, ScopeFunc pscpFunc=NULL,ExprConvFunc pConvFunc=NoExprConv);
	static STLString ExtractExpression(const STLString & inStr);
private:
	static RESULT ProcessInfluenceTo(OMEObject* pObj, const STLString & trgName, Model* pBm);
	static RESULT ProcessInfluenceFrom(OMEObject* pObj, const STLString & srcName, Model* pBm);
	static RESULT ProcessMove(OMEObject* pObj, const STLString & mdlName, OMEArray & omitted);
	static RESULT ProcessOmit(OMEObject* pObj, Model* pBm,OMEArray & omitted);
	static RESULT ProcessInitOnly(OMEObject* pObj);
	static RESULT ProcessExpression(OMEObject* pObj, const STLString & expr,ExprConvFunc pConvFunc,ScopeFunc pFunc=NULL);
	static RESULT ProcessToTS(OMEObject*& pObj);

	static void RemoveInflChain(OMEObject* pEval, OMEObject* pLast,OMEArray & omitted);

	/** Default Expression Conversion function, which just passes through str.
		@param str The expression to pass through;
		@param pEval not used.
		@return a copy of str.
	*/
	static STLString NoExprConv(const STLString & str,OMEObject* pEval){ return str; }
};

