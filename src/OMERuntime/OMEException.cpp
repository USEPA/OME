#include "OMEException.h"

#include "omeobjects/OMEObject.h"

/** @param msg The message to return on call to what().*/
OMEException::OMEException(const char* msg) 
:m_msg(msg){ }

/** Override of std::exception::what().
	@return message provided during the construction of the exception.
*/
const char* OMEException::what() NOEXCEPT
{ return m_msg.c_str(); }

//=====================================================

/** @param msg The message describing the reason for the exception.
	@param pObj Pointer to the object that is the cause of the exception.
*/
OMEObjException::OMEObjException(const char* msg, const OMEObject* pObj) 
:OMEException(msg), m_pObj(pObj)
{
	if (m_pObj)
	{
		oSTLSStream ret;
		ret << m_pObj->GetExprName() << " (" << m_pObj->LabelForType() << ") " << m_msg;
		m_msg = ret.str();
	}
}

/** @return Pointer ot the associated model object.*/
const OMEObject* OMEObjException::GetOMEObject() const
{ 
	return m_pObj; 
}

//=====================================================

/**	@param funcName The name of the originating function.
	@param msg The message describing the reason for the exception.
*/
OMEFuncOpException::OMEFuncOpException(const char* funcName, const char* msg) 
:OMEException(msg)
{
	m_msg = ": " + m_msg;
	m_msg = funcName + m_msg;
}

//=====================================================


/**	
	@param msg The message describing the reason for the exception.
*/
OMEOOBException::OMEOOBException(const char* msg)
:OMEException(msg)
{
	m_msg = "Out of bounds: " + m_msg;
}

//=====================================================

/**
@param lbl The label of the unsupported feature.
*/
OMESupportException::OMESupportException(const char* lbl) 
:OMEException(lbl)
{
	m_msg += ": unsupported call.";
}

//=====================================================

/**	@param lbl The label of the block of code throwing the exception.
	@param msg The message describing the reason for the exception.
*/
OMEMemoryException::OMEMemoryException(const char* lbl, const char* msg)
:OMEException(lbl)
{
	m_msg += ": ";
	m_msg += msg;
}
//=====================================================

/**	@param lbl The label of the block of code throwing the exception.
	@param msg The message describing the reason for the exception.
*/
OMEIOException::OMEIOException(const char* lbl, const char* msg)
:OMEException(lbl)
{
	m_msg += ": ";
	m_msg += msg;
}