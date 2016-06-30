#pragma once
#include "OMEObject.h"
#include "Link.h"
#include "Evaluable.h"

/** Model Flow Object.
	Object that represents direct flow of information from one object to another.
*/
class __EXPORT__ Flow : public Evaluable,public Link
{   
public:
	/** Default Constructor */
   Flow( void ) : Evaluable(), Link(),m_flowObj(NULL) { }
	  /** Simple Constructor
	Takes unique ids for source and target; assumes that pointers will be supplied later.
	@param src unique id of source object.
	@param trg unique id of target object.
*/
   Flow(STLString src, STLString trg) : Evaluable(), Link(src, trg), m_flowObj(NULL) {}
	 /** Simple Constructor
	Takes unique ids for source and target; assumes that pointers will be supplied later.
	@param id Unique id of Flow.
	@param src unique id of source object.
	@param trg unique id of target object.
*/
   Flow(STLString id, STLString src, STLString trg) : Evaluable(id, 0.0), Link(src, trg), m_flowObj(NULL) {}
	 /** Simple Constructor
	Takes unique ids for source and target; assumes that pointers will be supplied later.
	@param id unique id of Flow.
	@param src unique id of source object.
	@param trg unique id of target object.
	@param pSrc pointer to source object.
	@param pTrg pointer to target object.
*/
   Flow(STLString id, STLString src, STLString trg, Evaluable* pSrc, Evaluable* pTrg) : Evaluable(id, 0.0), Link(src, trg, pSrc, pTrg), m_flowObj(NULL) {}
	   /**	XML Constructor.
		This constructor instantiates the object based on the arguments passed in.
			@param pCurrElem  the current Tiny XML Element
			@param tag   the name of the desired tag containing the info.
			@param filename  the filename associated with the XML info.
			@see	GetXMLForObject()
   */
   Flow(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
	   :m_flowObj(NULL)
   {
	   OMEObject::PopulateFromXML(pCurrElem,tag,filename);
	   CheckConstant();
   }


   /** Copy Constructor
		@param flo the Flow object to duplicate
   */
   Flow(const Flow & flo) { Duplicate(flo); };
   /** Assignment operator.
		@param flo The Flow object to use in assignment.
		@return a reference to the Flow object.
   */
   Flow& operator=(const Flow & flo) { Duplicate(flo); return *this; };
   /** Assignment operator.
   @param val The raw value to assign.
   @return a reference to the Flow object.
   */
   Flow& operator=(const OME_SCALAR & val)
   {
	   Evaluable::SetValue(val);
	   return *this;
   }
   /** Assignment operator.
   @param pEE The enum value to assign.
   @return a reference to the Flow object.
   */
   Flow& operator=(EnumEntry* pEE)
   {
	   Evaluable::SetValue(pEE);
	   return *this;
   }
   /** Assignment operator.
   @param pME The enum class to assign.
   @return a reference to the Flow object.
   */
   Flow& operator=(ModelEnum* pME)
   {
	   Evaluable::SetValue(pME);
	   return *this;
   }
   bool operator==(const Flow & rhs);

   /** Default destructor. */
   virtual ~Flow(void) { Detach(); }

   virtual void Detach();

   /** Addition Assignment operator. Assigns the target object from the rhs to this.
		@param rhs The Flow object that provides the targets.
		@return a reference to the Flow object.
	*/
   Flow& operator+=(const Flow & rhs);
	 /** Addition operator. Non-commutable; Creates a new Flow object from the source of lhs and the target from rhs.
		@param lhs The Flow object that provides the sources.
		@param rhs The Flow object that provides the targets.
		@return a new Flow object that is a combination of the source from lhs and target from rhs.
	*/
   friend Flow operator+ (const Flow & lhs, const Flow & rhs);

   virtual OMETYPE GetOMEType( void ) const { return OME_FLOW; }
   virtual bool IsOMEType(OMETYPE type) const { return type==OME_FLOW ? true : Evaluable::IsOMEType(type); }
   virtual LINKTYPE GetLinkType( ) {return LN_FLOW;};

	virtual void SetPSource(OMEObject* src);
	virtual void SetPTarget(OMEObject* trg);


   virtual STLString GetXMLRepresentation(const unsigned int indent=0,const unsigned int inc=4);
   virtual void GetXMLAttributes(StrArray & out) const;
   virtual void GetXMLSubNodes(StrArray & out,const unsigned int indent=0,const unsigned int inc=4);
   virtual const OMEChar* GetXMLTag() const { return "flow"; }

	virtual void Reset(BaseManager* pBm);
	virtual unsigned int Prioritize(unsigned int currPriority, const bool isInitial=false);

	virtual Flow* Clone() { return new Flow(*this);};

	virtual OME_SCALAR GetValue() const;
	virtual OME_SCALAR GetValue(const size_t & inst) const;
protected:
	Evaluable* m_flowObj; ///< Object with derivative value.

	void Duplicate(const Flow & flo);

	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename);
}; 

//Flow operator+ (const Flow & lhs, const Flow & rhs);