#pragma once
#include<iostream>
#include "OMEIntegrationBlock.h"

/** Variant of OMEIntegrationBlock that produces more accurate results at the cost of memory and time.*/
class __EXPORT__ HiResIntegrationBlock : public OMEIntegrationBlock
{
public:

   HiResIntegrationBlock(void);
   HiResIntegrationBlock(const HiResIntegrationBlock & blck);
   ~HiResIntegrationBlock();

   virtual void Init(INT_METHOD method, int count, OME_SCALAR timeStep);
   virtual void Clear(void);

   virtual void TransferTo(OMEIntegrationBlock & blck);
   virtual OMEIntegrationBlock* Clone();

protected:

   OME_SCALAR *m_errs;		///< Pointer to error values for each State Variable.
   OME_SCALAR m_tsErr;		///< Error value for timer value.

   // specific integrators.  note:  These should update m_timeStep if they change it, but NOT m_time.
   virtual bool IntEuler(DERIVEFN deriveFn, void *dExtra, POSTINTFN postFn, void* pExtra);
   virtual bool IntRK4(DERIVEFN deriveFn, void *dExtra, POSTINTFN postFn, void* pExtra);

};
