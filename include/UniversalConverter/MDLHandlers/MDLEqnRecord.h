#pragma once

#include "OMEDefines.h"
#include "EvalXSetInterpTable.h"

/** Record of information useful to converting an MDL equation into a full model component. */
class MDLEqnRecord
{
public:
	MDLEqnRecord();
	MDLEqnRecord(const STLString & v, const STLString & e, const STLString & u, const STLString & c);
	MDLEqnRecord(const STLString & v, const EvalTable::DataArray & xV,const EvalTable::DataArray & yV, const STLString & u, const STLString & c);
	~MDLEqnRecord();

	STLString GetVariable() const;
	STLString GetEquation() const;
	STLString GetUnits() const;
	STLString GetComment() const;
	STLString GetName() const;

	void SetHasBeenApplied(const bool & applied);
	bool GetHasBeenApplied() const;

	EvalXSetInterpTable* GenTable();

	static STLString FixWhitespace(const STLString & inStr);
	static STLString FixIllegalChars(const STLString & inStr);
	static STLString Dequote(const STLString & inStr);
	static STLString DecimalizeNumbers(STLString inStr);

private:
	STLString m_varID;			   ///< The id of the associated variable.
	STLString m_eqn;			   ///< The actual expression represented by the equation.
	STLString m_units;			   ///< The units associated with the equation.
	STLString m_comment;		   ///< The comments associated with the equation.
	STLString m_name;			   ///< The name of the variable receiving the equation's assignment.

	bool m_hasBeenApplied;		   ///< True if the equation has been applied to a model component; false otherwise.
	//table specific
	bool m_isTable;				   ///< True if the equation object represents a table instead of a true expression.
	EvalTable::DataArray m_xVals;  ///< The X-values associated with a table object.
	EvalTable::DataArray m_yVals;  ///< The Y-values associated with a table object.
};

