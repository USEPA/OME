#pragma once
#include "OMENull.h"
#include "EvalTable.h"
#include <list>

//forward decls to allow friending with Evaluable
class Evaluable;
class Influence;

/** Temporary function object

	Some environments store functions as separate objects; this class stores such objects until they are merged with their targets during importation.
*/
class NullFunc : public OMENull
{   
public:

	/// stores a bunch of information that is stored as a table in a variable, seperate from value or equations
	struct TableData		
	{
		STLString file;						///< source file (used to refresh; should use current for actual values).
		STLString colName;					///<name of column in file.
		StrArray data;						///< column(s) to access ??.
		StrArray indices;						///< List of boundary indicies, and some other data.
		EvalTable::DataArray current;		///< last values input.
		int dims[2];							///< number of entries, how they are ordered. Assume 2 dims max.
		STLString units;						///< units used.
		StrArray bounds;						///< The type of value for boundaries.

		/** Default constructor.*/
		TableData()
		{
			dims[0]=-1;
			dims[1]=-1;
		}
		/** Copy Constructor.
			@param td The TableData to copy.
		*/
		TableData(const TableData & td)
		{
			file = td.file;
			colName = td.colName;
			data = td.data;
			indices = td.indices;
			current = td.current;
			dims[0] = td.dims[0];
			dims[1] = td.dims[1];
			units = td.units;
			bounds = td.bounds;
		}
	};

	/** Default Constructor */
	NullFunc() :OMENull(),m_out(NULL),m_complete(false),m_isArrayFunc(false),m_isConverted(false)/*,m_ghost(NULL)*/ {}
	/** Simple Constructor 
		@param n = name of object
	*/
	NullFunc(STLString n) : OMENull(n,""),m_out(NULL),m_complete(false),m_isArrayFunc(false),m_isConverted(false)/*,m_ghost(NULL)*/ {}

	/** Copy constructor
		@param nf the NullFunc to copy.
	*/
	NullFunc(const NullFunc & nf) { Duplicate(nf); };
	/** Assignment Operator.
		@param nf The NullFunc to copy.
		@return Reference to the NullFunc.
	*/
	NullFunc& operator=(const NullFunc & nf) { Duplicate(nf); return *this; };

	
   virtual NULLTYPE GetNullType( void ) { return NULL_FUNC; }

   void AddInfluence(Influence* pInf);
   void AddOutInfluence(Influence* pInf);

   void SetOutObject(Evaluable* pEval);
   Evaluable* GetOutObject();
   Influence* GetInfluence(const size_t i);
   size_t GetInfCount()const;

   void SetExpr(const STLString & expr);
   STLString GetExpr() const;

   void SetTableData(const STLString & f, const StrArray & dl,const StrArray & il,const EvalTable::DataArray & current,const int* d, const STLString & u);
   void SetTableData(const TableData & data);
   TableData GetTableData() const;

   void SetSpec(const STLString & spec);
   STLString GetSpec() const;

   void SetComplete(const bool comp);
   bool GetComplete() const;

   bool GetIsArrayFunction() const;
   EvalTable* GenerateEvalTable() const;

	/** @param units The units to use for function results. */
   inline void SetUnitStr(const STLString & units) { m_unitStr=units;}
	/** @return String which represents units used by Function. */
   inline STLString GetUnitStr() const {return m_unitStr;}
   static bool FindArrayDims(const STLString & units, IndVec & out, std::list<Model*> & mdls);

   static EvalTable* GenerateEvalTable(TableData data);
protected:
	STLString m_expr;			///< function expression
	STLString m_spec;			///< needed?
	STLString m_unitStr;		///< String containing unit info for expression. Used primarily to inform about application to arrays.
	TableData m_tData;		///< used to transfer an Evaluable's tabledata
	
	bool m_isConverted;						///< True if expression has been converted from Simile to OME syntax.
	std::vector<Influence*> m_inArray;		///< Any influences pointing to this function.
	std::vector<Influence*> m_outArray;		///< Any influences pointing away from this function (for when flow is m_out).
	Evaluable* m_out;							///< Object that is tied to this function.
	bool m_complete;							///< Function is complete.
	bool m_isArrayFunc;						///< Flag indicating whether or not the expression is intended to be applied to an array object.
	//NullGhost* m_ghost;						///< Associated NullGhost, if any. Used to map function from ghost to source object during conversion.

	bool IsAppliedToArray();

	void Duplicate(const NullFunc & nf);
}; 
