#pragma once
#include "XMLLoadable.h"
#include "OMEDefines.h"

/** Table of values that can be accessed by Evaluable expressions.*/
class __EXPORT__ EvalTable : public XMLLoadable
{
public:

	typedef ARRAY_TYPE<OME_SCALAR> DataArray;		///< Type that contains a list of values.

	EvalTable();
	EvalTable(const STLString & id, const IndVec & dims,const DataArray &data);
	EvalTable(const STLString & id, const IndVec & dims, const STLString & filepath, const STLString & colName, const DataArray &data);
	EvalTable(const STLString & id, const IndVec & dims, const STLString & filepath, const STLString & colName);
	EvalTable(const EvalTable & et);
	EvalTable(TI2Element* pCurrElem,const STLString & tag, const STLString & filename);

	EvalTable& operator=(const EvalTable & rhs);

	virtual ~EvalTable();

	/** @return The EvalTable's unique id.*/
	inline STLString GetID() const {return m_tblID;}
	/** @param id The unique id to assign to the EvalTable. */
	inline void SetID(const STLString & id) {m_tblID=id; }

	/** @return path to CSV used to populate EvalTable. */
	inline STLString GetFilePath() const { return m_filepath; }
	/** Set path for loading Table values.
		@param fp The path to the CSV file.
	*/
	inline void SetFilePath(const STLString & fp) { m_filepath = fp; }

	/** @return name of CSV column containing table values. */
	inline STLString GetColumnName() const { return m_colName; }
	/** Set name of CSV column containing table values. 
		@param cn The name of the column.
	*/
	inline void SetColumnName(const STLString & cn) { m_colName = cn; }

	virtual bool GetValue(OME_SCALAR & val,const unsigned int ind) const;
	virtual bool GetValue(OME_SCALAR & val,const unsigned int* inds, const unsigned int indCount) const;

	virtual bool SetValue(const OME_SCALAR & val,const unsigned int ind);
	virtual bool SetValue(const OME_SCALAR & val,const unsigned int* inds, const unsigned int indCount);

	unsigned int GetDimensionCount() const;

	IndVec GetAllDims() const;
	DataArray GetAllVals() const;

	virtual STLString GetXMLRepresentation(const unsigned int indent=0,const unsigned int inc=4);
	virtual const OMEChar* GetXMLTag() const { return "table_data"; }
   virtual void GetXMLAttributes(StrArray & out) const;
   virtual void GetXMLSubNodes(StrArray & out,const unsigned int indent=0,const unsigned int inc=4);

   bool PopulateFromCSV(const STLString & filepath, const STLString & colname);
   bool PopulateFromCSV();

protected:
	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename);

	void Duplicate(const EvalTable & et);

	bool CheckBounds(const unsigned int* inds, const unsigned int indCount) const;
	OME_SCALAR ValForIndex(const unsigned int* inds, const unsigned int indCount) const;
	void SetValAtIndex(const OME_SCALAR val,const unsigned int* inds, const unsigned int indCount);

	IndVec m_dims;		///< Dimensions used when accessing an element.
	DataArray m_vals;	///< Raw value storage.
	STLString m_tblID;///< Unique Identifier for table.
	STLString m_filepath;   ///< Optional path for file.
	STLString m_colName;	///< Name of column containing values.
};