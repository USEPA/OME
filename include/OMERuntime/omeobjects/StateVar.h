#pragma once

#include "OMEObject.h"
#include "Evaluable.h"

/** Model State Variable Object.
	Object that represents a tracked variable within a simulation.
*/
class __EXPORT__ StateVar : public Evaluable
{
public:
	/** Default Constructor */
   StateVar( void ) : Evaluable() { }
	  /**	Simple Constructor.
			@param id Unique ID of new object.
			@param name  name of new object.
   */
   StateVar( STLString id, STLString name ) : Evaluable(id,name,0.0) { }
   /**	XML Constructor.
		This constructor instantiates the object based on the arguments passed in.
			@param pCurrElem  the current Tiny XML Element
			@param tag   the name of the desired tag containing the info.
			@param filename  the filename associated with the XML info.
			@see	GetXMLForObject()
   */
   StateVar(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
   {
	   OMEObject::PopulateFromXML(pCurrElem,tag,filename);
	   CheckConstant();
   }

   /** Copy Constructor
		@param sv the StateVar to copy
   */
   StateVar(const StateVar & sv) { Duplicate(sv); };
   /** Assignment Operator.
		@param sv The StateVar to use in the assignment.
		@return A reference to the current StateVar.
	*/
   StateVar& operator=(const StateVar & sv) { Duplicate(sv); return *this; };
   bool operator==(const StateVar & sv);
   StateVar& operator=(const OME_SCALAR & val)
   {
	   Evaluable::SetValue(val);
	   return *this;
   }

   StateVar& operator=(EnumEntry* pEE)
   {
	   Evaluable::SetValue(pEE);
	   return *this;
   }

   StateVar& operator=(ModelEnum* pME)
   {
	   Evaluable::SetValue(pME);
	   return *this;
   }
   virtual ~StateVar(void) { Detach();  }

   virtual OMETYPE GetOMEType( void ) const { return OME_STATEVAR; }
   virtual bool IsOMEType(OMETYPE type) const { return type==OME_STATEVAR ? true : Evaluable::IsOMEType(type); }

   virtual void Reset(BaseManager* pBm);

   //virtual OME_SCALAR GetDerivative();

   //xml
   virtual STLString GetXMLRepresentation(const unsigned int indent=0, const unsigned int inc=4);
   virtual void GetXMLAttributes(StrArray & out) const;
  virtual void GetXMLSubNodes(StrArray & out,const unsigned int indent=0,const unsigned int inc=4);
  virtual const OMEChar* GetXMLTag() const { return "state"; }

  virtual unsigned int Prioritize(unsigned int currPriority,const bool isInitial=false);
  virtual StateVar* Clone() { return new StateVar(*this);};

  
	/** Find the number of Flows that contribute positively to the StateVars value.
		@return The number of Flows pointing toward the StateVar.
	*/
  inline unsigned int GetInputCount() const { return (unsigned int)m_inputs.size();};
  
	/** Find the number of Flows that contribute negatively to the StateVars value.
		@return The number of Flows pointing away from the StateVar.
	*/
  inline unsigned int GetOutputCount() const { return (unsigned int)m_outputs.size();};

  void AddInput(Flow* pFlow); 
  void AddOutput(Flow* pFlow);

  void RemoveInput(Flow* pFlow);
  void RemoveOutput(Flow* pFlow);

  /** Find the Flows that contribute positively to the StateVars value.
	@return An array of Flows pointing toward the StateVar.
	*/
  inline FlowArray& GetInputs() {return m_inputs;};
  /** Find the Flows that contribute negatively to the StateVars value.
	@return An array of Flows pointing away from the StateVar.
	*/
  inline FlowArray& GetOutputs(){return m_outputs;};

  virtual void Detach();
  
  virtual void AdjustFlows(const size_t & inst);

protected:
	FlowArray m_inputs;				///< Flows that contribute to an increase in value during an integration step.
	FlowArray m_outputs;			///< Flows that contribute to a decrease in value during an integration step.

	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename);
    void Duplicate(const StateVar & sv);
    virtual void CheckConstant() { SetIsConstant(false); }
};

