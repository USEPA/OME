#pragma once
#include "OMEDefines.h"

/** Virtual interface for parsing various models from other sources*/
class ModelFileParser
{
public:

	/** Entry point for parsing process.
	Function takes a filepath and returns a fully populated model if successful.
	@param path the path of the file to parse.
	@return a full model instance if successful, NULL otherwise.
	*/
	virtual Model* ParseModelFromFile(const STLString & path)=0;
};