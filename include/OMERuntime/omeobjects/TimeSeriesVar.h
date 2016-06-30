#pragma once
#include "OMEDefines.h"
#include "Variable.h"

/** Variable subclass used to represent a time series (ie a variable whose value changes based on timestep. */
class __EXPORT__ TimeSeriesVar : public Variable
{
public:
	/** Flag indicating how intermediate timesteps are handled.*/
	enum INTERPMODE 
	{
		LAST=1,		///< Use the last specified timestep value.
		NEAREST,		///< Use the value closest to the current time.
		INTERPOLATE,	///< Linearly interpolate between the upper and lower designated time values.
		FIXED			///< Value is constant and does not change.
	};
	TimeSeriesVar();
	TimeSeriesVar(const STLString & n, const STLString & d);

	virtual ~TimeSeriesVar();

	TimeSeriesVar(const TimeSeriesVar & tsv);
	TimeSeriesVar(const Variable & var);
	TimeSeriesVar(TI2Element* pCurrElem,const STLString & tag, const STLString & filename);

	virtual OMETYPE GetOMEType( void ) const { return OME_TSVAR; }
   virtual bool IsOMEType(OMETYPE type) const { return type==OME_TSVAR ? true : Variable::IsOMEType(type); }

   virtual OME_SCALAR Evaluate(const OME_SCALAR time,BaseManager* pBm);
	virtual OME_SCALAR Initialize(const OME_SCALAR time,BaseManager* pBm);
	virtual void Reset(BaseManager* pBm);

   virtual STLString GetXMLRepresentation(const unsigned int indent=0,const unsigned int inc=4);
   virtual void GetXMLAttributes(StrArray & out) const;
    virtual void GetXMLSubNodes(StrArray & out,const unsigned int indent=0,const unsigned int inc=4);
	virtual const OMEChar* GetXMLTag() const { return "varts"; }

	virtual TimeSeriesVar* Clone() { return new TimeSeriesVar(*this);};

	void SetSeries(const ARRAY_TYPE<OME_SCALAR> & times, const ARRAY_TYPE<OME_SCALAR> & values);
	void SetSeries(const OMESclrArray & values);
	void SetFixed(const OME_SCALAR & value);

	/** Add a value for a specific simulation time. 
		@param time The time to apply the value val.
		@param val The value to apply when the simulation time reaches time.
	*/
	inline void AddTimeVal(const OME_SCALAR & time, const OME_SCALAR & val) { m_timeEntries[time]=val; }
	OME_SCALAR GetValueForTime(OME_SCALAR time);
	OMESclrArray GetTimes();

	/** @return the repeat interval for this time series, or 0.0 if the time series does not repeat. */
	inline OME_SCALAR GetInterval() const { return m_interval; }
	/** @param inter The interval for repetition for the time series; 0.0 indicates no repetition to be applied. */
	inline void SetInterval(const OME_SCALAR & inter) {m_interval=inter; }
	/** @return A flag indicating the current interpolation method used for intermediate time steps. */
	inline INTERPMODE GetInterpolation() const { return m_mode; }
	/** @param m The new interpolation mode to use. */
	inline void SetInterpolation(const INTERPMODE & m) { m_mode=m; }
	virtual void CheckConstant() {SetIsConstant(false);}

	static TimeSeriesVar* ReplaceVariable(Variable* pVar);

	virtual const OMEChar* ToString(const OMEChar tabChar=' ', const unsigned int indent=0, const unsigned int inc=4) const;

	//use Evaluable's operators
	/*using Evaluable::operator<;
	using Evaluable::operator<=;
	using Evaluable::operator>;
	using Evaluable::operator>=;
	using Evaluable::operator==;
	using Evaluable::operator!=;
	using Evaluable::operator+=;
	using Evaluable::operator-=;
	using Evaluable::operator*=;
	using Evaluable::operator/=;
	using Evaluable::operator+;
	using Evaluable::operator*;
	using Evaluable::operator-;
	using Evaluable::operator/;*/
protected:

	typedef std::map<OME_SCALAR,OME_SCALAR> TSMap;		///< Map for associating specific times with values.
	TSMap m_timeEntries;								///< Map containing values for specific simulation times.
	INTERPMODE m_mode;								///< Method of clamping/interpolation to be used for times that fall between specified periods.
	OME_SCALAR m_interval;								///< Repeat interval of entire time series; an interval of 0.0 indicates no repitition.

	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename);
	void Duplicate(const TimeSeriesVar & var);

};

