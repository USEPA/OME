#pragma once

#include <fstream>
#include <map>
#include "OMEDefines.h"
#include "MDLEqnRecord.h"
#include "ModelFileParser.h"

/** Basic parser for extracting model data from Vensim's .mdl format. */
class MDLParser : public ModelFileParser
{
public:
	MDLParser();
	virtual ~MDLParser();


	virtual Model* ParseModelFromFile(const STLString & path);

private:
	
	/** MDL type identifiers. */
	enum { T_UNDEF=0     ///< No type defined.
		,T_ARROW         ///< Arrow type.
		, T_VAR = 10     ///< Variable type.
		, T_VALVE        ///< Valve type.
		, T_COMMENT      ///< Comment type.
		, T_BITMAP = 30  ///< Bitmap type.
		, T_METAFILES    ///< Metafile type.
	};

	/** Container for holding details about a .MDL arrow entry. */
	struct ArrowEntry
	{
		STLString id;				///< ID of arrow.
		STLString from;				///< ID of originating object.
		STLString to;				///< ID of destination object.
		int shape;					///< The type of shape to associate with arrow.
		bool hidden;				///< Whether or not the arrow is intended to be drawn.
		//char polarity;
		int thickness;				///< The thickness to apply to the arrow.
		//has font
		//dtype
		//color
		//font
		int numPoints;				///< The total number of points used to describe the arrow's path.
		std::list<OMEPoint> pts;	///< The list of points describing the arrow's path.
	};

	/** Container for holding details about a .MDL object entry. */
	struct ObjEntry
	{							   
		int type;				   ///< Type of object identifier.
		STLString id;			   ///< The object's ID.
		STLString name;			   ///< The name of the object.
		OMEBox dims;			   ///< The bounding box dimensions for the object when drawn.
		char shape;				   ///< The shape used to draw the object.
		char bits;				   ///< Bit flags for various behaviors.
		STLString comment;		   ///< Comment string for this object.
		bool hidden;			   ///<  Whether or not the object is intended to be drawn.
		//hasfont;
		//textpos
		//boxwidth
		//nav1
		//nav2
		//box color 
		//fill color
		//font
	};

	/** Record of objects going to/from a specific State Variable as parsed out of the .MDL.*/
	struct FlowEntry
	{
		std::list<STLString> inflows;	///< A list of Flows pointing toward the State Variable.
		std::list<STLString> outflows;	///< A list of Flows pointing away from the State Variable.

		FlowEntry()
		{}
		/** Copy constructor.
			@param fe The Flow Entry to copy.
		*/
		FlowEntry(const FlowEntry & fe)
			:inflows(fe.inflows), outflows(fe.outflows)
		{}
	};


	std::map<STLString, FlowEntry> flowMap;		   ///< Map of all flows connected to individual State Variables.
	std::map<STLString, MDLEqnRecord> m_eqnMap;	   ///< Name - equation entry map.
	std::map<STLString,ObjEntry> m_objRecMap;	   ///< Name - object entry map.
	std::deque<ArrowEntry> m_arrRecords;		   ///< Record of arrow entries. 
	std::map<OMEObject*, STLString> m_modifiers;   ///< Conversion modifier directives.

	void ParseEquations(STLString block);
	void ParseRepView(const STLString & block);

	void AddArrowEntry(StrArray & parts);
	void AddObjectEntry(StrArray & parts,const STLString & next);

	Model* AssembleModel();
	void UpdateEquations(Model* pMdl);

	void AssembleInfluences(Model* pMdl);

	STLString GenUniqueVarID(const STLString & prefix);

	static void AssembleInterpData(const STLString & inExpr, EvalTable::DataArray & outXVals, EvalTable::DataArray & outYVals, STLString & outID);
	static STLString ProcessExpression(Evaluable* pEval, Model* pRootMdl, STLString expr);
	static STLString ProcessQuotedVariables(Model* pRootMdl, const STLString & expr);
	static STLString ProcessFunction(Evaluable* pCaller, const STLString & funcName, const STLString & args);

	//for modifier processor
	static STLString ConvertExpression(const STLString & str, OMEObject* pObj);
};

