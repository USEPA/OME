#pragma once
#include "OMEDefines.h"
#include "DataSrc.h"
//#include "DebugOutput.h"
#include "BaseSpatialDataProvider.h"

class ISpatialDataProvider;
class CSVSDPIterator;

/** SpatialDataProvider which relies on csv data to describe spatial data.*/
class __SDP_EXPORT__ CSVSpatialDataProvider : public BaseSpatialDataProvider
{
public:
	/** Value representing the absence of a neighbor.*/
	static const unsigned int NO_NEIGHBOR;
	/** Flags to determine how neighbors are determined.
	*/
	enum NEIGHMODE {
		NM_NONE=0,				///< No neighbor determination is carried out.
		NM_COMPARE_ALL,	///< Every full record is compared to every other full record using m_neighbor_expr (args: recordIndex1,recordIndex2).
		NM_COMPARE_XY,		///< Compare each xy coordinate with every other xy coordinate using m_neighbor_expr (args: x1,y1,x2,y2).
		NM_USE_COLUMN		///< Interpret a column value for each record using m_neighbor_expr (args: columnVal).
	};

	/** Grid type used to determine what/how neighbors should be tracked.*/
	enum GRIDMODE {
		GM_NONE=0, ///< No grid representation.
		GM_4GRID,  ///< Square grid; track the for edge neighbors.
		GM_8GRID,  ///< Square grid; track edge and corner neighbors.
		GM_HEX	   ///< Hex grid; track edge neighbors.

	};

	CSVSpatialDataProvider();
	CSVSpatialDataProvider(const OMEChar* filename,const OMEChar delim=',',const STLString xLabel=STLString(),const STLString yLabel=STLString());
	CSVSpatialDataProvider(const CSVSpatialDataProvider & csdp);
	virtual ~CSVSpatialDataProvider();

	virtual int GetCapabilities() const { return m_ableFlags; };

	// basic load/save
	virtual int   Load( const OMEChar* filename,OMEParser* pPrsr=NULL );
	//virtual int   Save( const OMEChar* filename, int flags );

	// basic info about coverage
	virtual void  GetExtents    ( OME_SCALAR &vxMin, OME_SCALAR &vxMax, OME_SCALAR &vyMin, OME_SCALAR &vyMax ) const;
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
	virtual void SetData( int rec, int col, int value );
	virtual void SetData( int rec, int col, const OMEChar* value );

	// GetData(..) return true on success, false otherwise.
	virtual bool GetData( int rec, int col, OME_SCALAR &value );
	virtual bool GetData( int rec, int col, int &value );
	virtual bool GetData( int rec, int col, OMEChar* value, int maxChars );
	virtual bool GetData( int rec, int col, bool &value );

	virtual bool GetDataMinMax( int col, OME_SCALAR &minValue, OME_SCALAR &maxValue );  // note: col=-1 gets min/max for ALL columns
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

	//csv specific
	/** @return Delimiting character used when reading CSV files. */
	inline OMEChar GetDelim() const { return m_delim; };
	/** @param delim Delimiter character to use when reading CSV files. */
	inline void SetDelim(const OMEChar delim) {m_delim=delim;}

	bool SetXYCols(const STLString & xCol,const STLString & yCol);
	/** @return index of column containing data for x ordinates.*/
	inline size_t GetXCol() const { return m_xCol; }
	/** @return index of column containing data for y ordinates.*/
	inline size_t GetYCol() const { return m_yCol; }
	void SetNeighborExpr(const STLString & expr,const STLString & lang,OMEParser* pPrsr,const NEIGHMODE mode=NM_COMPARE_ALL,void* extra=NULL);
	void SetPolyExpr(const STLString & expr, const STLString & lang, OMEParser* pPrsr);
	bool SetPolyCols(const STLString & polyCol, const STLString & partCol);
	/**@return Expression used to evaluate neighbor relationships. In language identified by GetNeighborScriptLanguage().*/
	inline STLString GetNeighborExpr()const { return m_neighborsExpr; }
	/**@return A string identifying the scripting language used to evaluate the expression returned by GetNeighborExpr(). Currently lua is the only language supported.*/
	inline STLString GetNeighborScriptLanguage() const { return m_nScriptLang; }
	/**@return A string identifying the scripting language used to evaluate the expression used to determine polygon neighbor relationships. Currently lua is the only language supported.*/
	inline STLString GetPolyScriptLanguage() const { return m_pScriptLang; }
	/** @return A NEIGHMODE value indicating how neighbor relationships are identified.*/
	inline NEIGHMODE GetNeighborMode() const { return m_neighborMode; }
	/** @return Optional column label containing information on an entry's neighbor relationship.*/
	inline STLString GetNeighborLabel() const { return STLString(m_neighLbl); }
	void RefreshNeighbors(OMEParser* pPrsr);
	//void SetPolyExpr(const STLString & expr, const STLString & lang, OMEParser* pPrsr, const STLString & polyCol, const STLString & partCol);
	void RefreshPolys(OMEParser* pPrsr=NULL);

	/** Retrieve a label for a given column index.
		@param col The column index to query.
		@return String containing the column title.
	*/
	inline STLString GetColumnLabel(const size_t & col) const{ return m_dataSrc.GetLabelForCol(col); };

	/** @return Path to coverage file.*/
	inline STLString GetCoverageFilePath() const { return m_dataSrc.GetSrcPath(); }

	void SetCoverageType(const int & type);

	/** Set the grid mode for grid type representations.
		@param gm The GRIDMODE to assign.
	*/
	inline void SetGridMode(const GRIDMODE & gm) { m_gridMode = gm; }

private:

	/** Struct for holding basic polygon data*/
	struct SHPPoly
	{
		std::deque<std::deque<std::pair<OME_SCALAR, OME_SCALAR> > > parts;	///< vertex collection defining 1 or more polygon parts
		//additional data?
		OMESizeArray neighborInds;			///< Lookup for neighbor polygons.
		OME_SCALAR totalArea;				///< Quick Reference for Area.
	};

	DataSrc m_dataSrc;						///<Storage object for csv data;

	std::deque<std::deque<unsigned int> > m_neighbors; ///< A collection of lists of neighbors for each cell. The cell at index x is neighbor to cells whose indices are found at that index.
	std::deque<SHPPoly> m_polys;			///< Collection of Polygons used in coverage (if polygonal coverage).
	size_t m_xCol;							///< Identifier of column which contains x ordinates.
	size_t m_yCol;							///< Identifier of column which contains y ordinates.
	size_t m_polyCol;						///< Identifier of column which contains polygon indices.
	size_t m_partCol;						///< Identifier of column which contains polygon part indices.

	OMEChar m_delim;						///< Delimiter used to seperate cells in source CSV file.
	STLString m_nScriptLang;				///< Language identifier for evaluating a neighbor expression.
	STLString m_neighborsExpr;				///< Expression used to determine if cells are neighbors.
	STLString m_pScriptLang;				///< Language identifier for evaluating a polygon construction expression.
	STLString m_polyExpr;					///< Expression used to construct individual polygons. Currently unused?

	CSVSDPIterator* m_firstItr;   ///< List of associated iterators. Used to invalidate associated iterators when data changes size
	CSVSDPIterator* m_lastItr;	  ///< The last iterator in the iterator list.

	NEIGHMODE m_neighborMode;				///< Neighbor determination mode flag. @sa NEIGHMODE
	GRIDMODE m_gridMode;					///< The type of grid being represented, if any.
	OMEChar m_neighLbl[1024];				///< cached copy of label of column used when m_neighborMode is NM_USE_COLUMN. @sa NEIGHMODE
	
	int m_ableFlags;						///< Storage for SpatialDataProvider flags, which may vary based on limitations imposed by loaded data.
	unsigned int m_maxNeighbors;				///< Value indicating the largest number of neigbhors for any record.

	void CompareExtentsForCol(int col, OME_SCALAR & minVal, OME_SCALAR & maxVal) const;
	void InvalidateIters();
	void FullNeighborCompare(OMEParser* pPrsr);
	void ColumnNeighborCompare(OMEParser* pPrsr);

	int GetNeighbors(const int & rec, CSVSDPIterator & outItr);

	void AddIterator(CSVSDPIterator* pItr);
	
	void ResolveGridAssignments(const SclrResults & results, const int & numResults, std::deque<unsigned int> & nIDs);
	void Resolve4GridNeighbors(const SclrResults & results, const int & numResults, std::deque<unsigned int> & nIDs);
	void Resolve8GridNeighbors(const SclrResults & results, const int & numResults, std::deque<unsigned int> & nIDs);
	void ResolveHexNeighbors(const SclrResults & results, const int & numResults, std::deque<unsigned int> & nIDs);

};

//
//================================================================
//

/** Iterator for CSVSpatialDataProvider.*/
class CSVSDPIterator : public BaseSDPIterator
{
	friend class CSVSpatialDataProvider;
public:
	
	/** Iterator linked-list node.*/
	struct IndNode
	{
		IndNode* next;		///< Pointer to next node.
		IndNode* prev;		///< Pointer to previous node.
		int ind;			///< Index of targeted record.
		IndNode() :next(NULL),prev(NULL), ind(-1){}
	};
   /** Destructor. */
	virtual ~CSVSDPIterator();
   
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
	virtual bool operator==(const CSVSDPIterator & rhs)
	{
		return this==&rhs;
	}

protected:
	/** Basic constructor.
	@param sdp Valid ISpatialDataProvider pointer.
	@param dir The Direction of travel for the iterator.
	*/
	CSVSDPIterator(ISpatialDataProvider* sdp,const ISpatialDataProvider::ITR_DIR & dir = ISpatialDataProvider::ITR_FORWARD);

	IndNode* m_records;					///< Records associated with this iterator.
	unsigned int m_totRecords;			///< Total number of records.
	IndNode* m_currRecord;					///< Record that is currently being pointed to.
	ISpatialDataProvider::ITR_DIR m_dir;	///< The direction in which the iterator is allowed to increment.
	bool m_isValid;								///< true if iterator is currently valid; false otherwise.

	CSVSDPIterator* m_prevItr;				///< Previous iterator in the linked list.
	CSVSDPIterator* m_nextItr;				///< Next iterator in the linked list.
};