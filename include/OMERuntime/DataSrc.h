#pragma once
#include "OMEDefines.h"
#include "ValueArchive.h"

/** Used to store datasets in a 2d record-like format. */
class __EXPORT__ DataSrc
{
public:
	
	/// Identifiers for column types.
	enum COLTYPE { NUMBER, ///< Column contains numerical (floating point) values.
						STRING  ///< Column contains string values.
				};

	DataSrc() { Clear(); }
	~DataSrc() 
	{
	}
	void Clear();
	
	bool GetNumVal(const size_t & col,const size_t & rec, float & out) const;
	bool GetNumVal(const size_t & col,const size_t & rec, double & out) const;
	bool GetNumVal(const size_t & col,const size_t & rec, int & out) const;
	bool GetNumVal(const size_t & col,const size_t & rec, size_t & out) const;
	bool GetStrVal(const size_t & col,const size_t & rec, STLString & out) const;
	bool SetNumVal(const size_t & col,const size_t & rec, const float & value);
	bool SetStrVal(const size_t & col,const size_t & rec, const STLString & value);

	bool GetMaxValForCol(size_t col, OME_SCALAR & out);
	bool GetMinValForCol(size_t col, OME_SCALAR & out);


	OMESclrArray GetNumValsForCol(const size_t col) const;

	bool ReadCSV(const STLString & filename, const OMEChar delim=',');

	int GetRowCount() const;
	int GetColumnCount() const;
	

	int GetFieldCol( const OMEChar* label ) const;
	STLString GetLabelForCol(size_t i) const;
	int AddFieldCol( const OMEChar* label, const float initVal);
	int AddFieldCol(const OMEChar* label, const OME_SCALAR* vals, const OME_SCALAR & count);
	int AddFieldCol(const OMEChar* label, ValueArchive* pArch, const size_t & start, const OME_SCALAR & count);
	/** Check to see if the column stores numeric values.
		@param i Index of the column to check.
		@return true if the column at index i stores numeric values; false otherwise.
	*/
	inline bool IsNumCol(const int i) const { return i< m_columns.size() ? m_columns[i].m_type==NUMBER : false; }
	/** Check to see if the column stores string values.
		@param i Index of the column to check.
		@return true if the column at index i stores string values; false otherwise.
	*/
	inline bool IsStrCol(const int i) const { return i< m_columns.size() ? m_columns[i].m_type==STRING : false; } 
	/** @return Path to the source of internal data. */
	inline STLString GetSrcPath() const { return m_srcPath; }
	
	/** Determine if a coordinate is inbounds of the data set.
		@param rec The record ordinate to check.
		@param col The column ordinate to check.
		@return true if coordinate is inbounds; false otherwise.
	*/
	inline bool CheckIndBounds(const int rec, const int col) const { return rec>=0 && rec<m_rowCount && col>=0 && col<m_labels.size(); }

	void BuildIndexTree(IndexNode & retNode, std::set < IndPair, IndPairComp > & cols,const int & valCol) const;

private:
		/** Struct that represents value of cell in CSV record. Should be either string or float value,but not both.
		 \todo replace m_str with OMEChar and convert struct to union.
	*/
	struct SCCell
	{
		OME_SCALAR m_val;		///< float value for cell.
		STLString m_str;  ///< String value for cell.
	};
	/// Column representation for csv value collection.
	struct SCCol
	{
		COLTYPE m_type;					///< Type identifier for column; either NUMBER or STRING.
		//deque or vector?
		std::deque<SCCell> m_cells;	///< Ordered collection of cells in this column.
	};


	int m_rowCount;						///< The number of rows/records within this DataSrc.
	
	std::vector<STLString> m_labels; ///< Ordered list of column labels.
	std::vector<SCCol> m_columns;		///< Ordered list of columns in csv; effectively a 2d array of record cells.

	STLString m_srcPath;					///< Cached path to origin of loaded data.

};
