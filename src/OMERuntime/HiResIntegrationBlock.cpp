#include "HiResIntegrationBlock.h"
#include "OMEDefines.h"
#include "DebugOutput.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <math.h>
//#include <omp.h>

#define KAHAN_ADD(a,e,s)                   \
{                                          \
volatile OME_SCALAR c = (a) - (e);         \
volatile OME_SCALAR t = (s) + c;           \
(e) = (t - (s)) - c;                       \
(s) = t;                                   \
}


////////////////////////////////////////////////////////
// I N T E G R A T I O N   B L O C K 
////////////////////////////////////////////////////////

HiResIntegrationBlock::HiResIntegrationBlock(void)
	: OMEIntegrationBlock()
	, m_errs(NULL)
	, m_tsErr(0.0)
{ 

}
/** Copy Constructor.
	@param blck The HiResIntegrationBlock to copy.
*/
HiResIntegrationBlock::HiResIntegrationBlock(const HiResIntegrationBlock & blck)
	:OMEIntegrationBlock(blck)
	, m_errs(NULL)
	, m_tsErr(blck.m_tsErr)
{
	if (blck.m_errs)
		m_errs = AllocateArray(m_errs, blck.m_errs, blck.m_svCount);
}

HiResIntegrationBlock::~HiResIntegrationBlock(void)
{
}

void HiResIntegrationBlock::Init(INT_METHOD method, int count, OME_SCALAR timeStep)
{
	OMEIntegrationBlock::Init(method, count, timeStep);

	m_tsErr = 0.0;
	m_errs = AllocateArray(m_errs, m_svCount);
	for (size_t i = 0; i < m_svCount; ++i)
		m_errs[i] = 0.0;

}


void HiResIntegrationBlock::Clear( void )
{
	OMEIntegrationBlock::Clear();

	if ( m_errs != NULL ) delete [] m_errs;
	
	
	m_errs = NULL;

}


bool HiResIntegrationBlock::IntEuler(DERIVEFN deriveFn, void *dExtra, POSTINTFN postFn, void* pExtra)
{
	OME_SCALAR currentTime = m_time;

	//-- get derivative values from object at current time --//  
	deriveFn( currentTime, m_timeStep, m_svCount, m_k1, dExtra );

	OME_SCALAR ret = 0.0;

	// #pragma omp parallel for   
	for (int i = 0; i < m_svCount; i++)
		KAHAN_ADD(m_k1[i] * m_timeStep, m_errs[i], *(m_sv[i]));
	
	//call postFn if present
	postFn(currentTime, m_timeStep, pExtra);
	return true;
}

bool HiResIntegrationBlock::IntRK4(DERIVEFN deriveFn, void *dExtra, POSTINTFN postFn, void* pExtra)
{
	OME_SCALAR currentTime = m_time;

	//-- Step 1: get derivative values from object at current time --//  
	deriveFn( currentTime, m_timeStep/2, m_svCount, m_k1, dExtra );

#pragma omp parallel for   
	for (int i=0; i < m_svCount; i++ )
		m_svTemp[i] = *(m_sv[i]);     // remember original values

	//-- Step 2: get derivative values from object at current time + 1/2--//
	//#pragma omp parallel for
	for (int i=0; i < m_svCount; i++ )
	{
		*(m_sv[i]) = OME_SCALAR( m_svTemp[i] + m_k1[i] * m_timeStep / 2 );

		RoundErr(m_sv[i]);
	}  

	postFn(currentTime, m_timeStep/2, pExtra);

	currentTime = m_time;
	KAHAN_ADD(m_timeStep / 2, m_tsErr, currentTime);
	currentTime = m_time + m_timeStep/2;
	deriveFn( currentTime, 0.0, m_svCount, m_k2, dExtra );

	// Step 3: halfway across with improved values
	//#pragma omp parallel for
	for (int i=0; i < m_svCount; i++)
	{
		*(m_sv[i]) = OME_SCALAR( m_svTemp[i] + ( m_k2[i] * m_timeStep / 2 ) );

		RoundErr(m_sv[i]);
	}
	postFn(currentTime, 0.0, pExtra);
	deriveFn( currentTime, m_timeStep/2, m_svCount, m_k3, dExtra );

	// Step 4: all the way across
	//#pragma omp parallel for
	for (int i=0; i < m_svCount; i++)
	{
		*(m_sv[i]) = OME_SCALAR( m_svTemp[i] + ( m_k3[i] * m_timeStep ) );

		RoundErr(m_sv[i]);
	}
	postFn(currentTime, m_timeStep/2, pExtra);

	currentTime = m_time + m_timeStep;
	deriveFn(currentTime, 0.0, m_svCount, m_k4, dExtra);

	//--------------- done with k values ------------//
	OME_SCALAR err;
	OME_SCALAR addedVal;
	//#pragma omp parallel for
	for (int i=0; i < m_svCount; i++ )
	{
		addedVal = m_k1[i];
		err = 0.0;
		KAHAN_ADD(m_k2[i] * 2, err, addedVal);
		KAHAN_ADD(m_k3[i] * 2, err, addedVal);
		KAHAN_ADD(m_k4[i], err, addedVal);

		*(m_sv[i]) = m_svTemp[i];
		KAHAN_ADD(OME_SCALAR(addedVal)* m_timeStep / 6, m_errs[i], *(m_sv[i]));

		RoundErr(m_sv[i]);
	}
	postFn(currentTime, 0.0, pExtra);
	return true;
}

void HiResIntegrationBlock::TransferTo(OMEIntegrationBlock & blck)
{
	OMEIntegrationBlock::TransferTo(blck);

	HiResIntegrationBlock* hBlck = dynamic_cast<HiResIntegrationBlock*>(&blck);
	if (hBlck)
	{
		hBlck->m_errs = m_errs;
		m_errs=NULL;
		hBlck->m_tsErr=m_tsErr;
	}
}

OMEIntegrationBlock* HiResIntegrationBlock::Clone()
{
	HiResIntegrationBlock* pRet = new HiResIntegrationBlock(*this);

	return pRet;
}