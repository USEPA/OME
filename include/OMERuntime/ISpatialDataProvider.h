#pragma once
#include "OMEDefines.h"
#include "OMEParser.h"
#include "ValueArchive.h"

//bit-flag representation of capabilities (easy to expand)

#define NO_RECORD -1

/** \defgroup SDPFlags SpatialDataProvider Flags
   Predefined flags for use with Spatial Data Providers

   @{
*/


/** \name Attribute flags
   Flags which indicate the default capabilities of a Spatial Data Provider.
 @{
 */

enum
{
	NONE=				   0x0000,     ///< No attribute flags set.
	POINT_SUPPORT=		0x0001,     ///< Point data sources supported.
	HEX_SUPPORT=		0x0002,     ///< Hex grid data sources supported.
	GRID_SUPPORT=		0x0004,     ///< Square grid data sources supported.
    POLY_SUPPORT=		0x0008,     ///< Polygon-based data sources supported.
	QUERY_SUPPORT=		0x0010,     ///< Querying syntax supported.
	NEIGHBOR_SUPPORT=	0x0040,     ///< Neighbor data retrieval supported.
	ITR_SUPPORT=		0x0080,     ///< Result iterators supported.
//	REV_ITR_SUPPORT=	0x0040,
	BI_ITR_SUPPORT=	0x0180,     ///< Bidirectional iterators supported. This flag also implies ITR_SUPPORT.
	READ_ACCESS=		0x0200,     ///< Read Access supported.
	WRITE_ACCESS=		0x0400,     ///< Write Access supported.
	RW_ACCESS=			0x0600      ///< Read and Write access supported. this is equivalient to READ_ACCESS | WRITE_ACCESS
};
///@}

/** \name Loading Flags
   Flags which indicate the success or failure of a file loading operation.

   @{
*/
enum 
{
	LOAD_SUCCESS= 0,              ///< File loading was successful.
	LOAD_UNSUPPORTED_FILETYPE,    ///< File type was not recognized; file loading failed.
	LOAD_READ_FAIL						///< File reading failed for some reason.
};
///@}

/** \name Save Flags
   Flags which indicate success or failure of a file writing operation.
   @{
*/
enum
{
	SAVE_SUCCESS=0,         ///< File writing was successful.
	SAVE_UNSUPPORTED,       ///< File writing is unsupported.
	SAVE_WRITE_FAIL         ///< File writing failed for some reason.
};
///@}

//DO NOT REARRANGE ORDER!
/** Stand in for cardinal directions; used for grid formats*/
enum GRID_DIR
{
	GD_N=0,	 ///< North direction.
	GD_S,	 ///< South direction.
	GD_E,	 ///< East direction.
	GD_W,	 ///< West direction.
	GD_NE,	 ///< Northeast direction.
	GD_SE,	 ///< Southeast direction.
	GD_SW,	 ///< Southwest direction.
	GD_NW	 ///< Northwest direction.

};

///@}

/** Virtual interface for Iterators that are coupled with classes that inherit from ISpatialDataProvider. */
class ISDPIterator
{
public:

   /** Virtual destructor for iterators. */
   virtual ~ISDPIterator() {}

   /**
	 @return true if the iterator references a valid value, false otherwise.
    */
	virtual bool IsValid()=0;
	
	/**
      @return Index of current record. (-1 if iterator is invalid)
   */
	virtual int GetRecord()=0; 
	
	/** Set value of a given column for the current record.
      @param col The column in which to insert the new value.
      @param value The Scalar value to insert.
   */
	virtual void SetData( int col, OME_SCALAR value ) = 0;
    /** Set value of a given column for the current record.
      @param col The column in which to insert the new value.
      @param value The int value to insert.
   */
    virtual void SetData( int col, int value ) = 0;
    /** Set value of a given column for the current record.
      @param col The column in which to insert the new value.
      @param value The raw string value to insert.
   */
    virtual void SetData( int col, const OMEChar* value ) = 0;
    
    //value accessors
    /** Get the value for a given column in the current record as a OME_SCALAR value.
         @param col The column to retrieve data from.
         @param value Scalar variable to store value, if retrieval is successful.
         @return true if retrieval is successful, false otherwise.
    */
    virtual bool GetData( int col, OME_SCALAR &value ) = 0;

    /** Get the value for a given column in the current record as an int value.
         @param col The column to retrieve data from.
         @param value Int variable to store value, if retrieval is successful.
         @return true if retrieval is successful, false otherwise.
    */
    virtual bool GetData( int col, int &value ) = 0;
       /** Get the value for a given column in the current record as a string value.
         @param col The column to retrieve data from.
         @param value string variable to store value, if retrieval is successful.
         @param maxChars The maximum number of characters that value is allocated to hold.
         @return true if retrieval is successful, false otherwise.
    */
    virtual bool GetData( int col, OMEChar* value, int maxChars ) = 0;
       /** Get the value for a given column in the current record as a bool value.
         @param col The column to retrieve data from.
         @param value Bool variable to store value, if retrieval is successful.
         @return true if retrieval is successful, false otherwise.
    */
    virtual bool GetData( int col, bool &value ) = 0;
    
    /** Increment the iterator by one record, if possible.
         @return true if increment is successful; false otherwise.
    */
    virtual bool Next()=0;
    /** Decrement the iterator by one record, if possible.
         @return true if decrement is successful; false otherwise.
    */
    virtual bool Prev()=0;
    
   /** @return true if iterator is pointing to the first record in its list; false otherwise.
   */
	virtual bool IsAtBeginning()=0;
   /** @return true if iterator is pointing to the last record in its list; false otherwise.
   */
	virtual bool IsAtEnd()=0;

	/** Reset the iterator back to the beginning of the collection. */
	virtual void Reset() = 0;

   /** Alias for Next.
         @return true if increment is successful; false otherwise.
         @sa Next
    */
    bool operator++() { return Next(); };
      /** Alias for Prev.
         @return true if decrement is successful; false otherwise.
         @sa Prev
    */
    bool operator--() { return Prev(); };

};

/** Interface for SpatialDataProviders to be used with modeling environment. */
class ISpatialDataProvider
{
   public:

      /**
         \addtogroup SDPFlags
         @{
         \name Iterator Type Flags
         Flags for use with GetIterator() to determine which type of iterator to retrieve.
         @{
      */
      enum ITR_DIR
      {
         ITR_FORWARD,     ///< Iterator that starts at the beginning of the list of records and iterates all the way through.
         ITR_BACKWARD     ///< Iterator that starts at the end of the list of records and iterates all the way through.
      };
      ///@}
      ///@}

      /** Virtual Destructor. */
      virtual ~ISpatialDataProvider() {}

      /** Queries the SpatialDataProvider for its supported capabilities.
         @return value containing attribute flags for supported functionality.
         @sa SDPFlags
      */
      virtual int   GetCapabilities( void ) const = 0;

      /** Load a file.
         Attempts to load a file.
         @param filename The file to load.
			@param pPrsr Optional pointer to expression parser. Not all spatial providers will need this.
         @return Flag indicating success or failure of file load.
         @sa SDPFlags
      */
      virtual int   Load( const OMEChar* filename,OMEParser* pPrsr=NULL ) = 0;
      /** Save a file.
         Attempts to save a file to the provided destination.
         @param filename Destination of saved file.
         @param flags DataProvider-specified flags for save options.
         @return Flag indicating result of save operation.
         @sa SDPFlags
      */
      virtual int   Save( const OMEChar* filename, int flags ) = 0;
 
      // basic info about coverage
      /** Get data extents, if applicable.
         @param vxMin SCALAR value to store mininum x-value.
         @param vxMax SCALAR value to store maximum x-value.
         @param vyMin SCALAR value to store mininum y-value.
         @param vyMax SCALAR value to store maximum y-value.
      */
	  virtual void GetExtents(OME_SCALAR &vxMin, OME_SCALAR &vxMax, OME_SCALAR &vyMin, OME_SCALAR &vyMax) const = 0;
      /** @return The number of rows in the stored data.
      */
      virtual int GetRowCount   ( void ) const = 0;
      /** @return The number of columns used to slice up each record.
      */
      virtual int GetColumnCount( void ) const = 0;
      /** Look up a column index by name.
         @param label Raw string containing the name of the desired column.
         @return the index of the column with a matching name, or -1 if the column is not found.
      */
      virtual int   GetFieldCol   ( const OMEChar* label ) const = 0;
 
	  /** Add a column for storing data. 
		@param label The label of the new column.
		@param initVal The initial value for each record.
		@return The index if creation was successful, or -1 if creating the new column failed.
	  */
	  virtual int AddFieldCol( const OMEChar* label, const OME_SCALAR initVal=0.0) = 0;
	  /** Add a column for storing data.
	  @param label The label of the new column.
	  @param vals Pointer to array containing the initial values for each record.
	  @param count The length of the array pointed to by vals and the total number of records to initialize.
	  @return The index if creation was successful, or -1 if creating the new column failed.
	  */
     virtual int AddFieldCol(const OMEChar* label, const OME_SCALAR* vals, const OME_SCALAR & count) = 0;
	 /** Add a column for storing data.
	 @param label The label of the new column.
	 @param pArch Pointer to ValueArchive containing the initial values.
	 @param start The first index in pArch of the first record value.
	 @param count The length of the values in pArch and the total number of records to initialize.
	 @return The index if creation was successful, or -1 if creating the new column failed.
	 */
	 virtual int AddFieldCol(const OMEChar* label, ValueArchive* pArch, const size_t & start, const OME_SCALAR & count)=0;

      // spatial queries

      /**
         Check to see if one or more neigbhors exist that match query.
         @param polyIndex Index of polygon to check from.
         @param query Evaluation statement to use in query.
         @return true if neighbor matches query, false otherwise.
      */
      virtual bool  NextTo( int polyIndex, const OMEChar* query ) = 0;
      /**
         Find the total area of all neighbors that match the query.
         @param polyIndex Index of polygon to check from.
         @param query Evaluation statement to use in query.
         @return The total area of all neighboring polygons that match the query.
      */
      virtual OME_SCALAR NextToArea( int polyIndex, const OMEChar* query ) = 0;
      /**
         Check to see if any polgons within a given distance match the provided query.
         @param polyIndex Index of polygon to check from.
         @param query Evaluation statement to use in query.
         @param distance Distance from source polygon within which polygons are evaluated.
         @return true if any evaluated polygons match query, false otherwise.
      */
      virtual bool  Within( int polyIndex, const OMEChar* query, OME_SCALAR distance ) = 0;
       /**
         Find the total area of polgons within a given distance that match the provided query.
         @param polyIndex Index of polygon to check from.
         @param query Evaluation statement to use in query.
         @param distance Distance from source polygon within which polygons are evaluated.
         @return The total area of all evaluated polygons that match the query.
      */
      virtual OME_SCALAR WithinArea( int polyIndex, const OMEChar* query, OME_SCALAR distance ) = 0;
 
      // grid methods
      /** Find the dimensions of grid cells, if applicable.
         @param width Variable to hold the width of a grid cell.
         @param height Variable to hold the height of a grid cell.
         @return true if cell size lookup was successful, false otherwise.
      */
      virtual bool  GetCellSize ( OME_SCALAR &width, OME_SCALAR &height ) = 0;
 
      // all types – set data methods

      /** Set value of a given column for a given record.
         @param rec The record to modify.
         @param col The column in which to insert the new value.
         @param value The OME_SCALAR value to insert.
      */
      virtual void SetData( int rec, int col, OME_SCALAR value ) = 0;

      /** Set value of a given column for a given record.
         @param rec The record to modify.
         @param col The column in which to insert the new value.
         @param value The int value to insert.
      */
      virtual void SetData( int rec, int col, int value ) = 0;
      /** Set value of a given column for a given record.
         @param rec The record to modify.
         @param col The column in which to insert the new value.
         @param value The string value to insert.
      */
      virtual void SetData( int rec, int col, const OMEChar* value ) = 0;
 
      // GetData(..) return true on success, false otherwise.
      /** Get the value for a given column in a given record as a OME_SCALAR value.
         @param rec The record to retrieve the data from.
         @param col The column to retrieve data from.
         @param value OME_SCALAR variable to store value, if retrieval is successful.
         @return true if retrieval is successful, false otherwise.
    */
      virtual bool GetData( int rec, int col, OME_SCALAR &value ) = 0;
      /** Get the value for a given column in a given record as a int value.
         @param rec The record to retrieve the data from.
         @param col The column to retrieve data from.
         @param value Int variable to store value, if retrieval is successful.
         @return true if retrieval is successful, false otherwise.
    */
      virtual bool GetData( int rec, int col, int &value ) = 0;
      /** Get the value for a given column in a given record as a string value.
         @param rec The record to retrieve the data from.
         @param col The column to retrieve data from.
         @param value String variable to store value, if retrieval is successful.
         @param maxChars The maximum number of characters that value is allocated to hold.
         @return true if retrieval is successful, false otherwise.
    */
      virtual bool GetData( int rec, int col, OMEChar* value, int maxChars ) = 0;
       /** Get the value for a given column in a given record as a bool value.
         @param rec The record to retrieve the data from.
         @param col The column to retrieve data from.
         @param value Bool variable to store value, if retrieval is successful.
         @return true if retrieval is successful, false otherwise.
    */
      virtual bool GetData( int rec, int col, bool &value ) = 0;
      
      /** Retrieve the minimum and maximum values found in a given column across all records.
         @param col The column to search for minimum and maximum values. Pass -1 to search across all columns.
         @param minValue Variable to hold the minimum value.
         @param maxValue Variable to hold the maximum value.
         @return true if retrieval is successful, false otherwise.
      */
      virtual bool GetDataMinMax( int col, OME_SCALAR &minValue, OME_SCALAR &maxValue ) = 0;  // note: col=-1 gets min/max for ALL columns
      
		//if neighbors
		/** Get a list of row indices that represent the neighbors of the provided record.
			@param rec The index of the record to query against.
			@param count if neighbors are successfully retrieved, returns the size of the returned array; on failure, returns -1.
			@return An array of indices of the rec's neighbors, or NULL if unsupported.
		*/
		virtual int* GetNeighbors(const int & rec, int & count) = 0; 

		/** Find the number of neighbors for a given record.
			@param rec The record to query.
			@return The number of neighbors to record rec.
		*/
		virtual unsigned int GetNeighborCount(const int & rec) const = 0;

		/** Find the most neighbors a single record may have.
			@return The maximum number of records a single record may have.
		*/
		virtual unsigned int GetMaxNeighborCount() const = 0;

		/** Get the record ID for a given neighbor.
		@param rec The record to query.
		@param neighbor The relative neighbor ID to query.
		@return The record ID of neighbor.
		*/
		virtual int GetNeighborRecord(const int & rec, const int & neighbor)=0;

      //Iterators
      //virtual ISDPIterator& GetFirstIterator()=0;
      //virtual ISDPIterator& GetLastIterator()=0;

      /** Retrieve an iterator pointing to the records.
         @param kind Flag indicating what type of iterator to return. Flags should be defined by subclasses.
         @return Reference to requested iterator.
         @sa ISDPIterator
         @sa SDPFlags
      */
      virtual ISDPIterator& GetIterator(const ITR_DIR & kind = ITR_FORWARD)=0;  
      
      /** Retrieve an iterator pointing to a list of polygons that match the query.
         @param polyIndex Index of the reference polygon.
         @param count Optional pointer to store the number of returned records.
         @return An iterator pointing to a list of records.
         @sa ISDPIterator
         @sa SDPFlags
      */
      virtual ISDPIterator& GetNextTo(int polyIndex, unsigned int* count=NULL)=0;
      /** Retrieve an iterator pointing to a list of polygons that match the query.
         @param polyIndex Index of the reference polygon.
         @param query Evaluative statement that polygons are tested against.
         @param distance Distance from neighboring polygon to evaluate polygons
         @return An iterator pointing to a list of records.
         @sa ISDPIterator
         @sa SDPFlags
      */
      virtual ISDPIterator& GetWithin(int polyIndex, const OMEChar* query, OME_SCALAR distance)=0;

	  /** Retrieve the Neighbor for a specified direction.
		@param index The index of the record to check.
		@param dir The grid direction to look for a neighbor.
		@param outInd Index of neighbor in direction dir if successful.
		@return true if there is a neighbor in the direction of dir; false otherwise.
	  */
	  virtual bool GetNeighborForDirection(const int & index, const GRID_DIR & dir, int & outInd)=0;

	  /** Check for a neighbor in a specified direction.
	  @param index The index of the record to check.
	  @param dir The grid direction to look for a neighbor.
	  @return true if there is a neighbor in the direction of dir; false otherwise.
	  */
	  virtual bool HasNeighborForDirection(const int & index, const GRID_DIR & dir)=0;

};