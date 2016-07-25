#include "DebugOutput.h"
#include "events/CentralEventDispatcher.h"
#include "events/Eventable.h"
#include "OMEDefines.h"

#include <iostream>
#include <sstream>

#define TIME_SIGMA 1.0E-13
namespace CEDSingleton
{
	CentralEventDispatcher singleton;
};

CentralEventDispatcher& CentralEventDispatcher::GetSingletion() 
{
	return CEDSingleton::singleton;
}

/** Default constructor. */
CentralEventDispatcher::CentralEventDispatcher(void)
	:m_isRunning(false),m_lastCode(CEDR_NA),m_pErrs(NULL)
{
	//permanent events that the Dispatcher always listens for
	RegisterEventListener(CED_END,this,NULL,true);
	RegisterEventListener(CED_INCREMENT,this,NULL,true);
	RegisterEventListener(CED_PAUSE,this,NULL,true);
}

/** Destructor. */
CentralEventDispatcher::~CentralEventDispatcher(void)
{
}

/** Register an Eventable object to call any time a broadcast event of 
	a specific type is triggered.
	@param eventType The type of event to listen for.
	@param obj The eventable object to register for listening.
	@param extra Optional pointer to pass along when event handler is called.
	@param permanent If true, the listener entry will not be removed when clear listeners is called. It can still be removed by
			calling RemoveListener().
*/
void CentralEventDispatcher::RegisterEventListener(const std::string eventType, Eventable* obj,void* extra,bool permanent)
{
	EventListener ev;
	ev.listener=obj;
	ev.extra=extra;
	ev.noClear=permanent;

	m_listeners[eventType].push_back(ev);
	//ensure no duplicates
	m_listeners[eventType].unique();
}

/** Stop a specific Eventable object from listening for a specific event type.
	@param eventType The type of event to stop listening for.
	@param obj The Eventable object that will stop listening for the specified event type.
*/
void CentralEventDispatcher::RemoveListener(const std::string eventType, Eventable* obj)
{
	for(auto itr=m_listeners[eventType].begin(); itr !=m_listeners[eventType].end(); ++itr)
	{
		if((*itr).listener==obj)
		{
			m_listeners[eventType].remove(*itr);
			return;
		}
	}
}

/** Adds an event to the end of the Event Priority Queue.
	@param ev The event to enqueue.
*/
void CentralEventDispatcher::PostEvent(const CentralEvent & ev)
{
	std::lock_guard<std::recursive_mutex> lck(m_queueLock);
	m_events.push(ev);
}

/** Process the next event at the top of the event queue.
	This function can also be used to immediately dispatch an event, bypassing the queue.
	@param ev The CentralEvent to immediately dispatch.
*/
void CentralEventDispatcher::DispatchEvent(CentralEvent & ev)
{
//	DBG_PRINT("Processing Event " + ev.GetEventType() + " for time: " + std::to_string(ev.GetEventTime()));

	Eventable* receiver=ev.GetReceiver();

	//if event has a specified receiver, send it only to that object. Otherwise, broadcast event to all
	if(receiver) 
	{
		//targeted event
		receiver->EventAction(ev,receiver->GetEventExtra());
	}
	else
	{
		//broadcast event
		BroadcastEvent(ev);
	}
	if(ev.Repeats())
	{
		//if there is a repeat flag, increment the time and repost
		ev.IncEventTime();
		PostEvent(ev);
	}
	
}

/** Broadcast an event to any Eventable objects that are listening. 
	@param ev The Eventable object to broadcast.
*/
void CentralEventDispatcher::BroadcastEvent(const CentralEvent & ev)
{
	//TODO: make thread safe

	std::string evType=ev.GetEventType();
	
	EventListener curr;
	//#pragma omp parallel for
	for(auto itr=m_listeners[evType].begin(); itr!=m_listeners[evType].end(); ++itr)
	{
		curr=*itr;
		curr.listener->EventAction(ev,curr.listener->RefreshExtraOnCall() ? curr.listener->GetEventExtra() : curr.extra);
	}
}

/** Force immediate processing of any remaining events in the queue.
*/
void CentralEventDispatcher::FlushEvents()
{
	std::lock_guard<std::recursive_mutex> lck(m_queueLock);
	CentralEvent* batchBuffer = new CentralEvent[m_events.size()];
	int lastIndex;
	while(!m_events.empty())
	{
		lastIndex=-1;
		do
		{
			lastIndex++;
			batchBuffer[lastIndex]=m_events.top();
			m_events.pop();

		} while(!m_events.empty() && batchBuffer[lastIndex].SamePriority(m_events.top()));

		for(int i=0; i<=lastIndex; i++)
			DispatchEvent(batchBuffer[i]);

	}
	delete[] batchBuffer;
	
}


/** Remove all events from the queue.
*/
void CentralEventDispatcher::ClearEvents()
{
	std::lock_guard<std::recursive_mutex> lck(m_queueLock);
	while (!m_events.empty())
		m_events.pop();
}

/** Remove all registered listeners. */
void CentralEventDispatcher::ClearListeners()
{
	StrList toRemove;
	for (auto mItr = m_listeners.begin(); mItr != m_listeners.end(); ++mItr)
	{
		//only remove items that are allowed to be cleared
		std::remove_if((*mItr).second.begin(),(*mItr).second.end(),[](EventListener & el){return !el.noClear; });
		
		//if nothing left, mark map entry for erasure
		if((*mItr).second.empty())
			toRemove.push_back((*mItr).first);
	}

	//erase marked entries
	std::for_each(toRemove.begin(),toRemove.end(),[&](STLString & s){m_listeners.erase(s);});
}

/** Run the dispatcher for a given period.
	@param timeStart The starting time step. 
	@param timeEnd The final time step for the simulation.
	@param interval The increment between time steps.
   @return Code indicating why the loop terminated.
*/
CentralEventDispatcher::RUN_CODE CentralEventDispatcher::RunDispatcher(OME_SCALAR timeStart, OME_SCALAR timeEnd, OME_SCALAR interval)
{
	const unsigned int MAX_BUFF_EVENT=1028;
	static CentralEvent batchBuffer[MAX_BUFF_EVENT];

   //enable if we want to erase the history of the run code at the beginning of each run.
   //m_lastCode=CEDR_NA;

	//add initialization events, and increment events
	if(m_lastCode!=CEDR_PAUSE)
	{
		m_timeTicks = 0;
		m_currentTime=timeStart;
		m_timeStart = timeStart;
		m_timeEnd=timeEnd;
		m_timeInterval=interval;
		m_khErr = 0.0;
		//broadcast to registered members that it is time to initialize
	
		PostEvent(CentralEvent(this,NULL,CED_INIT,0.0,0.0,CE_TOP_PRIORITY,NULL));
	}
	//DumpQueueToConsole();
	m_isRunning=true;

	CentralEvent currEvent;
	int lastIndex;
	while(m_isRunning && !m_events.empty())
	{
		if(m_pErrs && m_pErrs->GetCount())
		{
			BroadcastAnyErrors();
			continue;
		}

		//grab all events with the same time and priority.
		//loop through them, configured for OMP.
		

		/// \optimization collapse the following two loops into a single loop if threading is not being used.
		lastIndex = -1;
		{
			std::lock_guard<std::recursive_mutex> lck(m_queueLock);
			do
			{

				lastIndex++;
				batchBuffer[lastIndex] = m_events.top();
				m_events.pop();

			} while (!m_events.empty() && batchBuffer[lastIndex].SamePriority(m_events.top()) && lastIndex < MAX_BUFF_EVENT);
			
		}

		for(int i=0; i<=lastIndex; i++)
		{
			//DBG_PRINTC("Dispatching "+batchBuffer[i].GetEventType()+" "+std::to_string(batchBuffer[i].GetEventTime()),DBG_MAGENTA);
			DispatchEvent(batchBuffer[i]);
		}

	
		{
			std::lock_guard<std::recursive_mutex> lck(m_queueLock);
			if (m_events.size())
			{
				CentralEvent nextEvent = m_events.top();
				//if we have rolled over to our next time, then immediately fire an Increment broadcast event
				if (nextEvent.GetEventType() != CED_END && nextEvent.GetEventTime() > m_currentTime)
					PostEvent(CentralEvent(this, NULL, CED_INCREMENT, m_currentTime, 0.0, CE_TOP_PRIORITY, NULL));
			}
			else
				PostEvent(CentralEvent(this, NULL, CED_INCREMENT, m_currentTime, 0.0, CE_TOP_PRIORITY, NULL));
		}

	}
	return m_lastCode;
}

void CentralEventDispatcher::EventAction(const CentralEvent & ev, void* extra)
{
	std::string eType=ev.GetEventType();
	ARRAY_TYPE<std::string> *errs;
	if(eType==CED_INCREMENT)
	{
		//straight addition is producing too many rounding issues.
		//implement Kahan error correction, which will work for variable timesteps
		///@question do we need to shield Kahan algorithm from compiler optimizations?
		//for now, use volatiles to prevent temps from being optimized out
		/*volatile OME_SCALAR corrected = m_timeInterval - m_khErr;
		volatile OME_SCALAR tempVal = m_currentTime + corrected;
		m_khErr = (tempVal - m_currentTime) - corrected;
		m_currentTime = tempVal;*/
		//end Kahan

		//m_currentTime += m_timeInterval;
		++m_timeTicks;
		m_currentTime = m_timeStart + (m_timeTicks*m_timeInterval);
		
		CentralEvent outEv;
		if(m_currentTime>(m_timeEnd+TIME_SIGMA))
		{
			outEv=CentralEvent(this,NULL, CED_END, m_currentTime+m_timeInterval, 0.0,CE_TOP_PRIORITY,NULL);
			PostEvent(outEv);
		}
		/*else
			outEv=CentralEvent(this,NULL, CED_INCREMENT, m_currentTime, 0.0,CE_TOP_PRIORITY,NULL);*/

		
#ifdef _DEBUG
		DBG_PRINTSMALLDIV();
		oSTLSStream dbgOut;
		dbgOut<<"Increment from "<<m_currentTime-m_timeInterval<<" to "<<m_currentTime;
		DBG_PRINT(dbgOut.str());
#endif
	}
	else if(eType==CED_END)
	{
		errs=(ARRAY_TYPE<std::string>*)ev.GetPayload();
	  m_lastCode = CEDR_COMPLETE;
		if(errs)
		{
			//display error
			CentralEvent errEv(this,NULL, CED_ERR, m_currentTime, 0.0,CE_TOP_PRIORITY,errs);
			BroadcastEvent(errEv);
		 m_lastCode= CEDR_ERROR;
		}
		m_isRunning=false;
		ClearEvents();
	}
   else if(eType==CED_PAUSE)
   {
	   //if asking to pause after run window, then we should just end the simulation instead.
	   if(ev.GetEventTime()<=m_timeEnd)
	   {
			//kill loop, but preserve everything.
			  m_lastCode=CEDR_PAUSE;
			  m_isRunning=false;
	   }
	   else
	   {
		   //recast as endEvent and repost.
		   CentralEvent endEvent(ev);
		   endEvent.SetEventType(CED_END);
		   PostEvent(endEvent);
	   }
   }
	else if(eType==CED_DUMP)
		DumpQueueToConsole();
}

/** Debug function; dumps all events to console/cout stream.
	
*/
void CentralEventDispatcher::DumpQueueToConsole()
{
	using namespace std;
	const OMEChar* buffer=DumpQueue();
#ifndef _DEBUG
	cout<<buffer<<endl;
#else
	DBG_PRINT(buffer);
#endif
	delete[] buffer;
}

/** Create C-String representation of the contents of the event queue
	__NOTE:__ Uses new to allocate returned pointer. Use deletep[] for disposal
	to avoid memory leak.
	@return C-string containing a description of all items found in the event queue.
*/
const char* CentralEventDispatcher::DumpQueue()
{
	using namespace std;

	std::lock_guard<std::recursive_mutex> lck(m_queueLock);
	EventQueue dmpQueue(m_events);

	CentralEvent currEvent;
	
	ostringstream outStr("Current Queue for time ");
	outStr<<m_currentTime<<":"<<endl<<endl;

	string dmpTemp;
	while(!dmpQueue.empty())
	{
		currEvent=dmpQueue.top();
		currEvent.ToString(dmpTemp);
		outStr<<dmpTemp<<endl;
		dmpQueue.pop();
	}

	//extract c-string, add to new buffer, and return
	//I don't know if string.length() includes null char, so add one to be sure
	string out=outStr.str();
	unsigned int length=out.length();
	char* buffer= new char[length+1];
	buffer[length]='\0';
	out.copy(buffer,length);
	return buffer;
}

/** If any errors have been registered to the CentralEventDisptacher, broadcast them to any listeners. */
void CentralEventDispatcher::BroadcastAnyErrors()
{
	if (m_pErrs && m_pErrs->GetCount())
	{
		CentralEvent errEvent = CentralEvent(this, NULL, CED_ERR, 0.0, 0, CE_TOP_PRIORITY, m_pErrs);
		DispatchEvent(errEvent);
		m_pErrs->ClearErrs();
	}
}