///////////////////////////////////////////////////////////////////////
//                                                                   //
//	OMEIntegrationBlock class based on original class from Envision,     //
//	a modeling framework developed by the Department of Biological    //
//	and Ecological Engineering at Oregon State University, and is     //
//	used with permission.                                             //
//                                                                   //
//	The Envision homepage:                                            //
//	http://envision.bioe.orst.edu                                     //
//                                                                   //
///////////////////////////////////////////////////////////////////////

#pragma once
#include "ValTypes.h"
#include<iostream>
#include "ValueArchive.h"

typedef void(*DERIVEFN)(OME_SCALAR time, OME_SCALAR timestep, int svCount, OME_SCALAR *deriveArray, void *extra);
typedef void(*POSTINTFN)(OME_SCALAR time, OME_SCALAR timestep, void *extra);

/** Class for executing integration solving algorithms. */
class __EXPORT__ OMEIntegrationBlock
{
public:
	static const OME_SCALAR TIME_TOLERANCE;
	/** Integration Solving methods.*/
	enum INT_METHOD {
		IM_UNDEF = 0,	///< No integration solving method defined. 
		IM_EULER = 1,  ///< Use Euler Method for integration solving.
		IM_RK4 = 2,	///< Use 4th order Runge-Kutta method for integration solving.
		IM_OTHER = 10	///< Some other undefined solving method.
	};

	OMEIntegrationBlock(void);
	OMEIntegrationBlock(const OMEIntegrationBlock & blck);
	virtual ~OMEIntegrationBlock();

	virtual void Init(INT_METHOD method, int count, OME_SCALAR timeStep);

	virtual void Clear(void);   // deletes arrays

	virtual bool CanBeUsed() const;

	OME_SCALAR Integrate(OME_SCALAR time, OME_SCALAR stopTime, DERIVEFN deriveFn, void *dExtra, POSTINTFN postFn, void* pExtra);
	OME_SCALAR IntegrateWithStep(OME_SCALAR time, OME_SCALAR stopTime, OME_SCALAR stepOverride, DERIVEFN deriveFn, void *dExtra, POSTINTFN postFn, void* pExtra);
	bool  SetStateVar(OME_SCALAR *sv, int index);

	/** @return The default time step.*/
	inline OME_SCALAR GetTimeStep() const { return m_timeStep; }
	/** Set the current time step.
		@param ts The new time step to use.
	*/
	inline void SetTimeStep(const OME_SCALAR & ts) { m_timeStep = ts; }

	/** @return The time at which the current integration process should stop. */
	inline OME_SCALAR GetStopTime() const { return m_stopTime; }
	/** Set the present stop time.
		@param st The time to stop.
	*/
	inline void SetStopTime(const OME_SCALAR & st) { m_stopTime = st; }

	virtual void TransferTo(OMEIntegrationBlock & blck);
	virtual OMEIntegrationBlock* Clone();

protected:
	OME_SCALAR m_timeStep;			///< Detault time step size.
	OME_SCALAR m_stopTime;			///< Time at which to cease current integration step.

	OME_SCALAR  m_time;				///< Present integration time.
	OME_SCALAR  m_currentTimeStep;	///< Current time step being used (not necessarily equal to m_timeStep).

	INT_METHOD m_method;				///< Currently selected integration method.

	OME_SCALAR *m_k1;				///< Temporary array for RK4
	OME_SCALAR *m_k2;				///< Temporary array for RK4
	OME_SCALAR *m_k3;				///< Temporary array for RK4
	OME_SCALAR *m_k4;				///< Temporary array for RK4

	OME_SCALAR  *m_svTemp;	///< Pointer to temporary State Variable; used during integration process.
	OME_SCALAR  **m_sv;		///< Handle for State Variable being evaluated.
	int     m_svCount;		///< Total number of state variables to be integrated.

	template <typename T> T *AllocateArray(T *base, const int & size);
	template <typename T> T *AllocateArray(T *base, T* src, const int & size);

	// specific integrators.  note:  These should update m_timeStep if they change it, but NOT m_time.
	virtual bool IntEuler(DERIVEFN deriveFn, void *dExtra, POSTINTFN postFn, void* pExtra);
	virtual bool IntRK4(DERIVEFN deriveFn, void *dExtra, POSTINTFN postFn, void* pExtra);

	/** Round some small value to 0.0.
		@param pVal Pointer to value to round down to 0.0 (if needed).
	*/
	inline void RoundErr(OME_SCALAR* pVal)
	{
		const OME_SCALAR ERR_LIMIT = 1E-30f;
		if (OMEABS(*pVal) < ERR_LIMIT)
			*pVal = 0.0;

	}

};

/** Allocate or reallocate an array.
	@param base Pointer to the source address of array.
	@param size number of slots to allocate/reallocate.
	@return The address passed in by base, or the address to the newly allocated array if base is NULL.
	*/
template <typename T>
T *OMEIntegrationBlock::AllocateArray(T* base, const int & size)
{
	if (base != NULL)
		delete[] base;

	base = new T[size];

	return base;
}

/** Allocate or reallocate an array and copy values from an existing array.
@param base Pointer to the source address of array.
@param src Pointer to array containing values to copy into newly allocated array.
@param size number of slots to allocate/reallocate.
@return The address passed in by base, or the address to the newly allocated array if base is NULL.
*/
template <typename T>
T *OMEIntegrationBlock::AllocateArray(T *base, T* src, const int & size)
{
	T* ret = AllocateArray(base, size);

	memcpy(ret, src, sizeof(T)*size);
	return ret;
}