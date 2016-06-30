#pragma once
#include "omeobjects/NullObjs.h"

#include "OMEDefines.h"


//using STL objects instead of MFC...

//class forward declarations
class PrologParser;

class PLToken; 
class PLTNode;
class PLTSource;
class PLTLinks;
class PLTArc;

//strings
typedef std::list<STLString> STLStringList;
typedef std::list<STLString>::const_iterator STLStringListIter;

//maps
typedef std::map<STLString, PLToken*> TokenMap;
typedef std::map<STLString, OMEObject*> OMEMap;
typedef std::map<STLString, Model*> ModelMap;
typedef std::map<STLString, Link*> LinkMap;
typedef std::map<STLString, NullArcLinkMap*> ALMMap;

//lists
typedef std::list<SubmodelPort*> SubPortList;
typedef std::list<SubmodelAssoc*> AssocList;

//iterators
typedef TokenMap::const_iterator TokenIter;
typedef OMEMap::const_iterator OMEIter;
typedef ModelMap::const_iterator ModelIter;
typedef LinkMap::const_iterator LinkIter;
typedef ALMMap::const_iterator ALMMapIter;

typedef SubPortList::const_iterator SubPortIter;

