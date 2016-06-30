#pragma once
#include <string>
#ifndef OME_UNICODE
	#define OMEChar char

#	if defined WIN32 || defined _WIN64
#		define SAFE_STRCPY(src,dest,dSize)	strcpy_s(dest,dSize,src)
#	else
#		define SAFE_STRCPY(src,dest,dSize)	strcpy(dest,src)
#	endif
#else
	#define OMEChar wchar_t
	//#define char32_t OMEChar,*POMEChar
	//#define wchar_t OMEChar,*POMEChar
#endif

typedef std::basic_string<OMEChar, std::char_traits<OMEChar>, std::allocator<OMEChar> > STLString;