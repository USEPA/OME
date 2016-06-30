#pragma once
#include "ValTypes.h"
#include "CharType.h"
#include <list>
#include <string>
#include <regex>

//! Type of Unit
/*! Describes the situational context for the application of the unit */
enum UNIT_TYPE { UT_NONE, UT_MASS, UT_ENERGY, UT_FORCE, UT_VOLUME, UT_AREA, UT_LENGTH, UT_TIME, UT_TEMP };

//! Specific unit of measure
/*! Units which include magnitude (ie Gram vs kilogram) 
	When adding new tags, make sure to keep metric and non metric grouped seperately,
	and order tags by magnitude. This makes the implementation of conversion functions much easier.
*/
enum UNIT_MEASURE 
   { 
	  UM_NONE,
	 /* mass    */ UM_GRAM, UM_KG,
	 /* energy  */ UM_CAL, UM_JOULE,
	 /* force   */ UM_LBS,
	 /* length  */ UM_MM, UM_CM, UM_METER, UM_KM, UM_IN, UM_FT, UM_MILE,
	 /* area    */ UM_CM2, UM_M2, UM_KM2, UM_IN2, UM_FT2, UM_ACRE, UM_MI2,
	 /* volume  */ UM_ML, UM_LITER,
	 /* time	*/ UM_SEC, UM_MIN, UM_HR, UM_DAY, UM_WEEK, UM_MONTH, UM_YEAR,
	/* temperature */ UM_KELVIN, UM_CELSIUS, UM_FAREN,
	 UM_COUNT
   };

const static unsigned int maxEntriesPerUnit=5;
const static OMEChar* measurePatterns[][maxEntriesPerUnit] = 
{
	/*
		Each row below corresponds to a UM entry (in order). Each entry is an acceptable 
		regex pattern match, with null strings ("") representing blank entries.
		match pattern will look something like ([0-9]*[.]?[0-9]+)[.]?\s*(<entry1>|<entry2>|<...>|<entryn>)\b
		Unfamiliar with regex control statements? Check out: http://www.cplusplus.com/reference/std/regex/ECMAScript/

		The patterns are presented this way for readability, and are concantenated into the form above when the 
		patternlist is initialized.
	*/
	{"\\bnone\\b",                      "\\bna\\b",              "",                  "",               ""          },
	{"\\bgrams?\\b",                    "\\bgs?\\b",             "",                  "",               ""          },
	{"\\bbkilograms?\\b",               "\\bkgs?\\b",            "\\bkilos?\\b",      "",               ""          },
																										            
	{"\\bcalories?\\b",                 "\\bcals?\\b",           "",                  "",               ""          },
	{"\\bjoules?\\b",                   "\\bjl\\b",              "",                  "",               ""          },
																										            
	{"\\bpounds?\\b",                   "\\blbs?\\b",            "",                  "",               ""          },
																										            
	{"\\bmillimeters?\\b",              "\\bmm\\b",              "",                  "",               ""          },
	{"\\bcentimeters?\\b",              "\\bcm\\b",              "",                  "",               ""          },
	{"\\bmeters?\\b",                   "\\bm\\b",               "",                  "",               ""          },
	{"\\bkilometers?\\b",				"\\bkm\\b",              "\\bclicks?\\b",     "",               ""          },
	{"\\binch(?:es)?\\b",               "\\bin\\b",              "\\b\"\\b",          "\\b''\\b",       ""          },
	{"\\bfoot\\b",                      "\\bfeet\\b",            "\\bft\\b",          "\\b'\\b",        ""          },
	{"\\bmiles?\\b",                    "\\bmi\\b",	             "",                  "",               ""          },
																										            
	{"\\bsquare\\s*centimeters?\\b",    "\\bsqcm\\b",            "\\bcmsq\\b",        "\\bcm2\\b",      ""          },
	{"\\bsquare\\s*meters?\\b",         "\\bsqm\\b",             "\\bmsq\\b",         "\\bm2\\b",       ""          },
	{"\\bsquare\\s*kilometers?\\b",     "\\bsqkm\\b",            "\\bkmsq\\b",        "\\bkm2\\b",      ""          },
	{"\\bsquare\\s*inch(?:es)?\\b",	    "\\bsqin\\b",            "\\binsq\\b",        "\\bin2\\b",      ""          },		
	{"\\bsquare\\s*foot\\b",            "\\bsquare feet\\b",     "\\bsqft\\b",        "\\bftsq\\b",     "\\bft2\\b"	},
	{"\\bacres?\\b",                    "\\bac\\b",              "",                  "",               ""          },
	{"\\bsquare\\s*mile\\b",            "\\bsqmi\\b",            "\\bmi2\\b",         "",               ""          },
																						                            
	{"\\bmilliliters?\\b",              "\\bml\\b",              "",                  "",               ""          },
	{"\\bliters?\\b",                   "\\bl\\b",               "",                  "",               ""          },
																						                            
	{"\\bseconds?\\b",                  "\\bsecs?\\b",           "\\bs\\b",           "",               ""          },
	{"\\bminutes?\\b",                  "\\bmins?\\b",           "",                  "",               ""          },
	{"\\bhours?\\b",                    "\\bhrs?\\b",            "\\bh\\b",           "",               ""          },
	{"\\bdays?\\b",                     "\\bd\\b",               "",                  "",               ""          },
	{"\\bweeks?\\b",                    "\\bwks?\\b",            "\\bw\\b",           "",               ""          },
	{"\\bmonths?\\b",                   "\\bmths?\\b",           "",                  "",               ""          },
	{"\\byears?\\b",                    "\\b[^a-zA-Z]y\\b",      "\\byrs?\\b",        "",               ""          },
																						                            
    {"\\bkelvins?\\b",                  "\\bk\\b",               "\\bklvns?\\b",      "",               ""          },
    {"\\bcelsius\\b",                   "\\b\370?\\s*c\\b",      "",                  "",               ""          },
    {"\\bfarenheit\\b",                 "\\b\370?\\s*f\\b",      "",                  "",               ""          }
};

/**
	Summary information for a measurement unit representation.
*/
struct UNIT
{
public:
   std::string name;		///< Name of unit.
   UNIT_TYPE type;			///< Type of unit.
   UNIT_MEASURE measure;	///< Type of measure representing unit.
};

/** Class for representing units and handling unit conversions.

*/
class __EXPORT__ Units
{
	/// \todo Test all unit conversions!!
public:
	Units();
	Units(std::string us);

   int Parse(UNIT & out,OME_SCALAR *pOutVal=NULL, UNIT_MEASURE unitConv=UM_NONE);


   static UNIT_TYPE GetTypeForMeasure(const UNIT_MEASURE um);
   static bool IsSI(const UNIT_MEASURE um);
   static bool IsMetric(const UNIT_MEASURE um);

   static int ConvertVal(const OME_SCALAR inVal, const UNIT_MEASURE inType, const UNIT_MEASURE outType, OME_SCALAR & outVal);
   

//more to come....

private:
	std::string m_unitStr;			///< String representation of units used.

	static std::list<std::regex> m_patternList;		///< list of regex patterns used to find measures.

	static std::list<std::regex> InitPatternList();

	//conversions self-explanatory; not documented in doxygen
	/// @cond DOX_NO_DOC
   static OME_SCALAR ConvertTime(const OME_SCALAR inVal, const UNIT_MEASURE inType, const UNIT_MEASURE outType);
   static OME_SCALAR ConvertMass(const OME_SCALAR inVal, const UNIT_MEASURE inType, const UNIT_MEASURE outType);
   static OME_SCALAR ConvertTemp(const OME_SCALAR inVal, const UNIT_MEASURE inType, const UNIT_MEASURE outType);
   static OME_SCALAR ConvertEnergy(const OME_SCALAR inVal, const UNIT_MEASURE inType, const UNIT_MEASURE outType);
   static OME_SCALAR ConvertLength(const OME_SCALAR inVal, const UNIT_MEASURE inType, const UNIT_MEASURE outType);
   static OME_SCALAR ConvertVolume(const OME_SCALAR inVal, const UNIT_MEASURE inType, const UNIT_MEASURE outType);
   static OME_SCALAR ConvertArea(const OME_SCALAR inVal, const UNIT_MEASURE inType, const UNIT_MEASURE outType);
   /// @endcond
};

