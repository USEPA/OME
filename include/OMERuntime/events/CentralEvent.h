#pragma once
//#include "stdafx.h"
#include "Summarizable.h"
#include "CharType.h"
#include "ValTypes.h"
#include <limits>

/** Represents the highest priority used by central events */
const unsigned int CE_TOP_PRIORITY = std::numeric_limits<int>::max();

class Eventable;
/**
	A class used to represent an event in a Centralized Event Driven environment.
*/
class __EXPORT__ CentralEvent : public Summarizable
{
public:

	/** Default Constructor. */
	CentralEvent(void) : m_sender(NULL), m_receiver(NULL), m_eventType(""),m_time(0.0),
						m_priority(0), m_payload(NULL), m_repeat(0.0) 
	{}

	/** Detailed Constructor. 
		@param s Pointer to source object; any pointer can act as source.
		@param r Pointer to receiving object, which much inherit from Eventable. r can be NULL, 
				in which case the object is expected to be treated as a broadcast event.
		@param et Arbitrary string used as the event type identifier, which in turn is used to either
				identify broadcast targets or to inform the receiver of what action is expected to be taken.
		@param t The time step at which the event should be triggered.
		@param ri Repeat Interval for event. If the event has a repeat interval greater than zero, it will be
				re-entered into the event queue after triggering with the trigger time incremented by the interval.
		@param p Priority of the event; 0 is the lowest priority, and CE_TOP_PRIORITY is the highest.
		@param pl Optional pointer to arbitrary data to be passed along with the event (the payload).
	*/
	CentralEvent(void* s, Eventable* r, STLString et, const OME_SCALAR & t, const OME_SCALAR & ri, const unsigned int & p, void* pl)
		:m_sender(s), m_receiver(r), m_eventType(et), m_time(t),
		 m_priority(p), m_payload(pl), m_repeat(ri)
	{}

	/** Copy Constructor
		@param e The Central Event to copy.
	*/
	CentralEvent(const CentralEvent & e) { Duplicate(e); }

	/** Destructor */
	~CentralEvent(void) {};

	/** Assignment Operator.
		@param rhs The right-hand side argument for the operator.
	*/
	CentralEvent& operator=(const CentralEvent & rhs) { Duplicate(rhs); return *this; }

	/** Operator used for Priority sorting, with time being the primary sorting value, and priority being secondary.
		earlier time is greater than later time, and higher priority is greater than lower priority.
	*/ 
	bool operator< (const CentralEvent & rhs) 
	{ 
		if(m_time!=rhs.m_time)
			return m_time>rhs.m_time;
		else
			return m_priority < rhs.m_priority; 
	}

	/** Sets the sender Pointer.
		@param s Pointer to the object to act as sender.
	*/
	inline void SetSender(void* s) { m_sender=s; }
	/** @return Pointer to current sender object. */
	inline void* GetSender() const { return m_sender; }

	/** Sets the receiver Pointer.
		@param r Pointer to object that inherits Eventable.
	*/
	inline void SetReceiver(Eventable* r) { m_receiver=r; }
	/** @return Pointer to current receiver object. */
	inline Eventable* GetReceiver() const { return m_receiver; }

	/** Sets the Event Type Identifier string.
		@param et string representing the event type.
	*/
	inline void SetEventType(STLString et) { m_eventType=STLString(et.c_str()); }
	/** Sets the Event Type Identifier string.
	@param et string representing the event type.
	*/
	inline void SetEventType(const OMEChar* et) { m_eventType = STLString(et); }
	/** @return Copy of string used to identify the event type. */
	inline STLString GetEventType() const { return m_eventType; }

	/** Sets the time step during which the event should be triggered.
		@param time the target time step.
	*/
	inline void SetEventTime(OME_SCALAR time) { m_time = time; }
	/** @return Time step during which the event will trigger. */
	inline OME_SCALAR GetEventTime() const { return m_time; }

	/** Sets the relative priority of the event.
		@param p value representing the event priority; the greater the value, the higher the priority.
	*/
	inline void SetPriority(unsigned int p) { m_priority=p; }
	/** @return Relative priority of event. */
	inline unsigned int GetPriority() const { return m_priority; }

	/** Sets the pointer of the payload that will be passed along with the event.
		@param pl Pointer to arbitrary data to be associated with the event.
	*/
	inline void SetPayload(void *pl) { m_payload=pl; }
	/** @return Generic pointer to payload data. */
	inline void *GetPayload() const { return m_payload; }

	/** Sets the repeat interval of the event; a repeat interval of 0 means that the event will
		only be triggered once.
		@param ri the repeat interval to apply to the event.
	*/
	inline void SetRepeatInterval(OME_SCALAR ri) { m_repeat = ri; }
	/** @return The current repeat interval. */
	inline OME_SCALAR GetRepeatInterval() const { return m_repeat; }

	/** @return true if valid repeat interval is set, false otherwise */
	inline bool Repeats() const { return m_repeat!=0.0; }
	/** Increments the target time step by the repeat interval */
	inline void IncEventTime() { m_time+=m_repeat; }

	/** Check to see if another event has the same priority.
		@param ce The CentralEvent to compare against.
		@return true if time and evaluation priorities are the same, false otherwise.
	*/
	inline bool SamePriority(const CentralEvent & ce) { return (m_priority==ce.m_priority) && (m_time==ce.m_time);}

	virtual const OMEChar* ToString(const OMEChar tabChar=' ', const unsigned int indent=0, const unsigned int inc=4) const;
	virtual void ToString(OMEChar* & out, unsigned int len, const OMEChar tabChar=' ', const unsigned int indent=0, const unsigned int inc=4) const
		{ Summarizable::ToString(out,len,tabChar,indent,inc); };
	virtual void ToString(std::string & out, const OMEChar tabChar = ' ', const unsigned int indent = 0, const unsigned int inc = 4) const
		{ Summarizable::ToString(out,tabChar,indent,inc); };

protected:
	void* m_sender;				///< Pointer to the object representing the event's sender
	Eventable* m_receiver;		///< Pointer to the Eventable object representing the event's receiver. Can be NULL.
	OME_SCALAR m_time;				///< Time step at which the event should fire.
	unsigned int m_priority;	///< Relative priority of the event. Higher values are higher priorities.
	void* m_payload;			///< Pointer to arbitrary data associated with the event.
	OME_SCALAR m_repeat;				///< Repeat Interval applied to event. Can be 0.

	void Duplicate(const CentralEvent & e);

	private:
		//private to defend against dll issues
		std::string m_eventType;		///< String used to identify the type of event.

};

static bool operator < (const CentralEvent & lhs, const CentralEvent & rhs) 
{
	OME_SCALAR lhsTime = lhs.GetEventTime();
	OME_SCALAR rhsTime = rhs.GetEventTime();

	if(lhsTime!=rhsTime)
		return lhsTime>rhsTime;
	else
		return lhs.GetPriority() < rhs.GetPriority();
}
