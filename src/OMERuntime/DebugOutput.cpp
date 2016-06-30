#if (defined WIN32 || defined _WIN64)
#include "..\..\..\include\Windows\stdafx.h"
#endif
#include "DebugOutput.h"
#include "Summarizable.h"
#include <iostream>
#include <sstream>

#include <time.h>

namespace static_output
{
	DebugOutput dbOut=DebugOutput();
};

/** Default Constructor.*/
DebugOutput::DebugOutput() 
	: m_indentChar(' '),m_level(0),m_indentWidth(4),m_printFunc(NULL),m_logFileFunc(NULL),m_secStart(std::clock()),
	m_disablePrint(false)
{
#ifdef DBG_COLOR_SUPPORT
	#if (defined WIN32 || defined _WIN64)
	HANDLE console=GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(console,&csbi);
	m_txColor=m_defaultTxColor=OMEDBG_COLOR(csbi.wAttributes & 15);
	m_bgColor=m_defaultBgColor=OMEDBG_COLOR((csbi.wAttributes &0xf0)>>4);
#else
	m_txColor=m_defaultTxColor=DBG_DEFAULT;
	m_bgColor=m_defaultBgColor=DBG_DEFAULT;
#endif
#endif
}

/** Copy Constructor 
	@param dop The DebugOutput to copy.
*/
DebugOutput::DebugOutput(const DebugOutput & dop)
{
    m_indentChar=dop.m_indentChar;
	m_level=dop.m_level;
	m_indentWidth=dop.m_indentWidth;
	
	m_secStart=dop.m_secStart;
    
	m_printFunc=dop.m_printFunc;
    m_logFileFunc=dop.m_logFileFunc;
	m_disablePrint = dop.m_disablePrint;
    if(m_logStream.good())
        m_logStream<<dop.m_logStream.rdbuf();
    
#ifdef DBG_COLOR_SUPPORT
	m_txColor=dop.m_txColor;
	m_bgColor=dop.m_bgColor;
#endif

}

/** Destructor. */
DebugOutput::~DebugOutput()
{
	if(m_logStream.is_open())
		m_logStream.close();
}

/** Decrease indentation level.
	For each indentation level, width number of indent chars are prepended to the dbg msg.
	This is useful for readable formatting.
	@param size The number of indentation levels to decrease by.
*/
void DebugOutput::DecreaseLevel(const unsigned int & size)
{
	m_level=(size<=m_level) ? m_level-size : 0;
}

/** Increase indentation level. For each indentation level, width number of indent chars are prepended to the dbg msg. 
This is useful for readable formatting.
	@param size The number of indentation levels to increase by.
*/
void DebugOutput::IncreaseLevel(const unsigned int & size)
{
	m_level+=size;
}

/** Sets the number of characters to use for indentation per level.
	@param width The number of indent chars to use per indentation level.
*/
void DebugOutput::SetIndentWidth(const unsigned int & width)
{
	m_indentWidth=width;
}

/** Set the character used for indentation prefix.
	@param chr The character to use for indentation.
*/
void DebugOutput::SetIndenOMEChar(const char & chr)
{
	m_indentChar=chr;
}

/** Set the path to where the log file should reside. 
	@param filepath Path to the location of the logfile. 
*/
void DebugOutput::SetLogFile(const std::string filepath)
{
	if(m_logStream.is_open())
		m_logStream.close();

	if(filepath.length())
	{
		m_logStream.open(filepath, std::ios_base::app);
		if(!m_logStream.good())
		{
			m_logStream.close();
			PrintDebug("Log file failed to open");
		}
	}
}

/** Print the current date and time to debug streams.*/
void DebugOutput::PrintDateTime()
{
	time_t rawTime;
	time(&rawTime);
#if (defined WIN32 || defined _WIN64)
    const int bSize=256;
	
    char buffer[bSize];
	struct tm buf;
	localtime_s(&buf, &rawTime);
	asctime_s(buffer, bSize, &buf);
#else
    char* buffer;
    buffer=asctime(localtime(&rawTime));
#endif
	PrintDebug(buffer); 

}

/** Print the time (in seconds) since the program was launched.
	@param label String to prepend to elapsed seconds.
*/
void DebugOutput::PrintElapsed(const std::string label)
{
	if (!m_disablePrint)
	{
		std::ostringstream outStrm;
		unsigned long secs=GetSecSinceInstance();
		outStrm<<label<<": "<<secs;
		PrintDebug(outStrm.str());
	}
}

/** Print debug message. Prepends indentation characters, using multiplier (width * level).
	@param msg The message to print to the debug out.
	@param noEnd If true, do not append an endline to the end of the stream.
	@param type Flag indicating severity of error.
*/
void DebugOutput::PrintDebug(const std::string & msg,const bool noEnd,const OMEDBG_TYPE type)
{
	using namespace std;
	if (!m_disablePrint)
	{

		//this will only work with ASCII strings, but 
		//outstream will handle it
		string indent(m_level*m_indentWidth, ' ');

		string label;
		//add default labels
		if (!m_printFunc || !m_logFileFunc)
		{
			switch (type)
			{
			case DBG_ERR:
				label = "ERR: ";
				break;
				/*case DBG_INFO:
					label="INFO: ";
					break;*/
			case DBG_WARN:
				label = "WARN: ";
				break;
			case DBG_CRIT:
				label = "!!!: ";
				break;
			default:
				break;
			};
		}
		if (!m_printFunc)
		{
			cout << indent << label << msg;

			if (!noEnd)
				cout << endl;
		}
		else
			m_printFunc((indent + msg).c_str(), noEnd, type);

		if (!m_logFileFunc)
		{
			if (m_logStream.is_open())
			{
				m_logStream << indent << label << msg;
				if (!noEnd)
					m_logStream << endl;
				m_logStream.flush();
			}
		}
		else
			m_logFileFunc((indent + msg).c_str(), noEnd, type);
	}
}

/** Print debug message. Prepends indentation characters, using multiplier (width * level).
	@param strm The ostream to print to the debug out.
	@param noEnd If true, do not append an endline to the end of the stream.
	@param type Flag indicating severity of error.
*/
void DebugOutput::PrintDebug(const std::ostream & strm,const bool noEnd,const OMEDBG_TYPE type)
{
	if (!m_disablePrint)
	{

		//this will only work with ASCII strings, but 
		//outstream will handle it
		std::string indent(m_level*m_indentWidth, ' ');

		std::cout << indent << strm.rdbuf();
		if (!noEnd)
			std::cout << std::endl;

		if (m_logStream.is_open())
		{
			m_logStream << indent << strm.rdbuf();
			if (!noEnd)
				m_logStream << std::endl;
			m_logStream.flush();
		}
	}
}

/** Print description of object. Prepends indentation characters, using multiplier (width * level).
	@param pObj to Summarizable object to dump.
	@param noEnd If true, do not append an endline to the end of the stream.
	@param type Flag indicating severity of error.
*/
void DebugOutput::PrintDebug(Summarizable* pObj,const bool noEnd,const OMEDBG_TYPE type)
{
	PrintDebug(*pObj,noEnd,type);
}

/** Print description of object. Prepends indentation characters, using multiplier (width * level).
	@param obj object to dump.
	@param noEnd If true, do not append an endline to the end of the stream.
	@param type Flag indicating severity of error.
*/
void DebugOutput::PrintDebug(Summarizable & obj,const bool noEnd,const OMEDBG_TYPE type)
{
	PrintDebug(obj.ToString(),noEnd,type);
}

#ifdef DBG_COLOR_SUPPORT
#if (defined WIN32 || defined _WIN64)
void DebugOutput::SetTextColor(const OMEDBG_COLOR color)
{
	HANDLE console=GetStdHandle(STD_OUTPUT_HANDLE);
	m_txColor= color==DBG_DEFAULT ? m_defaultTxColor : color;
	unsigned short wAttrs=((unsigned int)m_bgColor<<4) | (unsigned int)m_txColor;
	SetConsoleTextAttribute(console,wAttrs);
}

void DebugOutput::SetBGColor(const OMEDBG_COLOR color)
{
	HANDLE console=GetStdHandle(STD_OUTPUT_HANDLE);
	m_bgColor= color==DBG_DEFAULT ? m_defaultBgColor : color;
	unsigned short wAttrs=((unsigned int)m_bgColor<<4) | (unsigned int)m_txColor;
	SetConsoleTextAttribute(console,wAttrs);
}

OMEDBG_COLOR DebugOutput::GetTxColor()
{
	HANDLE console=GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(console,&csbi);
	return OMEDBG_COLOR(csbi.wAttributes & 15);
}

OMEDBG_COLOR DebugOutput::GetBgColor()
{
	HANDLE console=GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(console,&csbi);
	return OMEDBG_COLOR((csbi.wAttributes & 0xf0)>>4);
}
#else
void DebugOutput::SetTextColor(const OMEDBG_COLOR color)
{
    using namespace std;
	switch(color)
	{
	case DBG_DEFAULT:
		PrintDebug("\u001B[0m",true);
		if(m_bgColor!=DBG_DEFAULT)
			SetBGColor(m_bgColor);
		break;
	case DBG_BLACK:
		PrintDebug("\u001B[30m",true);
		break;
	case DBG_DK_BLUE:
		PrintDebug("\u001B[34m",true);
		break;
	case DBG_DK_GREEN:
		PrintDebug("\u001B[32m",true);
		break;
	case DBG_DK_CYAN:
		PrintDebug("\u001B[36m",true);
		break;
	case DBG_DK_RED:
		PrintDebug("\u001B[31m",true);
		break;
	case DBG_DK_MAGENTA:
		PrintDebug("\u001B[35m",true);
		break;
	case DBG_DK_YELLOW:
		PrintDebug("\u001B[33m",true);
		break;
	case DBG_DK_WHITE:
		PrintDebug("\u001B[37m",true);
		break;
	case DBG_GRAY:
		PrintDebug("\u001B[30m",true);
		break;
	case DBG_BLUE:
		PrintDebug("\u001B[34m",true);
		break;
	case DBG_GREEN:
		PrintDebug("\u001B[32m",true);
		break;
	case DBG_CYAN:
		PrintDebug("\u001B[36m",true);
		break;
	case DBG_RED:
		PrintDebug("\u001B[31m",true);
		break;
	case DBG_MAGENTA:
		PrintDebug("\u001B[35m",true);
		break;
	case DBG_YELLOW:
		PrintDebug("\u001B[33m",true);
		break;
	case DBG_WHITE:
		PrintDebug("\u001B[37m",true);
		break;
	}
	
	m_txColor=color;
}

void DebugOutput::SetBGColor(const OMEDBG_COLOR color)
{
    using namespace std;
	switch(color)
	{
	case DBG_DEFAULT:
		PrintDebug("\u001B[0m",true);
		if(m_txColor!=DBG_DEFAULT)
			SetTextColor(m_txColor);
		break;
	case DBG_BLACK:
		PrintDebug("\u001B[40m",true);
		break;
	case DBG_DK_BLUE:
		PrintDebug("\u001B[44m",true);
		break;
	case DBG_DK_GREEN:
		PrintDebug("\u001B[42m",true);
		break;
	case DBG_DK_CYAN:
		PrintDebug("\u001B[46m",true);
		break;
	case DBG_DK_RED:
		PrintDebug("\u001B[41m",true);
		break;
	case DBG_DK_MAGENTA:
		PrintDebug("\u001B[45m",true);
		break;
	case DBG_DK_YELLOW:
		PrintDebug("\u001B[43m",true);
		break;
	case DBG_DK_WHITE:
		PrintDebug("\u001B[47m",true);
		break;
	case DBG_GRAY:
		PrintDebug("\u001B[40m",true);
		break;
	case DBG_BLUE:
		PrintDebug("\u001B[44m",true);
		break;
	case DBG_GREEN:
		PrintDebug("\u001B[42m",true);
		break;
	case DBG_CYAN:
		PrintDebug("\u001B[46m",true);
		break;
	case DBG_RED:
		PrintDebug("\u001B[41m",true);
		break;
	case DBG_MAGENTA:
		PrintDebug("\u001B[45m",true);
		break;
	case DBG_YELLOW:
		PrintDebug("\u001B[43m",true);
		break;
	case DBG_WHITE:
		PrintDebug("\u001B[47m",true);
		break;
	}
	
	m_bgColor=color;
}

OMEDBG_COLOR DebugOutput::GetTxColor()
{

	return m_txColor;
}

OMEDBG_COLOR DebugOutput::GetBgColor()
{

	return m_bgColor;
}
#endif
#endif

/** @return DebugOutput Singleton used for global debugging output.*/
DebugOutput& DebugOutput::GetStaticOutput()
{
	return static_output::dbOut;
}