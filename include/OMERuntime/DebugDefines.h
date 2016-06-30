#pragma once
#if (defined _DEBUG || defined OME_SHOW_DEBUG) && !defined OME_SUPPRESS_DEBUG
#if (defined WIN32 || defined _WIN64 || defined __APPLE__)
#define DBG_COLOR_SUPPORT
#endif
#define DBG_POPLEVEL( )			DebugOutput::GetStaticOutput().DecreaseLevel( )
#define DBG_PUSHLEVEL( )		DebugOutput::GetStaticOutput().IncreaseLevel( )
#define DBG_SETINDENT( x )		DebugOutput::GetStaticOutput().SetIndentWidth( x )
#define DBG_SEOMEChar( x )		DebugOutput::GetStaticOutput().SetIndentOMEChar( x )
#define DBG_PRINT( x )			DebugOutput::GetStaticOutput().PrintDebug( x )
#define DBG_PRINT_FLAGGED( x, y )		DebugOutput::GetStaticOutput().PrintDebug(( x ), false, y)
#define DBG_PARTIAL_PRINT( x )	DebugOutput::GetStaticOutput().PrintDebug(( x ),true)
#define DBG_LOGFILE( x )		DebugOutput::GetStaticOutput().SetLogFile( x )
#define DBG_ALT_PRINT_FUNC( x ) DebugOutput::GetStaticOutput().SetPrintFunc( x )
#define DBG_ALT_LOGPRINT_FUNC( x ) DebugOutput::GetStaticOutput().SetLogFileFunc( x )
#define DBG_NOLOG( )			DebugOutput::GetStaticOutput().SetLogFile()
#define DBG_TIMESTAMP( )		DebugOutput::GetStaticOutput().PrintDateTime()
//#define DBG_PRINTLINE( )		DebugOutput::GetStaticOutput().PrintDebug(std::string(std::stringstream(__LINE__).str()))
#define DBG_PRINTFILE( )		DebugOutput::GetStaticOutput().PrintDebug(std::string(__FILE__))
#define DBG_PRINTBIGDIV()		DebugOutput::GetStaticOutput().PrintDebug("\n=========================================================\n")
#define DBG_PRINTSMALLDIV()		DebugOutput::GetStaticOutput().PrintDebug("\n------\n")
#define DBG_DISABLE()			DebugOutput::GetStaticOutput().DisablePrinting()
#define DBG_ENABLE()			DebugOutput::GetStaticOutput().EnablePrinting()

	#ifdef OME_SHOW_ELAPSED
		#define DBG_PRINTELAPSED(x)		DebugOutput::GetStaticOutput().PrintElapsed(x);
	#else
		#define DBG_PRINTELAPSED(x)		0
	#endif

	#ifdef DBG_COLOR_SUPPORT
	#define DBG_SETTXCOLOR(x)		DebugOutput::GetStaticOutput().SetTextColor(x)
	#define DBG_SETBGCOLOR(x)		DebugOutput::GetStaticOutput().SetBGColor(x)
	#define DBG_COLOR(x,y)			{ OMEDBG_COLOR color=DebugOutput::GetStaticOutput().GetTxColor(); DebugOutput::GetStaticOutput().SetTextColor(y); (x); DebugOutput::GetStaticOutput().SetTextColor(color);}
	#define DBG_PRINTC(x, y)		DBG_COLOR(DBG_PRINT(x),y)
	#define DBG_GETTXCOLOR(c)		c=DebugOutput::GetStaticOutput().GetTxColor();
	#define DBG_GETBGCOLOR(c)		c=DebugOutput::GetStaticOutput().GetBgColor();
	#else
	#define DBG_SETTXCOLOR(x)		0
	#define DBG_SETBGCOLOR(x)		0
	#define DBG_COLOR(x,y)			(x)
	#define DBG_PRINTC(x, y)		DBG_PRINT(x)
	#define DBG_GETTXCOLOR(c)		0
	#define DBG_GETBGCOLOR(c)		0
	#endif
#else

#define DBG_POPLEVEL( )			0
#define DBG_PUSHLEVEL( )		0
#define DBG_SETINDENT( x )		0
#define DBG_SEOMEChar( x )		0
#define DBG_PRINT( x )			0
#define DBG_PRINT_FLAGGED( x, y )		0
#define DBG_PARTIAL_PRINT( x )	0
#define DBG_LOGFILE( x )		0
#define DBG_NOLOG( )			0
#define DBG_TIMESTAMP( )		0
#define DBG_PRINTLINE( )		0
#define DBG_PRINTFILE( )		0
#define DBG_PRINTBIGDIV()		0
#define DBG_PRINTSMALLDIV()		0
#define DBG_ALT_PRINT_FUNC( x ) 0
#define DBG_ALT_PRINT_FUNC( x ) 0
#define DBG_ALT_LOGPRINT_FUNC( x ) 0
#define DBG_PRINTELAPSED(x)		0
#define DBG_SETTXCOLOR(x)		0
#define DBG_SETBGCOLOR(x)		0
#define DBG_COLOR(x,y)			0
#define DBG_PRINTC(x, y)		0
#define DBG_GETTXCOLOR(c)		0
#define DBG_GETBGCOLOR(c)		0
#define DBG_DISABLE()			0
#define DBG_ENABLE()			0
#endif