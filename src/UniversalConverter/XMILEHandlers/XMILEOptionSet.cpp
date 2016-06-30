#include "XMILEHandlers/XMILEOptionSet.h"

/** Default constructor. */
XMILEOptionSet::XMILEOptionSet()
:m_optBits()
{

}

/** Initialization constructor.
	@param defaultOpts The list of options to initially mark as supported.
*/
XMILEOptionSet::XMILEOptionSet(const std::initializer_list<XMILEOPTS> & defaultOpts)
: XMILEOptionSet()
{
	for (const XMILEOPTS & opt : defaultOpts)
	{
		m_optBits[opt] = 1;
	}
}

/**Retrieve the state of a specific option.
	@param opt The option state to retrieve.
	@return true if the option is active/supported; false otherwise.
*/
bool XMILEOptionSet::GetOption(const XMILEOPTS & opt) const
{
	return m_optBits[opt];
}

/** Set the state of a specific option.
	@param opt The option to set.
	@param inUse The state to apply to opt.
*/
void XMILEOptionSet::SetOption(const XMILEOPTS & opt, const bool & inUse)
{
	m_optBits[opt] = inUse ? 1 : 0;
}