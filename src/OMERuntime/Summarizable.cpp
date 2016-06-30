//#include "StdAfx.h"
#include "Summarizable.h"

std::ostream& operator<<(std::ostream &a_Stream, Summarizable &sum)
{
	using namespace std;
	string out;
	sum.ToString(out);
	return a_Stream<<out;
}

/**	Set Contents of a C-string to a description of the represented object.
	As much of the description as possible will be copied into the buffer, based on len.
		@param out The C-string to store the null-terminated result string in.
		@param len The absolute size of the buffer.
		@param tabChar Optional character used for indentation; default is ' '.
		@param indent Optional default indent depth for c-string; default is 0.
		@param inc Optional number of characters to add should the indent level need to be increased; default is 4
	*/
void Summarizable::ToString(OMEChar* & out, unsigned int len, const OMEChar tabChar, const unsigned int indent, const unsigned int inc) const
{
	const char* tempBuff=ToString(tabChar,indent,inc);

	int i=-1;
	if(len)
	{
		unsigned int lastIndex=len-1;
		do
		{
			i++;
			out[i]=tempBuff[i];
		} while(tempBuff[i]!='\0' && i<lastIndex);

		if(i>=lastIndex)
			out[lastIndex]='\0';
	}
	delete[] tempBuff;
}

/**	Set Contents of a STL string to a description of the represented object.
		@param out The STL string object to store the result string in.
		@param tabChar Optional character used for indentation; default is ' '.
		@param indent Optional default indent depth for c-string; default is 0.
		@param inc Optional number of characters to add should the indent level need to be increased; default is 4
		@return C-string summary of the object.
	*/
void Summarizable::ToString(std::string & out, const OMEChar tabChar, const unsigned int indent, const unsigned int inc) const
{
	const char* tempBuff=ToString(tabChar,indent,inc);
	out=tempBuff;
	delete[] tempBuff;
}