#pragma once

#include "OMEObject.h"
#include "OMEException.h"

class OMEParser;
struct DataSourceEntry;
class SDPMapEntry;
/** Type flags that determine Param behavior */
enum OME_PARAM_TYPE { PARAM_NULL=0, PARAM_CONST, PARAM_EXPR, PARAM_DATA, PARAM_SPATIAL,PARAM_OTHER};
/** Model Parameter.
	Representation of a parameter used by a model
*/
class  __EXPORT__ Param 
{   
public:
	
   Param( void ) 
	   : m_value( 0.0 ), 
	   m_isPublic( true ), 
	   m_paramType(PARAM_NULL)
   { }
    
   /** Simple Constructor.
		@param name Name to assign to Param.
		@param pt Flag indicating type of Param.
   */
   Param(const STLString & name, const OME_PARAM_TYPE & pt)
	   :m_targetName(name), m_value(0.0), m_isPublic(true), m_paramType(pt){}


   /**Copy Constructor
		@param parm the Param object to copy.
	*/
   Param(const Param & parm) { Duplicate(parm); };

   /** Single value constructor.
		@param name the name of the target component.
		@param val The const value used in initialization.
   */
   Param(const STLString & name, const OME_SCALAR & val)
	   :m_targetName(name), m_value(val), m_isPublic(true), m_paramType(PARAM_CONST){}

   /** SpatialDataProvider associated constructor. 
		@param name The name of the parameter.
		@param pME Pointer to SDPMapEntry.
   */
   Param(const STLString & name, SDPMapEntry* pME)
	   :m_targetName(name), m_pME(pME), m_isPublic(true), m_paramType(PARAM_SPATIAL){}

	/** Assignment operator.
		@param parm The Param to copy.
		@return Reference to this.
	*/
   Param& operator=(const Param & parm) { Duplicate(parm); return *this; };

   bool operator==(const Param & parm) const;
   /** Destructor */
   virtual ~Param(void) { ClearParam();}

	bool SetConstValue(const OME_SCALAR & val, const bool & oride=false);
	bool SetExpression(const STLString & expr, const bool & oride=false);
	
	//bool SetSpatialSource(ISpatialDataProvider* pSdp, const size_t & col, const bool & oride = false);
	/** Retreive constant value stored in Param.
		@return Value if Param is of the constant type.
		@throws OMEFuncOpException exception if Param is not of the PARAM_CONST type.
	*/
	OME_SCALAR GetConstValue() const { if (m_paramType != PARAM_CONST) throw OMEFuncOpException("Param::GetConstValue","Param not const."); return m_value; };

	bool GetExpression(STLString & expr);

	/** @return name of the Target object.*/
	inline STLString GetTargetName() const { return m_targetName; }
	/** Set the name of the target object. 
		@param name The name to assign to the Param.
	*/
	inline void SetTargetName(const STLString & name) { m_targetName = name; }

	/** @return Current Param type flag.*/
	OME_PARAM_TYPE GetParamType() const { return m_paramType; };

	void ClearParam();

	bool InitializeEval(Evaluable* pEval);

private:
	 bool m_isPublic;				///< Visibility of parameter.
	 STLString m_targetName;		///< The name of the target object.
	 OME_PARAM_TYPE m_paramType;	///< flag designating what type of data the parameter is storing

	union 
	{
		OME_SCALAR m_value;				///< basic stored value.
		OMEChar* m_expr;				///< Pointer to Param expression.
		SDPMapEntry* m_pME;	///< Pointer to SpatialDataProvider associated with Param value.
		
	};

   void Duplicate(const Param & parm);
   

}; 

