#pragma once
#include "ISpatialDataProvider.h"

#include <Maplayer.h>
#include <QueryEngine.h>
#include <PtrArray.h>

class EnvSDPIterator;
class OMEParser;

#define NEIGH_CACHE_SIZE 1024

/** Envision-based backend for SDP interface. */
class EnvSpatialDataProvider : public ISpatialDataProvider
{
public:
   EnvSpatialDataProvider( MapLayer *pIduLayer );
   ~EnvSpatialDataProvider(void);

   virtual int GetCapabilities(void) const;
 
   // basic load/save
   virtual int   Load( const OMEChar* filename,OMEParser* pPrsr=NULL );
   virtual int   Save( const OMEChar* filename, int flags );
 
   // basic info about coverage
   virtual void  GetExtents(OME_SCALAR &vxMin, OME_SCALAR &vxMax, OME_SCALAR &vyMin, OME_SCALAR &vyMax) const;
   virtual int   GetRowCount   ( void ) const;
   virtual int   GetColumnCount( void ) const;
   virtual int   GetFieldCol   ( const OMEChar* label ) const;
	virtual int AddFieldCol( const OMEChar* label, const OME_SCALAR initVal=0.0);
	virtual int AddFieldCol(const OMEChar* label, const OME_SCALAR* vals, const OME_SCALAR & count);
	virtual int AddFieldCol(const OMEChar* label, ValueArchive* pArch, const size_t & start, const OME_SCALAR & count);

   // spatial queries
   virtual bool  NextTo( int polyIndex, const OMEChar* query );
   virtual OME_SCALAR NextToArea( int polyIndex, const OMEChar* query );
   virtual bool  Within( int polyIndex, const OMEChar* query, OME_SCALAR distance );
   virtual OME_SCALAR WithinArea( int polyIndex, const OMEChar* query, OME_SCALAR distance );
 
   // grid methods
   virtual bool  GetCellSize ( OME_SCALAR &width, OME_SCALAR &height );
 
   // all types – set data methods   
   virtual void SetData( int rec, int col, OME_SCALAR value );
   virtual void SetData( int rec, int col, OME_SCALAR value );
   virtual void SetData( int rec, int col, int value );
   virtual void SetData( int rec, int col, const OMEChar* value );
 
   // GetData(..) return true on success, false otherwise.
   virtual bool GetData( int rec, int col, OME_SCALAR &value );
   virtual bool GetData( int rec, int col, OME_SCALAR &value);
   virtual bool GetData( int rec, int col, int &value );
   virtual bool GetData( int rec, int col, OMEChar *value, int maxChars );
   virtual bool GetData( int rec, int col, bool &value );
   
   virtual bool GetDataMinMax( int col, OME_SCALAR &minValue, OME_SCALAR &maxValue );  // note: col=-1 gets min/max for ALL columns
   virtual int* GetNeighbors(const int & rec, int & count); 
   virtual unsigned int GetNeighborCount(const int & rec) const;
   virtual int GetNeighborRecord(const int & rec, const int & neighbor);

   //Iterators
   virtual ISDPIterator& GetIterator(const ITR_DIR & kind = ITR_FORWARD);
 
   
   virtual ISDPIterator& GetNextTo(int polyIndex, unsigned int* count=NULL);
   virtual ISDPIterator& GetWithin(int polyIndex, const OMEChar* query, OME_SCALAR distance);

	//EnvSPD-specific
	bool GetMapLayerReadOnly() const;
	void SetMapLayerReadOnly(bool ro);

private:
   friend class EnvSDPIterator;   //may be a good idea to friend your iterator with appropriate provider

  	//each spatial data provider that supports iterators is expected to keep track of their own iterators, and mark
	//any existing iterators as invalid on destruction or if the internal record set is changed.
	PtrArray<ISDPIterator> m_iteratorArray; ///< storage for active iterators.

   MapLayer *m_pIduLayer;  ///< Pointer to referenced Envision MapLayer.
   QueryEngine *m_pQE;	   ///< Pointer to Envision query engine.

   int m_colArea;   ///< index of column that holds area information; assumes AREA for now.
   
   
   //cache for neighbor lookup
   int m_lastNeighborRecord;			   ///< Last record that neighbors were calculated for.
   int m_neighborCache[NEIGH_CACHE_SIZE];  ///< Cache of neighbor indicies.
   OME_SCALAR m_lastNeighborDist;		   ///< The distance used for the previous neighbor query.
   unsigned int m_nCount;				   ///< The count of neighors in m_neighborCache.
   
   /** Populate neighbor cache with data for a specific record.
		@param currInd The index of the record to cache.
		@param dist The search distance for neighbors.
   */
   inline void RefreshNeighborCache(const int & currInd,const OME_SCALAR & dist=0.0f)
      {
      if (m_lastNeighborRecord != currInd || m_lastNeighborDist!=dist)
         {
         Poly *pPoly = m_pIduLayer->GetPolygon(currInd);
         m_nCount = m_pIduLayer->GetNearbyPolys(pPoly, m_neighborCache, NULL, NEIGH_CACHE_SIZE, dist);
         m_lastNeighborRecord = currInd;
         m_lastNeighborDist = dist;
         }
      }

   /** Convert a string into an Envision-friendly identifier.
	@param oldLabel The label to be converted.
	@param newLabel buffer to store the new label on return.
	@param limit The max size of the newLabel buffer.
   */
   inline void EnvisionifyLabel(const OMEChar* oldLabel, OMEChar* newLabel, const size_t & limit)
      {
      //convert label here, according to Envision rules
      //(this will prevent false negatives)
      unsigned int i;
      for (i = 0; i<limit && oldLabel[i] != '\0'; ++i)
         {
         if (oldLabel[i] != '_')
            {
            if (oldLabel[i] < 0 || oldLabel[i] > 255 || !isalnum(oldLabel[i]))
               {
               //can add a flag here to let user know that the column name was modified
               newLabel[i] = '_';
               }
            else
               newLabel[i] = oldLabel[i];
            }
         else
            newLabel[i] = oldLabel[i];
         }
      
      //terminate newLabel
      newLabel[(i < limit) ? i : limit - 1] = '\0';
      }
   
};

////////////////////////////////////////////////////////////////////////////////////////////////
/** Iterator for EnvSpatialDataProvider*/
class EnvSDPIterator : public ISDPIterator
{
friend class EnvSpatialDataProvider;

public:
	/** Simple Constructor.
		@param sdp Pointer to the parent spatial data provider.
	*/
   EnvSDPIterator(ISpatialDataProvider* sdp) : m_currentRecord( NO_RECORD ), m_provider( sdp ), m_isValid( true ) { }
   
   ~EnvSDPIterator() { }

   //returns index to current record. NO_RECORD (-1) should be returned if iterator is at very end or is invalid
   virtual bool IsValid() { return m_isValid; }
   
   //returns current record
   virtual int GetRecord() { return m_currentRecord; }
   
   //Set value of current iterator object (may want to remove if we want read only)
   virtual void SetData( int col, OME_SCALAR value )  { if ( m_currentRecord != NO_RECORD ) m_provider->SetData( m_currentRecord, col, value ); }
   virtual void SetData( int col, int value )     { if ( m_currentRecord != NO_RECORD ) m_provider->SetData( m_currentRecord, col, value ); }
   virtual void SetData(int col, const OMEChar* value) { if (m_currentRecord != NO_RECORD) m_provider->SetData(m_currentRecord, col, value); }
    
   //value accessors
   virtual bool GetData( int col, OME_SCALAR &value)  { if ( m_currentRecord != NO_RECORD ) return false; return m_provider->GetData( m_currentRecord, col, value ); }
   virtual bool GetData( int col, int &value )    { if ( m_currentRecord != NO_RECORD ) return false; return m_provider->GetData( m_currentRecord, col, value ); }
   virtual bool GetData( int col,OMEChar *value, int maxChars ) { if ( m_currentRecord != NO_RECORD ) return false; m_provider->GetData( m_currentRecord, col, value, maxChars ); return true; }
   virtual bool GetData( int col, bool &value )   { if ( m_currentRecord != NO_RECORD ) return false; return m_provider->GetData( m_currentRecord, col, value ); }
   
   //step forward and backward(may only want to support unidirectional iterators)
   virtual bool Next() { if ( m_currentRecord < m_provider->GetRowCount()-1 ) { m_currentRecord++; return true; } return false; }
   virtual bool Prev() { if ( m_currentRecord > 0 ) { m_currentRecord--; return true; } return false; }

   virtual bool IsAtBeginning() { return m_currentRecord == 0; }
   virtual bool IsAtEnd() { return m_currentRecord == m_provider->GetRowCount()-1; }

   virtual void Dispose();

   virtual void Reset() { m_currentRecord=0; };

   //== operator needed for storage comparisons; for now, just compare addresses
   /** Comparison operator.
		@param rhs The iterator to compare against.
		@return true if the Iterators are the same (have the same address); false otherwise.
	*/
   virtual bool operator==(const EnvSDPIterator & rhs) { return this==&rhs; }

protected:
   ISpatialDataProvider* m_provider; ///< Pointer to the source provider.
   bool m_isValid;					 ///< True if the iterator is presently valid; false otherwise.
   int  m_currentRecord;			 ///< Index to the current record position.
};

/** Iterator for handling Query results. */
class EnvSDPQueryIterator : public EnvSDPIterator
{
public:
   //returns true if the iterator references a valid value, false otherwise
	/** Simple constructor.
		@param sdp Pointer to the originating SDP.
	*/
   EnvSDPQueryIterator(ISpatialDataProvider *sdp) : EnvSDPIterator( sdp ), m_currentIndex( NO_RECORD ) { }
   ~EnvSDPQueryIterator() { }

   //returns current record
   virtual int GetRecord() { return m_currentIndex; }
    
   //step forward and backward(may only want to support unidirectional iterators)
   virtual bool Next();
   virtual bool Prev();

   virtual bool IsAtBeginning() { return m_currentIndex == 0; }
   virtual bool IsAtEnd() { return m_currentIndex == (int) m_indexArray.GetSize()-1; }
   virtual void Reset() { m_currentIndex= m_indexArray.GetCount()>0 ? 0 : -1;}

   /** Add index to list of matched indices.*/
   void AddIndex( int index ) { m_indexArray.Add( index ); }

protected:
   CArray< int > m_indexArray;    ///< Array of indices into map layer satisfied by the query.
   int m_currentIndex;            ///< this is the index into the currently selected index.
};


inline
bool EnvSDPQueryIterator::Next()
   {
   if ( m_isValid ) 
      {
	  if(m_isValid = ++m_currentIndex < (int)m_indexArray.GetSize())
		m_currentRecord = m_indexArray[ m_currentIndex ]; 
      } 
   
   return m_isValid;
   }


inline 
bool EnvSDPQueryIterator::Prev() 
   {
	if (m_isValid)
	{
		if (m_isValid = --m_currentIndex>=0)
			m_currentRecord = m_indexArray[m_currentIndex];
	}
	return m_isValid;
   }
