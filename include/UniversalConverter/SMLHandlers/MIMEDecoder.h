#pragma once

#include "SMLHandlers/pltokens/PLParserDefines.h"
/** Virtual base class to define common interface for MIME-type decoders.
	There are a number of encoding methods used by MIME; this class ensures a common interface.
*/
class MIMEDecoder
{
public:
	MIMEDecoder(void);
	~MIMEDecoder(void);

	/**	Decode a buffer as a string.
		Best used for encoded text files.
		By packing the result in STL's string class, much of the book-keeping is taken care of.
		@param inBuffer Buffer containing the characters of the encoded file.
		@param inSize The number of characters in inBuffer.
		@return string containing the decoded information.
	*/
	virtual STLString Decode(const char * inBuffer, const int inSize)=0;

	/** Decode buffer as raw data.
		Best used with encoded binary files.
		@param inBuffer Buffer containing the characters of the encoded file.
		@param inSize The number of characters in inBuffer.
		@param outBuffer unallocated pointer that will point to bytebuffer of characters upon function completion.
		@param outSize set to size of outBuffer upon completion of function.
	*/
	virtual void Decode(const char * inBuffer, const int inSize, unsigned char* & outBuffer, unsigned int & outSize)=0;
};

