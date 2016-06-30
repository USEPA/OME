#pragma once
#include"OMEDefines.h"

class Model;


/** Base class for classes that serialize model expression statements for execution. */
class BaseEvalSerializer
{
public:
	/** Default Constructor. */
	BaseEvalSerializer();
	/** Default Destructor. */
	virtual ~BaseEvalSerializer();

	/** Serialize Evaluable expressions and store the result internally. 
		@param pRootModel The root model object for the model to explore. 

		@return true if serialization operation was successful; false otherwise.
	*/
	virtual bool SerializeEval(Model* pRootModel)=0;
	/** Serialize Evaluable expressions and store the result internally. 
		@param entries An array of Evaluables to optimize.
		@param forInit true if serializing for initialization instructions; false otherwise.

		@return true if serialization operation was successful; false otherwise.
	*/
	virtual bool SerializeEval(const EvalArray & entries,const bool forInit=false)=0;
	/** Serialize Evaluable expressions and store the result internally. 
		@param entry The Evaluable object whose expression is to be serialized.
		@param forInit true if serializing for initialization instructions; false otherwise.

		@return true if serialization operation was successful; false otherwise.
	*/
	virtual bool SerializeEval(Evaluable* entry,const bool forInit)=0;
	/** Retrieves the internal serialized initialization statements.
		@return a string of serialized initialization statements.
	*/
	virtual STLString GetInitStatements(const bool forPrint=false)=0;
	/** Retrieves the internal serialized evaluative statements.
		@return a string of serialized evaluative statements.
	*/
	virtual STLString GetEvalStatements(const bool forPrint=false)=0;

	/** Clears internal buffers storing serialized statements */
	virtual void ClearSerializer()=0;
	//submodels?

	/** Write out serial expressions in a text format
		@param initPath Path to destination for file containing init statements. Pass an empty string to skip writing.
		@param evalPath Path to destination for file containing eval statements. Pass an empty string to skip writing.

		@return true if writing is supported and writing was successful.
	*/
	virtual bool WriteSerialExpressions(const STLString & initPath, const STLString & evalPath)=0;

	/** Get total number of lines in produced serial representation.
		@param forInit true if line count should come from evaluation statements, false if count should come from initialization statements.
		@return The number of lines in the targeted serialized object.
	*/
	virtual size_t GetLineCount(const bool forInit)=0;

	/** Obtain a list of debug breakpoints from the serializer.
		@param breaks BreakSet to populate with any existing breakpoints.
		@param forInit true if breakpoints should be for the initialization script, false if for the evaluation/update script.
		@return true if breakpoints are supported, false otherwise.
	*/
	virtual bool PopulateBreaks(BreakSet & breaks,const bool forInit) {return false;};

	/** Set a breakpoint (if supported).
		@param line The line at which to apply the breakpoint.
		@param enabled If true, add a breakpoint; otherwise, remove a breakpoint.
		@param forInit If true, apply to batch init statements; otherwise, apply to batch update statements.
		@return true if set breakpoint is supported; false otherwise.
	*/
	virtual bool SetBreak(const size_t & line, const bool & enabled, const bool & forInit){ return false; }
	/** @param path The path to the init break point file. */
	virtual void SetInitBreakPath(const STLString & path)=0;
	/** @return Path to the init break point file. */
	virtual STLString GetInitBreakPath() const=0;

	/** @param path The path to the eval break point file. */
	virtual void SetEvalBreakPath(const STLString & path)=0;
	/** @return Path to the init break point file. */
	virtual STLString GetEvalBreakPath() const=0;

	/** Enable or disable debug options for a given serializer.
		@param dbg bool value indicating whether debug components should be enabled or disabled.
	*/
	virtual void SetDbgEnabled(const bool & dbg)=0;
	/** @return true if a serializer's debug mode is enabled; false otherwise. */
	virtual bool GetDbgEnabled() const=0;

protected:



};

