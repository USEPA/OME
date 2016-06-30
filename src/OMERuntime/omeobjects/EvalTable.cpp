#include "omeobjects/EvalTable.h"
#include "DataSrc.h"

EvalTable::EvalTable()
{
}

/** Simple Constructor.
	@param id Unique identifier of table.
	@param dims IndVec describing the dimensions of the table (from 1 to n dimensions)
	@param data DataArray containing actual values of table.
*/
EvalTable::EvalTable(const STLString & id, const IndVec & dims,const EvalTable::DataArray &data)
	:m_tblID(id),m_dims(dims),m_vals(data)
{
}

/** Detailed Constructor.
	@param id Unique id for Table.
	@param dims The dimensions of the table (1 or 2).
	@param filepath Path to CSV file containing table data.
	@param colName Column in file at filepath to use to populate 1D EvalTable.
	@param data The fallback value to load if filepath does not point to a valid file.
*/
EvalTable::EvalTable(const STLString & id, const IndVec & dims, const STLString & filepath, const STLString & colName, const DataArray &data)
	: m_tblID(id), m_dims(dims)
{
	if (!PopulateFromCSV(filepath, colName))
		m_vals = data;
}

/** Detailed Constructor.
@param id Unique id for Table.
@param dims The dimensions of the table (1 or 2).
@param filepath Path to CSV file containing table data.
@param colName Column in file at filepath to use to populate 1D EvalTable.
*/
EvalTable::EvalTable(const STLString & id, const IndVec & dims, const STLString & filepath, const STLString & colName)
	:m_tblID(id), m_dims(dims)
{
	PopulateFromCSV(filepath, colName);
}

/** Copy Constructor.
	@param et The EvalTable to copy.
*/
EvalTable::EvalTable(const EvalTable & et)	
{
	Duplicate(et);
}

/** TiXml Constructor.
	@param pCurrElem Element to extract definition from.
	@param tag xml-identifier associated with object.
	@param filename The name of the source file.
*/
EvalTable::EvalTable(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
{
	PopulateFromXML(pCurrElem,tag,filename);
}

/** Assignment operator.
	@param rhs The EvalTable to use in the assignment.
	@return Reference to this.
*/
EvalTable& EvalTable::operator=(const EvalTable & rhs)
{
	Duplicate(rhs);
	return *this;
}

/** Destructor. */
EvalTable::~EvalTable()	
{
}

/** Get value of 1D table for a given index.
	@param val If GetValue returns true, then val is set to the value at index ind.
	@param ind Index of value to retrieve.
	@return true if EvalTable is 1-dimensional and ind is inbounds; false otherwise.
*/
bool EvalTable::GetValue(OME_SCALAR & val,const unsigned int ind) const
{
	return GetValue(val,&ind,1);	
}

/** Get value of table for a given index set.
	@param val If GetValue returns true, then val is set to the value at location inds.
	@param inds An array of 1 or more indices used to retrieve a value.
	@param indCount the number of indices found in inds.
	@return true if EvalTable has indCount dimensions and all provided indices are inbounds; false otherwise.
*/
bool EvalTable::GetValue(OME_SCALAR & val,const unsigned int* inds, const unsigned int indCount) const
{
	bool ret=CheckBounds(inds,indCount);
	if(ret)
		val=ValForIndex(inds,indCount);
	return ret;
}

/** Set the value at a given index.
	@param val If SetValue returns true, the value at ind is set to val.
	@param ind The index of the value to set.
	@return true if EvalTable is 1-dimensional and ind is inbounds; false otherwise.
*/
bool EvalTable::SetValue(const OME_SCALAR & val,const unsigned int ind)
{
	return SetValue(val,&ind,1);
}

/** Set the value at the location indicated by a given index set.
	@param val If SetValue returns true, the value at inds is set to val.
	@param inds An array of 1 or more indices used to locate the location of the value.
	@param indCount the number of indices found in inds.
	@return true if EvalTable has indCount dimensions and all provided indices are inbounds; false otherwise.
*/
bool EvalTable::SetValue(const OME_SCALAR & val,const unsigned int* inds, const unsigned int indCount)
{
	bool ret=CheckBounds(inds,indCount);
	if(ret)
		SetValAtIndex(val,inds,indCount);
	return ret;
}

/** Query the number of dimensions.
@return The number of dimensions of the table. */
unsigned int EvalTable::GetDimensionCount() const
{ 
	return m_dims.size(); 
}

/** Query dimensions. 
	@return An IndVec object contain a list of extents for each of the EvalTable's dimensions.
*/
IndVec EvalTable::GetAllDims() const
{
	return m_dims;
}

/** Query all values.
	@return A DataArray object containing a 1-dimensional list of all values contained within the EvalTable.
*/
EvalTable::DataArray EvalTable::GetAllVals() const
{
	return m_vals;
}

STLString EvalTable::GetXMLRepresentation(const unsigned int indent,const unsigned int inc)
{
	oSTLSStream outStrm;
	outStrm<<STLString(indent,' ')<<'<'<<GetXMLTag()<<' ';
	StrArray attribs;
	GetXMLAttributes(attribs);

	std::for_each(attribs.begin(),attribs.end(),[&outStrm](STLString & s){ outStrm<<s<<' ';});

	outStrm<<"/>";

	return outStrm.str();
}

void EvalTable::GetXMLAttributes(StrArray & out) const
{
	oSTLSStream outStrm;
	//ensure that long decimals are written out
	//cout << outStrm.precision();
	outStrm.precision(OME_WRITE_PRECISION);

	//save id
	AddAttributeValue("id",m_tblID,out);

	if (!m_filepath.empty())
		AddAttributeValue("filename", m_filepath, out);

	if (!m_colName.empty())
		AddAttributeValue("column", m_colName, out);

	//encode dims
	unsigned int i;
	for(i=0; i<m_dims.size();i++)
		outStrm<<' '<<m_dims[i];
	AddAttributeValue("dimensions",outStrm.str().substr(1),out);

	//encode data
	outStrm.clear();
	outStrm.str(STLString());
	for(i=0; i<m_vals.size();i++)
		outStrm << ' ' << m_vals[i];

	AddAttributeValue("data",outStrm.str().substr(1),out);	
}

void EvalTable::GetXMLSubNodes(StrArray & out,const unsigned int indent,const unsigned int inc)
{

}

void EvalTable::GetXMLAttributeMap(XMLAttrVector & out)
{
	
}

int EvalTable::PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
{
	int ret=0;

	if(pCurrElem->Attribute("id"))
	{
		m_tblID=pCurrElem->Attribute("id");
		
		if(pCurrElem->Attribute("dimensions"))
		{
			STLString inString=pCurrElem->Attribute("dimensions");
			//parse dimensions
			size_t totEnt=1;
			unsigned int currDim;
			iSTLSStream inStrm(inString);
		
			while(!inStrm.eof())
			{
				inStrm>>currDim;
				totEnt*=currDim;
				m_dims.push_back(currDim);
			}

			//parse actual values
		
			m_vals.resize(totEnt,0.0);

			inStrm.clear();

			//if filename is present, load data from file.
			if (pCurrElem->Attribute("filename") && pCurrElem->Attribute("column"))
			{
				ret = PopulateFromCSV(pCurrElem->Attribute("filename"), pCurrElem->Attribute("column"));
			
			}

			//load default data (if filename isn't found)
			if (!ret && pCurrElem->Attribute("data"))
			{
				inString = pCurrElem->Attribute("data");
				inStrm.str(inString);

				for (size_t i = 0; i<m_vals.size() && !inStrm.eof(); i++)
					inStrm >> m_vals[i];

				ret = 1;
			}
		}
	}
	return ret;
}

/** Duplicate values from another EvalTable.
	@param et The Eval table to duplicate.
*/
void EvalTable::Duplicate(const EvalTable & et)
{
	m_vals=et.m_vals;
	m_dims=et.m_dims;
	m_tblID=et.m_tblID;
	m_filepath = et.m_filepath;
	m_colName = et.m_colName;
}

/** Determine if all the indices within an array are within the EvalTable's bounds.
	 @param inds Pointer to an array containing the indicies to test against.
	 @param indCount The number of entries in the array pointed to by inds.
	 @return true if indCount is the same as the number of dimensions in the EvalTable and each index is within the associated extents; false otherwise.
*/
bool EvalTable::CheckBounds(const unsigned int* inds, const unsigned int indCount) const
{
	//the use of unsigneds means we don't have to check the lower bounds

	//check dimensions first.
	bool ret=(indCount==m_dims.size());

	if(ret)
	{
		ret=true;
		for(unsigned int i=0; i< m_dims.size() && ret; i++)
			ret=m_dims[i]>inds[i];
	}

	return ret;
}


/** Retrieve a value at a given set of indices. Assumes CheckBounds() returns true. For bounds save version, see GetValue().
	@param inds Array of indices used to locate value.
	@param indCount The number of entries in inds.
	@return value located at position derived from inds.
	@sa GetValue
*/
OME_SCALAR EvalTable::ValForIndex(const unsigned int* inds, const unsigned int indCount) const
{
	//assume CheckBounds()==true
	return m_vals[DeriveIndex(inds,indCount,m_dims)];
}

/** Set a value at a given set of indices. Assumes CheckBounds() returns true. For bounds save version, see SetValue().
	@param val The value to assign to position derived from inds.
	@param inds Array of indices used to locate value.
	@param indCount The number of entries in inds.
	@sa SetValue
*/
void EvalTable::SetValAtIndex(const OME_SCALAR val,const unsigned int* inds, const unsigned int indCount)
{
	//assume CheckBounds()==true
	m_vals[DeriveIndex(inds,indCount,m_dims)]=val;
}

/** Populate the EvalTable using the provided filepath and column name.
	@param filepath The path of to the CSV file to retrieve value from.
	@param colname The name of the column containing the table values.
	@return true if CSV file was successfully loaded; false otherwise.
*/
bool EvalTable::PopulateFromCSV(const STLString & filepath, const STLString & colname)
{
	m_filepath = filepath;
	m_colName = colname;
	return PopulateFromCSV();
}

/** Populate the EvalTable using the filepath and column name previously assigned to it.
	@return true if CSV file was successfully loaded; false otherwise.
*/
bool EvalTable::PopulateFromCSV()
{
	bool ret = false;
	DataSrc ds;
	size_t len = 1;
	for (size_t i = 0; i < m_dims.size(); ++i)
		len *= m_dims[i];

	m_vals.resize(len);
	if (ds.ReadCSV(m_filepath))
	{
		int colID = ds.GetFieldCol(m_colName.c_str());

		if (colID >= 0)
		{
			for (size_t i = 0; i < ds.GetRowCount() && i < m_vals.size(); ++i)
				ds.GetNumVal(colID, i, m_vals[i]);
			ret = true;
		}
	}
	return ret;
}