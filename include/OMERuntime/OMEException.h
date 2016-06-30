#pragma once

#include "OMEDefines.h"
#include <exception>

//VS 2013 and below do not support noexcept. Use macro to avoid
#ifdef _MSC_VER
#define NOEXCEPT 
#else
#define NOEXCEPT noexcept
#endif

class OMEObject;

/** OME specific base exception.*/
class __EXPORT__ OMEException : public std::exception
{
public:
	OMEException(const char* msg);
	virtual const char* what() NOEXCEPT; 

protected:
	STLString m_msg;	///< Message to be populated by constructor or subclasses.
};

/** Exception that occurs due to a general issue specific to a model object.*/
class __EXPORT__ OMEObjException : public OMEException
{
public:
	OMEObjException(const char* msg, const OMEObject* pObj);
	
	const OMEObject* GetOMEObject() const;

protected:
	const OMEObject* m_pObj;	///< Pointer to object that caused error.
};

/** Exception that disrupts the normal flow of a function.*/
class __EXPORT__ OMEFuncOpException : public OMEException
{
public:
	OMEFuncOpException(const char* funcName, const char* msg);
};

/** Out of bounds exception. */
class __EXPORT__ OMEOOBException : public OMEException
{
public:
	OMEOOBException(const char* msg);

};

/** Exception thrown in leu of an unsupported feature.*/
class __EXPORT__ OMESupportException : public OMEException
{
public:
	OMESupportException(const char* lbl);

};

/** Exception stemming from memory management problems.*/
class __EXPORT__ OMEMemoryException : public OMEException
{
public:
	OMEMemoryException(const char* lbl, const char* msg);

};

/** File system-related exception.*/
class __EXPORT__ OMEIOException : public OMEException
{
public:
	OMEIOException(const char* lbl, const char* msg);

};
