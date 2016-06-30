#pragma once
#include <string>
#include <iostream>
#include "CharType.h"

/** Protocol base class for generating object descriptions */
class __EXPORT__ Summarizable
{
public:
	virtual ~Summarizable(){};
	//add protocols for description/summary related functions here.
	

	/**	Generate a string representation for the object.
		__NOTE:__ The returned string is allocated via new(); the returned pointer must be disposed of
		using delete[] to avoid a memory leak.
		@param tabChar Optional character used for indentation; default is ' '.
		@param indent Optional default indent depth for c-string; default is 0.
		@param inc Optional number of characters to add should the indent level need to be increased; default is 4
		@return C-string summary of the object.
	*/
	virtual const OMEChar* ToString(const OMEChar tabChar=' ', const unsigned int indent=0, const unsigned int inc=4) const=0;
   /**	Generate a string representation for the object, and store it in a preallocated buffer.
		@param out The OMEChar buffer to populate.
      @param len The length of out.
		@param tabChar Optional character used for indentation; default is ' '.
		@param indent Optional default indent depth for c-string; default is 0.
		@param inc Optional number of characters to add should the indent level need to be increased; default is 4
		
	*/
	virtual void ToString(OMEChar* & out, unsigned int len, const OMEChar tabChar=' ', const unsigned int indent=0, const unsigned int inc=4) const;
    /**	Generate a string representation for the object, and store it in a preallocated buffer.
		@param out The std::string to populate.
		@param tabChar Optional character used for indentation; default is ' '.
		@param indent Optional default indent depth for c-string; default is 0.
		@param inc Optional number of characters to add should the indent level need to be increased; default is 4
		
	*/
	virtual void ToString(std::string & out, const OMEChar tabChar=' ', const unsigned int indent=0, const unsigned int inc=4) const;
};

 __EXPORT__ std::ostream& operator<<(std::ostream &a_Stream, Summarizable &sum);
