#pragma once

#include "OMEDefines.h"
#include "omeobjects/SimObjs.h"
#include "CharType.h"
#include <map>
#include"XMILEOptionSet.h"
#include "ModifierProcessor.h"
#include "ModelFileParser.h"

class XMILENodeProcessor;


//C++11 function signatures
using XNPFunc = void*(*)(TI2Element*,XMILENodeProcessor*);
using XVPFunc = bool(*)(TI2Element*, Model*, XMILENodeProcessor*);

typedef std::map<STLString, XNPFunc> XMILEGHandlerMap;
typedef std::map<STLString, XVPFunc> XMILEVHandlerMap;

/** Class for translating XMILE files to OME model (.omem) files. */
class XMILENodeProcessor : public ModelFileParser
{
public:
	XMILENodeProcessor();

	virtual Model* ParseModelFromFile(const STLString & path);
	Model* ProcessXMILEDoc(TI2Document & doc);

private:
	
	/** Structure for storing processed header information. */
	struct HResults
	{
		STLString name;			 ///< The name supplied by the header.
		XMILEOptionSet options;	 ///< The options requested by the header.
	};

	/** Structure for storing processed Simulation Specification fields. */
	struct SimSpecResults
	{
		OME_SCALAR start;	   ///< The simulation start time.
		OME_SCALAR stop;	   ///< The simulation stop time.
		OME_SCALAR dt;		   ///< The requested step size.
		STLString method;	   ///<	The requested integration method.
		STLString timeUnits;   ///< The units used to measure the passage of time.
		OME_SCALAR pauseInt;   ///< The simulation pause frequency (not used by OME).
		STLString runModels;   ///< The run flag for how to run models (not presently used by OME).

		/**Simple constructor.
			@param b The start time to assign.
			@param e The end tim to assign.
		*/
		SimSpecResults(const OME_SCALAR & b, const OME_SCALAR & e)
			:start(b), stop(e), dt(1.0), method("euler"),/*timeunits(),*/ pauseInt(OME_SCALAR_INF), runModels("all")
		{}
		
	};

	unsigned int m_idCount;							 ///< The total number of unique model ids that have been auto-generated.
	std::map<STLString, EvalTable*> m_tables;		 ///< Collection of EvalTables sorted by variable ID.
	std::map<STLString, StateVar*> m_inflows;		 ///< Inbound Flow identifier for a given StateVar.
	std::map<STLString, StateVar*> m_outflows;		 ///< Outbound Flow identifier for a given StateVar.
	std::map<OMEObject*, STLString> m_modifiers;	 ///< Conversion modifier directives.
	ObjMap m_objMap;								 ///< Map of all construct model components.

	/** Generate a unique ID for a model component.
		@return A unique ID.
	*/
	STLString GenerateID() { return "var_" + std::to_string(m_idCount++); }
	bool WireFlows(Model* pMdl);
	OMEObject* RegisterObject(OMEObject * pObj);

	void* ProcessNode(TI2Element* pParentElem, const STLString & tag,const bool & isReq=true);
	void* ProcessNode(TI2Element* pCurrElem, const bool & isReq=true);

	static XMILEGHandlerMap s_generalHandlers; 	 ///< Map for functions that handle general model components.
	static XMILEVHandlerMap s_viewHandlers;		 ///< Map for handling view-based model components.

	static void InitNodeProcessors();
	static bool HasUnsupportedOptions(const XMILEOptionSet & opts);
	static STLString ConvertExpression(const STLString & str,OMEObject* pObj);

	//list functions here
	static void* ProcessHeader(TI2Element* pElem, XMILENodeProcessor* pNProc);
	static void* ProcessSimSpecs(TI2Element* pElem, XMILENodeProcessor* pNProc);

	static bool PopulateWithCommonFields(TI2Element* pElem, Evaluable* pEval, XMILENodeProcessor* pNProc);

	static void* ProcessStock(TI2Element* pElem, XMILENodeProcessor* pNProc);
	static void* ProcessFlow(TI2Element* pElem, XMILENodeProcessor* pNProc);
	static void* ProcessAux(TI2Element* pElem, XMILENodeProcessor* pNProc);
	static void* ProcessModel(TI2Element* pElem, XMILENodeProcessor* pNProc);

	static void* ProcessGraphFunction(TI2Element* pGFElem, XMILENodeProcessor* pNProc);

	static bool ProcessNodeView(TI2Element* pElem, Model* pMdl, XMILENodeProcessor* pNProc);
	static bool ProcessFlowView(TI2Element* pElem, Model* pMdl, XMILENodeProcessor* pNProc);
	static bool ProcessConnector(TI2Element* pElem, Model* pMdl, XMILENodeProcessor* pNProc);


	static STLString ProcessID(STLString inExpr);

};