#pragma once

//#include "StdAfx.h"
#include "Summarizable.h"
#include "DebugDefines.h"

#include <string>
#include <iostream>
#include <fstream>
#include <ctime>

//class intended to act as convenience for printing to console
//will encapsulate so can substitute different definitions for different uses/platforms

/** Debug message type identifiers.*/
enum OMEDBG_TYPE
{
	DBG_INFO,		///< Informational message.
	DBG_WARN,		///< Warning message.
	DBG_ERR,			///< Error message.
	DBG_CRIT			///< Super-bad critical error message; program must immediately terminate.
};

//win32 coloring stuff based on article: http://www.cplusplus.com/articles/Eyhv0pDG/
enum OMEDBG_COLOR
{
	DBG_DEFAULT=-1,
	
	DBG_BLACK=0,
	DBG_DK_BLUE,
	DBG_DK_GREEN,
	DBG_DK_CYAN,
	DBG_DK_RED,
	DBG_DK_MAGENTA,
	DBG_DK_YELLOW,
	DBG_DK_WHITE,
	DBG_GRAY, 
	DBG_BLUE,
	DBG_GREEN,
	DBG_CYAN,
	DBG_RED,
	DBG_MAGENTA,
	DBG_YELLOW,
	DBG_WHITE
};

typedef void (*DebugPrintFunction)(const OMEChar*,const bool,const OMEDBG_TYPE );

/** Utility class to simplify output messages to CLI.
	Intended to be wrapped in a static class, or in use in local instances
*/
class __EXPORT__ DebugOutput
{
public:
	DebugOutput();
	DebugOutput(const DebugOutput & dop);
	~DebugOutput();

	//void SetOutStream(const std::ostream& out);

	void DecreaseLevel(const unsigned int & size=1);
	void IncreaseLevel(const unsigned int & size=1);
	void SetIndentWidth(const unsigned int & width);
	void SetIndenOMEChar(const OMEChar & chr);
	void SetLogFile(const std::string filepath=std::string());
	/** Set the function for printing to the screen.
		@param printFunc Function to use for printing to screen.
	*/
	void SetPrintFunc(DebugPrintFunction printFunc) {m_printFunc=printFunc;}
	/** Set the function for printing to a log file.
		@param printFunc Function to use for printing to a log file.
	*/
	void SetLogFileFunc(DebugPrintFunction printFunc) {m_logFileFunc=printFunc;}

	void PrintDateTime();
	/** @return The amount of seconds elapsed since DebugOutput was instantiated. */
	unsigned long GetSecSinceInstance() { return (std::clock()-m_secStart)/CLOCKS_PER_SEC; }
	void PrintElapsed(const std::string label);
	void PrintDebug(const std::string & msg, const bool noEnd=false, const OMEDBG_TYPE type=DBG_INFO);
	void PrintDebug(const std::ostream & strm, const bool noEnd=false, const OMEDBG_TYPE type=DBG_INFO);
	void PrintDebug(Summarizable* pObj, const bool noEnd=false, const OMEDBG_TYPE type=DBG_INFO);
	void PrintDebug(Summarizable & obj, const bool noEnd=false, const OMEDBG_TYPE type=DBG_INFO);

	/** Disable Debug printing calls.*/
	inline void DisablePrinting() { m_disablePrint = true; }
	/** Enable Debug printing calls.*/
	inline void EnablePrinting()  { m_disablePrint = false; }
	static DebugOutput& GetStaticOutput();
	
//console color support is platform dependent; implmentations will vary
#ifdef DBG_COLOR_SUPPORT
	/** Set Text foreground color.
		@param color The color tag to use for drawing text foreground.
	*/
	void SetTextColor(const OMEDBG_COLOR color);
	/** Set Text background color.
		@param color The color tag to use for drawing text background.
	*/
	void SetBGColor(const OMEDBG_COLOR color);
	/** @return The current text foreground color.*/
	OMEDBG_COLOR GetTxColor();
	/** @return The current text background color.*/
	OMEDBG_COLOR GetBgColor();
#endif

private:
	//std::ostream & m_out;
	OMEChar m_indentChar;				///< Character to use for indentation.
	unsigned int m_level;				///< Level of indentation.
	unsigned int m_indentWidth;			///< Number of characters to use per level of indentation.
	
	clock_t m_secStart;					///< Start time of simulation run.

	DebugPrintFunction m_printFunc;		///< Function for writing to screen.
	DebugPrintFunction m_logFileFunc;	///< Function for writing to log file. 
	//std::ostream & m_consoleStream;
	std::ofstream m_logStream;			///< Stream to output file.
	bool m_disablePrint;				///< Flag indicating whether or not printing is disabled.

#ifdef DBG_COLOR_SUPPORT
	OMEDBG_COLOR m_txColor;				///< The currently selected foreground color.
	OMEDBG_COLOR m_bgColor;				///< The currently selected background color.
	OMEDBG_COLOR m_defaultTxColor;		///< The default foreground color.
	OMEDBG_COLOR m_defaultBgColor;		///< The default background color.

#endif
};

