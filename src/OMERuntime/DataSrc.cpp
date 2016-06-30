#include "DataSrc.h"
#include "OMEDefines.h"
#include <fstream>

#undef max
#undef min

/** Clear all data from the internal representation. */
void DataSrc::Clear()
{
	m_labels.clear();
	m_columns.clear();
	m_rowCount=-1; //-1 implies nothing has been loaded
	m_srcPath.clear();
}

/** Populate with data from a CSV file.
	@param filename The target file to load.
	@param delim Character used to delimit cells in the CSV file.
	@return true if the CSV was successfully loaded; false otherwise.
*/
bool DataSrc::ReadCSV(const STLString & filename, const OMEChar delim)
{
	bool ret=false;
	std::ifstream inFile(filename);
	if(inFile.good())
	{
		Clear();
		m_srcPath=filename;
		m_rowCount++;

		//get headers
		STLString temp;
		std::getline(inFile,temp);
        
        iSTLSStream buffer(temp);
        
		//this will load headless columns as well.
		while(std::getline(buffer,temp,delim))
		{
			if (!temp.empty() && ((temp[0] == '\'' && temp[temp.size() - 1] == '\'') || (temp[0] == '"' && temp[temp.size() - 1] == '"')))
				temp = temp.substr(1, temp.size());
			
			//trim whitespace on ends.
			temp = FullTrimString(temp);
			
			m_labels.push_back(temp);
			temp.clear();
		}

		size_t colCount=m_labels.size();
		m_columns.resize(colCount);
		unsigned int i;
		//get data and determine types
		if(inFile.good())
		{
			buffer.clear();
			std::getline(inFile,temp);
			buffer.str(temp);

			if(!temp.empty() && temp[0]!='\n' && temp[0]!='\r')
			{
				for(i=0; i<colCount; i++)
				{
					SCCell cell;
					char* endCheck=NULL;
					std::getline(buffer, temp, delim);
                    
                    //chome \r if present (stupid windows line endings with \r\n)
                    if(temp.back()=='\r')
                        temp.resize(temp.size()-1);
					cell.m_val=std::strtod(temp.c_str(),&endCheck);
					//if the last character is NULL or delim, then the column is numerical
					if(!temp.empty() && (*endCheck==0 || *endCheck==delim))
						m_columns[i].m_type=NUMBER;
					else	
					{
						m_columns[i].m_type=STRING;
						cell.m_str=temp;
					}
					m_columns[i].m_cells.push_back(cell);
				}
				m_rowCount++; 
			}
		}

		//get rest of data
		while(inFile.good())
		{
			buffer.clear();
			std::getline(inFile,temp);
			buffer.str(temp);
			
			if(!temp.empty() && temp[0]!='\n' && temp[0]!='\r')
			{
				for(i=0; i<colCount; i++)
				{
					SCCell cell;
				
					std::getline(buffer,temp,delim);
					//assume string
					cell.m_str=temp;
					if(m_columns[i].m_type==NUMBER)
						cell.m_val=std::strtod(temp.c_str(),NULL);
					
					m_columns[i].m_cells.push_back(cell);
				}
				m_rowCount++;
			}
							
					std::getline(buffer,temp,delim);

		}
		ret=true;
	}

	return ret;
}

/** Retrieve a float representation of a specific value.
	@param col The column index to query.
	@param rec The record/row index to query.
	@param out Float variable to store the return value.
	@return true if value retrieval was successful; false otherwise.
*/
bool DataSrc::GetNumVal(const size_t & col,const size_t & rec, float & out) const
{
	bool ret=false;

	if(CheckIndBounds(rec,col))
	{
		if(m_columns[col].m_type==NUMBER)
		{
			out=m_columns[col].m_cells[rec].m_val;
			ret=true;
		}
		else if(m_columns[col].m_type==STRING)
		{
			iSTLSStream trans(m_columns[col].m_cells[rec].m_str);
			ret=(trans>>out).good();
		}
	}

	return ret;
}

/** Retrieve a double representation of a specific value.
	@param col The column index to query.
	@param rec The record/row index to query.
	@param out Double variable to store the return value.
	@return true if value retrieval was successful; false otherwise.
*/
bool DataSrc::GetNumVal(const size_t & col,const size_t & rec, double & out) const
{
	bool ret=false;

	if(CheckIndBounds(rec,col))
	{
		if(m_columns[col].m_type==NUMBER)
		{
			out=m_columns[col].m_cells[rec].m_val;
			ret=true;
		}
		else if(m_columns[col].m_type==STRING)
		{
			iSTLSStream trans(m_columns[col].m_cells[rec].m_str);
			ret=(trans>>out).good();
		}
	}

	return ret;
}

/** Retrieve an int representation of a specific value.
@param col The column index to query.
@param rec The record/row index to query.
@param out Double variable to store the return value.
@return true if value retrieval was successful; false otherwise.
*/
bool DataSrc::GetNumVal(const size_t & col,const size_t & rec, int & out) const
{
	bool ret = false;
	if (CheckIndBounds(rec, col))
	{
		if (m_columns[col].m_type == NUMBER)
		{
			out = (int)(m_columns[col].m_cells[rec].m_val);
			ret = true;
		}
		else if (m_columns[col].m_type == STRING)
		{
			iSTLSStream trans(m_columns[col].m_cells[rec].m_str);
			ret = (trans >> out).good();
		}
	}
	return ret;
}

/** Retrieve a size_t representation of a specific value.
@param col The column index to query.
@param rec The record/row index to query.
@param out Double variable to store the return value.
@return true if value retrieval was successful; false otherwise.
*/
bool DataSrc::GetNumVal(const size_t & col,const size_t & rec, size_t & out) const
{
	bool ret = false;
	if (CheckIndBounds(rec, col))
	{
		if (m_columns[col].m_type == NUMBER)
		{
			out = (size_t)(m_columns[col].m_cells[rec].m_val);
			ret = true;
		}
		else if (m_columns[col].m_type == STRING)
		{
			iSTLSStream trans(m_columns[col].m_cells[rec].m_str);
			ret = (trans >> out).good();
		}
	}
	return ret;
}

/** Retrieve a String representation of a specific value.
	@param col The column index to query.
	@param rec The record/row index to query.
	@param out STLString variable to store the return value.
	@return true if value retrieval was successful; false otherwise.
*/
bool DataSrc::GetStrVal(const size_t & col,const size_t & rec, STLString & out) const
{
	bool ret=false;

	if(CheckIndBounds(rec,col))
	{
		
		if(m_columns[col].m_type==NUMBER)
		{
			STLString buff;
			oSTLSStream trans;
			trans<<m_columns[col].m_cells[rec].m_val;
			out=trans.str();
		} 
		else if(m_columns[col].m_type==STRING)
			out=m_columns[col].m_cells[rec].m_str;
		ret=true;
	}

	return ret;
}

/** Set a value at a specific cell.
	@param col The column index of the target cell.
	@param rec The record/row index of the target cell.
	@param value Float variable containing the new value.
	@return true if value assignment was successful; false otherwise.
*/
bool DataSrc::SetNumVal(const size_t & col,const size_t & rec, const float & value)
{
	bool ret=CheckIndBounds(rec,col);
	if(ret)
	{
		if(m_columns[col].m_type==NUMBER)
			m_columns[col].m_cells[rec].m_val=value;
		else if(m_columns[col].m_type==STRING)
		{
			oSTLSStream trans;
			trans<<value;
			m_columns[col].m_cells[rec].m_str=trans.str();
		}
	}
	return ret;
}

/** Retrieve a String representation of a specific value.
	@param col The column index to query.
	@param rec The record/row index to query.
	@param value STLString variable to store the return value.
	@return true if value retrieval was successful; false otherwise.
*/
bool DataSrc::SetStrVal(const size_t & col,const size_t & rec, const STLString & value)
{
	bool ret=CheckIndBounds(rec,col);
	if(ret)
	{
		if(m_columns[col].m_type==NUMBER)
		{
			//attempt to convert
			iSTLSStream trans(value);
			trans>>m_columns[col].m_cells[rec].m_val;
		}
		else if(m_columns[col].m_type==STRING)
		{
			m_columns[col].m_cells[rec].m_str=STLString(value);
		}
	}
	return ret;
}

/** @return The number of data rows, or -1 if no data is loaded.*/
int DataSrc::GetRowCount() const
{
	return m_rowCount;
}

/** @return The number of data columns, or -1 if no data is loaded.*/
int DataSrc::GetColumnCount() const
{
	return m_labels.size();
}

/** Find the index for a column based on a label.
	@param label The column name to search for.
	@return The index of the column with the specified label, or -1 if the label did not match any column.
*/
int DataSrc::GetFieldCol( const OMEChar* label ) const
{
	int ret=-1;
	//remove any spare whitespace on either end.
	const STLString labelStr = FullTrimString(label);
	for(unsigned int i=0; i< m_labels.size() && ret==-1; i++)
	{
		if(m_labels[i]==labelStr)
			ret=i;
	}

	return ret;
}

/** Add an additional column to the underlying datatable.
	@param label The name of the new column.
	@param initVal The initial numeric value of each cell in the column.
*/
int DataSrc::AddFieldCol( const OMEChar* label, const float initVal)
{
	int ind=m_labels.size();
	m_labels.push_back(FullTrimString(label));
	
	SCCell newCell;
	newCell.m_str="";
	newCell.m_val=initVal;
	SCCol newCol;
	newCol.m_type=NUMBER;
	newCol.m_cells.resize(m_rowCount>=0 ? m_rowCount : 0,newCell);
	m_columns.push_back(newCol);
	return ind;
}

/** Add an additional column to the underlying datatable.
@param label The label of the new column.
@param vals Pointer to array containing the initial values for each record.
@param count The length of the array pointed to by vals and the total number of records to initialize.
@return The index if creation was successful, or -1 if creating the new column failed.
*/
int DataSrc::AddFieldCol(const OMEChar* label,const OME_SCALAR* vals, const OME_SCALAR & count)
{
	int ind = AddFieldCol(label,0.0);

	for (size_t i = 0; i < m_rowCount && i < count; ++i)
		m_columns[ind].m_cells[i].m_val = vals[i];
	return ind;
}

/** Add an additional column to the underlying datatable.
@param label The label of the new column.
@param pArch Pointer to ValueArchive containing the initial values.
@param start The first index in pArch of the first record value.
@param count The length of the values in pArch and the total number of records to initialize.
@return The index if creation was successful, or -1 if creating the new column failed.
*/
int DataSrc::AddFieldCol(const OMEChar* label, ValueArchive* pArch, const size_t & start, const OME_SCALAR & count)
{
	int ind = AddFieldCol(label, 0.0);

	for (size_t i = 0; i < m_rowCount && i < count; ++i)
		m_columns[ind].m_cells[i].m_val = (*pArch)[start+i];
	return ind;
}

/** Retrieve the label for a given column index. 
	@param i Index for the column to retrieve.
	@return Name of column at index i, or an empty string if the column is out of bounds.
*/
STLString DataSrc::GetLabelForCol(size_t i) const
{
	return i< m_labels.size() ? m_labels[i] : STLString();
}

/** Retrieve all OME_SCALAR vals stored in a given column.
	@param col The column to return.
	@return An array containing OME_SCALAR representation of column values, or an empty array if col is an invalid index.
*/
OMESclrArray DataSrc::GetNumValsForCol(const size_t col) const
{
	OMESclrArray out;
	if(col<m_columns.size())
	{
		out.resize(m_columns[col].m_cells.size());
		for(size_t i=0; i<out.size(); i++)
			GetNumVal(col,i,out[i]);
	}
	return out;
}

/** Return the maximum value found in a given column.
	@param col Index of the column to query.
	@param out Variable to hold result of query upon successful search.
	@return true if col is a valid index of a numerical column, and there are records present.
*/
bool DataSrc::GetMaxValForCol(size_t col, OME_SCALAR & out)
{
	bool ret = false;
	if (IsNumCol(col) && GetRowCount())
	{
		out = std::numeric_limits<OME_SCALAR>::min();
		for (size_t i = 0; i < m_columns[col].m_cells.size(); i++)
			if (out < m_columns[col].m_cells[i].m_val)
				out = m_columns[col].m_cells[i].m_val;
		ret = true;
	}
	return ret;
}

/** Return the minimum value found in a given column.
@param col Index of the column to query.
@param out Variable to hold result of query upon successful search.
@return true if col is a valid index of a numerical column, and there are records present.
*/
bool DataSrc::GetMinValForCol(size_t col, OME_SCALAR & out)
{
	bool ret = false;
	if (IsNumCol(col) && GetRowCount())
	{
		out = std::numeric_limits<OME_SCALAR>::max();
		for (size_t i = 0; i < m_columns[col].m_cells.size(); i++)
			if (out > m_columns[col].m_cells[i].m_val)
				out = m_columns[col].m_cells[i].m_val;
		ret = true;
	}
	return ret;
}

/** Build an index tree for a given set of columns.
	@param rootNode The root node of the index tree.
	@param cols A set of IndPairs representing the columns for indexing.
	@param valCol The label of the column containing the target value.
*/
void DataSrc::BuildIndexTree(IndexNode & rootNode, std::set< IndPair, IndPairComp > & cols, const int & valCol) const
{
	std::vector<int> refCols(cols.size());
	unsigned short i = 0;
	for (auto itr = cols.begin(); itr != cols.end(); ++itr, ++i)
		refCols[i] = GetFieldCol((*itr).second.c_str());
	
	//walk through data source, building tree using cols
	int currInd;
	IndexNode* currNode;
	for (size_t r = 0; r < GetRowCount(); ++r)
	{
		currNode = &rootNode;
		//add to tree
		for (i = 0; i < refCols.size(); ++i)
		{
			GetNumVal(refCols[i], r, currInd);
			if (currInd >= 0)
			{
				if (currNode->m_subNodes.find((size_t)currInd) == currNode->m_subNodes.end())
					currNode->m_subNodes.emplace((size_t)currInd, (size_t)currInd);
				currNode = &currNode->m_subNodes[currInd];
			}
			//else err
		}
		//add pointer to final node
		currNode->m_pVal= &m_columns[valCol].m_cells[r].m_val;
	}

	//mark rootNode as active
	rootNode.m_ind = 1;
}