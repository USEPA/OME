#include "BaseSpatialDataProvider.h"


BaseSpatialDataProvider::BaseSpatialDataProvider(void)
{
}


BaseSpatialDataProvider::~BaseSpatialDataProvider(void)
{
	//normally, there would need to be a piece of code here that invalidates all existing
	//iterators for this provider.
}

int BaseSpatialDataProvider::GetCapabilities( void ) const
{
	//override with capabilities of this provider. See enums declared in ISpatialDataProvider.h
	return NONE;
}

int BaseSpatialDataProvider::Load( const char* filename,OMEParser* pPrsr )
{
	DebugCheckSupport(~NONE,"Load function implementation required for all providers.");
	//attempt to load the file at filename, and return result.
	//by default, we return LOAD_UNSUPPORTED_FILETYPE
	return LOAD_UNSUPPORTED_FILETYPE;
}

int BaseSpatialDataProvider::Save( const char* filename, int flags )
{
	DebugCheckSupport(~NONE,"Save function implementation required for all providers.");
	//attempt to save file to name with flags; return results.
	//by default, save is not supported.
	return SAVE_UNSUPPORTED;
}

void BaseSpatialDataProvider::GetExtents( OME_SCALAR &vxMin, OME_SCALAR &vxMax, OME_SCALAR &vyMin, OME_SCALAR &vyMax ) const
{
	DebugCheckSupport(~NONE,"GetExtents function implementation required for all providers.");

	//populate variables with appropriate 2D min and max values for loaded data
	//for now, return all 0s since base does not have any data.
	vxMin=0.0;
	vxMax=0.0;
	vyMin=0.0;
	vyMax=0.0;
}

int BaseSpatialDataProvider::GetRowCount() const
{
	DebugCheckSupport(~NONE,"GetRowCount function implementation required for all providers.");

	//return the number of rows/records in loaded data.
	return 0;
}

int BaseSpatialDataProvider::GetColumnCount() const
{
	DebugCheckSupport(~NONE,"GetColumnCount function implementation required for all providers.");

	//return the number of columns/fields per record.
	return 0;
}

int BaseSpatialDataProvider::GetFieldCol(const char* label) const
{
	DebugCheckSupport(~NONE,"GetFieldCol function implementation required for all providers.");

	//returns column number associated with label. If one does not exist, return -1
	return -1;
}

bool BaseSpatialDataProvider::NextTo( int polyIndex, const char* query )
{
	DebugCheckSupport(QUERY_SUPPORT,"NextTo function implementation required for query support.");
	//return true if query evaluates to true for any neighbors found at polyIndex; false otherwise
	return false;
}

OME_SCALAR BaseSpatialDataProvider::NextToArea( int polyIndex, const char* query )
{
	DebugCheckSupport(QUERY_SUPPORT,"NextToArea function implementation required for query support.");
	//return the area for all neighbors to polygon at polyIndex that match query conditions.
	return 0.0;
}

bool BaseSpatialDataProvider::Within( int polyIndex, const char* query, OME_SCALAR distance )
{
	DebugCheckSupport(QUERY_SUPPORT,"Within function implementation required for query support.");

	//return true if any polygons within distance of the polygon at polyIndex matches query; false otherwise
	return false;
}

OME_SCALAR BaseSpatialDataProvider::WithinArea( int polyIndex, const char* query, OME_SCALAR distance )
{
	DebugCheckSupport(QUERY_SUPPORT,"WithinArea function implementation required for query support.");
	//return total area of polygons that match query and are within distance of polygon at polyIndex
	return 0.0;
}

bool BaseSpatialDataProvider::GetCellSize( OME_SCALAR &width, OME_SCALAR &height )
{
	DebugCheckSupport(GRID_SUPPORT,"GetCellSize function implementation required for grid support.");
	//populate appropriate variables with cell dimensions used in grid representation.
	width=0.0;
	height=0.0;
	return false;
}

void BaseSpatialDataProvider::SetData( int rec, int col, OME_SCALAR value )
{
	DebugCheckSupport(WRITE_ACCESS,"SetData function implementation required for write/rw support.");
	//set col in record rec to value.
	//base implementation does nothing.
}

void BaseSpatialDataProvider::SetData( int rec, int col, int value )
{
	DebugCheckSupport(WRITE_ACCESS,"SetData function implementation required for write/rw support.");
	//set col in record rec to value.
	//base implementation does nothing.
}

void BaseSpatialDataProvider::SetData( int rec, int col, const OMEChar* value )
{
	DebugCheckSupport(WRITE_ACCESS,"SetData function implementation required for write/rw support.");
	//set col in record rec to value.
	//base implementation does nothing.
}
      
bool BaseSpatialDataProvider::GetData( int rec, int col, OME_SCALAR &value )
{
	DebugCheckSupport(READ_ACCESS,"GetData function implementation required for read/rw support.");
	//get value from col in rec and place in value, if possible. return true if successful, false otherwise
	return false;
}

bool BaseSpatialDataProvider::GetData( int rec, int col, int &value )
{
	DebugCheckSupport(READ_ACCESS,"GetData function implementation required for read/rw support.");
	//get value from col in rec and place in value, if possible. return true if successful, false otherwise
	return false;
}

bool BaseSpatialDataProvider::GetData( int rec, int col, OMEChar* value, int maxChars  )
{
	DebugCheckSupport(READ_ACCESS,"GetData function implementation required for read/rw support.");
	//get value from col in rec and place in value, if possible. return true if successful, false otherwise
	return false;
}

bool BaseSpatialDataProvider::GetData( int rec, int col, bool &value )
{
	DebugCheckSupport(READ_ACCESS,"GetData function implementation required for read/rw support.");
	//get value from col in rec and place in value, if possible. return true if successful, false otherwise
	return false;
}

bool BaseSpatialDataProvider::GetDataMinMax( int col, OME_SCALAR &minValue, OME_SCALAR &maxValue )
{
	DebugCheckSupport(READ_ACCESS,"GetDataMinMax function implementation required for read/rw support.");
	//get value from col in rec and place in minValue and maxValue, if possible. return true if successful, false otherwise
	return false;
}

int* BaseSpatialDataProvider::GetNeighbors(const int & rec, int & count)
{
	DebugCheckSupport(NEIGHBOR_SUPPORT,"GetNeighbors function implementation required for neighbor support.");
	count=-1;
	return NULL;
}

unsigned int BaseSpatialDataProvider::GetNeighborCount(const int & rec) const
{
	DebugCheckSupport(NEIGHBOR_SUPPORT, "GetNeighborCount function implementation required for neighbor support.");
	return 0;
}

unsigned int BaseSpatialDataProvider::GetMaxNeighborCount() const
{
	//non-optimal method; best to override in subclass.
	unsigned int ret = 0;
	unsigned int nCount;
	if (GetCapabilities() & NEIGHBOR_SUPPORT)
	{
		for (int i = 0; i < GetRowCount(); ++i)
		{
			nCount = GetNeighborCount(i);
			if (nCount>ret)
				ret = nCount;
		}
	}
	
	return ret;
}

int BaseSpatialDataProvider::GetNeighborRecord(const int & rect, const int & neighbor)
{
	DebugCheckSupport(NEIGHBOR_SUPPORT, "GetNeighborRecord function implementation required for neighbor support.");
	return -1;
}

ISDPIterator& BaseSpatialDataProvider::GetIterator(const ITR_DIR & kind)
{
   int flagCheck=ITR_SUPPORT;
   if(kind == ITR_BACKWARD)
      flagCheck |= BI_ITR_SUPPORT;
   
   DebugCheckSupport(flagCheck,"GetIterator must implement Iterator support. Additionally bidirectional iterator "
                               "support must be implemented for kind ITR_BACKWARD.");
   //return an iterator to the first record that iterates over entire record set.
	m_itrs.push_back(BaseSDPIterator(this));
	return m_itrs.back();
}

ISDPIterator& BaseSpatialDataProvider::GetNextTo(int polyIndex, unsigned int* count)
{
	//return an iterator that points to the beginning of the list of neighbor records to 
	//polyIndex that satisfy query. If none, return invalid iterator
	
	DebugCheckSupport(QUERY_SUPPORT | NEIGHBOR_SUPPORT | ITR_SUPPORT,
		"GetNextTo must be implemented if queries, neighbors, and iterators are all supported");
	m_itrs.push_back(BaseSDPIterator(this));
	return m_itrs.back();
}

ISDPIterator& BaseSpatialDataProvider::GetWithin(int polyIndex, const char* query, OME_SCALAR distance)
{
	//return an iterator that points to the beginning of the list of records that 
	//satisfy query and are within distance to the poly at polyIndex.
	DebugCheckSupport(QUERY_SUPPORT | ITR_SUPPORT,
		"GetNextTo must be implemented if queries and iterators are both supported");
	m_itrs.push_back(BaseSDPIterator(this));
	return m_itrs.back();
}

bool BaseSpatialDataProvider::GetNeighborForDirection(const int & index, const GRID_DIR & dir, int & outInd)
{
	DebugCheckSupport(GRID_SUPPORT | NEIGHBOR_SUPPORT,
		"GetNeighborForDirection must be implemented if Grid orientation and neighbors are both supported");
	DebugCheckSupport(HEX_SUPPORT | NEIGHBOR_SUPPORT,
		"GetNeighborForDirection must be implemented if Hex orientation and neighbors are both supported");
	return false;
}

bool BaseSpatialDataProvider::HasNeighborForDirection(const int & index, const GRID_DIR & dir)
{
	DebugCheckSupport(GRID_SUPPORT | NEIGHBOR_SUPPORT,
		"GetNeighborForDirection must be implemented if Grid orientation and neighbors are both supported");
	DebugCheckSupport(HEX_SUPPORT | NEIGHBOR_SUPPORT,
		"GetNeighborForDirection must be implemented if Hex orientation and neighbors are both supported");
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////

BaseSDPIterator::BaseSDPIterator(ISpatialDataProvider* sdp)
{
	//capture pointer to ISpatialDataProvider-compatible class that spawned iterator
	m_provider=sdp;
}

BaseSDPIterator::~BaseSDPIterator()
{
	//code template; constructor is empty
}

bool BaseSDPIterator::IsValid()
{
	//normally returns whether the iterator is still valid. If the associated data of the associated iterator changes,
	//then the iterator should be invalidated.
	return false;
}

int BaseSDPIterator::GetRecord()
{
	//returns index to current record. NO_RECORD (-1) should be returned if iterator is at very end or is invalid
	return NO_RECORD;
}
	
void BaseSDPIterator::SetData( int col, OME_SCALAR value )
{
	//SetData sets the col of the current record to value, assuming the iterator is presently pointing to a valid record.
	DebugCheckSupport(WRITE_ACCESS,"SetData for iterator must be implemented for write access iterator support");
}

void BaseSDPIterator::SetData( int col, int value )
{
	//SetData sets the col of the current record to value, assuming the iterator is presently pointing to a valid record.
	DebugCheckSupport(WRITE_ACCESS,"SetData for iterator must be implemented for write access iterator support");
}

void BaseSDPIterator::SetData( int col, const OMEChar* value )
{
	//SetData sets the col of the current record to value, assuming the iterator is presently pointing to a valid record.
	DebugCheckSupport(WRITE_ACCESS,"SetData for iterator must be implemented for write access iterator support");
}

bool BaseSDPIterator::GetData( int col, OME_SCALAR &value )
{
	//Get data should attempt to retrieve data for the col in the current record. If successful, return true and place data in value;
	//otherwise return false
	DebugCheckSupport(READ_ACCESS,"GetData for iterator must be implemented for Read access iterator support");
	return false;
}

bool BaseSDPIterator::GetData( int col, int &value )
{
	//Get data should attempt to retrieve data for the col in the current record. If successful, return true and place data in value;
	//otherwise return false
	DebugCheckSupport(READ_ACCESS,"GetData for iterator must be implemented for Read access iterator support");
	return false;
}

bool BaseSDPIterator::GetData( int col, OMEChar* value, int maxChars )
{
	//Get data should attempt to retrieve data for the col in the current record. If successful, return true and place data in value;
	//otherwise return false
	DebugCheckSupport(READ_ACCESS,"GetData for iterator must be implemented for Read access iterator support");
	return false;
}

bool BaseSDPIterator::GetData( int col, bool &value )
{
	//Get data should attempt to retrieve data for the col in the current record. If successful, return true and place data in value;
	//otherwise return false
	DebugCheckSupport(READ_ACCESS,"GetData for iterator must be implemented for Read access iterator support");
	return false;
}

    
    //step forward and backward(may only want to support unidirectional iterators)
bool BaseSDPIterator::Next()
{
	//increments the iterator by one, if not at end of list. returns true if increment occurs.
	//interface class implements post ++ that calls this function
	DebugCheckSupport(ITR_SUPPORT,"Next must be implemented for iterators");
	return false;
}

bool BaseSDPIterator::Prev()
{
	//decrements the iterator by one, if not at beginning of list. returns true if decrement occurs.
	//interface class implements post -- that calls this function
	DebugCheckSupport(BI_ITR_SUPPORT,"Prev must be implemented for bidirectional iterators");
	return false;
}

bool BaseSDPIterator::IsAtBeginning()
{
	//returns true if the iterator is at the beginning of its list; false otherwise
	DebugCheckSupport(ITR_SUPPORT,"IsAtBeginning must be implemented for iterators");
	return false;
}

bool BaseSDPIterator::IsAtEnd()
{
	//returns true if the iterator is at the end of its list; false otherwise.
	//NOTE: currently the "End" item should NOT be treated as an invalid sentinel value, as it is in STL.
	///question should SDPiterators use sentinels for the end?
	DebugCheckSupport(ITR_SUPPORT,"IsAtEnd must be implemented for iterators");
	return false;
}

void BaseSDPIterator::Reset()
{
	DBG_PRINT("Reset must be implemented ofr iterator support");
}