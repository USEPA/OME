#include "SMLHandlers/Base64Decoder.h"

Base64Decoder::Base64Decoder(void)
{
	//initialize decoder table
	unsigned char tablekeys[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	for(unsigned char x=0; x<64; x++)
		m_decoderTable[tablekeys[x]]=x;
}


Base64Decoder::~Base64Decoder(void)
{
}

STLString Base64Decoder::Decode(const char * inBuffer, const int inSize)
{
	unsigned int outSize;
	unsigned char* outBuffer=NULL;
	
	Decode(inBuffer,inSize,outBuffer,outSize);

	STLString outString((const char*)outBuffer);
	delete[] outBuffer;

	return outString;
}

void Base64Decoder::Decode(const char * inBuffer, const int inSize, unsigned char* & outBuffer, unsigned int & outSize)
{
	outSize=((inSize*3)/4)+3;
	unsigned int in, out;
	outBuffer=new unsigned char[outSize];
	
	const int limit= inSize/4;

	//assume number of characters are multiples of 12

#pragma omp parallel for default(none) private(in,out) shared(inBuffer,outBuffer)
	for(int i=0; i<limit; i++)
	{
		in=i*4;
		out=i*3;
		DecodeBase64Quad(&inBuffer[in],&outBuffer[out]);
	}
	//outSize=out;
}