#pragma once

#include "StateVar.h"

/** A Conveyor- style StateVariable.
	Presently, it is of a fixed size and discrete
	@todo update to conform to XMILE spec.
*/
class __EXPORT__ Conveyor : public StateVar
{
public:

	Conveyor();
	Conveyor(STLString id, STLString name);
	Conveyor(TI2Element* pCurrElem, const STLString & tag, const STLString & filename);
	Conveyor(const Conveyor & c);

	Conveyor& operator=(const Conveyor & c);
	bool operator==(const Conveyor & c);
	Conveyor& operator=(const OME_SCALAR & val);
	Conveyor& operator=(EnumEntry* pEE);
	Conveyor& operator=(ModelEnum* pME);
	
	virtual ~Conveyor(void);

	//virtual OME_SCALAR GetValue() const;
	//virtual OME_SCALAR GetValue(const size_t & inst) const;
	//virtual OME_SCALAR GetValueForRawInst(const size_t & inst) const;

	virtual OMETYPE GetOMEType(void) const { return OME_CONVEYOR; }
	virtual bool IsOMEType(OMETYPE type) const { return type == OME_CONVEYOR ? true : StateVar::IsOMEType(type); }
	virtual void Reset(BaseManager* pBm);
	virtual STLString GetXMLRepresentation(const unsigned int indent = 0, const unsigned int inc = 4);
	virtual void GetXMLAttributes(StrArray & out) const;
	virtual void GetXMLSubNodes(StrArray & out, const unsigned int indent = 0, const unsigned int inc = 4);
	virtual const OMEChar* GetXMLTag() const { return "conveyor"; }

	//virtual OME_SCALAR GetDerivative();
	virtual Conveyor* Clone() { return new Conveyor(*this); };

	virtual void AdjustFlows(const size_t & inst);
	virtual size_t GetRequestedSlotCount() const;

	virtual void BatchInitialize(const OME_SCALAR time, BaseManager* pBm);

	OME_SCALAR GetTotalInTransit() const;
	OME_SCALAR GetTotalInTransit(const size_t & inst) const;
	/** Set the transit time for any quantity going through the conveyor.
		@param tt The transit time to assign.
	*/
	inline void SetTransitTime(const OME_SCALAR & tt) { m_transitTime = tt; }
	/** @return The Conveyor's present transit time. */
	inline OME_SCALAR GetTransitTime() const { return m_transitTime; }

	/** @return true if the Conveyor has a limited capacity; false otherwise.*/
	inline bool HasCapacity() const { return m_capacity != OME_NOLIM; }
	/** Set a specific value capacity for the conveyor.
		@param cap The capacity to assign.
	*/
	inline void SetCapacity(const OME_SCALAR & cap) { m_capacity = cap; }
	/** Sets the Conveyor to unlimited capacity. */
	inline void SetNoCapacity() { SetCapacity(OME_NOLIM); }
	/** @return The Conveyor's capacity, or OME_NOLIM if the capacity is unlimited.*/
	inline OME_SCALAR GetCapacity() const { return m_capacity; }
	/** Stop or start the Conveyor.
		@param arrest if true, stops the Conveyor; otherwise values will continue to pass through the conveyor.
	*/
	inline void SetArrestedState(const bool & arrest) { m_isArrested = arrest; }

	/** @return true if the conveyor is arrested (stopped), false otherwise. */
	inline bool GetArrestedState() const { return m_isArrested; }

	/** @return the tail index used by the conveyor. */
	inline size_t GetTailIndex() const { return m_tailIndex; }

	/** @return The expression describing how values enter transit.*/
	inline STLString GetTransitExpr() const { return m_transitExpr; };

	/** Retrives the expression used for transit.
		@param expr The expression used to describe the Conveyor transit.
	*/
	inline void SetTransitExpr(const STLString & expr) { m_transitExpr = expr; }

	virtual const OMEChar* ToString(const OMEChar tabChar = ' ', const unsigned int indent = 0, const unsigned int inc = 4) const;
protected:
	OME_SCALAR m_transitTime;  ///< The time it should take for an object to pass through the conveyor.
	size_t m_tailIndex;		   ///< The model index for the final value slot.
	OME_SCALAR m_capacity;	   ///< The total amount of stuff that a conveyor can carry at a time.
	bool m_isArrested;		   ///< Flag indicated if the conveyor is stalled or not.
	STLString m_transitExpr;   ///< The expression used to describe what goes in to the conveyor.

	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem, const STLString & tag, const STLString & filename);
	void Duplicate(const Conveyor & c);

	virtual void SetModelIndex(const size_t & index);
};