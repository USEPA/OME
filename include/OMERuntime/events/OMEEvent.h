#pragma once
#include "CentralEvent.h"

class OMEObject;

/** OMEEvent-related presets.*/
enum OME_EVENT_BEHAVIOR {OMEEB_DEFAULT, OMEEB_VALIDATION, OMEEB_INIT, OMEEB_BROADCAST,OMEEB_URGENT,OMEEB_LOW};

/** Convenience class for producing CentralEvent-compatible events from OMEObjects.*/
class OMEEvent : public CentralEvent
{
public:
	/**Default constructor.*/
	OMEEvent() : CentralEvent() {};

	OMEEvent(OMEObject* obj, std::string et, OME_SCALAR t, void* pl=NULL, OME_EVENT_BEHAVIOR oeb=OMEEB_DEFAULT, OME_SCALAR rep=0.0);

	/**Copy constructor. */
	OMEEvent(const OMEEvent & ev) {Duplicate(ev);};

	virtual const OMEChar* ToString(const OMEChar tabChar=' ', const unsigned int indent=0, const unsigned int inc=4) const;

	~OMEEvent(void);
};

