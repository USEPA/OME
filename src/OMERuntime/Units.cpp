#include "Units.h"
#include <regex>
#include <list>
#include <string>
#include <sstream>

std::list<std::regex> Units::m_patternList=Units::InitPatternList();

/** Default constructor. */
Units::Units()
:m_unitStr("")
{

}

/** Simple Constructor.
	@param us String representing measure units used by this Unit object
*/
Units::Units(std::string us)
	:m_unitStr(us)
{

}

/** Derive specific unit type from arguments.
	@param out UNIT struct to populate.
	@param pOutVal Optional pointer to store the found value.
	@param unitConv The optional UNIT_MEASURE that dictates the unit conversion to take place.
	@return true if parsing and unit conversion was successful; 0 otherwise.
*/
int Units::Parse(UNIT & out, OME_SCALAR* pOutVal,UNIT_MEASURE unitConv)
{
	using namespace std;

	string searchString(m_unitStr);
	regex searchReg;
	smatch patternMatch;

	list<regex>::const_iterator itr;

   /// location could be found by using sub-indices of match
	for(itr=m_patternList.begin(); itr!=m_patternList.end() && patternMatch.empty(); ++itr)
	{
		searchReg=(*itr);
		regex_search(searchString,patternMatch,searchReg);
	}

	if(patternMatch.empty())
		return 0;

	//find measure based on which regex statement was successful
	out.measure=(UNIT_MEASURE)(distance<list<regex>::const_iterator>(m_patternList.begin(),itr)-1);

	//Find type for matched measure
	out.type=GetTypeForMeasure(out.measure);
   OME_SCALAR currVal;
   istringstream(patternMatch[1])>>currVal;
	if(unitConv)
	  {
	  //perform unit conversion if asked to do so
	  if(Units::ConvertVal(currVal,out.measure,unitConv,currVal))
		 out.measure=unitConv;
	  else
		 return 0;
	  }
   
	//grab the numeric value if asked to.
	if(pOutVal)
	  *pOutVal=currVal;

	//use matched pattern as unit name.
	istringstream(patternMatch[2])>>out.name;
	
	return 1;
}

/** Find unit type for a specific measure.
	@param um The UNIT_MEASURE to evaluate.
	@return The UNIT_TYPE associated with um.
*/
UNIT_TYPE Units::GetTypeForMeasure(const UNIT_MEASURE um)
{
	UNIT_TYPE ret=UT_NONE;

	switch(um)
	{
	//mass
	case UM_GRAM:
	case UM_KG:
		ret=UT_MASS;
		break;
	//energy
	case UM_CAL:
	case UM_JOULE:
		ret=UT_ENERGY;
		break;
	//force
	case UM_LBS:
		ret=UT_FORCE;
		break;
	//length
	case UM_MM:
	case UM_CM:
	case UM_METER:
	case UM_KM:
	case UM_IN:
	case UM_FT:
	case UM_MILE:
		ret=UT_LENGTH;
		break;
	//area
	case UM_CM2:
	case UM_M2:
	case UM_KM2:
	case UM_IN2:
	case UM_FT2:
	case UM_ACRE:
	case UM_MI2:
		ret=UT_AREA;
		break;
	//volume
	case UM_ML:
	case UM_LITER:
		ret=UT_VOLUME;
		break;
	//time
	case UM_SEC:
	case UM_MIN:
	case UM_HR:
	case UM_DAY:
	case UM_WEEK:
	case UM_MONTH:
	case UM_YEAR:
		ret=UT_TIME;
		break;
	//temperature
	case UM_CELSIUS:
	case UM_KELVIN:
	case UM_FAREN:
		ret=UT_TEMP;
		break;
	};

	return ret;
}

/** Attempt to convert one value from one measure to another.
	@param inVal The value to convert.
	@param inType The initial unit measure.
	@param outType The desired target unit measure.
	@param outVal Double that will hold the converted value, if successful.
	@return 1 if the conversion was successful; 0 otherwise. Conversion will fail if measures are of different types
		(such as attempting to convert from length to volume).
*/
int Units::ConvertVal(const OME_SCALAR inVal, const UNIT_MEASURE inType, const UNIT_MEASURE outType, OME_SCALAR & outVal)
   {
   UNIT_TYPE classification=GetTypeForMeasure(inType);
   //only allow conversion from like to like
   if( classification != GetTypeForMeasure(outType))
	  return 0;

   switch(classification)
   {
   
   case UT_TIME:
	  outVal=ConvertTime(inVal,inType,outType);
	  break;
	case UT_MASS:
	   outVal=ConvertMass(inVal,inType,outType);
	   break;
	case UT_TEMP:
	   outVal=ConvertTemp(inVal,inType,outType);
	   break;
	case UT_FORCE:
	   //no conversions for now
	   outVal=inVal;
	   break;
	case UT_ENERGY:
		outVal=ConvertTemp(inVal,inType,outType);
		break;
	case UT_LENGTH:
	   outVal=ConvertLength(inVal,inType,outType);
	   break;
	case UT_VOLUME: 
	   outVal=ConvertVolume(inVal,inType,outType);
		break;   
   case UT_AREA:
		outVal=ConvertArea(inVal,inType,outType);
   default:
	  return 0;
   }

   return 1;
   }

/** Check to see if a measure is a base SI unit.
	@param um The UNIT_MEASURE to test.
	@return true if the measure is a base SI unit; false otherwise.
*/
bool Units::IsSI(const UNIT_MEASURE um)
{
	bool ret=false;

	switch(um)
	{
	case UM_KG:
	case UM_JOULE:
	case UM_METER:
	case UM_M2:
	case UM_LITER:
	case UM_SEC:
	case UM_CELSIUS: //Kelvin?
		ret=true;
	}

	return ret;
}

/** Check to see if a measure is a metric unit.
	@param um The UNIT_MEASURE to test.
	@return true if the measure is a metricI unit; false otherwise.
*/
bool Units::IsMetric(const UNIT_MEASURE um)
{
	bool ret=false;

	switch(um)
	{
	case UM_GRAM:
	case UM_KG:
	case UM_JOULE:
	case UM_MM:
	case UM_CM:
	case UM_METER:
	case UM_KM:
	case UM_CM2:
	case UM_M2:
	case UM_KM2:
	case UM_ML:
	case UM_LITER:
	case UM_SEC:
	case UM_MIN:
	case UM_HR:
	case UM_DAY:
	case UM_WEEK:
	case UM_MONTH:
	case UM_YEAR:
	case UM_CELSIUS:
	case UM_KELVIN:
		ret=true;
	}

	return ret;
}

/** Initialize patterns used to recognize the various measures.
	@return a list of regex statements used to parse out measure info.
*/
std::list<std::regex> Units::InitPatternList()
{
	//cache concantenated pattern strings
	using namespace std;
	static const string patternHead("(?:([0-9]*[.]?[0-9]+)[.]?)?\\s*(");
	static const string patternTail(")\\b");

   list<regex> patList;
	const char *currChoice;
	int j;
	for(int i=0; i<UM_COUNT; i++)
	{
		string newEntry(patternHead);
		string parts;
		j=0;
		do
		{
			currChoice = measurePatterns[i][j];
			if(currChoice[0]!='\0')
				parts+='|'+string(currChoice);


			j++;

		} while(j< maxEntriesPerUnit && currChoice[0]!='\0');
		//add parts, omitting the first pipe
		newEntry.append(++parts.begin(),parts.end());
		patList.push_back(regex(newEntry+patternTail,regex::icase));
	}

   return patList;
}

///@cond DOX_NO_DOC
//================================================================================================
//metric defines
#define METRIC_UPSTEP( x ) x*0.1
#define METRIC_DOWNSTEP( x ) x*10.0

#define MILLI_STEP( x ) x*0.001
#define CENTI_STEP( x ) x*0.01
#define DECI_STEP( x ) METRIC_UPSTEP(x)
#define DECA_STEP( x ) METRIC_DOWNSTEP(x)
#define HECTO_STEP( x ) x*100.0
#define KILO_STEP( x )  x*1000.0

//time defines
#define MINTOSECS( x )  x*60.0
#define SECSTOMIN( x )  1/(MINTOSECS(x))
#define HRTOMIN( x ) MINTOSECS(x)
#define MINTOHR( x ) SECSTOMIN(x)
#define DAYTOHR( x ) x*24.0
#define HRTODAY( x ) 1/(DAYTOHR(x))
#define WKTODAY( x ) x*7.0
#define DAYTOWK( x ) 1/(WKTODAY(x))
#define YRTOMTH( x ) x*12.0
#define MTHTOYR( x ) 1/(YRTOMTH(x))
#define YRTODAY( x ) x*365.0
#define DAYTOYR( x ) 1/(YRTODAY(x))

#define MTHTODAY( x ) x*30.0
#define DAYTOMTH( x ) 1/(MTHTODAY(x))
#define MTHTOWK( x ) x*2.0
#define WKTOMTH( x ) x*0.25
#define YRTOWK( x ) x*52.0
#define WKTOYR( x ) 1/(YRTOWK(x))

//temp defines
#define CTOF( x ) (x*1.8)+32
#define FTOC( x ) (x-32)*5/9

//length defines
#define INCHTOMM( x ) x*25.4
#define MMTOINCH( x ) x*0.0393701
#define FOOTTOINCH( x ) x*12.0
#define INCHTOFOOT( x ) 1/(FOOTTOINCH(x))
#define MILETOFOOT( x ) x*5280.0
#define FOOTTOMILE( x ) 1/(MILETOFOOT(x))

//area defines
#define SQINTOSQCM( x ) x*6.4516
#define SQCMTOSQIN( x ) 1/(SQINTOSQCM(x))
#define SQFTTOSQIN( x ) x*144.0
#define SQINTOSQFT( x ) 1/(SQFTTOSQIN(x))
#define ACRETOSQFT( x ) x*43560.0
#define SQFTTOACRE( x ) 1/(ACRETOSQFT(x))
#define SQMITOACRE( x ) x*640.0
#define ACRETOSQMI( x ) 1/(SQMITOACRE(x))

//util define
/* CASE_UPSTEP and CASE_DOWNSTEP are for conversions involving increasing and decreasing measure units, respectively.
	The idea is this: have an ordered switch statement for the starting UNIT_MEASURE. Increment conversions until
	we have reached the target conversion measure.
	The CASE_UPSTEP/DOWNSTEP statement is placed in the body of the case statement; if w (the target type) is equal
	to x (the last value converted to), then the conversion is complete and we can break out of the switch statement.
	Otherwise, increment/decrement x by 1 and carry out the next unit conversion and store in y.

*/
#define CASE_UPSTEP( w,x,y,z ) if(w==x) \
									break; \
									x++;	\
								y=z
#define CASE_DOWNSTEP( w,x,y,z ) if(w==x) \
									break; \
									x--;	\
								y=z

//function-specific defines for CovertLength() and ConvertArea(); For arbitrary use use CASE_UPSTEP, CASE_DOWNSTEP
#define FCASE_UPSTEP(x) CASE_UPSTEP(modOType,modIType,outVal,x)
#define FCASE_DOWNSTEP(x) CASE_DOWNSTEP(modOType,modIType,outVal,x)

//function-specific defines for CovertTime; For arbitrary use use CASE_UPSTEP, CASE_DOWNSTEP
#define TCASE_UPSTEP(x) CASE_UPSTEP(modOType,modIType,ret,x)
#define TCASE_DOWNSTEP(x) CASE_DOWNSTEP(modOType,modIType,ret,x)

OME_SCALAR Units::ConvertTime(const OME_SCALAR inVal, const UNIT_MEASURE inType, const UNIT_MEASURE outType)
   {
   //conversion complete
   if(inType==outType)
	  return inVal;

   UNIT_MEASURE modOType=outType;
   int modIType=inType;

   OME_SCALAR ret=inVal;

   if(modIType < modOType)
	  {
	  //convert to coarser measure
	  switch(modIType)
		 {
	  case UM_SEC:
		  TCASE_UPSTEP(SECSTOMIN(ret));
	  case UM_MIN:
		 TCASE_UPSTEP(MINTOHR(ret));
		 break;
	  case UM_HR:
		 TCASE_UPSTEP(HRTODAY(ret));
	  case UM_DAY:
		 if(modIType==modOType)
			 break;

		 switch(outType)
			{
		 case UM_WEEK:
			ret=DAYTOWK(inVal);
			break;
		 case UM_MONTH:
			ret=DAYTOMTH(inVal);
			break;
		 case UM_YEAR:
			ret=DAYTOYR(inVal);
			break;
			};
		 break;
	  case UM_WEEK:
			if(outType==UM_MONTH)
			   ret=WKTOMTH(inVal);
			else
			   ret=WKTOYR(inVal);
		 break;
	  case UM_MONTH:
			TCASE_UPSTEP(MTHTOYR(ret));
	  case UM_YEAR:
		 //nothing to do here
		 break;
		 };
	  }
   else  //finer measures
	  {
	  switch(inType)
		 {
		case UM_YEAR:
			switch(outType)
			{
			case UM_MONTH:
				ret=YRTOMTH(inVal);
				break;
			case UM_WEEK:
				ret=YRTOWK(inVal);
			default:
				ret=ConvertTime(YRTODAY(inVal),UM_DAY,outType);
			}
			break;
	   case UM_MONTH:
			
			if(outType==UM_WEEK)
			   ret=MTHTOWK(outType);
			else
			   ret=ConvertTime(MTHTODAY(inVal),UM_DAY,outType);
			break;
			
	   case UM_WEEK:
			TCASE_DOWNSTEP(WKTODAY(inVal));
			break;
		case UM_DAY:
			TCASE_DOWNSTEP(DAYTOHR(ret));
		case UM_HR:
			TCASE_DOWNSTEP(HRTOMIN(ret));
	  case UM_MIN:
		 TCASE_DOWNSTEP(MINTOSECS(ret));
	  case UM_SEC:
		 //do nothing
		 break;     
		 };
	  }

   return ret;
   }

OME_SCALAR Units::ConvertMass(const OME_SCALAR inVal, const UNIT_MEASURE inType, const UNIT_MEASURE outType)
{
	if(inType==outType)
	  return inVal;

	//must be grams to kilograms
	if(inType==UM_GRAM)
		return MILLI_STEP(inVal);

	//must be kilograms to grams
	return KILO_STEP(inVal);
}

OME_SCALAR Units::ConvertTemp(const OME_SCALAR inVal, const UNIT_MEASURE inType, const UNIT_MEASURE outType)
{
	static const OME_SCALAR KELVIN_DIFF=272.15f;

	if(inType==outType)
	  return inVal;

	OME_SCALAR outVal;

	//in type is farenheit
	if(inType==UM_FAREN)
	{
		//convert to Celsius
		outVal=FTOC(inVal);

		//offset celsius if kelvin is output
		if(outType==UM_KELVIN)
			outVal+=KELVIN_DIFF;
	}
	else if(outType==UM_FAREN)
	{
		//target is farenheit
		outVal=inVal;
		//convert kelvin to celsius, if necessary
		if(inType==UM_KELVIN)
			outVal-=KELVIN_DIFF;

		//convert celsius to farenheit
		outVal=CTOF(outVal);
	}
	else if(outType==UM_KELVIN)
		//celsius to kelvin
		outVal=inVal+KELVIN_DIFF;
	else
		//kelvin to celsius
		outVal=inVal-KELVIN_DIFF;

	return outVal;
}

OME_SCALAR Units::ConvertEnergy(const OME_SCALAR inVal, const UNIT_MEASURE inType, const UNIT_MEASURE outType)
{
	OME_SCALAR outVal;
	const OME_SCALAR JTOC = 0.239005736;

	if(inType==outType)
	  return inVal;

	//Joule to calorie
	if(inType==UM_JOULE)
		outVal=inVal*JTOC;
	else
		//calorie to Joule
		outVal=inVal/JTOC;

	return outVal;
}

OME_SCALAR Units::ConvertLength(const OME_SCALAR inVal, const UNIT_MEASURE inType, const UNIT_MEASURE outType)
{
	if(inType==outType)
	  return inVal;

	OME_SCALAR outVal=inVal;

	UNIT_MEASURE modOType=outType;
	int modIType=inType;


	if(IsSI(outType))
	{
		//convert to SI type
		if(!IsSI(inType))
		{
			//convert non SI to base si (mm) before proceeding
			outVal=ConvertLength(inVal,inType,UM_IN);

			modIType=UM_MM;
			outVal=INCHTOMM(outVal);
		}

		if(modOType>inType)
		{
			//convert to coarser measure
			switch(modIType)
			{
			case UM_MM:
				FCASE_UPSTEP(DECI_STEP(outVal));
			case UM_CM:
				FCASE_UPSTEP(CENTI_STEP(outVal));
			case UM_METER:
				FCASE_UPSTEP(MILLI_STEP(outVal));
			case UM_KM:
				//nothing to do
				break;
			}
		}
		else
		{
			//convert to finer measure
			switch(modIType)
			{
			case UM_KM:
				FCASE_DOWNSTEP(KILO_STEP(outVal));
			case UM_METER:
				FCASE_DOWNSTEP(HECTO_STEP(outVal));
			case UM_CM:
				FCASE_DOWNSTEP(DECA_STEP(outVal));
			case UM_MM:
				//nothing to do
				break;
			}
		}
	}
	else
	{
		//convert to non-SI
		if(IsSI(inType))
		{
			//convert input to non-SI (inches) before proceeding
			outVal=ConvertLength(inVal,inType,UM_MM);
			modIType=UM_IN;
			outVal=MMTOINCH(outVal);
		}

		if(modOType>inType)
		{
			//coarser measure
			switch(modIType)
			{
			case UM_IN:
				FCASE_UPSTEP(INCHTOFOOT(outVal));
			case UM_FT:
				FCASE_UPSTEP(FOOTTOMILE(outVal));
			case UM_MILE:
				//do nothing
				break;
			}
		}
		else
		{
			//finer measure
			switch(modIType)
			{
			case UM_MILE:
				FCASE_DOWNSTEP(MILETOFOOT(outVal));
			case UM_FT:
				FCASE_DOWNSTEP(FOOTTOINCH(outVal));
			case UM_IN:
				//do nothing
				break;
			}
		}
		
	}
	return outVal;
}

OME_SCALAR Units::ConvertVolume(const OME_SCALAR inVal, const UNIT_MEASURE inType, const UNIT_MEASURE outType)
{
	if(inType==outType)
	  return inVal;

	OME_SCALAR outVal;
	
	if(outType==UM_LITER)
	{
		//mL to L
		outVal= MILLI_STEP(inVal);
	}
	else
	{
		//L to mL
		outVal= KILO_STEP(inVal);
	}

	return outVal;
}

OME_SCALAR Units::ConvertArea(const OME_SCALAR inVal, const UNIT_MEASURE inType, const UNIT_MEASURE outType)
{
	if(inType==outType)
	  return inVal;

	OME_SCALAR outVal=inVal;
	UNIT_MEASURE modOType=outType;
	int modIType=inType;

	if(IsSI(outType))
	{
		//convert to SI unit
		if(!IsSI(inType))
		{
			//convert to base SI (cm2) before proceeding
			outVal=ConvertArea(inVal,inType,UM_IN2);
			outVal=SQINTOSQCM(outVal);
			modIType=UM_CM2;
		}

		if(modIType < modOType)
		{
			//coarser measure
			switch(modIType)
			{
			case UM_CM2:
				FCASE_UPSTEP(MILLI_STEP(outVal)*0.1);
			case UM_M2:
				FCASE_UPSTEP(MILLI_STEP(MILLI_STEP(outVal)));
			case UM_KM2:
				//do nothing
				break;
			}
		}
		else
		{
			//finer measure
			switch(modIType)
			{
			case UM_KM2:
				FCASE_DOWNSTEP(KILO_STEP(KILO_STEP(outVal)));
			case UM_M2:
				FCASE_DOWNSTEP(KILO_STEP(outVal)*10.0);
			case UM_CM2:
				//do nothing
				break;
			}
		}
	}
	else
	{
		//convert to non-SI
		if(IsSI(inType))
		{
			//convert to non-SI (square inches) before proceeding
			outVal=ConvertArea(inVal,inType,UM_CM2);
			outVal=SQCMTOSQIN(outVal);
			modIType=UM_IN2;
		}

		if(modIType < modOType)
		{
			//coarser measure
			switch(modIType)
			{
			case UM_IN2:
				FCASE_UPSTEP(SQINTOSQFT(outVal));
			case UM_FT2:
				FCASE_UPSTEP(SQFTTOACRE(outVal));
			case UM_ACRE:
				FCASE_UPSTEP(ACRETOSQMI(outVal));
			case UM_MI2:
				//do nothing
				break;
			}
		}
		else
		{
			//finer measure
			switch(modIType)
			{
			case UM_MI2:
				FCASE_DOWNSTEP(SQMITOACRE(outVal));
			case UM_ACRE:
				FCASE_DOWNSTEP(ACRETOSQFT(outVal));
			case UM_FT2:
				FCASE_DOWNSTEP(SQFTTOSQIN(outVal));
			case UM_IN2:
				//do nothing
				break;
			}
		}
	}

	return outVal;
}

///@endcond