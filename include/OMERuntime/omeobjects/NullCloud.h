#pragma once
#include "OMENull.h"
#include <list>
#include <string>

/** Temporary cloud.

	Class used to temporarily store points of model inflow and outflow during model construction.
*/
class NullCloud : public OMENull
{   
public:
	/**Default Constructor */
	NullCloud() : OMENull() {}
		/**Simple Constructor
		@param n name of null object
		@param d description of null object
	*/
	NullCloud(STLString n, STLString d) : OMENull(n,d) {}
	/** Copy constructor
		@param nc The nullCloud to copy.
	*/
	NullCloud(const NullCloud & nc) { Duplicate(nc); };
	/** Assignment Operator.
		@param nc The NullCloud to copy.
		@return Reference to the NullCloud.
	*/
	NullCloud& operator=(const NullCloud & nc) { Duplicate(nc); return *this; };
	
   virtual NULLTYPE GetNullType( void ) { return NULL_CLOUD; }

protected:
   void Duplicate(const NullCloud & nc);
}; 
