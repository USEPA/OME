#pragma once

#include "OMEDefines.h"
#include "XMLLoadable.h"

enum PORT_DIR { PD_NONE = 0, PD_INBOUND, PD_OUTBOUND };

class Model;

/** Interface class that serves as a partial foundation for model-port/interface style objects
*/
class __EXPORT__ MultiPort
{
public:

	virtual ~MultiPort() { /* Equivalent of detatch*/ }

	/** Default Constructor */
	MultiPort(void) { }
	/** Add source object name to submodel names list.
	@param src The name to add.
	*/
	virtual void AddSubSourceID(STLString src)=0;
	/** Add target object name to submodel names list.
	@param trg The name to add.
	*/
	virtual void AddSubTargetID(STLString trg) = 0;

	/** Remove provided name from the list of named submodel sources.
	@param src The name to remove.
	*/
	virtual void RemoveSubSourceID(STLString src) = 0;
	/** Remove provided name from the list of named submodel targets.
	@param trg The name to remove.
	*/
	virtual void RemoveSubTargetID(STLString trg) = 0;

	/** Removes all associated pointers that reside in the provided submodel.
	@param mdl the submodel to remove from this SubmodelPort.
	*/
	virtual void RemoveSubmodel(Model* mdl)=0;

	/** Add an ID for an inbound object contained in the outer model.
	@param src The unique identifer for the object to be added from the outer model.
	*/
	virtual void AddOutSourceID(STLString src) = 0;
	/** Add an ID for an outbound object contained in the outer model.
	@param trg The unique identifer for the object to be added from the outer model.
	*/
	virtual void AddOutTargetID(STLString trg) = 0;

	/** Remove an ID for an inbound object contained in the outer model.
	@param src The unique identifer for the object to be removed.
	*/
	virtual void RemoveOutSourceID(STLString src) = 0;
	/** Remove an ID for an outbound object contained in the outer model.
	@param trg The unique identifer for the object to be removed.
	*/
	virtual void RemoveOutTargetID(STLString trg) = 0;

	/** @return true if pMdl points the interior model for this port; otherwise, false. */
	virtual bool IsSubModel(Model* pMdl) const = 0; //override should use m_pParentModel. // { return pMdl == m_pParentModel; }
	/** @param pMdl Pointer to the model to query against.
		@return true if pMdl points to the outer model, false otherwise.
	*/
	virtual bool IsOutModel(Model* pMdl) const = 0;

	/** Returns a collection of all names associated with the submodel.
	@param out the CStrArray to populate.
	*/
	virtual void GetSubIDs(StrArray & out) = 0;
	/** Returns names of parent source and targets, if available.
	@param out the CStrArray to populate.
	*/
	virtual void GetOutIDs(StrArray & out) = 0;

	/** @return The number of linked submodel source objects.*/
	virtual size_t GetSubSourceCount() const = 0;
	/** @return The number of linked submodel target objects.*/
	virtual size_t GetSubTargetCount() const=0;

	/** @return The number of linked outermodel source objects.*/
	virtual size_t GetOutSourceCount() const = 0;
	/** @return The number of linked outermodel target objects.*/
	virtual size_t GetOutTargetCount() const = 0;

	/** @return The total number of source objects.*/
	virtual size_t GetAllSourceCount() const = 0;
	/** @return The total number of target objects.*/
	virtual size_t GetAllTargetCount() const = 0;

	/** @return a StrArray containing the ids of all inbound SubmodelAssocs in the inner submodel.*/
	virtual StrArray GetSubSourceIDs() const = 0;
	/** @return a StrArray containing the ids of all outbound SubmodelAssocs in the inner submodel.*/
	virtual StrArray GetSubTargetIDs() const = 0;
	/** @return a StrArray containing the ids of all inbound SubmodelAssocs in the outer model.*/
	virtual StrArray GetOutSourceIDs() const = 0;
	/** @return a StrArray containing the ids of all outbound SubmodelAssocs in the outer model.*/
	virtual StrArray GetOutTargetIDs() const = 0;

	//virtual int AddUpObject  ( Evaluable *pObject ) {return -1;};
	//virtual int AddDownObject( Evaluable *pObject ) {return -1;};

	/** Use Identifiers to request the associated objects from the inner and outer models. */
	virtual void SyncWithParents()=0;

	/** Get Direction of information flow.
	@return PORT_DIR enum representing direction of flow.
	*/
	virtual PORT_DIR GetDirection() = 0;
	/** Update the direction-of-flow PORT_DIR flag.
	@return The updated PORT_DIR
	*/
	virtual PORT_DIR UpdateDirection() = 0;

	/** @return Pointer to Model that is on the "outside" of the SubmodelPort.*/
	virtual Model* GetOuterModel() = 0;
	/** @param pOm Pointer to Model that is to be the "outside" model of the SubmodelPort.*/
	virtual void SetOuterModel(Model* pOm) = 0;

protected:


	static inline void AddID(StrArray & vals, const STLString & id);
	static inline void RemoveID(StrArray & vals, const STLString & id);
};

#pragma region inline Declarations

/** Add an ID to a list of IDs if not already present.
@param vals The list of IDs to append id to.
@param id The id to append to vals (if it is unique within that list).
*/
void MultiPort::AddID(StrArray & vals, const STLString & id)
{
	if (ObjectPresent(id, vals) < 0)
		vals.push_back(id);
}

/** Remove an ID to a list of IDs if present.
@param vals The list of IDs to remove id from.
@param id The id to remove from vals.
*/
void MultiPort::RemoveID(StrArray & vals, const STLString & id)
{
	auto itr = vals.begin();
	for (; itr != vals.end() && *itr != id; ++itr);

	if (itr != vals.end())
		vals.erase(itr);
}

#pragma endregion