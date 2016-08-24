#include "RecordMgr.h"
#include "omeobjects/Model.h"
#include "ValueArchive.h"
#include "BaseManager.h"
#include <fstream>
#include <cstdio>
#include <stdexcept>
#include "FileExts.h"
#include "omeobjects/VarArray.h"

#ifndef RM_KEYFRAME_INT
#define RM_KEYFRAME_INT		1
#endif

const OMEChar DIR_NAME[]="OMETmp/";
//#define IRIND(x) ((size_t)(((x)-m_startTime)/m_interval))
#define IRTIME(x) ((OME_SCALAR)(((x)*m_interval)+m_startTime))

#define IRIND(x) ((size_t)((((x)-m_startTime)/m_interval)+0.5)) //+.5 is for rounding (assumes that x-m_startTime is evenly divisible by m_interval)


#define NO_LOADED_TIME	(((size_t)0)-1)
/** Constructor.
	@param pArch Pointer to ValueArchive to associate with the RecordMgr.
	@param startTime The time at the beginning of the simulation.
	@param reportInterval The interval on which records will be recorded.
	@param trgDir Path to a directory to use for storing temporary record files; ignored if useDisk is false.
	@param prefix The name to apply to temporary record files.
*/
RecordMgr::RecordMgr(ValueArchive* pArch, const OME_SCALAR & startTime, const OME_SCALAR & reportInterval, const STLString & trgDir, const STLString & prefix)
	:m_cacheToDisk(false), m_dirPath(trgDir), m_startTime(startTime), m_interval(reportInterval), m_totEntries(0),
	m_loadedRecord(pArch->m_values.size(), pArch->m_modelMap.size(), pArch->m_offsetMap.size()), m_pSelectedRecord(NULL),
	m_mdlTmpName(prefix), m_lastTimeInd(NO_LOADED_TIME), m_useMin(false)
{
	if(!m_dirPath.empty())
	{
		m_dirPath=PrepPath(DIR_NAME,m_dirPath);
		
		if(!OMECreateDirectory(m_dirPath.c_str()))
			throw OMEIOException("RecordMgr:", "could not create tmp directory");
		
		m_cacheToDisk = true;
		m_pSelectedRecord = &m_loadedRecord;
	}
}

/** Copy constructor.
	@param rm The RecordMgr to copy.
*/
RecordMgr::RecordMgr(const RecordMgr & rm)
	:m_cacheToDisk(rm.m_cacheToDisk), m_dirPath(rm.m_dirPath), m_startTime(rm.m_startTime), 
	m_interval(rm.m_interval), m_totEntries(rm.m_totEntries),m_loadedRecord(rm.m_loadedRecord), 
	m_pSelectedRecord(NULL), m_mdlTmpName(rm.m_mdlTmpName), m_lastTimeInd(rm.m_lastTimeInd), m_useMin(rm.m_useMin)
{
	m_pSelectedRecord = &m_loadedRecord;
}

RecordMgr::~RecordMgr()
{
	Clear();
}

/** Record the current state as appropriate.
	@param timeStamp The timestamp to assign to the new record.
	@param pArch Pointer to ValueArchive to record values from.
*/
void RecordMgr::RecordState(const OME_SCALAR & timeStamp,ValueArchive* pArch)
{

	//ensure that the most recent iteration is loaded (calls to lookbacks such as var_delay can change this.
	//if (m_cacheToDisk)
	//{
	//	size_t prevInd = IRIND(timeStamp);
	//	if (prevInd)
	//	{
	//		--prevInd;
	//		LoadRecordFromDisk(prevInd);
	//	}
	//}
	////else use an empty placeholder here
	//m_lastTimeInd = IRIND(timeStamp);
	if(!m_cacheToDisk)
	{
		size_t ind=IRIND(timeStamp);
		
		//if(m_allocated)
			DoPush(ind,pArch);
		/*else
		{
			DoFullCopy(ind);
			m_allocated=true;
		}*/
		//for(i=0; i<m_pRecModel->m_portVals.size(); ++i)
		//	m_records[timeStamp].m_portVals[i]=m_pRecModel->m_portVals[i];
	}
	else
	{	
		CopyArchiveToCache(pArch, timeStamp);
		
		//m_records[timeStamp].m_fileName=outName;
		
		WriteRecordToDisk(PathForInd(m_lastTimeInd),timeStamp,pArch);
	}
	++m_totEntries;
}

/** Record all values for the current state regardless of recording settings.
	@param timeStamp The timestamp to assign to the new record.
	@param pArch Pointer to ValueArchive to record values from.
*/
void RecordMgr::RecordFullState(const OME_SCALAR & timeStamp, ValueArchive* pArch)
{

	if (!m_cacheToDisk)
	{
		size_t ind = IRIND(timeStamp);
		//if(m_allocated)
		DoFullCopy(ind, pArch);
	}
	else
	{

		CopyArchiveToCache(pArch, timeStamp);

		//m_records[timeStamp].m_fileName=outName;
		STLString outPath = PathForInd(m_lastTimeInd);
		ofstream outFile(outPath, ios_base::out | ios_base::binary);
		char outFlag = 'f';
		outFile.write(&outFlag, sizeof(char));

		WriteFullRecord(outFile, pArch);
	}
	++m_totEntries;
}

/** @return An array contain all times at which records were recorded.*/
OMESclrArray RecordMgr::GetRecordTimes() const
{
	OMESclrArray ret;
	OME_SCALAR endTime=(m_interval*m_totEntries);
	for(OME_SCALAR time=0; time<endTime; time+=m_interval)
		ret.push_back(time);

	return ret;
}

/** Enable writing records to disk.
	@param trgDir Path to directory where record files will be written.
*/
void RecordMgr::EnableDiskMode(const STLString & trgDir)
{
	m_cacheToDisk=!trgDir.empty();
	m_dirPath=trgDir;
	if(!m_dirPath.empty())
	{
		m_dirPath=PrepPath(DIR_NAME,m_dirPath);
		if(!OMECreateDirectory(m_dirPath.c_str()))
			throw OMEIOException("RecordMgr:", "could not create tmp directory");
		m_pSelectedRecord = &m_loadedRecord;
	}
}

/** Move a specific record into the record cache.
	@param time The timestamp of the record to cache.
*/
void RecordMgr::CacheRecord(const OME_SCALAR & time)
{
	CacheRecord(IRIND(time));
}

/** Move a specific record into the record cache.
	@param tInd The index of the record to cache.
*/
void RecordMgr::CacheRecord(const size_t & tInd)
{
	if (!m_cacheToDisk)
		m_pSelectedRecord = &m_recordList[tInd];
	else
	{
		LoadRecordFromDisk(tInd);
		m_pSelectedRecord = &m_loadedRecord;
	}
}

/** Copy Values from record to ValueArchive.
	@param time The timestamp of the record to copy.
	@param pArch Pointer to the target ValueArchive.
*/
void RecordMgr::SyncArchiveWithCache(const OME_SCALAR & time, ValueArchive* pArch)
{
	SyncArchiveWithCache(IRIND(time),pArch);
}

/** Copy Values from record to ValueArchive.
	@param tInd The index of the record to copy.
	@param pArch Pointer to the target ValueArchive.
*/
void RecordMgr::SyncArchiveWithCache(const size_t & tInd,ValueArchive* pArch)
{
	if (m_cacheToDisk)
	{
		LoadRecordFromDisk(tInd);
		m_pSelectedRecord = &m_loadedRecord;
	}
	else
		m_pSelectedRecord = &m_recordList[tInd];
	//SyncArchiveWithCache(tInd,pArch);
	//load vals
	std::copy(m_pSelectedRecord->m_values.begin(), m_pSelectedRecord->m_values.end(), pArch->m_values.begin());
	//load ind data
	size_t i = 0;
	for (auto itr = pArch->m_offsetMap.begin(); itr != pArch->m_offsetMap.end(); ++itr, ++i)
	{
		(*itr).second.m_ind = m_pSelectedRecord->m_objInds[i];
		if ((*itr).second.m_pSrc->IsOMEType(OME_VARARRAY))
		{
			//load array dims
			((VarArray*)(*itr).second.m_pSrc)->GetDims() = m_pSelectedRecord->m_sizeOverrides[i];
			((VarArray*)(*itr).second.m_pSrc)->RecalcSize();
		}
	}

	//load flags
	size_t currFlags = 0;
	size_t currSub = 0;
	size_t recCount;
	for (auto itr = pArch->m_modelMap.begin(); itr != pArch->m_modelMap.end(); ++itr)
	{
		Model* pMdl = (*itr).second.pRefModel;
		recCount = m_pSelectedRecord->m_instCounts[currSub++];
		for (i = 0; i < recCount; ++i)
			pMdl->m_instances[i]->m_flags = m_pSelectedRecord->m_loadedFlags[currFlags++];
		for (; i < pMdl->m_instances.size(); ++i)
		{
			pMdl->m_instances[i]->Kill();
			pMdl->m_instances[i]->ApplyMortality();
		}
	}

	//sync restored inds with model inds
	pArch->RefreshInds();
}

/** Clear all records from the RecordMgr.*/
void RecordMgr::Clear()
{
	
	if(m_cacheToDisk)
	{
	//	//clear temp files on cleanup
		oSTLSStream pathStrm;

		for(size_t i=0; i<m_totEntries; ++i)
		{
			std::remove(PathForInd(i).c_str());
		}
	}
	m_totEntries = 0;
	ClearMinTrackList();
}

/** @return A map containing all values across all timesteps.*/
RecordMgr::FullValueMap RecordMgr::GetAllRecordedValues()
{
	size_t currInd = m_lastTimeInd;
	FullValueMap ret;
	for (size_t i = 0; i < m_totEntries; ++i)
	{
		CacheRecord(i);
		ret[i] = m_pSelectedRecord->m_values;
	}

	//restore currently loaded record
	CacheRecord(currInd);
	return ret;
}

/** Retrieve all values across all timesteps.
	@param fvm A FullValueMap which will contain all the values on return.
*/
void RecordMgr::GetAllRecordedValues(RecordMgr::FullValueMap & fvm)
{
	size_t currInd = m_lastTimeInd;
	for (size_t i = 0; i < m_totEntries; ++i)
	{
		CacheRecord(i);
		for (OME_SCALAR & v : m_pSelectedRecord->m_values)
			fvm[i].push_back(v);
	}

	//restore currently loaded record
	CacheRecord(currInd);
}

/** Write a record to disk.
	@param path Path to the file to write to.
	@param timeStamp The time stamp for the record being written.
	@param pArch Pointer to ValueArchive containing desired values to record.
*/
void RecordMgr::WriteRecordToDisk(const STLString & path, const OME_SCALAR & timeStamp, ValueArchive* pArch)
{
	
	ofstream outFile(path,ios_base::out | ios_base::binary);
	
	char writeMode;
	if (m_useMin)
		writeMode = 'm';
	else
		writeMode=(IRIND(timeStamp) % RM_KEYFRAME_INT) == 0 ? 'f' : 'd';
	
	if(outFile.good())
	{
		//ensure the current time is loaded (lookbacks may have changed the record)
		//if (timeStamp>=m_startTime+m_interval)
		//	LoadRecordFromDisk(timeStamp-m_interval);
		
		//write out mode
		outFile.write(&writeMode, sizeof(char));

		switch (writeMode)
		{
		case 'f':
			WriteFullRecord(outFile, pArch);
			break;
		case 'd':
			WriteDifferenceRecord(outFile,pArch);
			break;
		case 'm':
			WriteMinRecord(outFile, pArch);
			//m_lastTimeInd = NO_LOADED_TIME;		//force reload when necessary
			break;
		default:
			throw OMEIOException("WriteRecordToDisk","Unknown write mode");
		}
	
	}
	else
	{
		throw OMEIOException("WriteRecordToDisk", "Failed to open file for writing.");
	}
}

/** Write out a record to disk containing all values.
	@param outFile Stream pointing to target file.
	@param pArch Pointer to ValueArchive to take values from.
*/
void RecordMgr::WriteFullRecord(ofstream & outFile,ValueArchive* pArch)
{

	WriteModelFlags(outFile, pArch);
	//write out index count
	//write out indices.
		//if varArray, write out vardims
		//else write 0
	//write out val count
	//write out values.


	//write out index count
	size_t currSize = pArch->m_offsetMap.size();
	outFile.write((char*)&currSize, sizeof(size_t));
	
	for (auto itr = pArch->m_offsetMap.begin(); itr != pArch->m_offsetMap.end(); ++itr)
	{
		currSize = 0;
		//write out indices.
		outFile.write((char*)&(*itr).second.m_ind,sizeof(size_t));
		if (!(*itr).second.m_pSrc->IsOMEType(OME_VARARRAY))
			outFile.write((char*)&currSize, sizeof(unsigned short)); //else write 0
		else
		{
			//if varArray, write out vardims
			VarArray* pVa = (VarArray*)(*itr).second.m_pSrc;
			currSize = pVa->GetDimCount();
			outFile.write((char*)&currSize, sizeof(unsigned short));

			for (unsigned short i = 0; i < pVa->GetDimCount(); ++i)
			{
				currSize = pVa->GetDim(i);
				outFile.write((char*)&currSize, sizeof(size_t));
			}
		}
	}
	
	//write out val count
	currSize = pArch->m_values.size();
	outFile.write((char*)&currSize, sizeof(size_t));

	//write out values.
	for (auto itr = pArch->m_values.begin(); itr != pArch->m_values.end(); ++itr)
		outFile.write((char*)&(*itr), sizeof(OME_SCALAR));

}

/** Write out a record to disk only storing values that change at the target timestep.
	@param outFile Stream pointing to target file.
	@param pArch Pointer to ValueArchive to take values from.
*/
void RecordMgr::WriteDifferenceRecord(ofstream & outFile,ValueArchive* pArch)
{
	/*all values are essentially treated as arrays
		write total number of changed entries
		write refNode index (map struct always sorts in key order)
		write total number of entries
		write number of changed entries
		write number of dims
			if !varArray, 0
			else write out each dim
		write offset/value pair for each changed entry
	*/
	/// @todo update for ValueArchive
	//started work.
	//LoadRecordFromDisk(m_lastTimeInd);
	//WriteModelFlags(outFile, pArch);

	//size_t currCount = 0;
	//streampos changedPos, countPos, currPos,mRefPos;
	////document current position for total record later on
	//changedPos = outFile.tellp();
	//outFile.write((char*)&currCount, sizeof(size_t));

	//auto mItr = pArch->m_modelSet.begin();
	//for (size_t m = 0; mItr != pArch->m_modelSet.end(); ++m, ++mItr)
	//{

	//}
	//m_lastTimeInd += m_interval;
///////////////////////////////////////////////////
	//old code:
	//
	//size_t ind = 0;
	//size_t i, j, k;
	//streampos countPos, currPos, singlePos, arrPos, restartPos;
	//size_t countSize, arrSize;
	//size_t currArrOffs;
	//for (i = 0; i<m_pRecModel->m_instances.size(); ++i)
	//{
	//	countSize = 0;
	//	//make space for single val count entry
	//	singlePos = outFile.tellp();
	//	outFile.write((char*)&countSize, sizeof(size_t));
	//	//make space for arr val count entry
	//	arrPos = outFile.tellp();
	//	outFile.write((char*)&countSize, sizeof(size_t));


	//	if (m_pRecModel->m_instances[i]->IsActive())
	//	{

	//		for (ind = 0; ind < m_pRecModel->m_instances[i]->m_singleValues.size(); ++ind)
	//		{
	//			if (m_loadedRecord[i].m_singleVals[ind] != m_pRecModel->m_instances[i]->m_singleValues[ind])
	//			{
	//				m_loadedRecord[i].m_singleVals[ind] = m_pRecModel->m_instances[i]->m_singleValues[ind];
	//				outFile.write((char*)&ind, sizeof(size_t));
	//				outFile.write((char*)&m_loadedRecord[i].m_singleVals[ind], sizeof(OME_SCALAR));
	//				++countSize;
	//			}
	//		}

	//		//write out single val count
	//		currPos = outFile.tellp();
	//		outFile.seekp(singlePos);
	//		outFile.write((char*)&countSize, sizeof(size_t));
	//		outFile.seekp(currPos);
	//		currArrOffs = 0;
	//		
	//		countSize = 0;
	//		for (j = 0; j < m_pRecModel->m_instances[i]->m_arrayValues.size(); ++j)
	//		{
	//			//write array length
	//			SclrVArray& currArray = m_pRecModel->m_instances[i]->m_arrayValues[j];
	//			arrSize = currArray.size();
	//			size_t currArrSize = LengthForArray(j, m_loadedRecord[i]);

	//			if (arrSize>currArrSize)
	//			{
	//				//resize record space
	//				size_t arrDiff = arrSize - currArrSize;
	//				m_loadedRecord[i].m_sizeOverrides[j] = arrSize;
	//				m_loadedRecord[i].m_arrayVals.insert(m_loadedRecord[i].m_arrayVals.begin() + currArrOffs + currArrSize, arrDiff, OME_SCALAR_NAN);

	//				currArrSize = arrSize;
	//			}
	//			restartPos = outFile.tellp(); //save incase there is no array to write
	//			outFile.write((char*)&arrSize, sizeof(size_t));
	//			//grab stream position and save insertion spot
	//			arrSize = 0;
	//			countPos = outFile.tellp();
	//			//reserve space
	//			outFile.write((char*)&arrSize, sizeof(size_t));


	//			//for arrays, inds will be relative offsets
	//			for (k = 0; k < currArray.size(); ++k)
	//			{
	//				if (m_loadedRecord[i].m_arrayVals[currArrOffs + k] != currArray[k])
	//				{
	//					m_loadedRecord[i].m_arrayVals[currArrOffs + k] = currArray[k];
	//					outFile.write((char*)&k, sizeof(size_t));
	//					outFile.write((char*)&m_loadedRecord[i].m_arrayVals[currArrOffs + k], sizeof(OME_SCALAR));
	//					++countSize;
	//					++arrSize;
	//				}
	//			}

	//			currArrOffs += currArrSize;

	//			if (arrSize)
	//			{
	//				//jump back and write counted entries for array
	//				currPos = outFile.tellp();
	//				outFile.seekp(countPos);
	//				outFile.write((char*)&arrSize, sizeof(size_t));
	//				outFile.seekp(currPos);
	//			}
	//			else
	//			{
	//				//reclaim reserved space if there are now array values to save
	//				outFile.seekp(restartPos);
	//			}
	//		}
	//		currPos = outFile.tellp();
	//		outFile.seekp(arrPos);
	//		outFile.write((char*)&countSize, sizeof(size_t));
	//		outFile.seekp(currPos);
	//	}
	//	//else
	//	//{
	//	//	//write starting index of deadspace
	//	//	outFile.write((char*)&ind, sizeof(size_t));
	//	//	size_t tot = m_pRecModel->m_instances[i]->m_singleValues.size();
	//	//	for (j = 0; j < m_pRecModel->m_instances[i]->m_arrayValues.size(); ++j)
	//	//	{
	//	//		for (k = 0; k < m_pRecModel->m_instances[i]->m_arrayValues[j].size(); ++k, ++ind)
	//	//			++tot;
	//	//	}

	//	//	//write end index of dead space
	//	//	outFile.write((char*)&tot, sizeof(size_t));
	//	//}
	//}
}

/** Write out a record to disk only containing values from marked model objects.
	@param outFile Stream pointing to target file.
	@param pArch Pointer to ValueArchive to take values from.
*/
void RecordMgr::WriteMinRecord(ofstream & outFile, ValueArchive* pArch)
{
	//sort minIds?
	if (!m_minRefs.empty())
	{
		WriteModelFlags(outFile, pArch);

		/* Assume m_minRefs does not change at all during run.
			for each record:
				write full valueCount
				write dimCount
				write each dim
				write number of values
				write out all values in succession
		*/
		
		size_t currCount = 0,totCount=0;
		unsigned short dimCount;
		VarArray* pVa;
		Model* pMdl;
		streampos countPos, currPos;
		//reserve and save space for total count
		countPos = outFile.tellp();
		outFile.write((char*)&totCount, sizeof(size_t));

		//determine dimensions
		for (auto itr = m_minRefs.begin(); itr != m_minRefs.end(); ++itr)
		{
		
			pVa = NULL;
			if (!(*itr)->IsOMEType(OME_VARARRAY))
				dimCount = 0;
			else
			{
				pVa = (VarArray*)(*itr);
				dimCount = pVa->GetDimCount();
			}

			currCount = (*itr)->GetInstValCount();

			//write out dim count
			outFile.write((char*)&dimCount, sizeof(unsigned short));
			if (dimCount)
			{
				//write out each dim
				size_t dim;
				for (unsigned short i = 0; i < pVa->GetDimCount(); ++i)
				{
					dim = pVa->GetDim(i);
					outFile.write((char*)&dim, sizeof(size_t));
				}
			}
			//write out valCount
			outFile.write((char*)&currCount, sizeof(size_t));
			//write out each value
#ifdef OME_VARCHCONTIG
			//block copy if possible. this may be a bit faster
			outFile.write((char*)&pArch->m_values[(*itr)->GetModelIndex()], sizeof(OME_SCALAR)*currCount);
#else
			//if we can't guarantee that m_values is contiguous, use a linear copy instead
			for (size_t k = 0; k < currCount; ++k)
			{
				outFile.write((char*)&pArch->m_values[(*itr)->GetModelIndex() + k], sizeof(OME_SCALAR));
			}
#endif
			//add tot total count
			totCount += currCount;
		}

		//jump back and write out total count.
		currPos = outFile.tellp();
		outFile.seekp(countPos);
		outFile.write((char*)&totCount, sizeof(size_t));
		outFile.seekp(currPos);
	}
}

/** Write out model instance state flags as needed.
	@param outFile Stream pointing to target file.
	@param pArch Pointer to ValueArchive to take values from.
*/
void RecordMgr::WriteModelFlags(ofstream & outFile, ValueArchive* pArch)
{
	//write out modelCount
	//write out instances per model
	//write out flag count
	//write out flags

	//count total instances;
	size_t instCount = 0;
	size_t mdlNum = 0;
	size_t currCount;

	size_t currSize = pArch->m_modelMap.size();

	//write out modelCount
	outFile.write((char*)&currSize, sizeof(size_t));

	for (auto itr = pArch->m_modelMap.begin(); itr != pArch->m_modelMap.end(); ++itr, ++mdlNum)
	{
		currCount = (*itr).second.pRefModel->GetNumRawInstances();
		//write out instances per model
		outFile.write((char*)&currCount, sizeof(size_t));
		instCount += currCount;
	}

	//write out flag count
	outFile.write((char*)&instCount, sizeof(size_t));
	//record flags
	size_t i;
	for (auto itr = pArch->m_modelMap.begin(); itr != pArch->m_modelMap.end(); ++itr)
	{
		//write out flags
		for (size_t j = 0; j < (*itr).second.pRefModel->GetNumRawInstances(); ++j)
			outFile.write(&(*itr).second.pRefModel->GetRawInstance(j)->m_flags, sizeof(char));
	}
}

/** Load a record from disk for a specific time.
	@param time The time for which to retrieve the record for.
	@return The number of instances associated with the given time step.
*/
void RecordMgr::LoadRecordFromDisk(const OME_SCALAR & time)
{
	LoadRecordFromDisk(IRIND(time));
}

/** Load a record from disk for a specific time index.
@param trgInd The time index for which to retrieve the record for.
@return The number of instances associated with the given time step.
*/
void RecordMgr::LoadRecordFromDisk(const size_t & trgInd)
{
	size_t currInd;

	if(trgInd>m_lastTimeInd)
	{
		//walk upward toward next timestep
		for(currInd=m_lastTimeInd+1;currInd<=trgInd && LoadDeltasForTime(PathForInd(currInd));++currInd);
		m_lastTimeInd = trgInd;
	}
	else if(trgInd<m_lastTimeInd)
	{
		//walk downward toward previous timestep.
		//for(currInd=m_lastTimeInd-1;currInd>=trgInd && LoadDeltasForTime(PathForInd(currInd),ret);currInd--);
		//walking backwards will miss some changes; start from nearest keyframe record and move forward
		for (currInd = (trgInd/RM_KEYFRAME_INT)*RM_KEYFRAME_INT; currInd <= trgInd && LoadDeltasForTime(PathForInd(currInd)); ++currInd);
		m_lastTimeInd = trgInd;
	}
	//else time is already loaded

}

/** Load sparse values for a given record.
	@param path The path to the record file to load.
	@return true if record loading was successful; false otherwise.
*/
bool RecordMgr::LoadDeltasForTime(const STLString & path)
{
	bool ret=false;

	ifstream inFile(path,ios_base::in | ios_base::binary);
	if (inFile.good())
	{
		//grab mode
		char readMode;
		inFile.read(&readMode, sizeof(char));

		switch (readMode)
		{
		case 'f':
			ret = ReadFullRecord(inFile);
			break;
		case 'd':
			ret = ReadDifferenceRecord(inFile);
			break;
		case 'm':
			ret = ReadMinRecord(inFile);
			break;
		default:
			throw OMEIOException("LoadDeltasForTime","Unknown read mode");
			//ret = false;
		};

		m_loadedRecord.m_loadedRecordType = readMode;
	}
	
	return ret;
}

/** Read in a record to disk containing all values.
	@param inFile Stream pointing to source file.
	@return true If Record was successfully loaded.
*/
bool RecordMgr::ReadFullRecord(ifstream & inFile)
{

	//read in index count
	//read in indices.
		//if dimCount>0, read in dim record
	//read in val count
	//read in values.

	if (ReadModelFlags(inFile))
	{
		size_t entryCount, i;

		//read in index count
		inFile.read((char*)&entryCount, sizeof(size_t));

		unsigned short dimCount;
		ListDims tmpDims;
		size_t currDim;
		if (m_loadedRecord.m_objInds.size() < entryCount)
			m_loadedRecord.m_objInds.resize(entryCount);

		for (size_t currInd = 0; currInd < entryCount; ++currInd)
		{
			//read in indices.
			inFile.read((char*)&m_loadedRecord.m_objInds[currInd], sizeof(size_t));
			inFile.read((char*)&dimCount, sizeof(unsigned short));
			if (dimCount)
			{
				//DBG_PRINTC(std::to_string(dimCount), DBG_GREEN);
				//if dimCount>0, read in dim record
				tmpDims.ClearDims();
				for (i = 0; i < dimCount; ++i)
				{
					inFile.read((char*)&currDim, sizeof(size_t));
					tmpDims.AppendDim(currDim);

				}
				m_loadedRecord.m_sizeOverrides[currInd] = ListDims(tmpDims, 0, true);
			}
		}

		//read in val count
		inFile.read((char*)&m_loadedRecord.m_valCount, sizeof(size_t));
		if (m_loadedRecord.m_valCount>m_loadedRecord.m_values.size())
			m_loadedRecord.m_values.resize(m_loadedRecord.m_valCount, 0.0);

		//read in values.
		for (i = 0; i < m_loadedRecord.m_valCount; ++i)
			inFile.read((char*)&m_loadedRecord.m_values[i], sizeof(OME_SCALAR));
	}
	return true;
}

/** Read in a record to disk only storing values that change at the target timestep.
	@param inFile Stream pointing to source file.
	@return true If Record was successfully loaded.
*/
bool RecordMgr::ReadDifferenceRecord(ifstream & inFile)
{
	/// @todo update for ValueArchive
	
	//size_t i,j;
	//size_t ind;
	//size_t singleCount, arrayCount, entryCount, oldLen, currLen, arrOffs;

	//for (auto itr = m_loadedRecord.begin(); itr != m_loadedRecord.end(); ++itr)
	//{
	//	//load counts
	//	inFile.read((char*)&singleCount, sizeof(size_t));
	//	inFile.read((char*)&arrayCount, sizeof(size_t));

	//	//instance single vals
	//	for (i = 0; i < singleCount; ++i)
	//	{
	//		inFile.read((char*)&ind, sizeof(size_t));
	//		if (inFile.eof())
	//			break;
	//		inFile.read((char*)&(*itr).m_singleVals[ind], sizeof(OME_SCALAR));
	//	}

	//	arrOffs = 0;
	//	//instance array vals
	//	for (i = 0; arrOffs < arrayCount; ++i, arrOffs += currLen)
	//	{
	//		currLen = LengthForArray(i, (*itr));

	//		//grab array size(do nothing with it for now)
	//		inFile.read((char*)&oldLen, sizeof(size_t));
	//		//retrieve number of entries for Array
	//		inFile.read((char*)&entryCount, sizeof(size_t));
	//		for (j = 0; j < entryCount; ++j)
	//		{
	//			inFile.read((char*)&ind, sizeof(size_t));
	//			if (inFile.eof())
	//				break;
	//			inFile.read((char*)&(*itr).m_arrayVals[arrOffs + ind], sizeof(OME_SCALAR));
	//		}


	//		if (currLen > oldLen)
	//		{
	//			for (j = oldLen; j > currLen; ++j)
	//				(*itr).m_arrayVals[arrOffs + j] = OME_SCALAR_NAN;
	//		}
	//	}
	//}

	//return true;
	return false;
}

/** Read in a record to disk only containing values from marked model objects.
	@param inFile Stream pointing to source file.
	@return true If Record was successfully loaded.
*/
bool RecordMgr::ReadMinRecord(ifstream & inFile)
{
	bool ret = !m_minRefs.empty();
	if (ret)
	{
		ret=ReadModelFlags(inFile);
		/* Assume m_minRefs does not change at all during run.
		for each record:
		read totalRecordCount
		read dimCount
		read each dim
		read number of values
		read in all values in succession
		*/
		unsigned short dimCount;
		ListDims tmpDims;
		size_t i, currDim, currMapInd = 0, currCount;

		inFile.read((char*)&m_loadedRecord.m_valCount, sizeof(size_t));
		if (m_loadedRecord.m_values.size() < m_loadedRecord.m_valCount)
			m_loadedRecord.m_values.resize(m_loadedRecord.m_valCount);
		if (m_minRefs.size() > m_loadedRecord.m_objInds.size())
			m_loadedRecord.m_objInds.resize(m_minRefs.size());

		for (auto itr = m_minRefs.begin(); itr != m_minRefs.end(); ++itr)
		{
			inFile.read((char*)&dimCount, sizeof(unsigned short));
			if (dimCount)
			{
				tmpDims.ClearDims();
				for (i = 0; i < dimCount; ++i)
				{
					inFile.read((char*)&currDim, sizeof(size_t));
					tmpDims.AppendDim(currDim);

				}
				m_loadedRecord.m_sizeOverrides[(*itr)->GetModelIndex()] = ListDims(tmpDims, 0, true);
			}
			inFile.read((char*)&currCount, sizeof(size_t));
			for (i = currMapInd; i < currMapInd + currCount; ++i)
			{
				inFile.read((char*)&m_loadedRecord.m_values[i], sizeof(OME_SCALAR));
			}
			m_loadedRecord.m_indMap[(*itr)->GetModelIndex()] = currMapInd;
			currMapInd += currCount;
		}
	}

	//return true only if there is a record to be loaded
	return ret;
}

/** Read in model instance state flags.
	@param inFile Stream pointing to source file.
	@return true If Record was successfully loaded.
*/
bool RecordMgr::ReadModelFlags(ifstream & inFile)
{
	//read in modelCount
	//read in instances per model
	//read in flag count
	//read in flags
	//read in modelCount
	size_t entryCount, i;
	inFile.read((char*)&entryCount, sizeof(size_t));

	if (m_loadedRecord.m_instCounts.size() < entryCount)
		m_loadedRecord.m_instCounts.resize(entryCount);

	for (i = 0; i < entryCount; ++i)
	{
		//read in instances per model
		inFile.read((char*)&(m_loadedRecord.m_instCounts[i]), sizeof(size_t));
	}

	//read in flag count
	inFile.read((char*)&m_loadedRecord.m_flagCount, sizeof(size_t));
	if (m_loadedRecord.m_flagCount>m_loadedRecord.m_loadedFlags.size())
		m_loadedRecord.m_loadedFlags.resize(m_loadedRecord.m_flagCount);
	//read in flags
	for (i = 0; i < m_loadedRecord.m_flagCount; ++i)
		inFile.read(&m_loadedRecord.m_loadedFlags[i], sizeof(char));

	//for now, always return true
	return true;
}

/** Record any values that changed for the current time index.
	@param tInd The current time index.
	@param pArch Pointer to ValueArchive which contains the desired values.
*/
void RecordMgr::DoPush(const size_t & tInd, ValueArchive* pArch)
{
	/// @todo update for ValueArchive
	//if (ind != 0)
	//{
	//	//new references
	//	IRSclrMap* singles = &m_singleTimeMap[ind];
	//	IRSArrayMap* arrays = &m_arrayTimeMap[ind];

	//	OME_SCALAR toTest;
	//	size_t i, j, s;

	//	for (s = 0; s < m_instCount; ++s)
	//	{
	//		for (int i = 0; i < m_singleInstSize; ++i)
	//		{
	//			if (!SingleValForTime(i + m_singleInstSize*s, ind, toTest) || toTest != m_pRecModel->m_instances[s]->m_singleValues[i])
	//				(*singles)[i] = m_pRecModel->m_instances[s]->m_singleValues[i];
	//		}
	//		for (int i = 0; i < m_arrayInstSize; ++i)
	//		{
	//			for (j = 0; j < m_arraySizes[i]; ++j)
	//			{
	//				if (ArrayValForTime(i + m_arrayInstSize*s, j, ind, toTest) || toTest != m_pRecModel->m_instances[s]->m_arrayValues[i][j])
	//					(*arrays)[i][j] = m_pRecModel->m_instances[s]->m_arrayValues[i][j];

	//			}
	//		}
	//	}
	//}
	//else
		DoFullCopy(tInd,pArch);
}

/** Record all values for the provided time index.
	@param tInd The current time index.
	@param pArch Pointer to ValueArchive containing the desired values.
*/
void RecordMgr::DoFullCopy(const size_t & tInd, ValueArchive* pArch)
{
	//snapshot valueArchive
	if (tInd > m_recordList.size())
		m_recordList.resize(tInd+1, ArchiveRecord());
	m_recordList.emplace(m_recordList.begin()+tInd,pArch);
}

/** Pull values from time maps.
	@param tInd time index used for lookup.
	
*/
void RecordMgr::DoPull(const size_t & tInd)
{
	m_pSelectedRecord = &m_recordList[tInd];
}

/** Load a single value for a given time index.
	@param ind record index for value to load.
	@param time timestep for record to load.
	@param val populated with retrieved value upon successful return.
	@return true if value was successfully retrieved; false otherwise.
*/
bool RecordMgr::SingleValForTime(const size_t & ind, const OME_SCALAR & time, OME_SCALAR & val)
{
	return SingleValForTime(ind, IRIND(time), val);
}

/** Load a single value for a given time index.
	@param ind record index for value to load.
	@param tInd time index for record to load.
	@param val populated with retrieved value upon successful return.
	@return true if value was successfully retrieved; false otherwise.
*/
bool RecordMgr::SingleValForTime(size_t ind, const size_t & tInd, OME_SCALAR & val)
{
	bool ret=false;

	if(tInd < m_totEntries)
	{
		CacheRecord(tInd);
		if (m_useMin)
		{
			auto fItr = m_pSelectedRecord->m_indMap.upper_bound(ind);
			--fItr;
			ind = (*fItr).second + (ind - (*fItr).first);
		}
		val = m_pSelectedRecord->m_values[ind];
		ret = true;
	}

	return ret;
}

/** Load an array value for a given time index.
	@param ind record index for value to load.
	@param time timestamp for record to load.
	@param val Pointer to an array of scalars.
	@param inSize The allocated size of the array pointed to by val.
	@param outSize The number of values copied from the record into the array pointed to by val.
	@return true if value was successfully retrieved; false otherwise.
*/
bool RecordMgr::ArrayValForTime(const size_t & ind, const OME_SCALAR & time, OME_SCALAR* val, const size_t & inSize, size_t & outSize)
{
	return ArrayValForTime(ind, IRIND(time), val, inSize, outSize);
}

/** Load an array value for a given time index.
	@param ind record index for value to load.
	@param tInd time index for record to load.
	@param val Pointer to an array of scalars.
	@param inSize The allocated size of the array pointed to by val.
	@param outSize The number of values copied from the record into the array pointed to by val.
	@return true if value was successfully retrieved; false otherwise.
*/
bool RecordMgr::ArrayValForTime(size_t ind, const size_t & tInd, OME_SCALAR* val, const size_t & inSize,size_t & outSize)
{
	bool ret=false;

	if(tInd<m_totEntries)
	{
		CacheRecord(tInd);
		if (m_useMin)
			ind = m_loadedRecord.m_indMap[ind];
		//find offset to array
		size_t i;
		outSize = 0;
		for (i = ind; i < ind + inSize; ++i)
			val[outSize++] = m_pSelectedRecord->m_values[i];

		ret = true;
	}
	return ret;
}

/** Copy Values from ValueArchive to record cache.
	@param pArch Pointer to the source ValueArchive.
	@param timeStamp The time to associate with values in cache.
*/
void RecordMgr::CopyArchiveToCache(ValueArchive* pArch, const OME_SCALAR & timeStamp)
{
	if (m_useMin)
	{
		if (!m_minRefs.empty())
		{
			//only copy values of evals that will have lookup
			size_t i, currMapInd = 0, currCount,currInd;

			m_loadedRecord.m_valCount=0;
			for (auto itr = m_minRefs.begin(); itr != m_minRefs.end(); ++itr)
				m_loadedRecord.m_valCount += (*itr)->GetInstValCount();

			if (m_loadedRecord.m_values.size() < m_loadedRecord.m_valCount)
				m_loadedRecord.m_values.resize(m_loadedRecord.m_valCount);
			if (m_minRefs.size() > m_loadedRecord.m_objInds.size())
				m_loadedRecord.m_objInds.resize(m_minRefs.size());

			for (auto itr = m_minRefs.begin(); itr != m_minRefs.end(); ++itr)
			{
				if ((*itr)->IsOMEType(OME_VARARRAY))
					m_loadedRecord.m_sizeOverrides[(*itr)->GetModelIndex()] = ((VarArray*)(*itr))->GetDims();
				
				currCount = (*itr)->GetInstValCount();
				currInd = (*itr)->GetModelIndex();

				std::copy(pArch->m_values.begin() + currInd, pArch->m_values.begin() + currInd + currCount, m_loadedRecord.m_values.begin() + currMapInd);
				
				m_loadedRecord.m_indMap[(*itr)->GetModelIndex()] = currMapInd;
				currMapInd += currCount;
			}

		}
	}
	else
	{
		//copy all values
		if (m_loadedRecord.m_values.size() < pArch->m_values.size())
			m_loadedRecord.m_values.resize(pArch->m_values.size(), 0.0);
		std::copy(pArch->m_values.begin(), pArch->m_values.end(), m_loadedRecord.m_values.begin());
		m_loadedRecord.m_valCount = pArch->m_values.size();

		if (m_loadedRecord.m_objInds.size() < pArch->m_offsetMap.size())
			m_loadedRecord.m_objInds.resize(pArch->m_offsetMap.size());

		size_t i = 0;
		for (auto itr = pArch->m_offsetMap.begin(); itr != pArch->m_offsetMap.end(); ++itr, ++i)
		{
			//copy inds
			m_loadedRecord.m_objInds[i] = (*itr).second.m_ind;
			if ((*itr).second.m_pSrc->IsOMEType(OME_VARARRAY))
			{
				//copy array dims
				m_loadedRecord.m_sizeOverrides[i] = ((VarArray*)(*itr).second.m_pSrc)->GetDims();
			}
		}

		if (m_loadedRecord.m_instCounts.size() < pArch->m_modelMap.size())
			m_loadedRecord.m_instCounts.resize(pArch->m_modelMap.size());

		size_t currEntry = 0;
		m_loadedRecord.m_flagCount = 0;
		for (auto itr = pArch->m_modelMap.begin(); itr != pArch->m_modelMap.end(); ++itr)
		{
			m_loadedRecord.m_instCounts[currEntry++] = (*itr).second.pRefModel->m_instances.size();
			m_loadedRecord.m_flagCount += (*itr).second.pRefModel->m_instances.size();
		}

		if (m_loadedRecord.m_loadedFlags.size() < m_loadedRecord.m_flagCount)
			m_loadedRecord.m_loadedFlags.resize(m_loadedRecord.m_flagCount);

		currEntry = 0;
		for (auto itr = pArch->m_modelMap.begin(); itr != pArch->m_modelMap.end(); ++itr)
		{

			for (i = 0; i < (*itr).second.pRefModel->m_instances.size(); ++i)
				m_loadedRecord.m_loadedFlags[currEntry++] = (*itr).second.pRefModel->m_instances[i]->m_flags;
		}
	}
	m_lastTimeInd = IRIND(timeStamp);
	//Commented out to increase performance over a large amount of simulations.
	//DBG_PRINT("WRITING RECORD " + std::to_string(m_lastTimeInd));
}

/** Set the min record usage.
	@param modeOn If true, use minimum record keeping; otherwise, use full record keeping.
*/
void RecordMgr::SetMinMode(const bool & modeOn)
{
	m_useMin = modeOn;
}

// ArchiveRecord
/** Simple Constructor.
	@param pArch Pointer to ValueArchive to associate with ArchiveRecord.
*/
RecordMgr::ArchiveRecord::ArchiveRecord(ValueArchive *pArch)
	: m_instCounts(pArch->m_modelMap.size()), m_valCount(pArch->m_values.size())
{
	std::copy(pArch->m_values.begin(), pArch->m_values.end(), m_values.end());
	for (auto itr = pArch->m_modelMap.begin(); itr != pArch->m_modelMap.end(); ++itr)
	{
		Model* pMdl = (*itr).second.pRefModel;
		m_instCounts.push_back(pMdl->GetNumRawInstances());
		for (auto subItr = pMdl->m_instances.begin(); subItr != pMdl->m_instances.end(); ++subItr)
			m_loadedFlags.push_back((*subItr)->m_flags);
	}

	size_t i;
	for (auto itr = pArch->m_offsetMap.begin(); itr != pArch->m_offsetMap.end(); ++itr,++i)
	{
		m_objInds.push_back((*itr).second.m_ind);
		if ((*itr).second.m_pSrc->IsOMEType(OME_VARARRAY))
			m_sizeOverrides[i] = ((VarArray*)(*itr).second.m_pSrc)->GetDims();
	}
}
