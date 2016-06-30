#pragma once
#include "OMEDefines.h"
#include "FileExts.h"
#include "UpdateContainers/ListDims.h"
#include <map>


class ValueArchive;
class OMEParser;

/** Manager for records that are kept at each timestep. */
class __EXPORT__ RecordMgr
{
public:
	/** Map for containing values across all timesteps. */
	typedef std::map<OME_SCALAR, std::deque<OME_SCALAR> > FullValueMap;

	RecordMgr(ValueArchive* pArch, const OME_SCALAR & startTime, const OME_SCALAR & reportInterval, const STLString & trgDir = STLString(), const STLString & prefix = STLString());
	RecordMgr(const RecordMgr & rm);
	~RecordMgr();

	void RecordState(const OME_SCALAR & timestamp,ValueArchive* pArch);
	void RecordFullState(const OME_SCALAR & timestamp, ValueArchive* pArch);

	OMESclrArray GetRecordTimes() const;
	//MIValue GetValForTime(const OME_SCALAR & time,Evaluable* pEval, const size_t & inst);
	//bool GetValForTime(Evaluable* pEval, const size_t & inst, const OME_SCALAR & time, MIValue & val);
	bool SingleValForTime(const size_t & ind,const OME_SCALAR & time, OME_SCALAR & val);
	bool SingleValForTime(size_t ind, const size_t & tInd, OME_SCALAR & val);
	bool ArrayValForTime(const size_t & ind, const OME_SCALAR & time, OME_SCALAR* val, const size_t & inSize, size_t & outSize);
	bool ArrayValForTime(size_t ind, const size_t & tInd, OME_SCALAR* val, const size_t & inSize, size_t & outSize);

	/** Load a single value as previously recorded.
		@param ind record index for value to load.
		@param val populated with retrieved value upon successful return.
		@return true if value was successfully retrieved; false otherwise.
	*/
	inline bool SingleLastValForTime(const size_t & ind, OME_SCALAR & val) { return SingleValForTime(ind, m_lastTimeInd, val); }
	/** Load an array value as previously recorded.
		@param ind record index for value to load.
		@param val Pointer to an array of scalars.
		@param inSize The allocated size of the array pointed to by val.
		@param outSize The number of values copied from the record into the array pointed to by val.
		@return true if value was successfully retrieved; false otherwise.
	*/
	inline bool ArrayLastValForTime(const size_t & ind, OME_SCALAR* val, const size_t & inSize, size_t & outSize)
		{return ArrayValForTime(ind, m_lastTimeInd, val, inSize, outSize);}
	void EnableDiskMode(const STLString & trgDir);

	/** Set the prefix to use for temporary record files written to disk.
		@param name The value to prefix to each filename.
	*/
	inline void SetTempPrefix(const STLString & name) { m_mdlTmpName = name; }

	void CacheRecord(const OME_SCALAR & time);
	void CacheRecord(const size_t & tInd);

	void SyncArchiveWithCache(const OME_SCALAR & time, ValueArchive* pArch);
	void SyncArchiveWithCache(const size_t & tInd, ValueArchive* pArch);
	void Clear();

	FullValueMap GetAllRecordedValues();
	void GetAllRecordedValues(RecordMgr::FullValueMap & fvm);

	void SetMinMode(const bool & modeOn);
	/** Add an Evaluable object to the minimum record tracking list.
		@param pEval Pointer to the Evaluable object to include in minimum record keeping.
	*/
	void AddToMinTrackList(Evaluable* pEval) {m_minRefs.insert(pEval); }
	/** Remove all objects from the minimum record tracking list. */
	void ClearMinTrackList(){ m_minRefs.clear(); }
	
private:

	/** Struct for storing archived values for a specific timestep. Also used for 
	    storing values from an archive loaded from disk.
	*/
	struct ArchiveRecord
	{
		std::deque<OME_SCALAR> m_values;					///< List of values at this timestep.
		std::map<size_t, PersistListDims> m_sizeOverrides;	///< Associated Dimensions for values at this timestep.
		ARRAY_TYPE<size_t> m_instCounts;					///< Total number of instances for each submodel at this timestep.
		ARRAY_TYPE<size_t> m_objInds;						///< Indices for relevant objects.
		ARRAY_TYPE<char> m_loadedFlags;						///< The flags associated with the records loaded for the current ModelInstances.
		size_t m_flagCount;									///< Total number of flags.
		size_t m_valCount;									///< Total number of stored values.
		std::map<size_t, size_t> m_indMap;					///< Map of indices to objInds.
		char m_loadedRecordType;							///< Flag indicating the type of loaded record.


		/** Detailed Constructor.
			@param valCount The total number of values.
			@param instCounts The total number of instances.
			@param objCounts The total number of represented objects.
		*/
		ArchiveRecord(const size_t & valCount,const size_t & instCounts,const size_t & objCounts)
			:m_values(valCount, OME_SCALAR_NAN),m_instCounts(instCounts,0),m_objInds(objCounts,0),m_flagCount(0),m_valCount(valCount) {}

		/** Copy Constructor.
			@param rec the ArchiveRecord to copy.
		*/
		ArchiveRecord(const ArchiveRecord & rec)
		: m_values(rec.m_values), m_sizeOverrides(rec.m_sizeOverrides),m_loadedFlags(rec.m_loadedFlags),m_flagCount(rec.m_flagCount) {}
		
		/** Simple Constructor. 
			@param pArch Pointer to ValueArchive to use to construct a new ArchiveRecord.
		*/
		ArchiveRecord(ValueArchive *pArch);

		/** Default constructor. */
		ArchiveRecord()
			:m_flagCount(0), m_valCount(0){}
	};
	//typedef std::map<OME_SCALAR, ModelRecord> RecordMap;
	RecordMgr(); //disable;
	
	bool m_cacheToDisk;					///< Flag indicating whether or not record are cached to disk instead of being kept in memory.

	ArchiveRecord m_loadedRecord;		///<The presently loaded record.
	ArchiveRecord* m_pSelectedRecord;	///< Pointer to the currently selecteded record.

	STLString m_mdlTmpName;				///< Name of the temporary file name (if records are being written to disk).

	STLString m_dirPath;				///< Path to directory that contains the temporary record files (if records are being written to disk).
	
	//sparse record keeping stuff
	ARRAY_TYPE<ArchiveRecord> m_recordList;		///< List of records stored in memory.

	OME_SCALAR m_startTime;					///< The start time of the model.
	OME_SCALAR m_interval;					///< The interval used in timesteps.
	size_t m_lastTimeInd;				///< The index associated with the last time.
	size_t m_totEntries;				///< The total number of record entries (on disk or in memory).
	
	bool m_useMin;						///< If true, only record values that are previously looked up.
	std::set<Evaluable*> m_minRefs;		///< List of Pointers to Evaluables to record if m_useMin is true.

	void WriteRecordToDisk(const STLString & path, const OME_SCALAR & timeStamp, ValueArchive* pArch);
	void WriteFullRecord(ofstream & outFile,ValueArchive* pArch);
	void WriteDifferenceRecord(ofstream & outFile,ValueArchive* pArch);
	void WriteMinRecord(ofstream & outFile, ValueArchive* pArch);
	void WriteModelFlags(ofstream & outFile, ValueArchive* pArch);

	void LoadRecordFromDisk(const OME_SCALAR & time);
	void LoadRecordFromDisk(const size_t & trgInd);

	void DoPush(const size_t & tInd, ValueArchive* pArch);
	void DoPull(const size_t & tInd);
	void DoFullCopy(const size_t & tInd, ValueArchive* pArch);

	bool LoadDeltasForTime(const STLString & path);
	bool ReadFullRecord(ifstream & inFile);
	bool ReadDifferenceRecord(ifstream & inFile);
	bool ReadMinRecord(ifstream & inFile);
	bool ReadModelFlags(ifstream & inFile);
	
	void CopyArchiveToCache(ValueArchive* pArch, const OME_SCALAR & timeStamp);

	/** Retrieve the file path for a given record index.
		@param ind The index whose path is being referenced.
		@return A filepath to the temporary record file associated with index ind.
	*/
	inline STLString PathForInd(const size_t & ind)
	{
		static oSTLSStream pathStrm;
		pathStrm.clear();
		pathStrm.str(STLString());

		pathStrm << m_dirPath << m_mdlTmpName << m_startTime+ind << OME_EXT_TEMP;
		return pathStrm.str();
	}

	/*inline size_t LengthForArray(const size_t & arrInd,ArchiveRecord & rcrd)
	{
		size_t ret = m_arraySizes[arrInd];
		if (rcrd.m_sizeOverrides.find(arrInd) != rcrd.m_sizeOverrides.end())
			size_t ret = rcrd.m_sizeOverrides[arrInd];
		return ret;
	}*/
};