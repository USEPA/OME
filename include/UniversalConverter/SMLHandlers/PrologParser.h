#pragma once

#include "SMLHandlers/pltokens/PLParserDefines.h"
#include "SMLHandlers/pltokens/PLToken.h"
#include "SMLHandlers/pltokens/PLTNode.h"
#include "SMLHandlers/pltokens/PLTSource.h"
#include "SMLHandlers/pltokens/PLTLinks.h"
#include "SMLHandlers/pltokens/PLTArc.h"
#include "SMLHandlers/pltokens/PLTProperties.h"
#include "ModelFileParser.h"
#include <iostream>

typedef std::vector<NullFunc*> FuncArray;
/** Parser for prolog-based Simile files.
	Parsing tool intended to parse Simile-generated prolog files and build a representative model hierarchy
*/
class PrologParser : public ModelFileParser
{
public:
	/** Default constructor */
	PrologParser(void);

	/** Copy constructor
		@param pp the parser to copy
	*/
	PrologParser(const PrologParser & pp) { PopulateTokenMap(); Duplicate(pp); }
	/** Assignment Operator.
		@param pp The PrologParser to copy.
		@return Reference to the PrologParser.
	*/
	PrologParser& operator=(const PrologParser & pp) { Duplicate(pp); return *this; };

	/** Default destructor */
	virtual ~PrologParser(void);

	virtual Model* ParseModelFromFile(const STLString & path);

	void clearMaps(bool fullDelete=false);

	OMEMap m_objectIDMap;				///< catalog of all created objects
	ModelMap m_submodelMap;			///< catalog of all submodels
	LinkMap m_linkMap;				///< catalog of all link classes
	ALMMap m_interModelLinkMap;		///< catalog of intermodel links, sorted by submodel
	SubPortList m_subPortList;		///< catalog of submodelPorts, based on info from border nodes
	AliasArray m_ghostArray;		///< catalog of all ghosts found.
	AssocList m_assocList;			///< catalog of all associations found.
	FuncArray m_funcArray;			///< catalog of all Function objects found.

	/** Store modification statement for a given object.
		@param pObj Pointer to Object to associated with a modifier statement.
		@param str The modifier statement to add.
	*/
	void AddModifier(OMEObject* pObj,const STLString & str) { m_modifiers[pObj]=str;};
	/** Retrieve a modification statement for a given object.
		@param pObj Address of object to retrieve modification statement for.
		@return A modification statemetn for pObj, if one exists; otherwise an empty string.
	*/
	STLString GetModifier(OMEObject* pObj) { return m_modifiers.find(pObj)!=m_modifiers.end() ? m_modifiers[pObj] : STLString(); }
	/** Associate a Model as a submodel of another submodel.
		@param pMdl Pointer to the parent Model object.
		@param str ID of submodel to associate.
	*/
	void AddModelSubID(Model* pMdl,const STLString & str){ m_modelSubIDs[pMdl].push_back(str);}

	/** Add a NullFunc ID to associate with a specific flow.
		@param pFlow Pointer to the Flow to associate with the Func ID.
		@param funcID The ID of the NullFunc object to associate with pFlow.
	*/
	void AddFlowFunc(Flow* pFlow, const STLString & funcID) { m_attachedMap[pFlow] = funcID; }

	/** Record a Simile role attribute for an influence.
		@param role A string containing the role details.
		@param pInfl Pointer to the Influence that the role applies to.
	*/
	void AddInflRole(const STLString & role, Influence* pInfl) { m_roleMap[pInfl->GetID()] = role; };
protected:
	std::map<OMEObject*,STLString> m_modifiers;				///< A map of modification statements for specific objects.
	std::map<Model*,std::vector<STLString> > m_modelSubIDs;	///< A map of Models and the IDs of all their submodels.
	std::map<STLString, AssocPort*> m_assocPortMap;			///< ID-AssocPort map for use with constructing SubmodelAssoc chains.
	std::map<Flow*, STLString> m_attachedMap;				///< Flow-ID map for use with Simile 6x function association with flows.
	TokenMap m_tokenReferenceMap;							///< utility used for quick referencing token classes

	std::map<STLString, STLString> m_roleMap;				///< Mapping of role to Influence ID.

	Model* ParseStream(std::istream & inStrm);
	void ParseLine(STLString line);

	void Duplicate(const PrologParser & pp);
	void PopulateTokenMap();

private:
	void ConstructTimeSeries();
	void ConstructVarArrays();
	bool ExtractSource(Model* mdl);
	bool ExtractProperties(Model* mdl);
	void BindLinks(STLStringList & removeList);
	void VerifyGhostLinks(STLStringList & removeList);
	void BindPorts();
	void FixAliasPortBinding();
	void PopulateSubmodels();
	void PopulateModel(Model* mdl);
	void AnchorPortLinks(STLStringList & removeList);
	void AnchorLinks(STLStringList & removeList);
	void AnchorLinkObject(Link* pLinkObj);
	void AnchorAssocs();
	void WalkAnchorAssoc(SubmodelAssoc* pAssoc, OMEObject* pBase, OMEObject* pSub);
	void LinkRealignment(OMEObject* oldObj, OMEObject* newObj);
	void UpdateAssocReferences();
	void ReplaceAssocSyntaxForModel(STLString aName,const StrList & objNames);

	void FixAlias(Influence* pInfl);
	void AddScopeSyntax(const OMEMap & objMap);
	void DetermineUpGroups(const OMEMap & objMap,Model* pRootMdl);
	void RemoveMarkedIDs(STLStringList & remList);
	static AssocPort* ConvertToAssocPort(SubmodelPort* pPort);
	
	static void ApplyScope(Evaluable* pE);
	static void ApplyExprMod(const STLRegex & varReg,Influence* pInfl, const STLString & pathRep, std::list<OMEObject*> & visited);
	static void ApplyScopeToEnums(Model* pMdl);
};


//class PLTRoots : public PLToken
//{
//public:
//	PLTRoots();
//	~PLTRoots();
//
//	virtual int GetType() { return NT_ROOTS; };
//	virtual OMEObject* buildOMEObject(STLString args, PrologParser parser);
//}
//

	
