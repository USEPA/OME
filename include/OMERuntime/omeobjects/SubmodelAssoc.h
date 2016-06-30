#pragma once
#include "OMEObject.h"
#include "Link.h"
#include <map>

class Model;
class AssocPort;
/** A link-like object used to create associations between submodels; similar to Simile's Role arrows*/
class __EXPORT__ SubmodelAssoc : public OMEObject, public Link
{
public:

	/// \todo add support for submodel ports instead of just models

	SubmodelAssoc();
	/** Detailed Constructor.
		@param name Name of the Submodel object.
		@param baseId The id of the base Model.
		@param subId The id of the target Model.
		@param desc A description of the SubmodelAssoc.
	*/
	SubmodelAssoc(const STLString & name, const STLString & baseId, const STLString & subId, const STLString & desc)
		:OMEObject(name,desc),Link(baseId,subId)
	{};

	/** XML Constructor.
		@param pCurrElem Pointer to the current TinyXml element.
		@param tag Html tag used for SubmodelAssoc.
		@param filename Path to the source file.
	*/
	SubmodelAssoc(TI2Element* pCurrElem,const STLString & tag, const STLString & filename)
		{OMEObject::PopulateFromXML(pCurrElem,tag,filename);};

	/** Copy Constructor.
		@param sma The SubmodelAssoc to copy.
	*/
	SubmodelAssoc(const SubmodelAssoc & sma) { Duplicate(sma); };

	virtual ~SubmodelAssoc();

	void BindSubmodels(Model* pBase, Model* pSub);
	void BindSubmodels(AssocPort* pBase, Model* pSub);
	void BindSubmodels(Model* pBase, AssocPort* pSub);
	void BindSubmodels(AssocPort* pBase, AssocPort* pSub);
	void BindSubmodels(OMEObject* pBase, OMEObject* pSub);

	void JoinFromReferences(Model* pRoot);

	int GetBaseNumInstances();

	bool IsAssociated(const Model* pMc) const;
	
	bool IsBase(const Model* pMc) const;
	bool IsSub(const Model* pMc) const;

	/** @return The id for the base Model. */
	inline STLString GetBaseId() const { return m_source; };
	/** @return The id for the sub Model. */
	inline STLString GetSubId() const { return m_target; };

	virtual OMETYPE GetOMEType() const { return OME_SUBASSOC; }
	virtual bool IsOMEType(OMETYPE type) const { return type == OME_SUBASSOC || OMEObject::IsOMEType(type); };
	virtual STLString GetXMLRepresentation(const unsigned int indent, const unsigned int inc);
	virtual void GetXMLAttributes(StrArray & out) const;
	virtual const OMEChar* GetXMLTag() const { return "assoc"; }

	ModelInstance* GetBaseModelInstance(Model* bm) const;
//	EvalCollection* GetSubValues(const STLString key, const int instance);

	Model* GetBaseModel();
	Model* GetSubModel();

	void RefreshCollectionCache();
	virtual SubmodelAssoc* Clone() { return new SubmodelAssoc(*this); }

	virtual void SetPSource(OMEObject* src);
	virtual void SetPTarget(OMEObject* trg);

	void Detach();
protected:


    //templatization will not work here with EvalCollection on Clang/gcc with just a forward declaration
	//std::map<STLString,ARRAY_TYPE<EvalCollection> > m_subECMap;		///< Cache of EvalCollections from associated Model.

	void GetXMLAttributeMap(XMLAttrVector & out);
	void Duplicate(const SubmodelAssoc & sma);
	bool WalkAssociated(const Model* pMc, const bool & fromSrc);
	
};

