#include "CSVSpatialDataProvider.h"
#include "LuaOMEParser.h"
#include "MemPool.h"
#include <new>
#include <limits>

namespace ItrPools
{
	ObjMemPool<CSVSDPIterator> s_itrPool(100);
	ObjMemPool<CSVSDPIterator::IndNode> s_nodePool(50000);
};

const unsigned int CSVSpatialDataProvider::NO_NEIGHBOR = std::numeric_limits<unsigned int>::max();

/** Simple Constructor. */
CSVSpatialDataProvider::CSVSpatialDataProvider()
	:BaseSpatialDataProvider(), m_xCol(-1), m_yCol(-1), m_delim(','), m_neighborMode(NM_NONE), m_ableFlags(BI_ITR_SUPPORT | READ_ACCESS | GRID_SUPPORT | HEX_SUPPORT)
	, m_firstItr(NULL), m_lastItr(NULL), m_gridMode(GM_NONE)
{

}

/** Detailed Constructor.
	@param filename Path to CSV file.
	@param delim Delimiter for CSV entries.
	@param xLabel Label identifying column containing x ordinates.
	@param yLabel Label identifying column containing y ordinates.
*/
CSVSpatialDataProvider::CSVSpatialDataProvider(const OMEChar* filename,const OMEChar delim,const STLString xLabel,const STLString yLabel)
:BaseSpatialDataProvider(), m_xCol(-1), m_yCol(-1), m_delim(delim), m_neighborMode(NM_NONE), m_ableFlags(BI_ITR_SUPPORT | READ_ACCESS | GRID_SUPPORT |HEX_SUPPORT)
, m_firstItr(NULL), m_lastItr(NULL), m_gridMode(GM_NONE)
{
	if(Load(filename)==LOAD_SUCCESS)
		SetXYCols(xLabel,yLabel);
	else
		throw OMEIOException("CSVSpatialDataProvider",("Coverage file: " + STLString(filename) + " not found").c_str());
}

/** Copy Constructor.
	@param csdp CSVSpatialDataProvider to copy.
	\todo finish implementation.
*/
CSVSpatialDataProvider::CSVSpatialDataProvider(const CSVSpatialDataProvider & csdp)
{
	//...
}

/** Destructor. */
CSVSpatialDataProvider::~CSVSpatialDataProvider()
{
	InvalidateIters();
}

/** Add iterator to internal tracking list.
	@param pItr Pointer of iterator to add.
*/
void CSVSpatialDataProvider::AddIterator(CSVSDPIterator* pItr)
{
	if (!m_lastItr)
		m_firstItr = m_lastItr = pItr;
	else
	{
		pItr->m_prevItr = m_lastItr;
		m_lastItr = m_lastItr->m_nextItr = pItr;
	}
}

/** Parse out Grid relationships based on Grid mode.
	@param results The list of values returned by the neighbor evaluation.
	@param numResults The number of neighbor relations found.
	@param nIDs The list of neighbor ids to be updated.
*/
void CSVSpatialDataProvider::ResolveGridAssignments(const SclrResults & results, const int & numResults, std::deque<unsigned int> & nIDs)
{
	switch (m_gridMode)
	{
	case GM_4GRID:
		Resolve4GridNeighbors(results, numResults, nIDs);
		break;
	case GM_8GRID:
		Resolve8GridNeighbors(results, numResults, nIDs);
		break;
	case GM_HEX:
		ResolveHexNeighbors(results, numResults, nIDs);
		break;
	}
}

/** Parse out Grid-4 relationships based on Grid mode.
@param results The list of values returned by the neighbor evaluation.
@param numResults The number of neighbor relations found.
@param nIDs The list of neighbor ids to be updated.
*/
void CSVSpatialDataProvider::Resolve4GridNeighbors(const SclrResults & results, const int & numResults, std::deque<unsigned int> & nIDs)
{
    /*                    _____
                         |     |
                         |  N  | 
                    _____|_____|_____
                   |     |     |     |
                   |  W  |  C  |  E  |
                   |_____|_____|_____|
						 |     |
						 |  S  |
						 |_____|
	*/

	GRID_DIR dir;
	unsigned int index;
	for (int i = 0; i < numResults; ++i)
	{
		index = (unsigned int)results[i++];
		dir = (GRID_DIR)(unsigned int)results[i];

		switch (dir)
		{
		case GD_N:
		case GD_E:
		case GD_S:
		case GD_W:
			nIDs[dir] = index;
			break;
		default:
			throw OMEOOBException("CSVSpatialDataProvider: illegal direction for grid4 neighbor.");
		}
	}
}

/** Parse out Grid-8 relationships based on Grid mode.
@param results The list of values returned by the neighbor evaluation.
@param numResults The number of neighbor relations found.
@param nIDs The list of neighbor ids to be updated.
*/
void CSVSpatialDataProvider::Resolve8GridNeighbors(const SclrResults & results, const int & numResults, std::deque<unsigned int> & nIDs)
{
	/*                _________________
	                 |     |     |	   |
	                 | NW  |  N  | NE  |
	                 |_____|_____|_____|
	                 |     |     |     |
	                 |  W  |  C  |  E  |
	                 |_____|_____|_____|
	                 |     |     |	   |
	                 | SW  |  S  | SE  |
	                 |_____|_____|_____|
	*/

	GRID_DIR dir;
	unsigned int index;
	for (int i = 0; i < numResults; ++i)
	{
		index = (unsigned int)results[i++];
		dir = (GRID_DIR)(unsigned int)results[i];

		switch (dir)
		{
		case GD_N:
		case GD_E:
		case GD_S:
		case GD_W:
		case GD_NE:
		case GD_NW:
		case GD_SE:
		case GD_SW:
			nIDs[dir] = index;
			break;
		default:
			throw OMEOOBException("CSVSpatialDataProvider: unknown direction flag for 8grid.");
		}
	}
}

/** Parse out hex-Grid relationships based on Grid mode.
@param results The list of values returned by the neighbor evaluation.
@param numResults The number of neighbor relations found.
@param nIDs The list of neighbor ids to be updated.
*/
void CSVSpatialDataProvider::ResolveHexNeighbors(const SclrResults & results, const int & numResults, std::deque<unsigned int> & nIDs)
{
    /*          ____                                   / \   / \
               /    \                                 /   \ /   \
          ____/  N   \____                           | NW  | NE  |
         /    \      /    \                          |     |     |
        /  NW  \____/  NE  \                        / \   / \   / \
        \      /    \      /					   /   \ /   \ /   \
         \____/  C   \____/                       |  W  |  C  |  E  |
         /    \      /    \           - OR -      |     |     |     |
        /  SW  \____/  SE  \					   \   / \   / \   /	 
        \      /    \      /                        \ /   \ /   \ /
         \____/  S   \____/                          | SW  | SE  |
              \      /                               |     |     |
               \____/                                 \   / \   /
                                                       \ /   \ /
	*/

	GRID_DIR dir;
	unsigned int index;
	for (int i = 0; i < numResults; ++i)
	{
		index = (unsigned int)results[i++];
		dir = (GRID_DIR)(unsigned int)results[i];

		switch (dir)
		{
		case GD_N:
		case GD_E:
			nIDs[GD_N] = index;
			break;
		case GD_S:
		case GD_W:
			nIDs[GD_S] = index;
			break;

		case GD_NE:
		case GD_NW:
		case GD_SE:
		case GD_SW:
			nIDs[dir-2] = index;
			break;
		default:
			throw OMEOOBException("CSVSpatialDataProvider: unknown direction flag for 8grid.");
		}
	}
}

int CSVSpatialDataProvider::Load( const OMEChar* filename, OMEParser* pPrsr )
{
	int ret=LOAD_READ_FAIL;

	InvalidateIters();
	STLString fileStr(filename);
	if(m_dataSrc.ReadCSV(fileStr))
		ret=LOAD_SUCCESS;

	if(!m_neighborsExpr.empty())
		RefreshNeighbors(dynamic_cast<LuaOMEParser*>(pPrsr));

	if (m_neighborMode != NM_NONE)
		m_ableFlags |= NEIGHBOR_SUPPORT;
	else
		m_ableFlags &= ~NEIGHBOR_SUPPORT;
	return ret;
}

void CSVSpatialDataProvider::GetExtents( OME_SCALAR &vxMin, OME_SCALAR &vxMax, OME_SCALAR &vyMin, OME_SCALAR &vyMax ) const
{
	 if(m_dataSrc.GetRowCount()>0)
	 {
		//x extents
		if(m_xCol!=-1)
		{
			m_dataSrc.GetNumVal(m_xCol,0,vxMin);
			vxMax=vxMin;
			CompareExtentsForCol(m_xCol,vxMin,vxMax);
		}

		//y extents
		if(m_yCol!=-1)
		{
			m_dataSrc.GetNumVal(m_yCol,0,vyMin);
			
			CompareExtentsForCol(m_yCol,vyMin,vyMax);
		}
	 }
}

int CSVSpatialDataProvider::GetRowCount( void ) const
{
	if (m_polys.empty())
		return m_dataSrc.GetRowCount();
	else
		return m_polys.size();
}

int CSVSpatialDataProvider::GetColumnCount( void ) const
{
	return m_dataSrc.GetColumnCount();
}

int CSVSpatialDataProvider::GetFieldCol( const OMEChar* label ) const
{
	return m_dataSrc.GetFieldCol(label);
}

int CSVSpatialDataProvider::AddFieldCol( const OMEChar* label, const OME_SCALAR initVal)
{
	return m_dataSrc.AddFieldCol(label,initVal);
}

int CSVSpatialDataProvider::AddFieldCol(const OMEChar* label, const OME_SCALAR* vals, const OME_SCALAR & count)
{
	return m_dataSrc.AddFieldCol(label, vals,count);
}

int CSVSpatialDataProvider::AddFieldCol(const OMEChar* label, ValueArchive* pArch, const size_t & start, const OME_SCALAR & count)
{
	return m_dataSrc.AddFieldCol(label, pArch,start, count);
}

bool  CSVSpatialDataProvider::NextTo(int polyIndex, const OMEChar* query)
{
	bool ret=false;

	//...
	return ret;
}

OME_SCALAR CSVSpatialDataProvider::NextToArea(int polyIndex, const OMEChar* query)
{
	OME_SCALAR ret = 0.0;

	//...
	return ret;
}

bool  CSVSpatialDataProvider::Within(int polyIndex, const OMEChar* query, OME_SCALAR distance)
{
	bool ret = false;

	//...
	return ret;
}

OME_SCALAR CSVSpatialDataProvider::WithinArea(int polyIndex, const OMEChar* query, OME_SCALAR distance)
{
	OME_SCALAR ret = 0.0;

	//...
	return ret;
}

bool CSVSpatialDataProvider::GetCellSize ( OME_SCALAR &width, OME_SCALAR &height )
{
	//...
	return false;
}

void CSVSpatialDataProvider::SetData( int rec, int col, OME_SCALAR value )
{
	m_dataSrc.SetNumVal(rec,col,value);

}

void CSVSpatialDataProvider::SetData( int rec, int col, int value )
{
	SetData(rec,col,(OME_SCALAR)value);

}

void CSVSpatialDataProvider::SetData( int rec, int col, const OMEChar* value )
{
	m_dataSrc.SetStrVal(col,rec,STLString(value));
}

bool CSVSpatialDataProvider::GetData( int rec, int col, OME_SCALAR &value )
{
	return m_dataSrc.GetNumVal(col,rec,value);
}



bool CSVSpatialDataProvider::GetData( int rec, int col, int &value )
{
	OME_SCALAR temp=(OME_SCALAR)value;
	bool ret=GetData(rec,col,temp);
	if(ret)
		value=(int)temp;
	return ret;
}

bool CSVSpatialDataProvider::GetData( int rec, int col, OMEChar* value, int maxChars )
{
	STLString temp;
	bool ret=m_dataSrc.GetStrVal(col,rec,temp);

	if (ret)
	{
		temp.resize(maxChars,'\0');
      temp.back() = '\0';
		//can't just use temp.copy due to VS checks. avoid platform-specific checks by copying manually.
		for (size_t i = 0; i < maxChars;++i)
			value[i] = temp[i];
	}
	return ret;
}

bool CSVSpatialDataProvider::GetData( int rec, int col, bool &value )
{
	OME_SCALAR temp=(OME_SCALAR)value;
	bool ret=GetData(rec,col,temp);
	if(ret)
		value=(bool)temp;
	return ret;
}

bool CSVSpatialDataProvider::GetDataMinMax( int col, OME_SCALAR &minValue, OME_SCALAR &maxValue )
{
	unsigned int i=0;

	bool ret=false;
	if(m_dataSrc.GetRowCount()>0)
	{
		m_dataSrc.GetNumVal(col,0,minValue);
		maxValue=minValue;
		do
		{
			for(unsigned int j=0; j<m_dataSrc.GetRowCount(); j++)
				CompareExtentsForCol(j,minValue,maxValue);

			i++;
		} while(col==-1 && i<m_dataSrc.GetColumnCount());
		ret=true;
	}

	return ret;
}

/** Get a list of neighbor IDs.
	@param rec The record ID to query for neighbors.
	@param count On return, the total number of neighbor IDs returned.
	@return Pointer to an array of neighbor IDs, or NULL if there are no neighbors.
*/
int* CSVSpatialDataProvider::GetNeighbors(const int & rec, int & count)
{
	int* ret=NULL;
	if(!m_neighbors.empty())
	{
		if(rec>0 && rec<=m_dataSrc.GetRowCount())
		{
			count=m_neighbors[rec-1].size();
			if(count)
			{
				ret=new int[count];
				size_t i=0;
				for(auto itr=m_neighbors[rec-1].begin(); itr!=m_neighbors[rec-1].end(); ++itr,i++)
					ret[i]=*itr;
			}
		}
		else
			count=-1;
	}
	return ret;
}

unsigned int CSVSpatialDataProvider::GetNeighborCount(const int & rec) const
{
	unsigned ret = 0;

	if (!m_neighbors.empty())
	{
		if (rec > 0 && rec <= m_dataSrc.GetRowCount())
		{
			if (m_gridMode== GM_NONE)
				ret = m_neighbors[rec - 1].size();
			else
				ret=std::count_if(m_neighbors[rec - 1].begin(), m_neighbors[rec - 1].end(), 
				[](const unsigned int & n){return n != NO_NEIGHBOR; });
		}
	}
	return ret;
}

unsigned int CSVSpatialDataProvider::GetMaxNeighborCount() const
{
	return m_maxNeighbors;
}

int CSVSpatialDataProvider::GetNeighborRecord(const int & rec, const int & neighbor)
{
	int ret = -1;
	if (!m_neighbors.empty())
	{
		if (rec>0 && neighbor > 0 && rec <= m_dataSrc.GetRowCount() && neighbor<=m_neighbors[rec - 1].size())
			ret = m_neighbors[rec - 1][neighbor-1]+1;
	}
	return ret;
}

/** Retrieve count and an iterator to a list of neighbors.
	@param rec The index of the record to query.
	@param outItr On return, points to a list of neighbor indices.
	@return The total number of neighbors, or -1 if rec is an invalid index.
*/
int CSVSpatialDataProvider::GetNeighbors(const int & rec, CSVSDPIterator & outItr)
{
	int count = 0;
	if (!m_neighbors.empty())
	{
		if (rec>0 && rec<=m_dataSrc.GetRowCount())
		{
			count = m_neighbors[rec - 1].size();
			if (count)
			{
				size_t i = 0;
				CSVSDPIterator::IndNode* pCurrNode=NULL,*pNewNode;

				for (auto itr = m_neighbors[rec - 1].begin(); itr != m_neighbors[rec - 1].end(); ++itr, i++)
				{
					pNewNode = new(ItrPools::s_nodePool.NewCastPtr())CSVSDPIterator::IndNode();
					pNewNode->ind = (*itr);
					if (pCurrNode)
					{
						pCurrNode->next = pNewNode;
						pNewNode->prev = pCurrNode;
					}
					else
					{
						outItr.m_records = pNewNode;
					}
					pCurrNode = pNewNode;
				}
			}
			
			outItr.m_totRecords = count;
			outItr.m_currRecord = outItr.m_records;
		}
		else
			count = -1;
	}
	return count;
}

ISDPIterator& CSVSpatialDataProvider::GetIterator(const ITR_DIR & kind)
{
	CSVSDPIterator* pItr = new(ItrPools::s_itrPool.NewCastPtr())CSVSDPIterator(this,kind);
	if (pItr->m_isValid = m_dataSrc.GetRowCount() > 0)
	{
		CSVSDPIterator::IndNode* pCurr, *pLast;
		pCurr = new(ItrPools::s_nodePool.NewCastPtr())CSVSDPIterator::IndNode();
		pItr->m_records = pCurr;
		pItr->m_currRecord = pCurr;
		pLast = pCurr;
		if (kind == ISpatialDataProvider::ITR_FORWARD)
		{
			pCurr->ind = 0;
			for (unsigned int i = 1; i < m_dataSrc.GetRowCount(); ++i)
			{
				pCurr = new(ItrPools::s_nodePool.NewCastPtr())CSVSDPIterator::IndNode();
				pCurr->ind = i;
				pCurr->prev = pLast;
				pLast->next = pCurr;
				pLast = pCurr;
			}
		}
		else
		{
			pCurr->ind = m_dataSrc.GetRowCount()-1;
			for (unsigned int i = m_dataSrc.GetRowCount() - 2; i >= 0;--i)
			{
				pCurr = new(ItrPools::s_nodePool.NewCastPtr())CSVSDPIterator::IndNode();
				pCurr->ind = i;
				pCurr->prev = pLast;
				pLast->next = pCurr;
				pLast = pCurr;
			}
		}
	}
	AddIterator(pItr);
	return *pItr;
}

ISDPIterator& CSVSpatialDataProvider::GetNextTo(int polyIndex, unsigned int* count)
{
	//ignore query for now
	CSVSDPIterator* pItr = new(ItrPools::s_itrPool.NewCastPtr())CSVSDPIterator(this);
	int locCount;
	
	size_t startInd;
	locCount = GetNeighbors(polyIndex, *pItr);

	AddIterator(pItr);

	return *pItr;
}

ISDPIterator& CSVSpatialDataProvider::GetWithin(int polyIndex, const OMEChar* query, OME_SCALAR distance)
{
	//ignore quesry for now
	/// \todo implement

	CSVSDPIterator* pItr=new(ItrPools::s_itrPool.NewCastPtr())CSVSDPIterator(this);
	pItr->m_isValid=false;
	AddIterator(pItr);
	return *pItr;
}

bool CSVSpatialDataProvider::GetNeighborForDirection(const int & index, const GRID_DIR & dir, int & outInd)
{
	GRID_DIR adjDir = dir;
	if (m_gridMode == GM_NONE)
		return false;

	if (m_gridMode == GM_4GRID && dir > GD_W)
		return false;

	if (m_gridMode == GM_HEX && dir != GD_N && dir != GD_S)
		adjDir= (GRID_DIR)((int)adjDir-2);

	int ret = m_neighbors[index-1][adjDir];
	if (ret == NO_NEIGHBOR)
		return false;

	outInd = ret+1;
	return true;
}

bool CSVSpatialDataProvider::HasNeighborForDirection(const int & index, const GRID_DIR & dir)
{
	int dummy;
	return GetNeighborForDirection(index, dir, dummy);
}

/** Flag the columns used to identify xy coordinates.
	@param xCol Label identifying column containing x ordinates. Pass an empty string if no x ordinates are used.
	@param yCol Label identifying column containing y ordinates. Pass an empty string if no y ordinates are used.
	@return true if identified column labels are found; false otherwise.
*/
bool CSVSpatialDataProvider::SetXYCols(const STLString & xCol,const STLString & yCol)
{
	m_xCol=m_yCol=-1;
	if(!xCol.empty() && !yCol.empty())
	{
		for(unsigned int i=0; i< m_dataSrc.GetColumnCount() && (m_xCol==-1 || m_yCol==-1); i++)
		{
			if(!xCol.empty() && m_dataSrc.GetLabelForCol(i)==xCol)
				m_xCol=i;
			if(!yCol.empty() && m_dataSrc.GetLabelForCol(i)==yCol)
				m_yCol=i;
		}
	}
	return (xCol.empty() || m_xCol!=-1) && (yCol.empty() || m_yCol!=-1);
}

/** Retrieve the minimum and maximum values found in a given numeric column.
	@param col A valid column index identifying the column to query.
	@param minVal Variable that will contain the minimum numeric value found on return.
	@param maxVal Variable that will contain the maximum numeric value found on return.

*/
void CSVSpatialDataProvider::CompareExtentsForCol(int col, OME_SCALAR & minVal, OME_SCALAR & maxVal) const
{
	OME_SCALAR curr;
	if(col!=-1 && m_dataSrc.IsNumCol(col))
	{
		m_dataSrc.GetNumVal(col,0,maxVal);
		minVal=maxVal;
		for(unsigned int i=1;i<m_dataSrc.GetRowCount(); i++)
		{
			m_dataSrc.GetNumVal(col,i,curr);

			if(curr > maxVal)
				maxVal=curr;
			else if(curr <minVal)
				minVal=curr;
		}
	}
}

/** Set the expression needed to evaluate neighbor relationships.
	@param expr Expression to be evaluated by OMEParser.
	@param lang String identifier of language to be used to evaluate expr. Currently only Lua is supported.
	@param pPrsr Pointer to OMEParser that will refresh neighbor relationships. If NULL, RefreshNeighbors() is not called.
	@param mode Flag indicating how to interpret neighbor expression. See NEIGHMODE enums
	@param extra Pointer to additional data that may be needed depending on the value of mode.
	@sa NEIGHMODE
*/
void CSVSpatialDataProvider::SetNeighborExpr(const STLString & expr,const STLString & lang, OMEParser* pPrsr,const CSVSpatialDataProvider::NEIGHMODE mode,void* extra)
{
	m_neighborsExpr=expr;
	m_neighborMode=mode;

	if (m_neighborMode != NM_NONE)
		m_ableFlags |= NEIGHBOR_SUPPORT;
	else
		m_ableFlags &= ~NEIGHBOR_SUPPORT;

	m_nScriptLang=lang;
	//interpret extra data for mode

	switch(m_neighborMode)
	{
	case NM_USE_COLUMN:
#if (defined WIN32 || defined _WIN64)
		strcpy_s<1024>(m_neighLbl,(OMEChar*)extra);
#else
		size_t i;
		for(i=0;i<1024 && ((OMEChar*)extra)[i]!='\0'; i++)
			m_neighLbl[i]=((OMEChar*)extra)[i];
		if (i<1024)
			m_neighLbl[i] = '\0';
#endif
		break;

	}

	if(m_dataSrc.GetColumnCount()>0)
		RefreshNeighbors(pPrsr);
}

/** Refresh the cache of neighbor associations.
	@param pPrsr Pointer to the parser used to evaluate the neighbor expression.
*/
void CSVSpatialDataProvider::RefreshNeighbors(OMEParser* pPrsr)
{
	switch (m_gridMode)
	{
	case GM_4GRID:
		m_maxNeighbors = 4;
		break;
	case GM_8GRID:
		m_maxNeighbors = 8;
		break;
	case GM_HEX:
		m_maxNeighbors = 6;
		break;
	default:
		m_maxNeighbors = 0;
	}

	switch(m_neighborMode)
	{
	case NM_NONE:
		//msg?
		break;
	case NM_USE_COLUMN:
		ColumnNeighborCompare(pPrsr);
		break;
	default:
		FullNeighborCompare(pPrsr);
	}

}

/** Assign labels that identify columns containing polygon identification data.
	@param polyCol The name of the column containing polygon indices.
	@param partCol The name of the column containing polygon part indices.
*/
bool CSVSpatialDataProvider::SetPolyCols(const STLString & polyCol, const STLString & partCol)
{
	m_partCol = m_polyCol = -1;
	for (unsigned int i = 0; i< m_dataSrc.GetColumnCount() && (m_partCol == -1 || m_polyCol == -1); i++)
	{
		if (!polyCol.empty() && m_dataSrc.GetLabelForCol(i) == polyCol)
			m_polyCol = i;
		if (!partCol.empty() && m_dataSrc.GetLabelForCol(i) == partCol)
			m_partCol = i;
	}
	return (polyCol.empty() || m_polyCol != -1) && (partCol.empty() || m_partCol != -1);
}

/** Set the expression used to construct polygons.
	@param expr The expression to use.
	@param lang The language used to interpret expr; currently only "lua" is supported.
	@param pPrsr Pointer to parser used to execute the expression.
*/
void CSVSpatialDataProvider::SetPolyExpr(const STLString & expr, const STLString & lang, OMEParser* pPrsr)
{
	m_polyExpr = expr;
	m_pScriptLang = lang;
	
	RefreshPolys(pPrsr);
}

void CSVSpatialDataProvider::RefreshPolys(OMEParser* pPrsr)
{
	m_polys.clear();

	//for now, assume 1 record = 1 vertex
	if (m_pScriptLang.empty())
	{
		OME_SCALAR maxSize = 0;
		m_dataSrc.GetMaxValForCol(m_polyCol, maxSize);
		m_polys.resize((size_t)maxSize);

		size_t poly,part;
		OME_SCALAR x, y;
		for (size_t i=0; i < m_dataSrc.GetRowCount(); i++)
		{
			m_dataSrc.GetNumVal(m_polyCol,i,poly);
			m_dataSrc.GetNumVal(m_partCol, i, part);
			m_dataSrc.GetNumVal(m_xCol, i, x);
			m_dataSrc.GetNumVal(m_yCol, i, y);

			poly--;
			part--;
			if (m_polys[poly].parts.size() <= part)
				m_polys[poly].parts.resize(part+1);
			m_polys[poly].parts[part].emplace_back(x, y);
		}
	}
	else
	{
		/// \todo add code to evaluate script to construct polygons
	}

	//find areas - assume clockwise
	for (auto polyItr = m_polys.begin(); polyItr != m_polys.end(); ++polyItr)
	{
		(*polyItr).totalArea = 0.0;
		for (auto partItr = (*polyItr).parts.begin(); partItr != (*polyItr).parts.end(); ++partItr)
		{
			for (size_t i = 0, j = (*partItr).size() - 1; i < (*partItr).size(); ++i)
			{
				(*polyItr).totalArea += ((*partItr)[j].first + (*partItr)[i].first) * ((*partItr)[j].second + (*partItr)[i].second);
				j = i;
			}
		}
		(*polyItr).totalArea/=2.0;
	}
	//find neighbors
	/// \todo add neighbor stuff

}


/** Invalidate any extant iterators associated with this instance of the SDP.
*/
void CSVSpatialDataProvider::InvalidateIters()
{
	if (m_firstItr)
	{
		CSVSDPIterator* pCurr = m_firstItr;
		while (pCurr)
		{
			pCurr->m_isValid = false;
			pCurr = pCurr->m_nextItr;
		}
	}
}

/** Compare every record with every other record to determine neighbor relationships.
	@param pPrsr Pointer to OMEParser that will interpret neighbor evaluation expression.
*/
void CSVSpatialDataProvider::FullNeighborCompare(OMEParser* pPrsr)
{
		//create a local state
	OME_SCALAR *results=NULL;
	int count;

	if(!m_neighborsExpr.empty() && m_dataSrc.GetColumnCount()>0 && m_dataSrc.GetRowCount()>0 && pPrsr)
	{
		m_neighbors.clear();
		pPrsr->PushModeID("CSV Neighbor");
		pPrsr->PushBatch(m_neighborsExpr.c_str());
		SclrArgs args(m_neighborMode==NM_COMPARE_XY ? 6 : 4);
		
		//precache xy columns for modes that use it
		args[2]=m_xCol;
		args[3]=m_yCol;
		OME_SCALAR temp;
		std::deque<unsigned int> nList;
		if (m_gridMode != GM_NONE)
			nList.resize(m_maxNeighbors, NO_NEIGHBOR);
		for(unsigned int i=0; i<m_dataSrc.GetRowCount(); i++)
		{
			m_gridMode == GM_NONE ? nList.clear() : nList.assign(m_maxNeighbors,NO_NEIGHBOR);
			for(unsigned int j=0; j<m_dataSrc.GetRowCount();j++)
			{
				if(i!=j)
				{
					count= m_gridMode==GM_NONE ? 1 : m_maxNeighbors;
					
					switch(m_neighborMode)
					{
					case NM_COMPARE_ALL:
						args[0]=i;
						args[1]=j;
						break;
					case NM_COMPARE_XY:

						args[0] = i;
						m_dataSrc.GetNumVal(m_xCol,i,temp);
						args[1]=(OME_SCALAR)temp;
						m_dataSrc.GetNumVal(m_yCol,i,temp);
						args[2]=(OME_SCALAR)temp;

						args[3] = j;
						m_dataSrc.GetNumVal(m_xCol,j,temp);
						args[4]=(OME_SCALAR)temp;
						m_dataSrc.GetNumVal(m_yCol,j,temp);
						args[5]=(OME_SCALAR)temp;
						break;
					}

					pPrsr->BatchEvaluate(results,count,args);
					if (m_gridMode == GM_NONE)
					{
						if (count > m_maxNeighbors)
							m_maxNeighbors = count;
						if (*results)
							nList.push_back(j);
					}
					else if (count)
					{
						SclrResults sRes(count, 0.0);
						for (int a = 0; a < count; ++a)
							sRes[a] = results[a];
						ResolveGridAssignments(sRes,count, nList);
					}
				}
			}

			m_neighbors.push_back(nList);
		}
		pPrsr->PopBatch();
		pPrsr->PopModeID();
	}
	if (results)
		delete[] results;
}

/** Compare the xy coordinate of every record with every other record.
	@param pPrsr Pointer to OMEParser that will interpret neighbor evaluation expression.
*/
void CSVSpatialDataProvider::ColumnNeighborCompare(OMEParser* pPrsr)
{
	SclrResults results;
	int count;

	if(!m_neighborsExpr.empty() && m_dataSrc.GetColumnCount()>0 && m_dataSrc.GetRowCount()>0 && pPrsr)
	{
		m_neighbors.clear();
		pPrsr->PushModeID("CSV Neighbor");
		pPrsr->PushBatch(m_neighborsExpr.c_str());
		SclrArgs args(2);

		args[1]=GetFieldCol(m_neighLbl);
		
		//assume that neighbor calcs use a 1-based index.
		//that way, we can use 0 in the case of null terminated lists
		std::deque<unsigned int> nList;
		for(unsigned int i=0; i<m_dataSrc.GetRowCount(); i++)
		{
			args[0]=i;
			args[0] = i;
			pPrsr->BatchEvaluate(results, count, args);
		 
			if (m_gridMode == GM_NONE)
			{
				 if (count>m_maxNeighbors)
					 m_maxNeighbors = count;

					nList.clear();
					for (int j = 0; j < count; j++)
					{
						if (results[j]>0.0)
							nList.push_back((unsigned int)results[j]);
					}
			}
			else
			{
				 nList.assign(m_maxNeighbors, NO_NEIGHBOR);
				 ResolveGridAssignments(results, count, nList);
			}
			m_neighbors.push_back(nList);

			//DBG_COLOR(cout << i << "  ", DBG_RED);
			//for (size_t j = 0; j < nList.size(); ++j)
			//	DBG_COLOR(cout << nList[j] << "  ", DBG_GREEN);

			//cout << endl;
		}
		pPrsr->PopBatch();
		pPrsr->PopModeID();
	}
}

/** Specify the types of coverage supported by the loaded coverage.
	@param type Variable that is either set to NONE, or contains more than one coverage flag: GRID_SUPPORT, HEX_SUPPORT, and/or POLY_SUPPORT.
*/
void CSVSpatialDataProvider::SetCoverageType(const int & type)
{
	//clear coverage types
	m_ableFlags |= ~(GRID_SUPPORT | HEX_SUPPORT | POLY_SUPPORT);

	//only grab specific flags from type
	m_ableFlags |= type & GRID_SUPPORT;
	m_ableFlags |= type & HEX_SUPPORT;
	m_ableFlags |= type & POLY_SUPPORT;
	
}

//
//==============================================================================
//

CSVSDPIterator::CSVSDPIterator(ISpatialDataProvider* sdp,const ISpatialDataProvider::ITR_DIR & dir)
	: BaseSDPIterator(sdp), m_isValid(true), m_currRecord(NULL), m_records(NULL)
{

}

CSVSDPIterator::~CSVSDPIterator()
{
	if (m_prevItr)
		m_prevItr->m_nextItr = m_nextItr;
	if (m_nextItr)
		m_nextItr->m_prevItr = m_prevItr;
	ItrPools::s_itrPool.ReleasePtr(this);
	IndNode* pCurr=m_records,*pDead=m_records;
	while (pCurr)
	{
		pDead = pCurr;
		pCurr = pCurr->next;
		ItrPools::s_nodePool.ReleasePtr(pDead);
	}
}
   
bool CSVSDPIterator::IsValid()
{
	return m_isValid;
}

int CSVSDPIterator::GetRecord()
{
	return m_isValid ? m_currRecord->ind : -1;
}
	
void CSVSDPIterator::SetData( int col, OME_SCALAR value )
{
	if(m_isValid)
		m_provider->SetData(m_currRecord->ind, col, value);
}

void CSVSDPIterator::SetData( int col, int value )
{
	if(m_isValid)
		m_provider->SetData(m_currRecord->ind, col, value);
}

void CSVSDPIterator::SetData( int col, const OMEChar* value )
{
	if(m_isValid)
		m_provider->SetData(m_currRecord->ind, col, value);
}
	
bool CSVSDPIterator::GetData( int col, OME_SCALAR &value )
{
	return m_isValid ? m_provider->GetData(m_currRecord->ind, col, value) : false;
}

bool CSVSDPIterator::GetData( int col, int &value )
{
	return m_isValid ? m_provider->GetData(m_currRecord->ind, col, value) : false;
}

bool CSVSDPIterator::GetData( int col, OMEChar* value, int maxChars )
{
	return m_isValid ? m_provider->GetData(m_currRecord->ind, col, value, maxChars) : false;
}

bool CSVSDPIterator::GetData( int col, bool &value )
{
	return m_isValid ? m_provider->GetData(m_currRecord->ind, col, value) : false;
}

bool CSVSDPIterator::Next()
{
	if(m_isValid)
	{ 
		//if (m_totRecords != numeric_limits<unsigned int>::max())
		m_currRecord = m_currRecord->next;
		m_isValid = m_currRecord != NULL;
		//else //unsigneds wrap around.
		//	m_isValid=(m_dir == ISpatialDataProvider::ITR_FORWARD ? ++m_currRecord : --m_currRecord) < m_provider->GetRowCount();
			
	}
	return m_isValid;
}

bool CSVSDPIterator::Prev()
{
	if (m_isValid)
	{
		//if (m_totRecords != numeric_limits<unsigned int>::max())
		m_currRecord = m_currRecord->prev;
			m_isValid = m_currRecord!=NULL;
		//else //unsigneds wrap around.
		//	m_isValid = (m_dir == ISpatialDataProvider::ITR_BACKWARD ? ++m_currRecord : --m_currRecord) < m_provider->GetRowCount();

	}
	return m_isValid;
}

bool CSVSDPIterator::IsAtBeginning()
{
	return m_isValid && m_currRecord->prev==NULL;
}

bool CSVSDPIterator::IsAtEnd()
{
	return m_isValid && m_currRecord->next==NULL;
}

void CSVSDPIterator::Reset()
{
	//if(m_isValid)
		m_currRecord=m_records;
		m_isValid = true;
}
