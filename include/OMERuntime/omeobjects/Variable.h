#pragma once
#include "OMEObject.h"
#include "Evaluable.h"
/** Model Variable Object.
	Object that represents auxillary/intermediate values during a simulation run.
*/
class __EXPORT__ Variable : public Evaluable
{   
public:

	/** Limit type indicators */
	enum LIMITTYPE {NO_LIMIT=0, ///< Has no limits.
					HAS_MIN,	///< Has a minimum limit.
					HAS_MAX,	///< Has a maximum limit.
					HAS_MINMAX	///< Has both a minimum and maximum limit.
	};

	/** Default Constructor */
   Variable( void ) : Evaluable(),m_minMaxLimit(0),m_expectsFile(false),m_initOnly(false){ }
   /** Simple Constructor
		@param id Unique identifier for the object.
		@param name the name of the object.
		@param v a default value.
	*/
   Variable( STLString id, STLString name, OME_SCALAR v=0.0 ) : Evaluable( id,name,v ),m_minMaxLimit(0),m_expectsFile(false),m_initOnly(false) { }
   /**	XML Constructor.
		This constructor instantiates the object based on the arguments passed in.
			@param pCurrElem  the current Tiny XML Element.
			@param tag   the name of the desired tag containing the info.
			@param filename  the filename associated with the XML info.
			@see	GetXMLForObject()
   */
   Variable(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
	   :Evaluable(),m_minMaxLimit(0), m_expectsFile(false), m_initOnly(false)
   {
	   OMEObject::PopulateFromXML(pCurrElem,tag,filename);
	   CheckConstant();
   }

   /** Copy Constructor 
		@param var variable to copy from.
   */
   Variable(const Variable & var) { Duplicate(var); };
   /** Assignment operator.
		@param var The Variable to use in assingment.
		@return Reference to the current Variable.
	*/
   Variable& operator=(const Variable & var) { Duplicate(var); return *this; };
   Variable& operator=(const OME_SCALAR & val)
   {
	   Evaluable::SetValue(val);
	   return *this;
   }

   Variable& operator=(EnumEntry* pEE)
   {
	   Evaluable::SetValue(pEE);
	   return *this;
   }

   Variable& operator=(ModelEnum* pME)
   {
	   Evaluable::SetValue(pME);
	   return *this;
   }
   bool operator==(const Variable & var);

   virtual ~Variable(void) { Detach(); }

   virtual OMETYPE GetOMEType( void ) const { return OME_VARIABLE; }
   virtual bool IsOMEType(OMETYPE type) const { return type==OME_VARIABLE ? true : Evaluable::IsOMEType(type); }

   //returns false if, STLString d OME_SCALAR v value is outside of set min/max bounds
   virtual bool SetValue(const OME_SCALAR value, bool clamp);
   virtual void SetValue(const OME_SCALAR value) { SetValue(value, false); }

   //the following 3 functions return false if min > max
   bool SetMinValue(OME_SCALAR min); 
   bool SetMaxValue(OME_SCALAR max);
   bool SetMinMaxValue(OME_SCALAR min, OME_SCALAR max);
   
   void ClearMin();
   void ClearMax();
   void ClearMinMax();

   virtual void BatchInitialize(const OME_SCALAR time, BaseManager* pBm);

   //call minMaxLimit first to make sure that min/max values are set;
   //if they are not, then GetMin/GetMax will return junk.
   OME_SCALAR GetMin() {return m_minValue;}		/**< Getter for min value */
   OME_SCALAR GetMax() {return m_maxValue;}		/**< Getter for max value */
   
   int GetMinMaxLimit() {return m_minMaxLimit;}	/**< Getter for Limit type */

	virtual void Reset(BaseManager* pBm);

   virtual STLString GetXMLRepresentation(const unsigned int indent=0,const unsigned int inc=4);
   virtual void GetXMLAttributes(StrArray & out) const;
	virtual void GetXMLSubNodes(StrArray & out,const unsigned int indent=0,const unsigned int inc=4);
	virtual const OMEChar* GetXMLTag() const { return "variable"; }

	virtual Variable* Clone() { return new Variable(*this);};

	/** Set whether or not this Variable expects to be initialized from a file.
		@param ef If true Variable will be marked as expecting a file for initialization.
	*/
	inline void SetExpectsFile(const bool & ef) { m_expectsFile=ef; }
	/** @return Whether or not the Variable expects initialization values from a file. */
	inline bool GetExpectsFile() const { return m_expectsFile; }
	/** Set whether or not this Variable is only evaluated during the intialization process.
	@param io If true Variable will only be evaluated during intialization.
	*/
	void SetInitOnly(const bool & io);
	/** @return true if Variable is only evaluated during initialization; false otherwise. */
	inline bool GetInitOnly() const { return m_initOnly; }

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

	OME_SCALAR m_minValue;			///< optional minimum accepted value (inclusive lower bound)
	OME_SCALAR m_maxValue;			///< optional maximum accepted value (inclusive upper bound)
	int m_minMaxLimit;				///< value used to track status of value bounds.

	bool m_expectsFile;				///< Flag indicating whether or not the Variable anticipates being initialized from a file source.
	bool m_initOnly;				///< Flag indicating that the Variable should only be Evaluated during initialization.

	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename);
	void Duplicate(const Variable & var);
	bool WalkInfluences(Variable *start);

	virtual void CheckConstant();
}; 
