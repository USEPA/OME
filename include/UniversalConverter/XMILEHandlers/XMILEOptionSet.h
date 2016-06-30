#pragma once

#include <bitset>
#include <initializer_list>

/** Container for storing and sharing XMILE option flags*/
class XMILEOptionSet
{
public:
	/** Flags denoting specific XMILE options. */
	enum XMILEOPTS {CONV=0  ///< Conveyor Stocks are used.
		,QUEUE				///< Queue Stocks are used.
		,ARRAY				///< Array variables are used.
		,SUBMODELS			///< Submodels are included in the model.
		,MACROS				///< Macros are in use.
		,EVENTS				///< Event posting is relied upon.
		,VIEW				///< Model views are supplied for visualizations.
		,OUTPUTS			///< Output points are in use.
		,INPUTS				///< Input points are in use.
		,ANNOTS				///< Annotations are present.
							
		, XMILEOPTS_COUNT	///< The total number of option flags.
	
	};

	XMILEOptionSet();
	XMILEOptionSet(const std::initializer_list<XMILEOPTS> & defaultOpts);

	bool GetOption(const XMILEOPTS & opt) const;
	void SetOption(const XMILEOPTS & opt,const bool & inUse);

private:
	std::bitset<XMILEOPTS_COUNT> m_optBits;	///< The collection of bits representing the state of each option.
};