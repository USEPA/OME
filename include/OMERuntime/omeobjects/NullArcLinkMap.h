#pragma once
#include "OMENull.h"
#include <list>
#include <string>

//placeholder for parser catalog
typedef std::pair<STLString,STLString> arcLink;
typedef std::list<arcLink> arcLinkList;
typedef std::list<arcLink>::const_iterator arcLinkIter;

/** Temporary link map.

	Class used to temporarily store intermodel linkage information during model construction.
*/
class NullArcLinkMap : public OMENull
{   
public:
	/**Default Constructor */
	NullArcLinkMap() : OMENull() {}
		/**Simple Constructor
		@param n name of null object
		@param d description of null object
	*/
	NullArcLinkMap(STLString n, STLString d) : OMENull(n,d) {}
	/** Copy constructor
		@param nalm the NullArcLinkMap to copy
	*/
	NullArcLinkMap(const NullArcLinkMap & nalm) { Duplicate(nalm); };
	/** Assignment Operator.
		@param nalm The NullArcLinkMap to copy.
		@return Reference to the NullArcLinkMap.
	*/
	NullArcLinkMap& operator=(const NullArcLinkMap & nalm) { Duplicate(nalm); return *this; };
	
	/** @param id The id of the associated submodel. */
	inline void SetSubmodelID(const STLString & id) { m_subModelID=id; };
	/** @return ID of associated of submodel.*/
	inline STLString GetSubmodelID() const { return m_subModelID; } 

	/** Add link to link map.
		@param al The ArcLink to add.
	*/
	inline void AddArcLink(const arcLink & al) { m_links.push_back(al); }
   virtual NULLTYPE GetNullType( void ) { return NULL_ALM; }
	
	/** @return Beginning iterator to internal arc link map.*/
	inline arcLinkIter GetLinksBegin() { return m_links.begin(); }
	/** @return End iterator to internal arc link map.*/
	inline arcLinkIter GetLinksEnd() {return m_links.end(); }

protected:
	STLString m_subModelID;	///< unique identifier of associated submodel.
	arcLinkList m_links;	///< list of paired objects linked across model boundaries.

   void Duplicate(const NullArcLinkMap & nalm);
}; 
