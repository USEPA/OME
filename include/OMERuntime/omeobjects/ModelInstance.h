#pragma once
#include"OMEDefines.h"
#include "OMEObject.h"
#include "Evaluable.h"
#include "Spawner.h"

class Conveyor;
class Model;
/** Stores the bare-minimum information necessary to differentiate between model collection instances.
	Designed for compactness of size and quick access.
	There is minimal arg/bounds checking here; responsibility is left up to the caller to perform checks.
*/
class __EXPORT__ ModelInstance
{
public:

	friend class RecordMgr;

	ModelInstance(Model* pM,const size_t & ind, const bool & act=true);
	ModelInstance(const ModelInstance & inst, const size_t & ind, const size_t & parentInst, Spawner* pSpwn);
	~ModelInstance();

	/** @return true if the ModelInstance is marked as active and not dead; false otherwise. */
	inline bool IsActive() const { return GetFlag(ACTIVE_FLAG) && !GetFlag(DEAD_FLAG); };
	/** @param a The flag to use to mark activity; ModelInstance will be treated as active if a is true, and inactive if it is false.
		If the model is marked as dead, this flag is ignored.
	*/
	inline void SetActive(const bool a) { SetFlag(a,TOACTIVE_FLAG); };

	/** @return true if the ModelInstance is marked as dead; false otherwise. */
	inline bool IsDead() const { return GetFlag(DEAD_FLAG); };
	/** Marks the ModelInstance as dead. Once a ModelInstance is "dead", it becomes available for garbage collection. 
	Dead ModelInstances cannot currently be ressurrected. */
	inline void Kill() { m_flags|=TOKILL_FLAG; };
	void ApplyMortality();

	/** @return true if the ModelInstance store unique initialization vals; false otherwise. */
	inline bool HasUniqueInit() const { return GetFlag(INIT_FLAG); };

	/** @return true if the ModelInstance contains array values; false otherwise. _may not be needed_.*/
	inline bool HasArrays() const { return GetFlag(ARRAYS_FLAG); };

	/** @return true if the ModelInstance contains single values; false otherwise. _may not be needed_.*/
	inline bool HasSingleValues() const { return GetFlag(VALS_FLAG); };

	/** @return The last simulation time values were updated for this ModelInstance.*/
	inline OME_SCALAR GetLastTime() const { return m_lastEvalTime;};
	/** Set the last time values were set for this ModelInstance. 
		@param time The simulation time to set as the last update time.
	*/
	inline void SetLastTime(const OME_SCALAR time) { m_lastEvalTime=time; };

	size_t CalcDerivative(StateVar* pSv,const size_t & inst,OME_SCALAR* deriveArray, size_t ind);
	size_t CalcDerivative(Conveyor* pSv, const size_t & inst, OME_SCALAR* deriveArray, size_t ind);
	
	/** @return parent model of ModelInstance. */
	inline Model* GetParentModel() { return m_pParentModel; }
	ModelInstance* Clone();

	/** Retrieve the index of the parent instance, if any.
		@param out variable to hold parent index, if any.
		@return true if The instance has a parent; false otherwise.
	*/
	inline bool GetParentInstance(size_t & out) const { if (m_flags & SUB_FLAG) { out = m_parentInstance; return true; } else return false; }
	/** Determine if Spawner spawned the instance.
		@param pSpwn Pointer to Spawner to query against.
		@return true if Spawner spawned the instance; false otherwise.
	*/
	inline bool IsFromSpawner(Spawner* pSpwn) const { return m_pParentSpawner && pSpwn == m_pParentSpawner; }

private:

	//ensure that booleans use the minimum space
	/** Enumeration of bitflags for functionality*/
	enum {
		CLEAR_FLAGS=0x0,		///< Clear flag value.

		ACTIVE_FLAG=0x01,		///< Active flag; ModelInstance is considered active if this is set and DEAD_FLAG is clear.
		DEAD_FLAG=0x02,			///< If set, implies that the instance is dead and is available for garbage collection.
		INIT_FLAG=0x04,			///< If set, ModelInstance has space allocated for initialization values.
		ARRAYS_FLAG=0x08,		///< If set, ModelInstance contains array values; _may not be needed_.
		VALS_FLAG=0x10,			///< If set, ModelInstance contains single values; _may not be needed_.
		SUB_FLAG=0x20,			///< If set, ModelInstance was spawned by another instance.

		TOACTIVE_FLAG=0x40,		///< If set, ModelInstance will be set to active in the next status check.
		TOKILL_FLAG=0x80		///< If set, ModelInstance will be killed in the next status check.
	};

	char m_flags;				///< Byte used to store conditional bit flags.

	OME_SCALAR m_lastEvalTime;	///< Double which stores the last time which this instance was evaluated.

	Model* m_pParentModel; ///< Pointer to parent model that originated the instance.
	
	Spawner* m_pParentSpawner;			///< Pointer to Spawner object that lead to the creation of this instance, if any.
	size_t m_parentInstance;			///< Index of parent ModelInstance, if any.

	size_t m_index;						///< Index in parent model

	/** Determine if a specific flag is set or cleared.
		@param flag The flag to query.
		@return true if the flag is set; false if the flag is clear.
	*/
	inline bool GetFlag(const char flag) const { return m_flags & flag; }
	/** Set a flag to a specific value.
		@param val The new flag value.
		@param flag The flag to modify.
	*/
	inline void SetFlag(const bool & val, const char flag) { val ? m_flags|=flag : m_flags&=~flag; }
};