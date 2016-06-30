#pragma once
#include "OMENull.h"

//placeholder for parser catalog
/** Temporary object for storing file information

	Stores information imported from another file type; discarded after importation
*/
class NullSource : public OMENull
{   
public:
	/** Default Constructor */
   NullSource();
   /** Full Constructor.
		@param p originating program name.
		@param v version of file.
		@param e edition of originating program.
		@param d description of file.
	*/
   NullSource(STLString p, STLString v, STLString e, STLString d);

   virtual NULLTYPE GetNullType( void ) { return NULL_SOURCE; }

private:
   STLString m_program;	///< Name of originating program.
   STLString m_edition;	///< Edition of originating program.
   STLString m_version;	///< Version of originating program.
   STLString m_modDate;	///< Last Modification date of file.

}; 
