#pragma once
#include "ISpatialDataProvider.h"
#include "DebugOutput.h"
#include <list>

class BaseSDPIterator;

/** Base provider that doesn't actually provide any data; instead, it provides a framework for subclasses to inform them if a capability
   is set without implementing the appropriate function.
*/
class __EXPORT__ BaseSpatialDataProvider :
	public ISpatialDataProvider
{
public:
	BaseSpatialDataProvider(void);
	~BaseSpatialDataProvider(void);

	  virtual int GetCapabilities() const;
 
      // basic load/save
      virtual int   Load( const OMEChar* filename,OMEParser* pPrsr=NULL );
      virtual int   Save( const OMEChar* filename, int flags );
 
      // basic info about coverage
      virtual void  GetExtents    ( OME_SCALAR &vxMin, OME_SCALAR &vxMax, OME_SCALAR &vyMin, OME_SCALAR &vyMax ) const;
      virtual int   GetRowCount   ( void ) const;
      virtual int   GetColumnCount( void ) const;
      virtual int   GetFieldCol   ( const OMEChar* label ) const;
 
      // spatial queries
      virtual bool  NextTo( int polyIndex, const OMEChar* query );
      virtual OME_SCALAR NextToArea( int polyIndex, const OMEChar* query );
      virtual bool  Within( int polyIndex, const OMEChar* query, OME_SCALAR distance );
      virtual OME_SCALAR WithinArea( int polyIndex, const OMEChar* query, OME_SCALAR distance );
 
      // grid methods
      virtual bool  GetCellSize ( OME_SCALAR &width, OME_SCALAR &height );
 
      // all types – set data methods   
      virtual void SetData( int rec, int col, OME_SCALAR value );
      virtual void SetData( int rec, int col, int value );
      virtual void SetData( int rec, int col, const OMEChar* value );
 
      // GetData(..) return true on success, false otherwise.
      virtual bool GetData( int rec, int col, OME_SCALAR &value );
      virtual bool GetData( int rec, int col, int &value );
      virtual bool GetData( int rec, int col, OMEChar* value, int maxChars );
      virtual bool GetData( int rec, int col, bool &value );
      
      virtual bool GetDataMinMax( int col, OME_SCALAR &minValue, OME_SCALAR &maxValue );  // note: col=-1 gets min/max for ALL columns
      
		//neighbors
		virtual int* GetNeighbors(const int & rec, int & count);

		virtual unsigned int GetNeighborCount(const int & rec) const;

		virtual unsigned int GetMaxNeighborCount() const;

		virtual int GetNeighborRecord(const int & rec, const int & neighbor);

      //Iterators
      virtual ISDPIterator& GetIterator(const ITR_DIR & kind = ITR_FORWARD);
      
	  virtual ISDPIterator& GetNextTo(int polyIndex, unsigned int* count=NULL);
      virtual ISDPIterator& GetWithin(int polyIndex, const OMEChar* query, OME_SCALAR distance);

	  virtual bool GetNeighborForDirection(const int & index, const GRID_DIR & dir, int & outInd);
	  virtual bool HasNeighborForDirection(const int & index, const GRID_DIR & dir);

      /** Used to assist developers in overriding the appropriate functions. Only prints error messages when compiled for debug builds.
         Basically, the BaseManager checks to see if a function is being called by subclass that needs to be overridden. If so, this function
         will print an error.
         @param flags The attribute flags to check for.
         @param errMsg The message to display if the flags are found.
      */
	  inline void DebugCheckSupport(int flags, const OMEChar* errMsg) const
	  {
#ifdef _DEBUG
		  //if one's compliment of capabilities AND flags resolves to anything other than 0, trigger error message
		  if(~GetCapabilities() & flags)
			  DBG_PRINT(errMsg);
#endif
	  }

private:
	friend class BaseSDPIterator;	//may be a good idea to friend your iterator with appropriate provider

	/**A list of allocated iterators. Each spatial data provider that supports iterators is expected to keep track of their own iterators, and mark
	any existing iterators as invalid on destruction or if the internal record set is changed. */
	std::list<BaseSDPIterator> m_itrs;
};

/** Base provider that doesn't actually provide any data; instead, it provides a framework for subclasses to inform them if a capability
   is set without implementing the appropriate function.
*/
class __EXPORT__ BaseSDPIterator : public ISDPIterator
{
public:
	/** Basic constructor.
      @param sdp Valid ISpatialDataProvider pointer.
   */
	BaseSDPIterator(ISpatialDataProvider* sdp);
   /** Destructor. */
	virtual ~BaseSDPIterator();
   
	virtual bool IsValid();
	
	//returns current record
	virtual int GetRecord(); 
	
	//Set value of current iterator object (may want to remove if we want read only)
	virtual void SetData( int col, OME_SCALAR value );
    virtual void SetData( int col, int value );
    virtual void SetData( int col, const OMEChar* value );
    
    //value accessors
    virtual bool GetData( int col, OME_SCALAR &value );
    virtual bool GetData( int col, int &value );
    virtual bool GetData( int col, OMEChar* value, int maxChars );
    virtual bool GetData( int col, bool &value );
    
    //step forward and backward(may only want to support unidirectional iterators)
    virtual bool Next();
    virtual bool Prev();

	virtual bool IsAtBeginning();
	virtual bool IsAtEnd();

	virtual void Reset();
	/**== operator needed for storage comparisons; for now, just compare addresses. */
	virtual bool operator==(const BaseSDPIterator & rhs)
	{
		return this==&rhs;
	}

	
   /** Used to assist developers in overriding the appropriate functions. Only prints error messages when compiled for debug builds.
         Basically, the BaseManager checks to see if a function is being called by subclass that needs to be overridden. If so, this function
         will print an error.
         @param flags The attribute flags to check for.
         @param errMsg The message to display if the flags are found.
      */
	  inline void DebugCheckSupport(int flags, const OMEChar* errMsg)
	  {
#ifdef _DEBUG
		 //check provider's abilities, if appropriate
		 BaseSpatialDataProvider* sdpCast=dynamic_cast<BaseSpatialDataProvider*>(m_provider);
		 if(sdpCast)
			sdpCast->DebugCheckSupport(flags,errMsg);
#endif
	  }
protected:
	ISpatialDataProvider* m_provider; ///< Spatial Data Provider associated with this iterator.
};