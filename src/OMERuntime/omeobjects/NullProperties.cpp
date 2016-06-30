#include "omeobjects/NullProperties.h"

/** Default constructor
*/
NullProperties::NullProperties()
	:OMENull()
{}

/** Simple constructor
	@param n of NullProperties object
*/
NullProperties::NullProperties(STLString n)
	:OMENull(n,"")
{}