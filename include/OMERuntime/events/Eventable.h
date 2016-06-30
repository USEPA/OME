#pragma once
#include "CentralEvent.h"

/** Virtual base class that implements an interface for receiving and responding to events.
*/
class Eventable
{
public:

	virtual ~Eventable() {};

	/** Take action based on Event that is passed in.
		@param ev The event that triggered the call of this function.
		@param extra Optional arbitrary data to be passed in. This data typically
					comes from somewhere other than the event.
	*/
	virtual void EventAction(const CentralEvent & ev, void* extra)=0;

	/** @return A pointer to extra data to be paired with an event.*/
	virtual void* GetEventExtra() { return NULL; }

	/** Function intended to be overridden for specialized behavior. 
		If the function returns true, then GetEventExtra() will be called 
		each time a related event is evaluated. Otherwise, GetEventExtra() is only 
		called when the event is defined, and the results are cached with the event itself.
		
		@return true if extra value should be refreshed every time a related event is evaluated; false
			if the extra object never changes for the duration of an event's existance.
		*/
	virtual bool RefreshExtraOnCall() const { return false; }

	//more here as needed
};