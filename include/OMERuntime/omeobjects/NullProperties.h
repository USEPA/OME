#pragma once
#include "OMENull.h"

/**placeholder for parser catalog */
class NullProperties : public OMENull
{   
public:
   NullProperties();
   NullProperties(STLString n);

   /** Copy Constructor
		@param np the NullProperties object to copy
   */
   NullProperties(const NullProperties & np) { Duplicate(np); };
	/** Assignment Operator.
		@param np The NullProperties to copy.
		@return Reference to the NullProperties.
	*/
   NullProperties& operator=(const NullProperties& np) { Duplicate(np); return *this; };

   //insert whatever settings need to be carried over here

public:
   virtual NULLTYPE GetNullType( void ) { return NULL_PROPERTIES; }
 
protected:
	/** Copy another NullProperties object.
		@param np The NullProperties object to copy.
	*/
	void Duplicate(const NullProperties & np) { OMEObject::Duplicate(np); };
}; 
