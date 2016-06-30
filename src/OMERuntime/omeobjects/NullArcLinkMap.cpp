#include "omeobjects/NullArcLinkMap.h"

/** Copy values from another NullArcLinkMap
	@param nalm the NullArcLinkMap to copy
*/
void NullArcLinkMap::Duplicate(const NullArcLinkMap & nalm)
{
	m_subModelID=nalm.m_subModelID;
	m_links=nalm.m_links;

	OMENull::Duplicate(nalm);
}