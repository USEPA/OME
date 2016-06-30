#pragma once

#include "OMEObject.h"
#include "Link.h"
#include "Evaluable.h"
#include "Model.h"
#include "ModelInstance.h"
#include "MultiPort.h"

/** Model object that represents submodel connection point
*/
class __EXPORT__ SubmodelPort : public MultiPort, public Evaluable
{
public:

	virtual ~SubmodelPort() { Detach();  }


	/** Default Constructor */
	SubmodelPort(void) : MultiPort(), Evaluable(), m_pOuterModel(NULL), m_direction(PD_NONE){}
   /** Simple Constructor
	Takes unique ids for source and target; assumes that pointers will be supplied later.
	@param name The name of the SubmodelPort.
*/
   SubmodelPort( STLString name)
	   : MultiPort(), Evaluable(name, 0.0), m_pOuterModel(NULL), m_direction(PD_NONE){}
	 /**	XML Constructor.
		This constructor instantiates the object based on the arguments passed in.
			@param pCurrElem  the current Tiny XML Element
			@param tag   the name of the desired tag containing the info.
			@param filename  the filename associated with the XML info.
			@see	GetXMLForObject()
   */
   SubmodelPort(TI2Element* pCurrElem, const STLString & tag, const STLString & filename): m_pOuterModel(NULL), m_direction(PD_NONE)
   {
	   OMEObject::PopulateFromXML(pCurrElem,tag,filename);
	   CheckConstant();
   }

   /** Copy Constructor
	@param smp the SubmodelPort to copy.
	*/
   SubmodelPort(const SubmodelPort & smp) { Duplicate(smp); };
   /** Assignment Operator.
		@param smp The SubmodelPort to use in the assignment.
		@return A reference to the current SubmodelPort.
	*/
   SubmodelPort& operator=(const SubmodelPort & smp) { Duplicate(smp); return *this; };
   bool operator==(const SubmodelPort & rhs);

   virtual OMETYPE GetOMEType( void ) const { return OME_SUBPORT; }
   virtual bool IsOMEType(OMETYPE type) const { return type == OME_SUBPORT || Evaluable::IsOMEType(type); };

	void AddSubmodelSource(Evaluable* pSrc,const bool useID=true);
	void AddSubmodelTarget(Evaluable* pTrg,const bool useID=true);

	virtual void SetParentModel(Model*mdl);

	void RemoveSubmodelSource(Evaluable* pSrc);
	void RemoveSubmodelTarget(Evaluable* pTrg);
	virtual void RemoveSubmodel(Model* mdl);

	void AddOutmodelSource(Evaluable* pSrc,const bool useID=true);
	void AddOutmodelTarget(Evaluable* pTrg,const bool useID=true);
	void RemoveOutmodelSource(Evaluable* pSrc);
	void RemoveOutmodelTarget(Evaluable* pTrg);

	/** Return a pointer to a specific instance of a connected Submodel source object.
		@param ind index of the desired instance.
		@return a pointer to the requested Evaluable object, or NULL if the index is out of bounds.
	*/
	Evaluable* GetSubmodelSource(const int ind) const { return (ind >= 0 && ind < m_smPSources.size()) ? m_smPSources[ind] : NULL;};

	/** Return a pointer to a specific instance of a connected Submodel target object.
		@param ind index of the desired instance.
		@return a pointer to the requested Evaluable object, or NULL if the index is out of bounds.
	*/
	Evaluable* GetSubmodelTarget(const int ind) const { return (ind >= 0 && ind < m_smPTargets.size()) ? m_smPTargets[ind] : NULL;};

	/** Return a pointer to a specific instance of a connected Outer model source object.
		@param ind index of the desired instance.
		@return a pointer to the requested Evaluable object, or NULL if the index is out of bounds.
	*/
	Evaluable* GetOutmodelSource(const int ind) const { return (ind >= 0 && ind < m_omPSources.size()) ? m_omPSources[ind] : NULL;};
	/** Return a pointer to a specific instance of a connected Outer model target object.
		@param ind index of the desired instance.
		@return a pointer to the requested Evaluable object, or NULL if the index is out of bounds
	*/
	Evaluable* GetOutmodelTarget(const int ind) const { return (ind >= 0 && ind < m_omPTargets.size()) ? m_omPTargets[ind] : NULL;};

	Evaluable* GetAllSource(const int ind) const;
	Evaluable* GetAllTarget(const int ind) const;

	virtual OME_SCALAR Evaluate(const OME_SCALAR time,BaseManager* pBm);
	virtual void Reset(BaseManager* pBm);

	virtual STLString GetXMLRepresentation(const unsigned int indent=0, const unsigned int inc=4);
	virtual void GetXMLAttributes(StrArray & out) const;
	virtual void GetXMLSubNodes(StrArray & out,const unsigned int indent=0,const unsigned int inc=4);
	virtual const OMEChar* GetXMLTag() const { return "port"; }

	bool IsSubEval(Evaluable* eval);
	bool IsOutEval(Evaluable* eval);

	virtual bool IsSubModel(Model* pMdl) const { return pMdl==m_pParentModel; }

	void GetSubObjects(EvalArray & out);
	void GetOutObjects(EvalArray & out);

	void GetSubRepObjects(EvalArray & out);
	void GetOutRepObjects(EvalArray & out);

	virtual void SyncWithParents();

	virtual unsigned int Prioritize(unsigned int currPriority, const bool isInitial);

	virtual void FindNextDownObjects(EvalArray & out, bool excludeInfl=true,bool bypassPorts=true);
	//FindAllDownObjects

	virtual void RemoveUpObject  ( Evaluable *pObject );
	virtual void RemoveDownObject( Evaluable *pObject );

	void RemoveEval(Evaluable* pEval);
	void PurgeInfluences();

	virtual SubmodelPort* Clone() { return new SubmodelPort(*this); }

	virtual OME_SCALAR GetValue() const;
	
	virtual const OME_SCALAR* GetValueAddr();
	virtual void SetValue(const OME_SCALAR val);
	virtual void Detach();

	bool LinkedToFlow() const;
// Multiport stuff
	virtual void AddSubSourceID(STLString src);
	virtual void AddSubTargetID(STLString trg);

	virtual void RemoveSubSourceID(STLString src);
	virtual void RemoveSubTargetID(STLString trg);

	virtual void AddOutSourceID(STLString src);
	virtual void AddOutTargetID(STLString trg);

	virtual void RemoveOutSourceID(STLString src);
	virtual void RemoveOutTargetID(STLString trg);

	virtual bool IsOutModel(Model* pMdl) const { return pMdl == m_pOuterModel; }

	virtual void GetSubIDs(StrArray & out);
	virtual void GetOutIDs(StrArray & out);

	virtual size_t GetSubSourceCount() const { return m_smSources.size(); }
	virtual size_t GetSubTargetCount() const { return m_smTargets.size(); }

	virtual size_t GetOutSourceCount() const { return m_omSources.size(); }
	virtual size_t GetOutTargetCount() const { return m_omTargets.size(); }

	virtual size_t GetAllSourceCount() const { return m_smSources.size() + m_omSources.size(); }
	virtual size_t GetAllTargetCount() const { return m_smTargets.size() + m_omTargets.size(); }

	virtual StrArray GetSubSourceIDs() const { return m_smSources; }
	virtual StrArray GetSubTargetIDs() const { return m_smTargets; }
	virtual StrArray GetOutSourceIDs() const { return m_omSources; }
	virtual StrArray GetOutTargetIDs() const { return m_omTargets; }

	virtual PORT_DIR GetDirection() { return m_direction; };
	virtual PORT_DIR UpdateDirection();

	virtual Model* GetOuterModel() { return m_pOuterModel; }
	virtual void SetOuterModel(Model* pOm) { m_pOuterModel = pOm; }

	
protected:

	EvalArray m_smPSources;			///< Pointers to instances of submodel sources.
	EvalArray m_smPTargets;			///< Pointers to instances of submodel targets.

	EvalArray m_omPSources;			///< Pointers to instances of outer model sources.
	EvalArray m_omPTargets;			///< Pointers to instances of outer model targets.

	StrArray m_smSources;		///< List of submodel source objects.
	StrArray m_smTargets;		///< List of submodel target objects.

	StrArray m_omSources;		///< List of outer model source objects.
	StrArray m_omTargets;		///< List of outer model target objects.

	PORT_DIR m_direction;		///< Flag identifying uniform direction of information flow (or lack thereof).

	Model* m_pOuterModel;	///< Model that encompasses the submodel that SubmodelPort belongs to.

	/** Copy another SubmodelPort
		@param smp the SubmodelPort to copy.
	*/
	void Duplicate(const SubmodelPort & smp);

	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename);

   virtual void CheckConstant() { SetIsConstant(false); }

private:

	void RemoveFromDependencies(Evaluable *pEval);

	static inline void RemoveObj(EvalArray & evals,Evaluable* pEval);
	inline void PurgeCollInfluences(StrArray & ids, EvalArray & evals, const bool useSrc);
};

#pragma region Inline Declarations

/** Remove an object from the SubmodelPort.
	@param evals Array of object pointers to remove pEval from.
	@param pEval Pointer to an Evaluable object to disassociate from the SubmodelPort. 
*/
void SubmodelPort::RemoveObj(EvalArray & evals,Evaluable* pEval)
{
	auto itr=evals.begin();
	for(; itr!=evals.end() && *itr!=pEval; ++itr);
   
	if(itr!=evals.end())
		evals.erase(itr);
}

/** Purge Influence links from SubmodelPort.
	@param ids A list of ids of objects added to evals on return.
	@param evals A list of Evaluable objects that were linked in to SubmodelPort on return.
	@param useSrc If true, update source relationships; otherwise, update target relationships.
*/
void SubmodelPort::PurgeCollInfluences(StrArray & ids, EvalArray & evals, const bool useSrc)
{
	ids.clear();
	for(unsigned int i=0; i< evals.size(); i++)
	{
		if(evals[i]->GetOMEType()==OME_INFLUENCE)
		{
			if(useSrc)
			{
				Evaluable::RemoveUpObject(evals[i]);
				evals[i]=((Influence*)evals[i])->GetPEvalSource();
				Evaluable::AddUpObject(evals[i]);
			}
			else
			{
				Evaluable::RemoveDownObject(evals[i]);
				evals[i]=((Influence*)evals[i])->GetPEvalTarget();
				Evaluable::AddDownObject(evals[i]);
			}
		}
		ids.push_back(evals[i]->GetName());
	}
}

#pragma endregion