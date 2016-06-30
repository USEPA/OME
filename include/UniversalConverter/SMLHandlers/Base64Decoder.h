#pragma once
#include "MIMEDecoder.h"

/** Decoder for data encoded using base64 algorithm.
	assumes unsigned chars
*/
class Base64Decoder : public MIMEDecoder
{
public:
	Base64Decoder(void);
	~Base64Decoder(void);

	virtual STLString Decode(const char * inBuffer, const int inSize);
	virtual void Decode(const char * inBuffer, const int inSize, unsigned char* & outBuffer, unsigned int & outSize);

private:
	std::map<unsigned char,unsigned char> m_decoderTable;  ///< map that returns numerical value for character code.

	int DecodeBase64Quad(const char* inBuffer, unsigned char* outBuffer);

};

//inline functions
/** Decodes a block of data using a base64 algorithm.
	A buffer of 4 characters is taken in, and a buffer of 3 characters is returned.
	@param inBuffer buffer that passes in a 4-byte buffer.
	@param outBuffer buffer that passes back between 1 and 3 characters.
	@return the number of characters passed back in outbuffer.

*/
inline int Base64Decoder::DecodeBase64Quad(const char* inBuffer, unsigned char* outBuffer)
{
	unsigned int i,ls,rs; 

	/*          0               1               2		 <----- Endcoded chars
		|---------------|---------------|---------------|
		 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 <----- bit pattern
		|-----------|-----------|-----------|-----------|
		      0           1           2           3      <----- base64 representation
	*/

	//iterate through base64 chars, extracting encoded chars.
	//if '=' is encountered, that means there are no further characters encoded
	for(i=0, ls=2, rs=4; i<3 && inBuffer[i+1]!='='; i++, ls+=2, rs-=2)
		outBuffer[i]=(m_decoderTable[inBuffer[i]] << ls) | (m_decoderTable[inBuffer[i+1]] >> rs);

	//pad any extra space with nulls.
	for(; i<3; i++)
		outBuffer[i]='\0';

	return i;
}