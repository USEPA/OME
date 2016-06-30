#pragma once

#include "OMEObject.h"
#include "Link.h"
#include "Evaluable.h"
#include "ErrCollection.h"

/** Model Influence Object.
	Object that represents an influence of one object on another.
*/
class  __EXPORT__ Influence : public Evaluable, public Link
{
public:
	/** Default Constructor */
	Influence(void) : Evaluable(), Link(), m_useCurrentVal(false), m_priorOnly(false){}
   /** Simple Constructor.
	Takes unique ids for source and target; assumes that pointers will be supplied later.
	@param src unique id of source object.
	@param trg unique id of target object.
*/
	Influence(STLString src, STLString trg) : Evaluable(), Link(src, trg), m_useCurrentVal(false), m_priorOnly(false) {}
   /** Simple Constructor.
	Takes unique ids for source and target; assumes that pointers will be supplied later.
	@param name name of influence.
	@param src unique id of source object.
	@param trg unique id of target object.
*/
	Influence(STLString name, STLString src, STLString trg) : Evaluable(name, 0.0), Link(src, trg), m_useCurrentVal(false), m_priorOnly(false) {}

     /** Full Constructor.
	Takes unique ids for source and target; assumes that pointers will be supplied later.
	@param name name of influence.
	@param src unique id of source object.
	@param trg unique id of target object.
	@param pSrc pointer to source object.
	@param pTrg pointer to target object.
*/
   Influence( STLString name, STLString src, STLString trg, Evaluable* pSrc, Evaluable* pTrg)
	   : Evaluable(name, 0.0), Link(src, trg, pSrc, pTrg), m_useCurrentVal(false), m_priorOnly(false) {}

	/** Constructor.
			@param id Unique identifier for Influence object.
			@param name name of influence.
			@param pSrc pointer to source object.
			@param pTrg pointer to target object.
	*/
   Influence(STLString id, STLString name, OMEObject* pSrc, OMEObject* pTrg)
	   : Evaluable(id, name, 0.0), Link(pSrc ? pSrc->GetID() : "", pTrg ? pTrg->GetID() : ""), m_useCurrentVal(false), m_priorOnly(false)
   {
	   //assign pointers here so that full linking process will be applied.
		if(pSrc)
			SetPSource(pSrc);
		if(pTrg)
			SetPTarget(pTrg);
   }
     /**	XML Constructor.
		This constructor instantiates the object based on the arguments passed in.
			@param pCurrElem  the current Tiny XML Element
			@param tag   the name of the desired tag containing the info.
			@param filename  the filename associated with the XML info.
			@see	GetXMLForObject()
   */
   Influence(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
	   :m_priorOnly(false)
   {
       OMEObject::PopulateFromXML(pCurrElem,tag,filename);
       CheckConstant();
   }

   /** Copy Constructor.
	@param infl the influence to copy from
	*/
   Influence(const Influence & infl) { Duplicate(infl); };

   virtual ~Influence(){ Detach();}

   /** Assignment operator.
		@param infl The Influence object to use in assignment.
		@return a reference to the Influence object.
   */
   Influence& operator=(const Influence & infl) { Duplicate(infl); return *this; };
   bool operator==(const Influence & rhs);

      /** Addition Assignment operator. Assigns the target object from the rhs to this.
		@param rhs The Influence object that provides the target.
		@return a reference to the Influence object.
   	*/
   Influence & operator+=(const Influence & rhs);
   //friend Influence operator+ (const Influence & lhs, const Influence & rhs);
   


   virtual OMETYPE GetOMEType( void ) const { return OME_INFLUENCE; }
   virtual bool IsOMEType(OMETYPE type) const { return type==OME_INFLUENCE ? true : Evaluable::IsOMEType(type); }
   virtual LINKTYPE GetLinkType( ) {return LN_INFLUENCE;};
	
	virtual void SetPSource(OMEObject* src);
	virtual void SetPTarget(OMEObject* trg);


	virtual OME_SCALAR Evaluate(const OME_SCALAR time,BaseManager* pBm);
	virtual void Reset(BaseManager* pBm);

   virtual STLString GetXMLRepresentation(const unsigned int indent=0, const unsigned int inc=4);
   virtual void GetXMLAttributes(StrArray & out) const;
   virtual void GetXMLSubNodes(StrArray & out,const unsigned int indent=0,const unsigned int inc=4);
   virtual const OMEChar* GetXMLTag() const { return "influence"; }

   virtual unsigned int Prioritize(unsigned int currPriority, const bool isInitial=false);

   virtual OME_SCALAR GetValue() const;
   virtual Influence* Clone() { return new Influence(*this);};

   virtual void Detach();

   /** @return Flag indicating that current values should be used.*/
   inline bool GetUseCurrentVal() const { return m_useCurrentVal; }
   /** Set the currentVal flag.
		@param use The value of the current flag.*/
   inline void SetUseCurrentVal(const bool & use) { m_useCurrentVal = use; }

   /** @return Flag indicating if the Influence is solely used for prioritization.*/
   inline bool GetForPrioritizationOnly() const { return m_priorOnly; }
   /** Set the proirOnly flag.
   @param fpo The value of the proirOnly flag.*/
   inline void SetForPrioritizationOnly(const bool & fpo) { m_priorOnly = fpo; }

protected:

	bool m_priorOnly;		///< flag that identifies whether the Influence solely exists for prioritizing generated objects.
	bool m_useCurrentVal;	///< flag that identifies whether to use value in current timestep or not.

	void Duplicate(const Influence & infl);

	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename);
};

//Influence operator+ (const Influence & lhs, const Influence & rhs);