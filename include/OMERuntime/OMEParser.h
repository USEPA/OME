#pragma once

#include "OMEDefines.h"
#include "ModelEnum.h"

class OMEObject;
class Evaluable;
class Param;
class Model;
class SubmodelAssoc;
class BaseEvalSerializer;
class ISpatialDataProvider;

/** Virtual class defining an interface for Parsers used to evaluate equations. */
class OMEParser
{
public:
	OMEParser(void);
	virtual ~OMEParser(void);

	/** Initialization function. 
		@param pParentMdl model that contains the parser
		@return true if successfully initialized, false otherwise
	*/
	virtual bool Init(Model* pParentMdl=NULL)=0;

	/** Prepare temporary data containers for use with initialization and update statements.*/
	virtual void PrepTempDataHandler() = 0;

	/** Alternate Initialization function.
		@param args String containing values that dictate how the Parser initializes itself.
		@param pBm Pointer to BaseManager object to associate with initialization.
		@return true if successfully initialized, false otherwise
	*/
	virtual bool Init(const STLString & args,BaseManager* pBm) { return false; };
	/** Compiles a given expressoin
		@param expr the expression to compile
		@param pCaller The calling object that is compiling the expression.
		@return true if successfully compiled, false otherwise
	*/
	virtual bool CompileExpression(const OMEChar* expr, OMEObject* pCaller=NULL)=0;

	/** Checks to see if expr is a valid expression
		@param expr the expression to be tested
		@return true if expression passes test, false otherwise
	*/
	virtual bool TestExpression(const OMEChar* expr)=0;

	/** Define a variable
		@param varName the name to use for the variable
		@param pEval the object to bind to the variable
		@return true if define is successful, false otherwise
	*/
	virtual bool DefineVar(const OMEChar* varName, Evaluable *pEval)=0;

	/** Define a variable
		@param varName the name to use for the variable
		@param pSclr the OME_SCALAR to bind to the variable
		@return true if define is successful, false otherwise
	*/
	virtual bool DefineVar(const OMEChar* varName, OME_SCALAR *pSclr)=0;

	/** Redefine a variable
		@param varName the name to use for the variable
		@param pEval the object to bind to the variable
		@return true if redefine is successful, false otherwise
	*/
	virtual bool RedefineVar(const OMEChar* varName, Evaluable *pEval)=0;
	
	/** Evaluate compiled expression.
		@param value OME_SCALAR variable to store resulting value in.
		@param isTest If true, the expression omits some exception handling and just returns the success or failure of the evaluation.
		@param args Optional list of arguments to pass to the expression.
		@return true if stored expression was successfully evaluated, false otherwise.
	*/
	virtual bool Evaluate(OME_SCALAR & value, const bool isTest=false,const SclrArgs & args=SclrArgs())=0;

	/** Evaluate a compiled expression
		@param values A pointer that is unallocated or NULL if count is <= 0, or pointing to an array of count values if count > 0.
		@param count A dual purpose variable; on input, count can optionally define a pre-allocated length for values if it is > 0.
			On return, count will hold the number of valid entries in values, which can be < the original value passed in with count.
		@param isTest If true, the expression omits some exception handling and just returns the success or failure of the evaluation.
		@param args Optional list of arguments to passed to the expression.
		@return true if stored expression was successfully evaluated, false otherwise.
	*/
	virtual bool Evaluate(OME_SCALAR* & values, int & count, const bool isTest=false,const SclrArgs & args=SclrArgs())=0;
	/** Evaluate a compiled expression
		@param values a SclrResults object to catch count values.
		@param count A dual purpose variable; on input, count can optionally define a pre-allocated length for values if it is > 0.
			On return, count will hold the number of valid entries in values, which can be < the original value passed in with count.
		@param isTest If true, the expression omits some exception handling and just returns the success or failure of the evaluation.
		@param args Optional list of arguments to passed to the expression.
		@return true if stored expression was successfully evaluated, false otherwise.
	*/
	virtual bool Evaluate(SclrResults & values, int & count, const bool isTest=false,const SclrArgs & args=SclrArgs())=0;

	/** Perform an evaluation of a series of expressions that were previously loaded using LoadBatch().
		@param args A list of OME_SCALAR values to pass in as arguments.
		@param isTest Flag indicating that evaluation is occurring during test mode.
		@return true If evaluation was supported and successful; false otherwise.
	*/
	virtual bool BatchEvaluate(const SclrArgs & args=SclrArgs(), const bool isTest=false)=0;
	/** Perform an evaluation of a series of expressions that were previously loaded using LoadBatch().
		@param values Container to hold results from batch evaluation.
		@param count A dual purpose variable; on input, count can optionally define a pre-allocated length for values if it is > 0.
			On return, count will hold the number of valid entries in values, which can be < the original value passed in with count.
		@param args A list of OME_SCALAR values to pass in as arguments.
		@param isTest Flag indicating that evaluation is occurring during test mode.
		@return true If evaluation was supported and successful; false otherwise.
	*/
	virtual bool BatchEvaluate(SclrResults & values, int & count, const SclrArgs & args=SclrArgs(), const bool isTest=false)=0;
	/** Perform an evaluation of a series of expressions that were previously loaded using LoadBatch().
		@param values A pointer that is unallocated or NULL if count is <= 0, or pointing to an array of count values if count > 0.
		@param count A dual purpose variable; on input, count can optionally define a pre-allocated length for values if it is > 0.
			On return, count will hold the number of valid entries in values, which can be < the original value passed in with count.
		@param args A list of OME_SCALAR values to pass in as arguments.
		@param isTest Flag indicating that evaluation is occurring during test mode.
		@return true If evaluation was supported and successful; false otherwise.
	*/
	virtual bool BatchEvaluate(OME_SCALAR* & values, int & count,const SclrArgs & args=SclrArgs(), const bool isTest=false)=0;
	/** Produce a copy of the parser
		@return a pointer to a copy of the parser
	*/
	virtual OMEParser* Copy()=0;

	/** Define an enumeration set (typically as a series of constants).
		@param enumVals The enumeration set used in the definition.
	*/
	virtual void DefineEnumVals(const ModelEnum & enumVals)=0;
	/** Redefine an enumeration set (typically as a series of constants.
		@param enumVals The enumeration set used in the redefinition.
	*/
	virtual void RedefineEnumVals(const ModelEnum & enumVals)=0;

	/** Define an array based on an array of doubles
		@param varName The name of the array.
		@param pArr Pointer to an array of doubles.
		@return true if successful, false otherwise.
	*/
	virtual bool DefineArray(const OMEChar* varName, SclrVArray* pArr)=0;

	/** Define a parameter-type entry in parser
		@param pParam the parameter object.
		@return true if successful, false otherwise.
	*/
	virtual bool DefineParameter(Param* pParam)=0;

	/** Redefine a parameter-type entry in parser
		@param pParam the parameter object.
		@return true if successful, false otherwise.
	*/
	virtual bool RedefineParameter(Param* pParam)=0;

   /** Define a constant int value.
	  @param constName The name of the constant.
	  @param val The integer value of the constant.
	  @return true if the define action was successful, false otherwise.
   */
	virtual bool DefineConstant(const OMEChar* constName, const int & val) { return false; }
	/** Define a constant OME_SCALAR value.
	  @param constName The name of the constant.
	  @param val The OME_SCALAR value of the constant.
	  @return true if the define action was successful, false otherwise.
   */
	virtual bool DefineConstant(const OMEChar* constName, const OME_SCALAR & val) { return false; }
	/** Define a constant string value.
	  @param constName The name of the constant.
	  @param val The string value of the constant.
	  @return true if the define action was successful, false otherwise.
   */
	virtual bool DefineConstant(const OMEChar* constName, const OMEChar* & val) { return false; }

   /** Redefine a previously defined constant int value.
	  @param constName The name of the constant.
	  @param val The integer value of the constant.
	  @return true if the redefine action was successful, false otherwise.
   */
	virtual bool RedefineConstant(const OMEChar* constName, const int & val) { return false; }
   /** Redefine a previously defined constant OME_SCALAR value.
	  @param constName The name of the constant.
	  @param val The OME_SCALAR value of the constant.
	  @return true if the redefine action was successful, false otherwise.
   */
	virtual bool RedefineConstant(const OMEChar* constName, const OME_SCALAR & val) { return false; }
   /** Redefine a previously defined constant string value.
	  @param constName The name of the constant.
	  @param val The string value of the constant.
	  @return true if the redefine action was successful, false otherwise.
   */
	virtual bool RedefineConstant(const OMEChar* constName, const OMEChar* & val) { return false; }

	/** Define a submodel entry within the parser.
		@param pMdl Pointer to the Model to register.
		@return true If pMdl was registered successfully.
	*/
	virtual bool DefineSubmodel(Model* pMdl)=0;

	/** Define an association entry within the parser.
		@param pSa Pointer to the SubmodelAssoc to register.
		@return true If pSa was registered successfully.
	*/
	virtual bool DefineAssoc(SubmodelAssoc* pSa)=0;

	/** Define a Model and all contained Evaluables and Models contained within.
		@param pMdl Pointer to the root Model to register.
		@return true If pMdl and all its relevant components were registered successfully.
	*/
	virtual bool DefineAll(Model* pMdl) {return false;}

	/** Updates representations of a single model instance.
		@param pMdl Pointer to the Model containing the instance to update.
		@param ind The index of the instance to update.
		@return true if the update is supported and successful; false otherwise.
	*/
	virtual bool UpdateSubmodelEntry(Model* pMdl, const int ind)=0;
	/** Updates representations of a series of model instances.
		@param pMdl Pointer to the Model containing the instances to update.
		@param start The index of the first instance to update.
		@param count The number of instances to update starting at the index start.
		@return true if all updates are supported and successful; false otherwise.
	*/
	virtual bool UpdateSubmodelEntries(Model* pMdl, const int start,const int count)=0;

	/** Convert an expression to a parser's native format; can be used to preprocess expressions in memory.*/
	virtual void ConvertExpressionForEval(Evaluable* pEval)=0;

	/** Load a batch of evaluation statements into OMEParser. The batch object will be placed at the top of the batch stack
		@param batch A string containing a batch of statements.
	*/
	virtual void PushBatch(const OMEChar* batch)=0;
	/** Clear all batch statements from the OMEParser.*/
	virtual void ClearBatch()=0;
	/** Pop the top batch statement on the stack.*/
	virtual void PopBatch()=0;
	/** @return true if parser is presently running batch statements during evaluation; false otherwise.*/
	virtual bool InBatchMode()=0;

	/** @param breaks The break points to assign to the parser.*/
	virtual void SetBreaks(const BreakSet & breaks)=0;

	/** Reset Parser to initial state */
	virtual void Reset(Model* pMdl=NULL)=0;

	/** Retrieve optional mode identifier.
		@return A string containing the current mode identifier if set, or an empty string if not used.
	*/
	virtual STLString GetModeID() const { return STLString(); };
	/** Set the mode identifier. The mode identifier is strictly informational; support in subclasses is optional.
		@param eh The string used as the new mode ID.
	*/
	virtual void PushModeID(const STLString & eh) {};
	/** Revert to previous mode identifier. Support in subclasses is optional.
		@return A string containing the Mode ID being popped, or an empty string if ModeID is not implemented.
	*/
	virtual STLString PopModeID() {return STLString();};
	/** Clear all stacked mode ids and reset current mode id to the default value. Support in subclasses is optional. */
	virtual void ClearModeIDs() {};

	/** Generate the associated serializer that can produce code or script that can be executed by this parser.
		@return Pointer to newly initialized serializer, or NULL if no such Serializer exists.
	*/
	virtual BaseEvalSerializer* GenerateSerializer()=0;

	/** Associate the parser with a specific serializer. This can be useful if the parser requires some JIT conversion of eval statements.
		@param pS Pointer to serializer to associate.
		@return true if pS is of a compatible Serializer type and has been associated; false otherwise.
	*/
	virtual bool AssociateWithSerializer(BaseEvalSerializer* pS)=0;

	/** Load SDP function bindings for this parser.
		@param pSDP Pointer to the targeted SpatialDataProvider.
	*/
	virtual void LoadSDPFuncs(ISpatialDataProvider* pSDP)=0;

	/** Flag indicating if OMEParser subclass requires objects to be resorted every time om model load.
		@return true if a resort is required on load; false otherwise.
	*/
	virtual bool RequiresRuntimePrioritization() const { return true; }

	static void PerformPack(Evaluable* pEval);
	static void PerformUnPack(Evaluable* pEval);

protected:

	OME_SCALAR* GetValueAddress(Evaluable* pEval);

	/** Function used to unpack a specific expression.*/
	using UnpackFunc = STLString(*)(Evaluable*, const STLString&);
	
	static std::map<STLString, UnpackFunc> s_unpackFuncs;  ///< Registry of functions for unpacking expressions that inject model componnents.


	static void BuildFuncLists();
	static UnpackFunc UnpackFuncForID(const STLString & call);

	//unpack funcs
	static STLString UnpackDelay1(Evaluable* pCaller, const STLString & call);
	static STLString UnpackDelay3(Evaluable* pCaller, const STLString & call);
	static STLString UnpackDelayN(Evaluable* pCaller, const STLString & call);
	static STLString UnpackForcst(Evaluable* pCaller, const STLString & call);
	static STLString UnpackSmth1(Evaluable* pCaller, const STLString & call);
	static STLString UnpackSmth3(Evaluable* pCaller, const STLString & call);
	static STLString UnpackSmthN(Evaluable* pCaller, const STLString & call);
	static STLString UnpackTrend(Evaluable* pCaller, const STLString & call);

	//pack/unpack utils
	static STLString UnpackDelay(Evaluable* pCaller, const STLString & input, const STLString & delayDuration, const unsigned int & order, const STLString & init);
	static STLString UnpackSmooth(Evaluable* pCaller, const STLString & input, const STLString & avgTime, const unsigned int & order, const STLString & init);

	static Influence* InsertAutoInfluence(Evaluable* pSrc, Evaluable* pTrg, Model* pMdl);
	static void UnpackArgs1(const STLString & call, STLString & input, STLString & arg, STLString & init, const bool & fillInitWith0 = false);
	static void UnpackArgs2(const STLString & call, STLString & input, STLString & arg1,STLString & arg2, STLString & init,const bool & fillInitWith0=false);
};

