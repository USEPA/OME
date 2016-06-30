#include "StdAfx.h"
#include "EnvSpatialDataProvider.h"
#include "OMEParser.h"

/** Simple constructor.
	@param pIduLayer Pointer to Envision MapLayer to query against.
*/
EnvSpatialDataProvider::EnvSpatialDataProvider(MapLayer *pIduLayer)
   : ISpatialDataProvider()
   , m_pIduLayer(pIduLayer)
   , m_colArea(-1)
   , m_lastNeighborRecord(-1)
   {
   m_pQE = new QueryEngine(pIduLayer);
   m_colArea = pIduLayer->GetFieldCol("AREA");

   EnvSDPIterator *pIterator = new EnvSDPIterator(this);
   m_iteratorArray.Add(pIterator);
   }


EnvSpatialDataProvider::~EnvSpatialDataProvider(void)
   {
   //normally, there would need to be a piece of code here that invalidates all existing
   //iterators for this provider.
   if (m_pQE)
      delete m_pQE;

   m_iteratorArray.RemoveAll();
   }


int EnvSpatialDataProvider::GetCapabilities(void) const
   {
   return POINT_SUPPORT | GRID_SUPPORT | QUERY_SUPPORT | NEIGHBOR_SUPPORT | ITR_SUPPORT | BI_ITR_SUPPORT | RW_ACCESS;
   //HEX_SUPPORT=		
   }


int EnvSpatialDataProvider::Load(const OMEChar* filename, OMEParser* pPrsr)
   {
   return LOAD_SUCCESS;  // Envision will have already loaded the file
   }


int EnvSpatialDataProvider::Save(const OMEChar* filename, int flags)
   {
   return SAVE_UNSUPPORTED;   // managed by Envision
   }


void EnvSpatialDataProvider::GetExtents(OME_SCALAR &vxMin, OME_SCALAR &vxMax, OME_SCALAR &vyMin, OME_SCALAR &vyMax) const
   {
   ASSERT(m_pIduLayer != NULL);
   m_pIduLayer->GetExtents(vxMin, vxMax, vyMin, vyMax);
   }


int EnvSpatialDataProvider::GetRowCount() const
   {
   ASSERT(m_pIduLayer != NULL);
   return m_pIduLayer->GetRowCount();
   }


int EnvSpatialDataProvider::GetColumnCount() const
   {
   ASSERT(m_pIduLayer != NULL);
   return m_pIduLayer->GetColCount();
   }

int EnvSpatialDataProvider::GetFieldCol(const OMEChar* label) const
   {
   ASSERT(m_pIduLayer != NULL);
   return m_pIduLayer->GetFieldCol(label);
   }

int EnvSpatialDataProvider::AddFieldCol(const OMEChar* label, const OME_SCALAR initVal)
   {
   ASSERT(m_pIduLayer != NULL);

   //if coverage is saved to dbf, limit is 10 chars + terminal char (11)
   const size_t DBF_LIM = 11;
   int col = -1;

   size_t len = strlen(label)+1;
   OMEChar* newLabel = new OMEChar[len]();
   EnvisionifyLabel(label, newLabel, len>DBF_LIM ? DBF_LIM : len);
   bool success = m_pIduLayer->CheckCol(col, newLabel, TYPE_FLOAT, CC_AUTOADD);

   if (success)
      {
      for (int i = 0; i < m_pIduLayer->GetRecordCount(); i++)
         m_pIduLayer->SetData(i, col, initVal);
      }
   return col;
   }

int EnvSpatialDataProvider::AddFieldCol(const OMEChar* label, const OME_SCALAR* vals, const OME_SCALAR & count)
   {
   ASSERT(m_pIduLayer != NULL);
   int col = -1;
   const size_t DBF_LIM = 11;
   size_t len=strlen(label)+1;
   OMEChar* newLabel = new OMEChar[len]();
   EnvisionifyLabel(label, newLabel, len>DBF_LIM ? DBF_LIM : len);
   bool success = m_pIduLayer->CheckCol(col, newLabel, TYPE_FLOAT, CC_AUTOADD);

   if (success)
      {
      for (int i = 0; i < m_pIduLayer->GetRecordCount() && i < count; i++)
         m_pIduLayer->SetData(i, col, vals[i]);
      }
   delete[] newLabel;
   return col;
   }

int EnvSpatialDataProvider::AddFieldCol(const OMEChar* label, ValueArchive* pArch, const size_t & start, const OME_SCALAR & count)
{
	ASSERT(m_pIduLayer != NULL);
	int col = -1;
   const size_t DBF_LIM = 11;
   size_t len = strlen(label)+1;
   OMEChar* newLabel = new OMEChar[len]();
   EnvisionifyLabel(label, newLabel, len>DBF_LIM ? DBF_LIM : len);
	bool success = m_pIduLayer->CheckCol(col, newLabel, TYPE_FLOAT, CC_AUTOADD);

	if (success)
	{
		for (int i = 0; i < m_pIduLayer->GetRecordCount() && i < count; i++)
			m_pIduLayer->SetData(i, col, (*pArch)[start+i]);
	}
   delete[] newLabel;
	return col;
}

bool EnvSpatialDataProvider::NextTo(int polyIndex, const OMEChar* query)
   {
   ASSERT(m_pIduLayer != NULL);
   ASSERT(m_pQE != NULL);

   Query *pQuery = m_pQE->ParseQuery(query, 0, "OMEAdapter");
   if (pQuery == NULL)
      {
      CString msg("OME Adapter: Unable to parse query '");
      msg += query;
      msg += "'";
      Report::ErrorMsg(msg);
      return false;
      }

   RefreshNeighborCache(polyIndex);
   // see if any neighbors satisfy the query
   bool found = false;
   for (int i = 0; i < m_nCount; i++)
      {
      int nIndex = m_neighborCache[i];
      m_pQE->SetCurrentRecord(nIndex);

      bool result = false;
      bool ok = pQuery->Run(nIndex, result);

      if (ok)
         {
         found = true;
         break;
         }
      }

   m_pQE->RemoveQuery(pQuery);
   return found;
   }


OME_SCALAR EnvSpatialDataProvider::NextToArea(int polyIndex, const OMEChar* query)
   {
   ASSERT(m_pIduLayer != NULL);
   ASSERT(m_pQE != NULL);

   if (m_colArea < 0)
      return 0;

   Query *pQuery = m_pQE->ParseQuery(query, 0, "OMEAdapter");
   if (pQuery == NULL)
      {
      CString msg("OME Adapter: Unable to parse query '");
      msg += query;
      msg += "'";
      Report::ErrorMsg(msg);
      return 0;
      }

   RefreshNeighborCache(polyIndex);
   // see if any neighbors satisfy the query
   OME_SCALAR nArea = 0;

   for (int i = 0; i < m_nCount; i++)
      {
      int nIndex = m_neighborCache[i];
      m_pQE->SetCurrentRecord(nIndex);

      bool result = false;
      bool ok = pQuery->Run(nIndex, result);

      if (ok)
         {
         OME_SCALAR area = 0;
         m_pIduLayer->GetData(nIndex, m_colArea, area);
         nArea += area;
         }
      }

   m_pQE->RemoveQuery(pQuery);
   return nArea;
   }


bool EnvSpatialDataProvider::Within(int polyIndex, const OMEChar* query, OME_SCALAR distance)
   {
   ASSERT(m_pIduLayer != NULL);
   ASSERT(m_pQE != NULL);

   Query *pQuery = m_pQE->ParseQuery(query, 0, "OMEAdapter");
   if (pQuery == NULL)
      {
      CString msg("OME Adapter: Unable to parse query '");
      msg += query;
      msg += "'";
      Report::ErrorMsg(msg);
      return false;
      }

   RefreshNeighborCache(polyIndex, distance);
  
   // see if any neighbors satisfy the query
   bool found = false;
   for (int i = 0; i < m_nCount; i++)
      {
      int nIndex = m_neighborCache[i];
      m_pQE->SetCurrentRecord(nIndex);

      bool result = false;
      bool ok = pQuery->Run(nIndex, result);

      if (ok)
         {
         found = true;
         break;
         }
      }

   m_pQE->RemoveQuery(pQuery);
   return found;
   }


OME_SCALAR EnvSpatialDataProvider::WithinArea(int polyIndex, const OMEChar* query, OME_SCALAR distance)
   {
   ASSERT(m_pIduLayer != NULL);
   ASSERT(m_pQE != NULL);

   if (m_colArea < 0)
      return 0;

   Query *pQuery = m_pQE->ParseQuery(query, 0, "OMEAdapter");
   if (pQuery == NULL)
      {
      CString msg("OME Adapter: Unable to parse query '");
      msg += query;
      msg += "'";
      Report::ErrorMsg(msg);
      return 0;
      }

   RefreshNeighborCache(polyIndex, distance);
   // see if any neighbors satisfy the query
   OME_SCALAR nArea = 0;

   for (int i = 0; i < m_nCount; i++)
      {
      int nIndex = m_neighborCache[i];
      m_pQE->SetCurrentRecord(nIndex);

      bool result = false;
      bool ok = pQuery->Run(nIndex, result);

      if (ok)
         {
         OME_SCALAR area = 0;
         m_pIduLayer->GetData(nIndex, m_colArea, area);
         nArea += area;
         }
      }

   m_pQE->RemoveQuery(pQuery);
   return nArea;
   }


bool EnvSpatialDataProvider::GetCellSize(OME_SCALAR &width, OME_SCALAR &height)
   {
   m_pIduLayer->GetCellSize(width, height);
   return true;
   }

void EnvSpatialDataProvider::SetData(int rec, int col, OME_SCALAR value)
   {
   ASSERT(m_pIduLayer != NULL);
   m_pIduLayer->SetData(rec, col, value);
   }

void EnvSpatialDataProvider::SetData(int rec, int col, OME_SCALAR value)
   {
   ASSERT(m_pIduLayer != NULL);
   m_pIduLayer->SetData(rec, col, value);
   }

void EnvSpatialDataProvider::SetData(int rec, int col, int value)
   {
   ASSERT(m_pIduLayer != NULL);
   m_pIduLayer->SetData(rec, col, value);
   }

void EnvSpatialDataProvider::SetData(int rec, int col, const OMEChar* value)
   {
   ASSERT(m_pIduLayer != NULL);
   m_pIduLayer->SetData(rec, col, value);
   }


bool EnvSpatialDataProvider::GetData(int rec, int col, OME_SCALAR &value)
   {
   ASSERT(m_pIduLayer != NULL);
   return m_pIduLayer->GetData(rec, col, value);
   }

bool EnvSpatialDataProvider::GetData(int rec, int col, OME_SCALAR &value)
   {
   ASSERT(m_pIduLayer != NULL);
   return m_pIduLayer->GetData(rec, col, value);
   }

bool EnvSpatialDataProvider::GetData(int rec, int col, int &value)
   {
   ASSERT(m_pIduLayer != NULL);
   return m_pIduLayer->GetData(rec, col, value);
   }

bool EnvSpatialDataProvider::GetData(int rec, int col, OMEChar* value, int maxChars)
   {
   ASSERT(m_pIduLayer != NULL);
   CString _value;
   bool result = m_pIduLayer->GetData(rec, col, _value);
   if (result)
      lstrcpyn(value, _value, maxChars);

   return result;
   }


bool EnvSpatialDataProvider::GetData(int rec, int col, bool &value)
   {
   ASSERT(m_pIduLayer != NULL);
   return m_pIduLayer->GetData(rec, col, value);
   }


bool EnvSpatialDataProvider::GetDataMinMax(int col, OME_SCALAR &minValue, OME_SCALAR &maxValue)
   {
   ASSERT(m_pIduLayer != NULL);
   return m_pIduLayer->GetDataMinMax(col, &minValue, &maxValue);
   }

int* EnvSpatialDataProvider::GetNeighbors(const int & rec, int & count)
   {
   int* out = NULL;

   //This may not initialize correctly. I may need to look into this.
   EnvSDPQueryIterator *itr = (EnvSDPQueryIterator*)&(GetNextTo(rec-1));

   std::list<int> temp;
   for (; !itr->IsAtEnd(); ++itr)
      temp.push_back(itr->GetRecord());

   count = temp.size();

   if (count)
      {
      out = new int[count];

      unsigned int i = 0;
      for (auto lItr = temp.begin(); lItr != temp.end(); ++lItr, i++)
         out[i] = *lItr;
      }
   itr->Dispose();

   return out;
   }

unsigned int EnvSpatialDataProvider::GetNeighborCount(const int & rec) const
   {
   //not the greatest way to do this, but it work work for now

   RefreshNeighborCache(rec-1);
   return m_nCount;
   }

int EnvSpatialDataProvider::GetNeighborRecord(const int & rec, const int & neighbor)
   {
   int ret = -1;

   //neighbor record will likely be accessed repreatedly; cache lookup results

   RefreshNeighborCache(rec-1);
   // see if any neighbors satisfy the query
   if (neighbor >= 0 && neighbor < m_nCount)
      ret = m_neighborCache[neighbor-1];
   return ret;
   }

ISDPIterator& EnvSpatialDataProvider::GetNextTo(int polyIndex, unsigned int* count)
   {
   //return an iterator that points to the beginning of the list of neighbor records to 
   //polyIndex that satisfy query. If none, return invalid iterator
   ASSERT(m_pIduLayer != NULL);
   ASSERT(m_pQE != NULL);

   //script level uses a 1 based index; shift to 0 based index
   polyIndex--;

   EnvSDPQueryIterator *pIterator = new EnvSDPQueryIterator(this);
   m_iteratorArray.Add(pIterator);

   ////ensure that query is a modifiable buffer
   //OMEChar* buffer=new OMEChar[strlen(query)+1];
   //strcpy(buffer,query);

   //Query *pQuery = m_pQE->ParseQuery( buffer, 0, "OMEAdapter" );
   //if ( pQuery == NULL )
   //   {
   //   CString msg( "OME Adapter: Unable to parse query '" );
   //   msg += query;
   //   msg += "'";
   //   Report::ErrorMsg( msg );

   //   pIterator->m_isValid = false;
   //   return *pIterator;
   //   }

   RefreshNeighborCache(polyIndex);
   if (count)
      *count = m_nCount;
   // see if any neighbors satisfy the query
   bool found = false;
   for (int i = 0; i < m_nCount; i++)
      {
      int nIndex = m_neighborCache[i];
      m_pQE->SetCurrentRecord(nIndex);

      bool result = false;
      //bool ok = pQuery->Run( nIndex, result );

      //if ( ok )
      pIterator->AddIndex(nIndex);
      }

   pIterator->Reset();
   //m_pQE->RemoveQuery( pQuery );
   return *pIterator;
   }

ISDPIterator& EnvSpatialDataProvider::GetIterator(const ITR_DIR & kind)
   {
   /// \todo update to return real iterators.
   return EnvSDPQueryIterator(this);
   }

ISDPIterator& EnvSpatialDataProvider::GetWithin(int polyIndex, const OMEChar* query, OME_SCALAR distance)
   {
   //return an iterator that points to the beginning of the list of neighbor records to 
   //polyIndex that satisfy query. If none, return invalid iterator
   ASSERT(m_pIduLayer != NULL);
   ASSERT(m_pQE != NULL);

   EnvSDPQueryIterator *pIterator = new EnvSDPQueryIterator(this);
   m_iteratorArray.Add(pIterator);

   Query *pQuery = m_pQE->ParseQuery(query, 0, "OMEAdapter");
   if (pQuery == NULL)
      {
      CString msg("OME Adapter: Unable to parse query '");
      msg += query;
      msg += "'";
      Report::ErrorMsg(msg);

      pIterator->m_isValid = false;
      return *pIterator;
      }

   Poly *pPoly = m_pIduLayer->GetPolygon(polyIndex);

   int neighbors[1024];
   int count = m_pIduLayer->GetNearbyPolys(pPoly, neighbors, NULL, 1024, distance);

   // see if any neighbors satisfy the query
   bool found = false;
   for (int i = 0; i < count; i++)
      {
      int nIndex = neighbors[i];
      m_pQE->SetCurrentRecord(nIndex);

      bool result = false;
      bool ok = pQuery->Run(nIndex, result);

      if (ok)
         pIterator->AddIndex(nIndex);
      }

   m_pQE->RemoveQuery(pQuery);
   return *pIterator;
   }

/** @return true if the associated MapLayer is read-only; false otherwise.*/
bool EnvSpatialDataProvider::GetMapLayerReadOnly() const
   {
   ASSERT(m_pIduLayer != NULL);
   return m_pIduLayer->m_readOnly;
   }

/** Override the associated MapLayer's ReadOnly flag. This is sometimes
	needed for some non-mutating operations.
	@param ro The setting to apply to the readonly flag.
*/
void EnvSpatialDataProvider::SetMapLayerReadOnly(bool ro)
   {
   ASSERT(m_pIduLayer != NULL);
   m_pIduLayer->m_readOnly = ro;
   }
//////////////////////////////////////////////////////////////////////////////////////////////

/** Remove Iterator from parent container.*/
void EnvSDPIterator::Dispose()
   {
   //tell the object storing the iterator that we would like to permanently remove it from memory.
   EnvSpatialDataProvider * pProvider = dynamic_cast<EnvSpatialDataProvider*>(m_provider);

   if (pProvider)
      pProvider->m_iteratorArray.Remove(static_cast<ISDPIterator*>(this));
   }

