#include "events/CentralEvent.h"
#include <sstream>

/** Copies another Central Event 
	@param e The event to copy.
*/
void CentralEvent::Duplicate(const CentralEvent & e)
{
	m_sender=e.m_sender;
	m_receiver=e.m_receiver;
	m_eventType=e.GetEventType();
	m_time=e.m_time;
	m_priority=e.m_priority;
	m_payload=e.m_payload;
	m_repeat=e.m_repeat;
}

const OMEChar* CentralEvent::ToString(const OMEChar tabChar, const unsigned int indent, const unsigned int inc) const
{
	using namespace std;

	string head(indent,tabChar);
	ostringstream out;

	//send entries through stream to buffer
	out<<head<<"Event Type: "<<string(m_eventType)<<endl;
	out<<head<<"Scheduled Time: "<<m_time<<endl;
	out<<head<<"Priority: "<<m_priority<<endl;
	if(m_repeat)
		out<<head<<"Repeat: "<<m_repeat<<endl;

	if(m_sender)
		out<<head<<"Sender: "<<m_sender<<endl;

	if(m_receiver)
	{
		out<<head<<"Receiver: ";
		out<<m_receiver<<endl;
	}

	if(m_payload)
		out<<head<<"Payload: "<<m_payload<<endl;

	//copy string into c-string buffer that can be returned
	int outSize=out.str().size();
	char* buff= new char[outSize+1];
	buff[outSize]='\0';
	out.str().copy(buff,outSize);

	return buff;
}