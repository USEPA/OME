#include "omeobjects/NullSource.h"

NullSource::NullSource()
	:OMENull(), m_program(""), m_edition(""),m_version(""),m_modDate("")
{

}

NullSource::NullSource(STLString p, STLString v, STLString e, STLString d)
	:OMENull(), m_program(p), m_edition(e),m_version(v),m_modDate(d)
{

}