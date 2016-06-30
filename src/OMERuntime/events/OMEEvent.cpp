#include "events/OMEEvent.h"
#include "omeobjects/OMEObject.h"
#include <sstream>

/**Detailed constructor.

	The specifics of the constructed event largely depend on the oeb flag. Here are 
	the currently supported values:
	-# __OMEEB_DEFAULT:__ Priority is taken from obj; obj is both sender and receiver. This is the default.
	-# __OMEEB_INIT:__ Priority is set to 1.
	-# __OMEEB_VALIDATION:__ Priority is set to 2.
	-# __OMEEB_BROADCAST:__ Receiver is set to NULL, causing it to be broadcasted when triggered.
	-# __OMEEB_URGENT:__ Priority set to the max priority (CE_TOP_PRIORITY).
	-# __OMEEB_LOW:__ Priority set to 0.

	@param obj The OMEObject as basis for event.
	@param et The Event type.
	@param t The time step during which the event should be triggered.
	@param pl Optional pointer to data to associate with event.
	@param oeb Optional flag used to determine different traits, detailed in function summary.
	@param rep Optional flag for how often the event should repeat, if at all.
*/
OMEEvent::OMEEvent(OMEObject* obj, std::string et, OME_SCALAR t, void* pl, OME_EVENT_BEHAVIOR oeb, OME_SCALAR rep)
{
	unsigned int prior;
	switch(oeb)
	{
	case OMEEB_URGENT:
		prior= CE_TOP_PRIORITY;
		break;
	case OMEEB_LOW:
		prior=0;
		break;
	case OMEEB_VALIDATION:
		prior=2;
		break;
	case OMEEB_INIT:
		prior=1;
		break;
	default:
		prior=obj->GetEvalPriority();
	};

	m_sender=obj;
	m_receiver=(oeb!=OMEEB_BROADCAST) ? obj : NULL;
	SetEventType(et);
	m_time=t;
	m_repeat=rep;
	m_priority=prior;
	m_payload=pl;
}


const OMEChar* OMEEvent::ToString(const char tabChar, const unsigned int indent, const unsigned int inc) const
{
	using namespace std;

	std::string head(indent,tabChar);
	ostringstream out;

	out<<head<<"Event Type: "<<GetEventType()<<endl;
	out<<head<<"Scheduled Time: "<<m_time<<endl;
	out<<head<<"Priority: "<<m_priority<<endl;

	if(m_sender)
	{
		out<<head<<"Sender:\n"<<((OMEObject*)m_sender)->ToString(tabChar,indent+inc,inc)<<endl;
	}
	if(m_receiver)
		out<<head<<"Receiver:\n"<<((OMEObject*)m_receiver)->ToString(tabChar,indent+inc,inc)<<endl;

	if(m_payload)
		out<<head<<"Payload: "<<m_payload<<endl;

	string buff=out.str();
	return buff.c_str();
}


OMEEvent::~OMEEvent(void)
{
}
