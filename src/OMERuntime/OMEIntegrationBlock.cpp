///////////////////////////////////////////////////////////////////////
//                                                                   //
//	OMEIntegrationBlock class based on original class from Envision, //
//	a modeling framework developed by the Department of Biological   //
//	and Ecological Engineering at Oregon State University, and is    //
//	used with permission.                                            //
//                                                                   //
//	The Envision homepage:                                           //
//	http://envision.bioe.orst.edu                                    //
//                                                                   //
///////////////////////////////////////////////////////////////////////
#include "OMEIntegrationBlock.h"
#include "OMEDefines.h"
#include "DebugOutput.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <math.h>
//#include <omp.h>


////////////////////////////////////////////////////////
// I N T E G R A T I O N   B L O C K 
////////////////////////////////////////////////////////

/** The sigma to use when comparing time values.*/
const OME_SCALAR OMEIntegrationBlock::TIME_TOLERANCE = 0.00001f;

OMEIntegrationBlock::OMEIntegrationBlock( void )
	: m_method( IM_RK4 )
	, m_time( 0.0f )
	, m_stopTime( 0.0f )
	, m_timeStep( 1.0f )                // days?
	, m_currentTimeStep( 1.0f )
	//, m_useVariableStep( true )
	, m_k1( NULL )
	, m_k2( NULL )
	, m_k3( NULL )
	, m_k4( NULL )
	//, m_derive( NULL )
	, m_svTemp( NULL )
	, m_sv( NULL )
	, m_svCount( -1 )
{ 

}

/** Copy Constructor.
	@param blck The OMEIntegrationBlock to copy.
*/
OMEIntegrationBlock::OMEIntegrationBlock(const OMEIntegrationBlock & blck)
{
	m_timeStep = blck.m_timeStep;
	m_stopTime = blck.m_stopTime;

	m_time = blck.m_time;
	m_currentTimeStep = blck.m_currentTimeStep;

	m_method = blck.m_method;

	if (blck.m_k1)
		m_k1 = AllocateArray(m_k1,blck.m_k1, blck.m_svCount);
	if (blck.m_k2)
		m_k2 = AllocateArray(m_k2,blck.m_k2, blck.m_svCount);
	if (blck.m_k3)
		m_k3 = AllocateArray(m_k3,blck.m_k3, blck.m_svCount);
	if (blck.m_k4)
		m_k4 = AllocateArray(m_k4,blck.m_k4, blck.m_svCount);
	if (blck.m_svTemp)
		m_svTemp = AllocateArray(m_svTemp,blck.m_svTemp, blck.m_svCount);
	if (blck.m_sv)
		m_sv = AllocateArray(m_sv,blck.m_sv, blck.m_svCount);

	m_svCount = blck.m_svCount;
}

OMEIntegrationBlock::~OMEIntegrationBlock( void )
{
	Clear();
}

/** Perform an integration solving step using the selected method.
	@param time The current time for the integration.
	@param stopTime The time at which the solving process is intended to stop.
	@param deriveFn Function to call to calculate derivatives.
	@param dExtra Optional extra data to pass to deriveFn on call.
	@param postFn Function to call after each integration sub-step.
	@param pExtra Optional extra data to pass to postFn on call.
	@return Time at which integration stopped.

*/
OME_SCALAR OMEIntegrationBlock::Integrate(OME_SCALAR time, OME_SCALAR stopTime, DERIVEFN deriveFn, void *dExtra, POSTINTFN postFn, void* pExtra)
{
	m_time = time;
	m_stopTime = stopTime;
	m_currentTimeStep = m_timeStep;

	if ( m_time > m_stopTime )
		return false;   // not quite right, should make sure that the block has been integrated to the stop time

	while ( m_time < ( m_stopTime - TIME_TOLERANCE ) )
	{
		switch( m_method )
		{
		case IM_EULER:
			IntEuler(deriveFn, dExtra, postFn,pExtra);
			break;

		case IM_RK4:
			IntRK4(deriveFn, dExtra, postFn, pExtra);
			break;

	/*	case IM_RKF:
			IntRKF( deriveFn, extra );
			std::ostringstream outStrm;
			outStrm<<"RKF: timeStep: "<<std::setprecision(4)<<m_timeStep;
			DBG_PRINT(outStrm.str());
			break;*/
		}  // end of: switch ( m_method )


		m_time += m_currentTimeStep;

		// are we close to the end?
		OME_SCALAR timeRemaining = m_stopTime - m_time;

		if ( timeRemaining  < ( m_timeStep + TIME_TOLERANCE ) )
			m_currentTimeStep = m_stopTime - m_time;
	}  // end of: while ( m_time < m_stopTime )   // note - doesn't guarantee alignment with stop time

	return m_time;
}

/** Perform an integration solving step using a custom time step with the selected method.
@param time The current time for the integration.
@param stopTime The time at which the solving process is intended to stop.
@param stepOverride The timestep to use for this specific integration call.
@param deriveFn Function to call to calculate derivatives.
@param dExtra Optional extra data to pass to deriveFn on call.
@param postFn Function to call after each integration sub-step.
@param pExtra Optional extra data to pass to postFn on call.
@return Time at which integration stopped.

*/
OME_SCALAR OMEIntegrationBlock::IntegrateWithStep(OME_SCALAR time, OME_SCALAR stopTime, OME_SCALAR stepOverride, DERIVEFN deriveFn, void *dExtra, POSTINTFN postFn, void* pExtra)
{
	OME_SCALAR oldStep=m_timeStep;
	m_timeStep=stepOverride;
	OME_SCALAR result = Integrate(time, stopTime, deriveFn, dExtra, postFn, pExtra);
	m_timeStep=oldStep;
	return result;
}

/** Set the address for a speceific state variable.
	@param sv Pointer to the state variable value.
	@param index The index of the state variable to assign sv to.
	@return True if index is a valid index; false otherwise
*/
bool OMEIntegrationBlock::SetStateVar(OME_SCALAR *sv, int index)
{ 
	if (index >= m_svCount || index<0) 
		return false; 
	m_sv[index] = sv; 
	return true;
}

/** Allocate space and configure details for integration solving.
	@param method The method to use for integration solving.
	@param count The number of state variables to include in integration solving.
	@param timeStep The default timestep for use in integration solving.
*/
void OMEIntegrationBlock::Init( INT_METHOD method, int count, OME_SCALAR timeStep )
{
	m_timeStep=timeStep;
	m_method = method;

	if ( m_svCount > 0 )
		Clear();

	m_svCount = count;

	if ( method == IM_OTHER || method== IM_UNDEF)
		m_svCount = 0;

	switch( m_method )
	{
	case IM_EULER:
		m_sv = AllocateArray( m_sv, m_svCount );
		m_k1 = AllocateArray( m_k1, m_svCount );
		
		break;

	case IM_RK4:
		m_sv = AllocateArray( m_sv, m_svCount );
		m_k1 = AllocateArray( m_k1, m_svCount );
		m_k2 = AllocateArray( m_k2, m_svCount );
		m_k3 = AllocateArray( m_k3, m_svCount );
		m_k4 = AllocateArray( m_k4, m_svCount );
		m_svTemp = AllocateArray( m_svTemp, m_svCount );
		break;
	}
}

/** Deallocate all storage space.*/
void OMEIntegrationBlock::Clear( void )
{
	if ( m_k1     != NULL ) delete [] m_k1;
	if ( m_k2     != NULL ) delete [] m_k2;
	if ( m_k3     != NULL ) delete [] m_k3;
	if ( m_k4     != NULL ) delete [] m_k4;
	if ( m_sv     != NULL ) delete [] m_sv;
	if ( m_svTemp != NULL ) delete [] m_svTemp;

	m_k1 = NULL;
	m_k2 = NULL;
	m_k3 = NULL;
	m_k4 = NULL;
	m_sv = NULL;
	m_svTemp = NULL;

	m_svCount = -1;
}

/** Used for determining whether the present environment satisfies the requirements to run
	the integration solver. Intended for use with subclasses.
*/
bool OMEIntegrationBlock::CanBeUsed() const
{ 
	return true; 
}

/** Perform an integration solving step using Euler's method.
@param deriveFn Function to call to calculate derivatives.
@param dExtra Optional extra data to pass to deriveFn on call.
@param postFn Function to call after each integration sub-step.
@param pExtra Optional extra data to pass to postFn on call.
@return True if the function completed successfully; otherwise false.

*/
bool OMEIntegrationBlock::IntEuler(DERIVEFN deriveFn, void *dExtra, POSTINTFN postFn, void* pExtra)
{
	OME_SCALAR currentTime = m_time;

	//-- get derivative values from object at current time --//  
	deriveFn( currentTime, m_timeStep, m_svCount, m_k1, dExtra );

	// #pragma omp parallel for   
	for (int i=0; i < m_svCount; i++ )
		*(m_sv[i]) = OME_SCALAR( *m_sv[i] + m_k1[i] * m_timeStep ); 

	//call postFn if present
	postFn(currentTime, m_timeStep, pExtra);
	return true;
}


/** Perform an integration solving step using the fourth order Runge-Kutta method.
@param deriveFn Function to call to calculate derivatives.
@param dExtra Optional extra data to pass to deriveFn on call.
@param postFn Function to call after each integration sub-step.
@param pExtra Optional extra data to pass to postFn on call.
@return True if the function completed successfully; otherwise false.

*/
bool OMEIntegrationBlock::IntRK4(DERIVEFN deriveFn, void *dExtra, POSTINTFN postFn, void* pExtra)
{
	OME_SCALAR currentTime = m_time;


	//-- Step 1: get derivative values from object at current time --//  
	deriveFn( currentTime, m_timeStep/2, m_svCount, m_k1, dExtra );

//#pragma omp parallel for   
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
	//#pragma omp parallel for
	for (int i=0; i < m_svCount; i++ )
	{
		*(m_sv[ i ]) = m_svTemp[ i ] + (OME_SCALAR( m_k1[i] + m_k2[i]*2 + m_k3[i]*2 + m_k4[i] ) * m_timeStep / 6 );
		RoundErr(m_sv[i]);
	}

	postFn(currentTime, 0.0, pExtra);
	return true;
}

/** Transfer data and allocated space from one OMEIntegrationBlock to another.
	@param blck The target block for the transfer.
*/
void OMEIntegrationBlock::TransferTo(OMEIntegrationBlock & blck)
{
	blck.m_timeStep=m_timeStep;
	blck.m_stopTime=m_stopTime;

	blck.m_time=m_time;
	blck.m_currentTimeStep=m_currentTimeStep;

	blck.m_method=m_method;

	blck.m_k1=m_k1;
	blck.m_k2=m_k2;
	blck.m_k3=m_k3;
	blck.m_k4=m_k4;
	blck.m_svTemp=m_svTemp;
	blck.m_sv=m_sv;

	//pointers transferred; zero out to avoid accidental cleanup
	m_k1=NULL;
	m_k2=NULL;
	m_k3=NULL;
	m_k4=NULL;
	m_svTemp = NULL;
	m_sv = NULL;
	blck.m_svCount=m_svCount;
}

/** Allocate a copy of the OMEIntegrationBlock.
	@return Pointer to newly allocated copy of OMEIntegrationBlock.
*/
OMEIntegrationBlock* OMEIntegrationBlock::Clone()
{
	OMEIntegrationBlock* pRet=new OMEIntegrationBlock(*this);

	return pRet;
}