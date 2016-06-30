#pragma once

#include "OMEObject.h"
#include "SubmodelAssoc.h"
#include "MultiPort.h"

class SubmodelPort;
/** Port type to be used with SubmodelAssoc objects. */
class __EXPORT__ AssocPort : public OMEObject, public MultiPort
{
public:
	virtual ~AssocPort() { /*Detach();*/ }

	/** Default Constructor */
	AssocPort(void) : OMEObject(),m_pOuterModel(NULL), m_direction(PD_NONE)  { }
	
	/**	XML Constructor.
	This constructor instantiates the object based on the arguments passed in.
	@param pCurrElem  the current Tiny XML Element
	@param tag   the name of the desired tag containing the info.
	@param filename  the filename associated with the XML info.
	@see	GetXMLForObject()
	*/
	AssocPort(TI2Element* pCurrElem, const STLString & tag, const STLString & filename) : m_pOuterModel(NULL), m_direction(PD_NONE)
	{
		OMEObject::PopulateFromXML(pCurrElem, tag, filename);
	}

	/** Copy Constructor
	@param smp the AssocPort to copy.
	*/
	AssocPort(const AssocPort & smp) { Duplicate(smp); };
	/** Assignment Operator.
	@param smp The AssocPort to use in the assignment.
	@return A reference to the current AssocPort.
	*/
	AssocPort& operator=(const AssocPort & smp) { Duplicate(smp); return *this; };
	bool operator==(const AssocPort & rhs);

	virtual OMETYPE GetOMEType(void) const { return OME_ASSOCPORT; }
	virtual bool IsOMEType(OMETYPE type) const { return type == OME_ASSOCPORT || OMEObject::IsOMEType(type); };

	void AddSubInAssoc(SubmodelAssoc* pSrc, const bool useID = true);
	void AddSubOutAssoc(SubmodelAssoc* pTrg, const bool useID = true);

	virtual void SetParentModel(Model*mdl);

	void RemoveSubInAssoc(SubmodelAssoc* pSrc);
	void RemoveSubOutAssoc(SubmodelAssoc* pTrg);
	virtual void RemoveSubmodel(Model* mdl);

	void AddOutInAssoc(SubmodelAssoc* pSrc, const bool useID = true);
	void AddOutOutAssoc(SubmodelAssoc* pTrg, const bool useID = true);
	void RemoveOutInAssoc(SubmodelAssoc* pSrc);
	void RemoveOutOutAssoc(SubmodelAssoc* pTrg);

	/** Return a pointer to a specific instance of a connected Submodel source object.
	@param ind index of the desired instance.
	@return a pointer to the requested Evaluable object, or NULL if the index is out of bounds.
	*/
	SubmodelAssoc* GetSubInAssoc(const int ind) const { return (ind >= 0 && ind < m_smInAssocs.size()) ? m_smInAssocs[ind] : NULL; };

	/** Return a pointer to a specific instance of a connected Submodel target object.
	@param ind index of the desired instance.
	@return a pointer to the requested Evaluable object, or NULL if the index is out of bounds.
	*/
	SubmodelAssoc* GetSubOutAssoc(const int ind) const { return (ind >= 0 && ind < m_smOutAssocs.size()) ? m_smOutAssocs[ind] : NULL; };

	/** Return a pointer to a specific instance of a connected Outer model source object.
	@param ind index of the desired instance.
	@return a pointer to the requested Evaluable object, or NULL if the index is out of bounds.
	*/
	SubmodelAssoc* GetOutInAssoc(const int ind) const { return (ind >= 0 && ind < m_omInAssocs.size()) ? m_omInAssocs[ind] : NULL; };
	/** Return a pointer to a specific instance of a connected Outer model target object.
	@param ind index of the desired instance.
	@return a pointer to the requested Evaluable object, or NULL if the index is out of bounds
	*/
	SubmodelAssoc* GetOutOutAssoc(const int ind) const { return (ind >= 0 && ind < m_omInAssocs.size()) ? m_omInAssocs[ind] : NULL; };

	SubmodelAssoc* GetInAssoc(const int ind) const;
	SubmodelAssoc* GetOutAssoc(const int ind) const;

	virtual STLString GetXMLRepresentation(const unsigned int indent = 0, const unsigned int inc = 4);
	virtual void GetXMLAttributes(StrArray & out) const;
	virtual void GetXMLSubNodes(StrArray & out, const unsigned int indent = 0, const unsigned int inc = 4);
	virtual const OMEChar* GetXMLTag() const { return "assoc_port"; }

	bool IsSubAssoc(SubmodelAssoc* sa);
	bool IsOutAssoc(SubmodelAssoc* sa);

	virtual bool IsSubModel(Model* pMdl) const { return pMdl == m_pParentModel; }

	virtual void SyncWithParents();

	virtual AssocPort* Clone() { return new AssocPort(*this); }

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

	AssocArray m_smInAssocs;		///< Pointers to instances of submodel sources.
	AssocArray m_smOutAssocs;		///< Pointers to instances of submodel targets.

	AssocArray m_omInAssocs;		///< Pointers to instances of outer model sources.
	AssocArray m_omOutAssocs;		///< Pointers to instances of outer model targets.

	StrArray m_smSources;		///< List of submodel source objects.
	StrArray m_smTargets;		///< List of submodel target objects.

	StrArray m_omSources;		///< List of outer model source objects.
	StrArray m_omTargets;		///< List of outer model target objects.

	PORT_DIR m_direction;		///< Flag identifying uniform direction of information flow (or lack thereof).

	Model* m_pOuterModel;	///< Model that encompasses the submodel that SubmodelPort belongs to.

	void Duplicate(const AssocPort & ap);
	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem, const STLString & tag, const STLString & filename);


private:
	inline void RemoveAssoc(AssocArray & assocs, SubmodelAssoc* pAssoc);
};

#pragma region Inline Declarations

/** Remove a SubmodelAssoc from an AssocPort
@param assocs Array of SubmodelAssoc pointers to remove pAssoc from.
@param pAssoc Pointer to a SubmodelAssoc object to disassociate from the AssocPort.
*/
void AssocPort::RemoveAssoc(AssocArray & assocs, SubmodelAssoc* pAssoc)
{
	auto itr = assocs.begin();
	for (; itr != assocs.end() && *itr != pAssoc; ++itr);

	if (itr != assocs.end())
		assocs.erase(itr);
}

#pragma endregion