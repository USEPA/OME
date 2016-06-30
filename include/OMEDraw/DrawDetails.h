#pragma once
#include "OMEDefines.h"
#include "omeobjects/XMLLoadable.h"
#include "omeobjects/OMEObject.h"
#include "Summarizable.h"

/** 
	Class used to encapsulate drawing data for OMEObjects.
*/
class __EXPORT__ DrawDetails : public XMLLoadable,public Summarizable
{
public:
	DrawDetails(OMEObject* pObj);

	template<class T>
	DrawDetails(TI2Element* pTie,STLString & tag, STLString & filename);

	~DrawDetails();

	/** Dereference represented object as a specific pointer type.
		@return A Pointer of type T if conversion is valid; otherwise NULL.
	*/
	template<class T>
	T* operator*() { return dynamic_cast<T>(m_pSrcObj); }

	/** Dereference represented object as a specific const pointer type.
		@return A Pointer of type T if conversion is valid; otherwise NULL.
	*/
	template<class T>
	const T* operator*() const { return dynamic_cast<T>(m_pSrcObj); }

	void SetSrcObj(OMEObject* pObj);
	/** @return Pointer to represented object. */
	inline OMEObject* GetSrcObj() { return m_pSrcObj; }
	/** @return const Pointer to represented object. */
	inline const OMEObject* GetSrcObj() const { return m_pSrcObj; }

	/** @param pt The point containing the value for control point 1. */
	inline void SetCtrlPt1(const OMEPoint & pt) { m_ctrlPoint1=pt; }
	/** @return OMEPoint equivalent point to Control Point 1. */
	inline OMEPoint GetCtrlPt1() const { return m_ctrlPoint1; }

	/** @param pt The point containing the value for control point 2. */
	inline void SetCtrlPt2(const OMEPoint & pt) { m_ctrlPoint2=pt; }
	/** @return OMEPoint equivalent point to Control Point 2. */
	inline OMEPoint GetCtrlPt2() const { return m_ctrlPoint2; }

	/** @param pt The point containing the value for the origin. */
	inline void SetOrigin(const OMEPoint & pt) { m_origin=pt; }
	/** @return OMEPoint equivalent point to the object's origin. */
	inline OMEPoint GetOrigin() const { return m_origin; }

	/** @param exts The point containing the extents to apply to the object. */
	inline void SetExtents(const OMEPoint & exts) { m_dims=exts; }
	/** @return OMEPoint containing the object's extent values. */
	inline OMEPoint GetExtents() const { return m_dims; }

	/** @param bx The box used to define the objects bounding box (origin and extents). */
	inline void SetBoundingBox(const OMEBox & bx) { m_origin=bx.tl; m_dims=bx.br-m_origin; }
	/** @return Bounding box representation of origin and extents. */
	inline OMEBox GetBoundingBox() const { return OMEBox(m_origin,m_dims+m_origin); }

	/** @param bx Bounds for objects which contain internal containers. */
	inline void SetInternalBox(const OMEBox & bx) { m_internalExts=bx; }
	/** @return Box representing internal extents. */
	inline OMEBox GetInternalBox() const { return m_internalExts; }

	/** @param lbo The point containing the value for the associated label's origin. */
	inline void SetLabelOrigin(const OMEPoint & lbo) { m_lblOrigin=lbo; }
	/** @return OMEPoint equivalent point to the object's label origin. */
	inline OMEPoint GetLabelOrigin() const { return m_lblOrigin; }

	/** @param desc The description to associate with the object. */
	inline void SetDescription(const STLString & desc) { m_description=desc; }
	/** @return The description of the object. */
	inline STLString GetDescription() const { return m_description; }

	/** @param md String description of the last modification date. */
	inline void SetModDate(const STLString & md) { m_lastModifiedDate=md; }
	/** @return The last modification date. */
	inline STLString GetModDate() const { return m_lastModifiedDate; }

	/** @param vers Brief string description of the last version. */
	inline void SetVersion(const STLString & vers) { m_version=vers; }
	/** @return The object's version. */
	inline STLString GetVersion() const { return m_version; }

	/** @param un String description of the last modification date. */
	inline void SetUnits(const STLString & un) { m_unitStr=un; }
	/** @return The units associated with the object. */
	inline STLString GetUnits() const { return m_unitStr; }

	virtual STLString GetXMLRepresentation(const unsigned int indent=0,const unsigned int inc=4);
    virtual void GetXMLAttributes(StrArray & out) const;
	virtual void GetXMLSubNodes(StrArray & out,const unsigned int indent=0,const unsigned int inc=4);
	virtual const OMEChar* GetXMLTag() const { return m_pSrcObj ? m_pSrcObj->GetXMLTag() : "draw_detail"; };

	virtual const OMEChar* ToString(const OMEChar tabChar=' ', const unsigned int indent=0, const unsigned int inc=4) const;

	//master map stuff.
	static DrawDetails* DDForObject(OMEObject* pObj);
	static void ReplaceObjectForDD(OMEObject* pOld,OMEObject* pNew);
	static void CleanupAllDDs();

protected:
	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag,const STLString & filename);

private:

	OMEObject* m_pSrcObj;			///< The object being wrapped

	//union to conserve space and effectively alias values.
	union
	{
		struct
		{
			//control points are for spline types
			OMEPoint  m_ctrlPoint1;				///< Control point 0.
			OMEPoint  m_ctrlPoint2;				///< Control Point 1.
		};

		struct
		{
		   //basic box dimensions; origin should be top-left
		   OMEPoint m_origin;					///< graphical origin
		   OMEPoint m_dims;					///< XY-Dimesions
		};
	};
	
	OMEPoint m_lblOrigin;			///< Point origin for text/labels

	OMEBox m_internalExts;			///< Limits on internal dimensions. Relevant for submodels.

   //other descriptors
   STLString m_description;			///< Description of object.
   STLString m_lastModifiedDate;	///< Date/time stamp of last recorded edit session.
   STLString m_version;				///< Present version of object.
   STLString m_unitStr;				///< Unparsed string representing units.

  
   ///@cond DOX_NO_DOC
	/* Disabled copy constructor. */
   DrawDetails(DrawDetails & d) {}
   ///@endcond

	static std::map<OMEObject*,DrawDetails*>& GetDrawObjectCache();

    void ZeroOutPts();
    
   template<class T>
	void CollectionXMLDrawables(const STLString & collName, const ARRAY_TYPE<T> & arr, StrArray & out, const unsigned int indent, const unsigned int inc,const Model* pMdl,const bool & parentCheck=false);

};

#pragma region =========== Template/inline Definitions =============

/** XML constructor. Also constructs object of class type T.
	@param pTie Pointer to current tinyxml2 element.
	@param tag The current identifying tag.
	@param filename Name of the original file.
*/
template<class T>
DrawDetails::DrawDetails(TI2Element* pTie,STLString & tag, STLString & filename)
{
	ZeroOutPts();
	PopulateFromXML(pTie,tag,filename);
	m_pSrcObj=new T(pTie,tag,filename);
	if(m_pSrcObj) 
		GetDrawObjectCache()[m_pSrcObj]=this;
}

/** Effectively an override for Model's CollectionXMLRepresentation.
	@param collName The name of the collection to be applied to the grouping node.
	@param arr The objections to include in the collection.
	@param out The variable to hold the resulting xml string.
	@param indent The base indent to use for each newline.
	@param inc The amount to increase the indent by for each indentation level.
	@param pMdl Pointer to model which this DrawDetails represents.
	@param parentCheck If true, only get xml representation for each item whose parent is the current model.
*/
template<class T>
void DrawDetails::CollectionXMLDrawables(const STLString & collName, const ARRAY_TYPE<T> & arr, StrArray & out, const unsigned int indent, const unsigned int inc,const Model* pMdl,const bool & parentCheck)
{
	const STLString baseIndent(indent,' ');
	oSTLSStream temp;
	bool hit=false;
	if(!arr.empty())
	{
		temp<<baseIndent<<'<'<<collName<<'>'<<std::endl;
		for(auto itr=arr.begin(); itr!=arr.end();++itr)
		{
			if(!parentCheck || (*itr)->GetParentModel()==pMdl)
			{
				hit=true;
				temp<<GetDrawObjectCache()[(*itr)]->GetXMLRepresentation(indent+inc,inc)<<std::endl;
			}
		}
		if(hit)
		{
			temp<<baseIndent<<"</"<<collName<<'>';
			out.push_back(temp.str());
		}
	}
}
#pragma endregion