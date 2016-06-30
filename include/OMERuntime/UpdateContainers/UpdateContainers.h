#pragma once
#include "DynamicUpArray.h"
//#include "DynamicUpVAArray.h"
#include "SimpleArrayWrapper.h"
#include "StaticUpArray.h"
//#include "StaticUpVAArray.h"
#include "TempVCArray.h"
#include "TempPoolArray.h"
#include "ShuffledList.h"
//#include "SubVAList.h"
//#include "TransposeArray.h"
#include "IndexJumpTable.h"

namespace OMEUpdateContainers
{
	inline void RefitAllUCPools()
	{
		DynamicUpArray::RefitTempPool();
		//DynamicUpVAArray::RefitTempPool();
		SimpleArrayWrapper::RefitTmpPool();
		StaticUpArray::RefitTempPool();
		//StaticUpVAArray::RefitTempPool();
		TempVCArray::RefitTempPool();
		TempPoolArray::RefitTempPool();
		TempPoolArray::RefitNodePool();
		ShuffledList::RefitShuffledListPool();
		//TransposeArray::RefitTempPool();

		IndexJumpTable::RefitNodePool();
		ListDims::RefitTempNodePool();
	}

	inline void ClearAllUCPools()
	{
		DynamicUpArray::ClearTempPool();
		//DynamicUpVAArray::ClearTempPool();
		SimpleArrayWrapper::ClearTmpPool();
		StaticUpArray::ClearTempPool();
		//StaticUpVAArray::ClearTempPool();
		TempVCArray::ClearTempPool();
		TempPoolArray::ClearTempPool();
		TempPoolArray::ClearNodePool();
		ShuffledList::ClearShuffledListPool();
		//TransposeArray::ClearTempPool();

		IndexJumpTable::ClearNodePool();
		ListDims::ClearTempNodePool();

		//the following may need to be moved somewhere else, but should work here for the moment
		TempVCArray::ResetGlobalCache();
	}
};