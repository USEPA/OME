#pragma once
#include <random>
#include <sstream>
#include <new>
#include <functional>
#include <initializer_list>

#include "MemPool.h"

#include "ValCache.h"
#include "SimManager.h"
#include "SubIndexVal.h"
#include "CompiledMdlWrappers.h"
#include "OMEException.h"

/// @cond DOX_NO_DOC
typedef std::default_random_engine	RAN_ENGINE;
#ifndef OME_RAND_DEBUG
	#define SEED time(0)
#else
	#define SEED 1
#endif

#define NEWMAT3 new OME_SCALAR[9]
#define XY(x,y)		((x)*3)+(y)
#define IFRET(x,y,z) [](){if(x) return (y); else return (z);}	
/// @endcond

//forward declares
class CPPLoopCluster;

/** Namespace used for CPP-ized update expressions; only func sig documented since it has use outside of this logical region.*/
namespace OMECFuncs
{
	//constants for argument lists
	//static const OMEChar* TMP_INT_NAME = "_tmpInt";
	//static const OMEChar* TMP_SCLR_NAME = "_tmpSclr";
	//static const OMEChar* TMP_LIST_NAME = "_tmpList";

	/** Dimension function for use with MakeArray. */
	typedef function < OME_SCALAR(Evaluable&, BaseManager*,ISpatialDataProvider*)> MADimFunc;


	/** Stores details about a function's invocation; used to assist in a model logic's serialization. */
	struct __EXPORT__ FuncSig
	{
		/** underlying type for FS_ARGS */
		typedef unsigned short FSA_TYPE;

		/** Flags indicating potential return types.*/
		enum FS_ARGS : FSA_TYPE
		{
			NO_ARGS=0,			 ///< No known argument type.
			INT_ARG=0x0001,		 ///< Integer argument.
			SCLR_ARG=0x0002,		 ///< Scalar argument.
			EVAL_ARG=0x0004,		 ///< Evaluable pointer argument.
			ENUM_ARG=0x0008,		 ///< Enumerated value argument.
			SPWN_ARG=0x0010,		 ///< Spawner argument.
			ALIAS_ARG=0x0020,		 ///< Evaluable Alias argument.
			LIST_ARG=0x0040,		 ///< List argument.
			STR_ARG=0x0080,		 ///< String argument.
			VA_ARG=0x0044,		 ///< VarArray argument.
			
			IF_ARG=0x0100		///< If statement that needs type resolved.
		};

		/// @cond NO_DOX
		//bits for configuring basic overridable functions
		using ArgProcFunc= FS_ARGS(*)(const FSA_TYPE &,const unsigned int &);
		using CustomFunc = FS_ARGS(*)(const STLString&,oSTLSStream&,Evaluable*, CPPLoopCluster*,size_t &,const STLString &, const bool &);
		static FS_ARGS DefaultAPFunc(const FSA_TYPE & args,const unsigned int & unused) { return (FS_ARGS)args; }
		///@endcond

		/** Behavior flags.*/
		enum FLAGS
		{
			NO_FLAGS = 0,			///< No behavior flags set.
			MULTI_ARG = 0x1,		///< The function can be called with multiple objects in a list; the function will be applied to each individual object.
			MULTI_RET = 0x02,		///< Returns a list of results instead of a single result.
			CROSS_AGG = 0x04,		///< Function forces parent to aggregate across instances.
			PAST_LOOKUP = 0x08,		///< Function looks up values that were previously recorded.
			LOCAL_ACCUM = 0x10,		///< Function performs inline/local aggregation of list values.
			SHALLOW_DIFF = 0x20,		///< Different function called if list dimension count is 1. Function called is expected to be [fname]Sclr
			CUSTOM_SIG = 0x40		///< A custom function is supplied to function signature to be called during the function's construction.
		};


		/** Constructor.
			@param n Name of function.
			@param ap Argument pattern.
			@param rt Return pattern.
			@param f or'd bit flags that describe the function's attributes.
			@param apf Optional ArgProcFunc override.
			@param cf Pointer to CustomFunc if needed.
		*/
		FuncSig(const STLString & n, const STLString & ap, const STLString & rt, const unsigned char & f = NO_FLAGS, ArgProcFunc apf = DefaultAPFunc, CustomFunc cf = NULL)
		:name(n), arg_pattern(ap), resType(rt), flags(f), ArgFunc(apf), CustFunc(cf){}

		/** Default constructor. */
		FuncSig()
			:flags(NO_FLAGS), ArgFunc(DefaultAPFunc) {}

		STLString name;			///< Name of function.
		STLString arg_pattern;	///< String containing pattern used to define argument types and order.
		STLString resType;		///< String indicating what type of object is returned upon completion.

		unsigned char flags;	///< Variable containing bitflags.

		ArgProcFunc ArgFunc;	///< Function for processing arguments.
		CustomFunc CustFunc;	///< Function override to be used to generate function C++ code.

		//Sig Factories
		/** Generate a FuncSig for a variable argument function.
			@param n Name of function.
			@param ap Pattern used to interpret arguments.
			@param rt Pattern to interpret return values.
			@param addl Additional flags to apply to FuncSig.
			@param apf Optional ArgProcFunc override.
			@return A new FuncSig.
		*/
		inline static FuncSig MultiFuncSig(const STLString & n, const STLString & ap, const STLString & rt, const unsigned char & addl = NO_FLAGS, ArgProcFunc apf = DefaultAPFunc)
		{ return FuncSig((n), (ap), (rt), FuncSig::MULTI_ARG | addl,apf); }
		/** Generate a FuncSig for a function with multiple return values.
		@param n Name of function.
		@param ap Pattern used to interpret arguments.
		@param rt Pattern to interpret return values.
		@param addl Additional flags to apply to FuncSig.
		@param apf Optional ArgProcFunc override.
		@return A new FuncSig.
		*/
		inline static FuncSig MultiRetSig(const STLString & n, const STLString & ap, const STLString & rt, const unsigned char & addl = NO_FLAGS, ArgProcFunc apf = DefaultAPFunc)
		{ return FuncSig((n), (ap), (rt), FuncSig::MULTI_RET | addl,apf); }
		/** Generate a FuncSig for a function that aggregates across instances.
		@param n Name of function.
		@param ap Pattern used to interpret arguments.
		@param rt Pattern to interpret return values.
		@param addl Additional flags to apply to FuncSig.
		@param apf Optional ArgProcFunc override.
		@return A new FuncSig.
		*/
		inline static FuncSig CrossAggFuncSig(const STLString & n, const STLString & ap, const STLString & rt, const unsigned char & addl = NO_FLAGS, ArgProcFunc apf = DefaultAPFunc)
		{ return FuncSig((n), (ap), (rt), FuncSig::CROSS_AGG | addl,apf); }

		/** Generate a FuncSig for a custom generation process.
		@param n Name of function.
		@param cf The custom func to use to generate C++ code.
		@param addl Additional flags to apply to FuncSig.
		@param apf Optional ArgProcFunc override.
		@return A new FuncSig.
		*/
		inline static FuncSig CustomFuncSig(const STLString & n, CustomFunc cf, const unsigned char & addl = NO_FLAGS, ArgProcFunc apf = DefaultAPFunc)
		{
			return FuncSig((n), (""), (""), FuncSig::CUSTOM_SIG | addl, apf,cf);
		}
	};

	/** List of FS_ARGS.*/
	typedef list<FuncSig::FS_ARGS> FSArgList;

///@cond DOX_NO_DOC

//=========== namespace globals ===================================
	static RAN_ENGINE sGen;     ///< C++11 engine used for random number generation.

	//====================================================================\\
	//                                                                    \\
	//                         Scalar Funcs                               \\
	//                                                                    \\
	//====================================================================\\

#define DISAMBIGUATE(FUNC)			  			   \
	inline BaseWrap& FUNC(BaseWrap & bw)		   \
	{											   \
		BaseWrap* ret;							   \
		if (bw.m_type == BaseWrap::LIST)		   \
			ret = &Wrap(FUNC((Listable&)(bw)));    \
		else									   \
			ret = &Wrap(FUNC((OME_SCALAR)(bw)));   \
		return *ret;							   \
	}											   \


#define DISAMBIGUATEC(FUNC)			  			   				 \
	inline BaseWrap& FUNC(Evaluable& caller,BaseWrap & bw)		 \
		{											   			 \
		BaseWrap* ret;							   				 \
		if (bw.m_type == BaseWrap::LIST)		   				 \
			ret = &Wrap(FUNC(caller,(Listable&)(bw)));   		 \
				else									   		 \
			ret = &Wrap(FUNC(caller,(OME_SCALAR)(bw)));   		 \
		return *ret;								  			 \
		}														 \


//================= transform_val( x ) ========================
// Adjust the random value generated using a set of parameters; min, max, shift, stretch
// Input: random num (double)		Output: adjusted num (double)
inline void transform_val(OME_SCALAR & num, const OME_SCALAR mn, const OME_SCALAR mx, const OME_SCALAR sht, const OME_SCALAR stch)
{
	num *= stch;
	num += sht;
	if (num < mn){ num = mn; }
	if (num > mx){ num = mx; }
}

//================= abs( x ) ==========================
//FunctionSignatures["abs"]={"abs","@SI",true};
inline OME_SCALAR abs(const OME_SCALAR & arg)
{
	return OMEABS(arg);
}

inline Listable& abs(Listable & arg)
{
	return *NewTempLambdaList(arg.GetSize(), [&arg](const size_t & i){return abs(arg[i]); });
}

//DISAMBIGUATE(abs)

//================= acos( x ) =========================
//FunctionSignatures["acos"]={"acos","@SI",true};
inline OME_SCALAR acos(const OME_SCALAR & arg)
{
	return OMEACOS(arg);
}

inline Listable& acos(Listable& arg)
{
	return *NewTempLambdaList(arg.GetSize(), [&arg](const size_t & i){return acos(arg[i]); });
}

//DISAMBIGUATE(acos)

//================= asin( x ) =========================
//FunctionSignatures["asin"]={"asin","@SI",true};
inline OME_SCALAR asin(const OME_SCALAR & arg)
{
	return OMEASIN(arg);
}

inline Listable& asin(Listable& arg)
{
	return *NewTempLambdaList(arg.GetSize(), [&arg](const size_t & i){return asin(arg[i]); });
}

//DISAMBIGUATE(asin)

//================= at_init( x ) ======================
//FunctionSignatures["at_init"]={"at_init","@EI",true};

/** @todo at_init refactor
	-have serializer move at_init statement to new global variable.
	-run initialization once.
	-replace at_init with variable instance.
*/
inline OME_SCALAR at_init(Evaluable& eval)
{
	return eval.GetParentModel()->GetCurrentInstanceFirstVal(eval.GetModelIndex());
}

inline Listable& at_init(Listable& arg)
{
	/// @todo implement properly.
	/// @todo add equation support
	return *NewTempCopyList(arg);
}

inline BaseWrap& at_init(BaseWrap & bw)
{
	BaseWrap* ret;
	if (bw.m_type == BaseWrap::LIST)
		ret = &Wrap(at_init((Listable&)(bw)));
	else
		ret = &Wrap(at_init((Evaluable&)(bw)));
	return *ret;
}

//================= atan( x ) =========================
//FunctionSignatures["atan"]={"atan","@SI",true};
inline OME_SCALAR atan(const OME_SCALAR& arg)
{
		return OMEATAN(arg);
}

inline Listable& atan(Listable& arg)
{
	return *NewTempLambdaList(arg.GetSize(), [&arg](const size_t & i){return atan(arg[i]); });
}

////DISAMBIGUATE(atan)

//================= atan2( x, y ) ======================
//FunctionSignatures["atan2"]={"atan2","@SI,@SI",true};
inline OME_SCALAR atan2(const OME_SCALAR & arg1, const OME_SCALAR & arg2)
{
		return OMEATAN2(arg1,arg2);
}

inline Listable& atan2(Listable& arg)
{
	/// @question add check for odd number of list entries?
	return *NewTempLambdaList(arg.GetSize() / 2, [&arg](const size_t & i){return OMEATAN2(arg[2*i],arg[(2*i)+1]); });
	
}

//================= gamma_ran(Order) ======================
//NEEDS FUNCTION SIGNATURE
//Produce random number using the gamma distribution.
//Order: Order (alpha value) of the gamma distribution 
inline OME_SCALAR gamma_ran(const OME_SCALAR alpha, const OME_SCALAR beta)
{
	std::gamma_distribution<double> distrib(alpha, beta);
	return distrib(sGen);
}

inline OME_SCALAR gamma_ran(const OME_SCALAR order, const OME_SCALAR stretchVal, const OME_SCALAR minVal, const OME_SCALAR maxVal, const OME_SCALAR shiftVal)
{
	OME_SCALAR rannum = gamma_ran(order, stretchVal);
	transform_val(rannum, minVal, maxVal, shiftVal, stretchVal);
	return rannum;
}


//================= beta_ran(Order) ======================
//sigs["beta_ran"] = FuncSig("beta_ran", "@SI,@SI,@SI,@SI,@SI,@SI", "@sO", FuncSig::NO_FLAGS, SclrOnlyFunc);
//Produce random number using the beta distribution.
//alpha:		beta:
inline OME_SCALAR beta_ran(const OME_SCALAR alpha, const OME_SCALAR beta, const OME_SCALAR minVal, const OME_SCALAR maxVal, const OME_SCALAR shiftVal, const OME_SCALAR stretchVal)
{
	OME_SCALAR X = gamma_ran(alpha, stretchVal);
	OME_SCALAR Y = gamma_ran(beta, stretchVal);

	OME_SCALAR rannum = X / (X + Y);

	transform_val(rannum, minVal, maxVal, shiftVal, stretchVal);
	return rannum;

	//Generating beta-distributed random variates
	//https://en.wikipedia.org/wiki/Beta_distribution#Generating_beta-distributed_random_variates
}


//================= binome( prob, maxdata ) ======================
//FunctionSignatures["binome"]={"binome","@SI,@SI",false};
//Produce random number using the binomial distribution. 
inline OME_SCALAR binome(const OME_SCALAR & prob, const int & maxdata)
{
	if (prob <= 0.0 || prob >= 1.0)
		throw OMEFuncOpException("binome","probability must be in the range of (0,1).");
	if (maxdata<=0.0)
		throw OMEFuncOpException("binome","maxdata must be greater than 0.");
	std::binomial_distribution<int> distrib(maxdata, prob);
	return distrib(sGen);
}

inline OME_SCALAR binome(const OME_SCALAR & prblty, const int & dataPts, const OME_SCALAR minVal, const OME_SCALAR maxVal, const OME_SCALAR shiftVal, const OME_SCALAR stretchVal)
{
	OME_SCALAR rannum = binome(prblty, dataPts);
	transform_val(rannum, minVal, maxVal, shiftVal, stretchVal);
	return rannum;
}



//================= negBinome_ran( prob, maxdata ) ======================
//Produce random number using the binomial distribution. 
inline OME_SCALAR negBinome_ran(const OME_SCALAR & prob, const int & maxdata)
{
	if (prob <= 0.0 || prob >= 1.0)
		throw OMEFuncOpException("negBinome_ran","probability must be in the range of (0,1).");
	if (maxdata <= 0.0)
		throw OMEFuncOpException("negBinome_ran","maxdata must be greater than 0.");
	std::negative_binomial_distribution<int> distrib(maxdata, prob);
	return distrib(sGen);
}

inline OME_SCALAR negBinome_ran(const OME_SCALAR & prblty, const int & reqPts, const OME_SCALAR minVal, const OME_SCALAR maxVal, const OME_SCALAR shiftVal, const OME_SCALAR stretchVal)
{
	OME_SCALAR rannum = negBinome_ran(prblty, reqPts);
	transform_val(rannum, minVal, maxVal, shiftVal, stretchVal);
	return rannum;
}



//================= exponent_ran(lambda) ======================
//NEEDS FUNCTION SIGNATURE, MIGHT NOT WORK VENSIM DESCRIPTION NO LAMBDA
//Produce random number using the exponential distribution.
//Vensim fixes eventRate = 1 while Simile allows for own input
inline OME_SCALAR exponent_ran(const OME_SCALAR lambda)
{
	std::exponential_distribution<double> distrib(lambda);
	return distrib(sGen);
}

inline OME_SCALAR exponent_ran(const OME_SCALAR minVal, const OME_SCALAR maxVal, const OME_SCALAR shiftVal, const OME_SCALAR stretchVal)
{
	OME_SCALAR rannum = exponent_ran(1);
	transform_val(rannum, minVal, maxVal, shiftVal, stretchVal);
	return rannum;
}


//================= poidev( x ) ========================
//Produce random number using the poisson distribution. 
//FunctionSignatures["poidev"]={"poidev","@SI",false};
inline OME_SCALAR poidev(const OME_SCALAR & mean)
{
	std::poisson_distribution<long> distrib(mean);
	return distrib(sGen);
}

inline OME_SCALAR poidev(const OME_SCALAR & mean, const OME_SCALAR minVal, const OME_SCALAR maxVal, const OME_SCALAR shiftVal, const OME_SCALAR stretchVal)
{
	OME_SCALAR rannum = poidev(mean);
	transform_val(rannum, minVal, maxVal, shiftVal, stretchVal);
	return rannum;
}


//================= triangle_ran(st, nd, ht) ======================
//NEEDS FUNCTION SIGNATURE
//Produce random number using the triangular distribution. 
//min st, max nd, peak ht.  
inline OME_SCALAR triangle_ran(const OME_SCALAR st, const OME_SCALAR nd, const OME_SCALAR ht)
{
	if (nd > st || ht < st || ht > nd)
	{
		throw OMEFuncOpException("triangle_ran","Incorrect parameters. Peak must be between min and max");
	}


	double num = rand() / (double)RAND_MAX;
	double Func = (ht - st) / (nd - st);
	if (num > Func)
		return nd - sqrt((1 - num) * (nd - st) * (nd - ht));
	else
		return st + sqrt(num * (nd - st) * (ht - st));
	//return distrib(sGen);

	//Generating Triangular-distributed random variates	
	//https://en.wikipedia.org/wiki/Triangular_distribution  
}



inline OME_SCALAR triangle_ran(const OME_SCALAR st, const OME_SCALAR nd, const OME_SCALAR ht, const OME_SCALAR minVal, const OME_SCALAR maxVal)
{
	OME_SCALAR rannum = triangle_ran(st, nd, ht);
	transform_val(rannum, minVal, maxVal, 0, 1);
	return rannum;
}



//================= weib_ran(Order) ======================
//NEEDS FUNCTION SIGNATURE
//Produce random number using the weibull distribution.
//Vensim fixes eventRate = 1 while Simile allows for own input
inline OME_SCALAR weib_ran(const OME_SCALAR a, const OME_SCALAR b)
{
	std::weibull_distribution<double> distrib(a, b);
	return distrib(sGen);
}

inline OME_SCALAR weib_ran(const OME_SCALAR shape, const OME_SCALAR stretchVal, const OME_SCALAR minVal, const OME_SCALAR maxVal, const OME_SCALAR shiftVal)
{
	OME_SCALAR rannum = weib_ran(shape, stretchVal);
	transform_val(rannum, minVal, maxVal, shiftVal, stretchVal);
	return rannum;
}


//================= gaussian_var( x, y ) ===============
//FunctionSignatures["gaussian_var"]={"gaussian_var","@SI,@SI",false};
inline OME_SCALAR gaussian_var(const OME_SCALAR & arg1, const OME_SCALAR & arg2)
{
	std::normal_distribution<OME_SCALAR> distrib(arg1, arg2);
	return distrib(sGen);
}

inline OME_SCALAR gaussian_var(const OME_SCALAR & shift, const OME_SCALAR & stretch, const OME_SCALAR & minVal, const OME_SCALAR & maxVal)
{
	OME_SCALAR rawVal;
	do
	{
		rawVal = gaussian_var(shift, stretch);
	} while (rawVal<minVal || rawVal>maxVal);

	return rawVal;
}


//================= ceil( x ) ==========================
//FunctionSignatures["ceil"]={"ceil","@SI",true};
inline OME_SCALAR ceil(const OME_SCALAR & arg)
{
	int intCast;
	double v=(arg);
	intCast=(int)v;
	if(v>=0.0 && v > intCast)
			intCast+=1;
	return (OME_SCALAR)intCast;

}

inline Listable& ceil(Listable& arg)
{
	return *NewTempLambdaList(arg.GetSize(), [&arg](const size_t & i){return ceil(arg[i]); });
}

//DISAMBIGUATE(ceil)

//================= channel_is( x )=====================
//FunctionSignatures["channel_is"]={"channel_is","@EC,@SP",false};
inline bool channel_is(const Evaluable& caller, Spawner& spwn)
{
	return caller.GetParentModel()->GetCurrentInstanceRef()->IsFromSpawner(&spwn);
}

//================= const_delay( x, t )=================
//FunctionSignatures["const_delay"]={"const_delay","@EC,@SI",false};
inline OME_SCALAR const_delay(BaseWrap & bw, const OME_SCALAR & offset)
{
	Evaluable* pE = bw.m_pEval;
	Model* pMdl = pE->GetParentModel();
	
		return pMdl->GetCurrentInstanceBackVal(pE->GetModelIndex(), offset);
}

inline OME_SCALAR const_delay(AliasWrap & aw, const OME_SCALAR & offset)
{
	Evaluable* pE = aw.m_pEval;
	Model* pMdl = pE->GetParentModel();

	return pMdl->GetCurrentInstanceBackVal(pE->GetModelIndex() + aw.m_ind, offset);
}

inline OME_SCALAR const_delay(Evaluable& eval, const OME_SCALAR & offset)
{
	Model* pMdl=eval.GetParentModel();
	return pMdl->GetCurrentInstanceBackVal(eval.GetModelIndex(), offset);
}

inline OME_SCALAR const_delay(VarArray& eval, OME_SCALAR* addr, const OME_SCALAR & offset)
{
	//this is a non-optimal solution, but it should work for now.
	size_t index=0;
	for (; index < eval.GetSize() && &(eval[index])!=addr; index++);

	if (index >= eval.GetSize())
		throw OMEObjException("address for value not found.", &eval);

	Model* pMdl = eval.GetParentModel();
	
	return pMdl->GetCurrentInstanceBackVal(eval.GetModelIndex()+index, offset,eval.GetSize());
}

inline OME_SCALAR const_delay(const SubIndexVal<ValueArchive&> & siv, const OME_SCALAR & offset)
{
	return siv.GetPrev(offset);
}

//inline OME_SCALAR const_delay(BaseWrap & bw, const size_t & ind,const OME_SCALAR & offset)
//{
//	Evaluable* pE = bw.m_pEval;
//	Model* pMdl = pE->GetParentModel();
//	if (!bw.m_type == BaseWrap::ALIAS)
//		return pMdl->GetCurrentInstanceBackVal(pE->GetModelIndex() + ind - 1, offset, ((VarArray*)pE)->GetSize());
//	else
//		return pMdl->GetCurrentInstanceBackVal(pE->GetModelIndex() + ((AliasWrap&)bw).m_ind+ind - 1, offset, ((VarArray*)pE)->GetSize());
//}


inline OME_SCALAR const_delay(VarArray& eval, const size_t & ind, const OME_SCALAR & offset)
{
	Model* pMdl = eval.GetParentModel();

	return pMdl->GetCurrentInstanceBackVal(eval.GetModelIndex() + ind-1, offset, eval.GetSize());
}



//================= cos( x ) ===========================
//FunctionSignatures["cos"]={"cos","@SI",true};
inline OME_SCALAR cos(const OME_SCALAR & arg)
{
		return OMECOS(arg);
}

inline Listable& cos(Listable& arg)
{
	return *NewTempLambdaList(arg.GetSize(), [&arg](const size_t & i){return cos(arg[i]); });
}

//DISAMBIGUATE(cos)

//================= cosh( x ) ==========================
//FunctionSignatures["cosh"]={"cosh","@SI",true};
inline OME_SCALAR cosh(const OME_SCALAR & arg)
{
		return OMECOSH(arg);
}

inline Listable& cosh(Listable& arg)
{
	return *NewTempLambdaList(arg.GetSize(), [&arg](const size_t & i){return cosh(arg[i]); });
}

//DISAMBIGUATE(cosh)

//================= dies_of( x ) =======================
//FunctionSignatures["dies_of"]={"dies_of","@EC,@SP",true};
inline bool dies_of(Evaluable& caller, Spawner& spwn)
{
	return caller.GetParentModel()->IsMarkedBySpawner(&spwn);
}

/// @question do we need list form of dies_of?
//below needed?
//inline Listable& dies_of(Evaluable* caller, Listable & il)
//{
//	return *NewTempLambdaList(arg.GetSize(), [&caller,&il](const size_t & i){return atan(arg[i]); });
//	Listable& out(il.GetSize());
//	for (WAItr itr = out.Begin(); itr.IsValid(); ++itr)
//		new(*itr)SclrWrapper(dies_of(caller, (Spawner*)&(*itr)->ToEval()));
//
//	return out;
//}

//================= dt( x ) ============================
//FunctionSignatures["dt"]={"dt","@CI,@SI",true};
inline OME_SCALAR dt(Evaluable& caller, const OME_SCALAR & arg=0.0)
{
	//for now, ignore arg; this will need to change if variable timesteps are introduced
	return caller.GetParentModel()->GetStepSize();
}

inline Listable& dt(Evaluable& caller, Listable & il)
{
	return *NewTempLambdaList(il.GetSize(), [&caller,&il](const size_t & i){return dt(caller,il[i]); });
}

//DISAMBIGUATEC(dt)

//================= end_time() =========================
//FunctionSignatures["end_time"]={"end_time","@CI,@SI",true};
inline OME_SCALAR end_time(BaseManager* pBm)
{
	return pBm->GetStopTime();
}


//================= exp( x ) ===========================
//FunctionSignatures["exp"]={"exp",",@SI",true};
inline OME_SCALAR exp(const OME_SCALAR& arg)
{
	return OMEEXP(arg);
}

inline Listable& exp(Listable& arg)
{
	return *NewTempLambdaList(arg.GetSize(), [&arg](const size_t & i){return OMEEXP(arg[i]); });
}

//DISAMBIGUATE(exp);

//================= factorial( x ) =====================
//FunctionSignatures["factorial"]={"factorial",@SI",true};
inline OME_SCALAR factorial(const OME_SCALAR & arg)
{
	int f = (int)arg;
	OME_SCALAR result=1.0;

	for(int i=2; i<=f; i++)
		result*=i;
	return (OME_SCALAR)result;
}

inline Listable& factorial(Listable & il)
{
	return *NewTempLambdaList(il.GetSize(), [&il](const size_t & i){return factorial(il[i]); });
}

//DISAMBIGUATE(factorial)

//================= first( x ) =========================
//FunctionSignatures["first"]={"first","@NI",true};
inline bool first(const EnumEntry* arg)
{
	return arg->ind == 1;
}

//FunctionSignatures["first"]={"first","@NI",true};
//inline bool first(const BaseWrap & arg)
//{
//	return (OME_SCALAR)arg == 1;
//}



//================= floor( x ) =========================
//FunctionSignatures["floor"]={"floor","@SI",true};
inline OME_SCALAR floor(const OME_SCALAR & arg)
{
	OME_SCALAR d = arg;
	int intCast = (int)d;
	if(d < 0.0 && d < intCast)
		intCast-=1;
	return (OME_SCALAR)intCast;
}

inline Listable& floor(Listable& arg)
{
	return *NewTempLambdaList(arg.GetSize(), [&arg](const size_t & i){return floor(arg[i]); });
}

//DISAMBIGUATE(floor)

//================= fmod( x, y ) =======================
//FunctionSignatures["fmod"]={"::fmod","@SI,@SI",false};
//inline OME_SCALAR fmod(const OME_SCALAR & arg1, const OME_SCALAR & arg2)
//{
//	return ::fmod(arg1,arg2);
//}

//================= following( x ) =====================
//FunctionSignatures["following"]={"following","@NI",true};
inline EnumEntry following(const EnumEntry & arg)
{
	//for now, return 0 if arg is the last entry
	if (arg.ind != arg.pCollection->GetEnumValueCount())
		return arg.pCollection->GetEnumEntry(arg.ind + 1);
	else
		return EnumEntry::NO_ENTRY;
}

inline Listable& following(Listable& arg)
{
	//cheap way to handle lists;
	/// @todo come up with better representation of enum types list
	Listable* fList = NewTempLambdaList(arg.GetSize(), [&arg](const size_t & i){return arg[i] + 1; });
	return *fList;
}


//================= graph( x ) =========================
//FunctionSignatures["graph"]={"interpTable","@CI,@BM,@SI",false};
inline OME_SCALAR interpTable(Evaluable& caller, BaseManager* pBm, const OME_SCALAR & inVal,const size_t & index=0)
{
	OME_SCALAR out = 0.0;
	if (!caller.GetInterpTableValue(inVal, out,index))
		throw OMEFuncOpException("graph", (caller.GetName() + ": No graph data").c_str());
		//pBm->ReportError(caller.GetName() + ": Bad Graph Call");
	return out;
}

inline OME_SCALAR interpTable(Evaluable& caller, BaseManager* pBm, Evaluable& pEval,const size_t & index=0)
{
	return interpTable(caller, pBm, pEval.GetValue(),index);
}


//================= hypergeom( x, y, z ) ===============
//FunctionSignatures["hypergeom"]={"hypergeom","@SI,@SI,@SI",false};

inline OME_SCALAR HGChoose(const unsigned int & n, const unsigned int & k)
{
	/// \optimization use more efficient binomial coefficient function
	OME_SCALAR ret=1.0;

	if(k>0)
	{
		for(int i=1; i<=k; i++)
			ret*=(n-(k-i))/(OME_SCALAR)i;
			
	}
	return ret;
}

inline int hypergeom(const OME_SCALAR & arg1, const OME_SCALAR & arg2, const OME_SCALAR & arg3)
{
	std::uniform_real_distribution<OME_SCALAR> distrib(0.0,1.0);
	OME_SCALAR randInput=distrib(sGen);

	int pop=(int)arg1;
	int mark=(int)arg2;
	int sample=(int)arg3;
	//iterate through possible combinations of successful picks
	//compare sum of pick probs to rand uniform number
	//select successful picks if current sum of probs >= rand uniform number
	/// \optimization use alternate hypergeometric distribution implementation
	unsigned int upper=min(mark,sample);
	int result=-1;
	OME_SCALAR limit=0.0;
	int i;
	for(i=0; i <= upper && result < 0; i++)
	{
		limit+=(HGChoose(mark,i)*HGChoose(pop-mark,sample-i))/HGChoose(pop,sample);
		if(randInput<=limit)
			result=i;
	}
	if(result < 0)
		result=upper;
		
	return result;
}

//================= hypot( x, y ) ======================
//FunctionSignatures["hypot"]={"hypot","@SI,@SI",false};
inline OME_SCALAR hypot(const OME_SCALAR & x, const OME_SCALAR & y)
{
	return OMESQRT(OMEPOW(x,2)+OMEPOW(y,2));
}

//================= init_time( I ) =====================
//FunctionSignatures["init_time"]={"init_time","@CI,@SI",true};
inline OME_SCALAR init_time(Evaluable& caller)
{
	return caller.GetParentModel()->GetManager()->GetStartTime();
}

//inline Listable& init_time(Evaluable& caller, Listable & lst)
//{
//	return *NewTempLambdaList(lst.GetSize(), [&caller](const size_t & i){return caller.GetParentModel()->GetInitTime(); });
//}

//DISAMBIGUATEC(init_time)

//================= in_preceding( x ) ==================
//FunctionSignatures["in_preceding"]={"in_preceding","@CI,@TI",true};
//inline BaseWrap& in_preceding(Evaluable& caller, const STLString & varName)
//{
//	/// @todo update so second argument is function, not string
//	/// @todo update to use wrappers
//
//	Model* pMdl=caller.GetParentModel();
//	size_t actInst=pMdl->GetCurrentInstance();
//	SclrWrap ret(0.0);
//	//if(actInst>0)
//	//{
//	//		ModelInstance* trg=pMdl->GetInstance((unsigned int)actInst-1);
//	//		if(trg)
//	//		{
//
//	//			Evaluable* eval=dynamic_cast<Evaluable*>(pMdl->ObjectForKey(varName));
//	//			if(eval)
//	//				ret=trg->ValForQuery(eval);
//	//		}
//	//}
//	//return ret;
//    return Wrap(0.0);
//}

//inline Listable& in_preceding(Evaluable & caller, Listable& & wa)
//{
//	Listable& out(wa.GetSize());
//	//for(size_t i=0; i<out.GetSize();i++)
//	//	out[i](in_preceding(pCaller,*wa[i]));
//	
//	/// \todo update for grouping of texts
//	return out;
//}

//================= in_progenitor( x ) =================
//FunctionSignatures["in_progenitor"]={"in_progenitor","@CI,@TI",true};
//inline BaseWrap& in_progenitor(Evaluable & caller, const STLString & varName)
//{	
//		Model* pMdl=caller.GetParentModel();
//	 
//		/// @todo update so second argument is function, not string
//		/// @todo update to use wrappers
//
//		/*size_t actInst=pMdl->GetCurrentInstance();
//		if(actInst>0)
//		{
//			ModelCollection* pCollection=dynamic_cast<ModelCollection*>(pMdl->GetSpawningParent());
//			if(pCollection)
//			{
//				Model* trg=pCollection->GetInstance(pMdl->m_instanceParent);
//				
//				if(pMdl)
//				{
//					Evaluable* eval=dynamic_cast<Evaluable*>(trg->ObjectForKey(varName));
//					if(eval)
//						ret=eval->GetValue();
//				}
//			}
//		}*/
//		
//		return Wrap(0.0);
//}

//inline Listable& in_progenitor(Evaluable & caller, Listable& & wa)
//{
//	Listable& out(wa.GetSize());
//	//for(size_t i=0; i<out.GetSize();i++)
//	//	out[i](in_preceding(pCaller,*wa[i]));
//	
//	/// \todo update for grouping of texts
//	return out;
//}

//================= index( x ) =========================
//FunctionSignatures["index"]={"index","@CI,@SI",true};
inline OME_SCALAR index(Evaluable& caller, unsigned int depth)
{
	Model* pMdl= caller.GetParentModel();
	if (pMdl == NULL)
		throw OMEFuncOpException("index", (caller.GetExprName() + " has no parent model").c_str());
	//step up to the appropriate depth
	for(int i=depth; i>1 && pMdl; i--,pMdl=pMdl->GetParentModel());
			
	//grab instance index
	return (OME_SCALAR)pMdl->GetCurrentInstance()+1;
}

inline Listable& index(Evaluable& caller, Listable & lst)
{
	return *NewTempLambdaList(lst.GetSize(), [&lst, &caller](const size_t & i){return index(caller, (unsigned int)lst[i]); });
}

//DISAMBIGUATEC(index)

//================= int( x ) ===========================
//FunctionSignatures["int"]={"toInt","@SI",true};
inline OME_SCALAR toInt(const OME_SCALAR & sclr)
{
	return (int)sclr;
}

inline Listable& toInt(Listable& lst)
{
	return *NewTempLambdaList(lst.GetSize(), [&lst](const size_t & i){return (int)lst[i]; });
}

//DISAMBIGUATE(toInt)

//================= last( x ) ==========================
//FunctionSignatures["last"]={"last","@SI,@BM",true};
//inline OME_SCALAR last(Evaluable & eval)
//{
//	return OMECFuncs::const_delay(eval,POOL(1.0));
//}

inline OME_SCALAR last(BaseManager* pBm, BaseWrap & bw)
{
	Evaluable* pE=bw.m_pEval;
	
		return pE->GetParentModel()->GetCurrentInstanceLastVal(pE->GetModelIndex(), !pE->IsOMEType(OME_VARARRAY) ? 1 : ((VarArray*)pE)->GetSize());
}

inline OME_SCALAR last(BaseManager* pBm, AliasWrap & aw)
{
	Evaluable* pE = aw.m_pEval;
	return pE->GetParentModel()->GetCurrentInstanceLastVal(pE->GetModelIndex() + aw.m_ind, !pE->IsOMEType(OME_VARARRAY) ? 1 : ((VarArray*)pE)->GetSize());
}

inline OME_SCALAR last(BaseManager* pBm,Evaluable& pE)
{
	//if (pE->GetOMEType() != OME_VARARRAY)
		return pE.GetParentModel()->GetCurrentInstanceLastVal(pE.GetModelIndex(), !pE.IsOMEType(OME_VARARRAY) ? 1 : ((VarArray*)&pE)->GetSize());
}

inline OME_SCALAR last(BaseManager* pBm, const SubIndexVal<ValueArchive&> & siv)
{
	return siv.GetPrev();
}

//================= log( x ) ===========================
//FunctionSignatures["log"]={"log","@SI",true};
inline OME_SCALAR log(const OME_SCALAR & arg)
{
	return OMELOG(arg);
}

inline OME_SCALAR log(const OME_SCALAR & arg,const OME_SCALAR & base)
{
	return OMELOG10(arg)/OMELOG10(base);
}


inline Listable& log(Listable& lst)
{
	return *NewTempLambdaList(lst.GetSize(), [&lst](const size_t & i){return log(lst[i]); });
}

//DISAMBIGUATE(log)

//================= log10( x ) =========================
//FunctionSignatures["log10"]={"log10","@SI",true};
inline OME_SCALAR log10(const OME_SCALAR & arg)
{
	return OMELOG10(arg);
}

inline Listable& log10(Listable &lst)
{
	return *NewTempLambdaList(lst.GetSize(), [&lst](const size_t & i){return log10(lst[i]); });
}

//DISAMBIGUATE(log10)

//================= max( x, y ) ========================
//FunctionSignatures["max"]={"OMEMax","@SI,@SI",false};
inline OME_SCALAR OMEMax(const OME_SCALAR & arg1, const OME_SCALAR & arg2)
{
	return ::max(arg1,arg2);
}

//================= min( x, y ) ========================
//FunctionSignatures["min"]={"OMEMin","@SI,@SI",false};
inline OME_SCALAR OMEMin(const OME_SCALAR & arg1, const OME_SCALAR & arg2)
{
	return ::min(arg1,arg2);
}

//================= parent() ===========================
//FunctionSignatures["parent"]={"parent","@CI",false};
inline OME_SCALAR parent(Evaluable& caller)
{
	size_t ret=0;
	if (caller.GetParentModel()->GetCurrentInstanceRef()->GetParentInstance(ret))
		ret++; //move to 1-based index
	return ret;
}


//================= pow( x, y ) ========================
//FunctionSignatures["pow"]={"pow","@SI,@SI",false};
inline OME_SCALAR pow(const OME_SCALAR & arg1, const OME_SCALAR & arg2)
{
	return OMEPOW(arg1,arg2);
}

inline Listable& pow(const OME_SCALAR & arg1, Listable & arg2)
{
	return *NewTempLambdaList(arg2.GetSize(), [&arg1,&arg2](const size_t & i){return pow(arg1,arg2[i]); });
}

inline Listable& pow(Listable & arg1, const OME_SCALAR & arg2)
{
	return *NewTempLambdaList(arg1.GetSize(), [&arg1, &arg2](const size_t & i){return pow(arg1[i], arg2); });
}

//inline BaseWrap& pow(BaseWrap & arg1, BaseWrap & arg2)
//{
//	BaseWrap* ret=NULL;
//	enum { SS=0,LS,SL,LL};
//
//	unsigned short mode=0;
//	if (arg1.m_type == BaseWrap::LIST)
//		mode += 1;
//	if (arg2.m_type == BaseWrap::LIST)
//		mode += 2;
//
//	switch (mode)
//	{
//	case LL:
//		//throw error
//		break;
//	case SL:
//		ret = &Wrap(pow((OME_SCALAR)arg1, (Listable&)arg2));
//		break;
//	case LS:
//		ret = &Wrap(pow((Listable&)arg1, (OME_SCALAR)arg2));
//		break;
//	case SS:
//		ret = &Wrap(pow((OME_SCALAR)arg1, (OME_SCALAR)arg2));
//	}
//
//	return *ret;
//}

//================= preceding( T ) =====================
//FunctionSignatures["preceding"]={"preceding","@NI",true};
inline EnumEntry preceding(const EnumEntry& arg)
{
	if (arg.ind > 1)
		return arg.pCollection->GetEnumEntry(arg.ind - 1);
	else
		return EnumEntry::NO_ENTRY;
}

inline BaseWrap& preceding(Listable& arg)
{
	//cheap way to handle lists;
	/// @todo come up with better representation of enum types list
	Listable* fList = NewTempLambdaList(arg.GetSize(), [&arg](const size_t & i){return arg[i] - 1.0; });
	return Wrap(*fList);
}

//================= prev( x ) ==========================
//FunctionSignatures["prev"]={"prev","@CI,@SI",false};
inline OME_SCALAR prev(Evaluable& caller, const OME_SCALAR & stepBack)
{
	return caller.GetParentModel()->GetCurrentInstanceBackVal(caller.GetModelIndex(), stepBack);
}

inline Listable* prev(VarArray& caller, const OME_SCALAR & stepBack)
{
	Listable* ret = NewTempCopyList(caller);
	
	for (size_t i = 0; i < caller.GetSize();++i)
		caller[i]=caller.GetParentModel()->GetCurrentInstanceBackVal(caller.GetModelIndex()+i, stepBack,caller.GetSize());
	return ret;
}

inline OME_SCALAR prev(const SubIndexVal<ValueArchive&> & siv, const OME_SCALAR & stepBack)
{
	return siv.GetPrev(stepBack);
}

//================= pulse( x, y,z ) ====================
inline OME_SCALAR pulse(const OME_SCALAR & mag, const OME_SCALAR & startTime, const OME_SCALAR & periodWidth, BaseManager* pBm)
{
	OME_SCALAR time = pBm->GetPresentTime();
	if (time >= startTime && time < (startTime + periodWidth))
		return 1.0;
	//if (interval && time>= startTime && OMEFMOD(time - startTime, interval) == 0)
	//	return mag / pBm->GetStepSize();
	
	return 0.0;
}

inline OME_SCALAR pulse(const OME_SCALAR & mag, const OME_SCALAR & startTime, BaseManager* pBm)
{
	return pulse(mag, startTime, 0, pBm);
}

//================= ramp( x, y ) ======================
inline OME_SCALAR ramp(const OME_SCALAR & slope, const OME_SCALAR & startTime, BaseManager* pBm)
{
	OME_SCALAR time = pBm->GetPresentTime();

	OME_SCALAR ret = 0.0;
	if (time > startTime)
		ret = slope*(time - startTime);
	return ret;
}


//================= rand_var( x, y ) ===================
//FunctionSignatures["rand_var"]={"rand_var","@SI,@SI",false};
inline OME_SCALAR rand_var(const OME_SCALAR & low, const OME_SCALAR & high)
{
		std::uniform_real_distribution<OME_SCALAR> distrib(low, high);
		return distrib(sGen);
}

//================= rand_const( x, y ) =================
//FunctionSignatures["rand_const"]={"rand_const","@SV,@SI,@SI",false};
inline OME_SCALAR rand_const(const OME_SCALAR & low, const OME_SCALAR & high)
{
	/// @todo have rand_const be replaced by at_init(rand_var(low,high))
	return rand_var(low, high);
}

//================= round( x ) =========================
//FunctionSignatures["round"]={"round","@SI",true};
inline OME_SCALAR round(const OME_SCALAR & arg)
{
	//classic float rounding trick
	return (int)((arg)+0.5);
	
}

inline Listable& round(Listable& lst)
{
	return *NewTempLambdaList(lst.GetSize(), [&lst](const size_t & i){return round(lst[i]); });
}

//DISAMBIGUATE(round)

//================= sgn( x ) ===========================
//FunctionSignatures["sgn"]={"sgn","@SI",true};
inline OME_SCALAR sgn(const OME_SCALAR & arg)
{
	return arg >=0.0 ? 1.0 : -1.0;
}

inline Listable& sgn(Listable& lst)
{
	return *NewTempLambdaList(lst.GetSize(), [&lst](const size_t & i){return sgn(lst[i]); });
}

//DISAMBIGUATE(sgn)

//================= sin( x ) ===========================
//FunctionSignatures["sin"]={"sin","@SI",true};
inline OME_SCALAR sin(const OME_SCALAR& arg)
{
	return OMESIN(arg);
}

inline Listable& sin(Listable& lst)
{
	return *NewTempLambdaList(lst.GetSize(), [&lst](const size_t & i){return sin(lst[i]); });
}

//DISAMBIGUATE(sin)

//================= sinh( x ) ==========================
//FunctionSignatures["sinh"]={"sinh","@SI",true};
inline OME_SCALAR sinh(const OME_SCALAR& arg)
{
	return OMESINH(arg);
}

inline Listable& sinh(Listable& lst)
{
	return *NewTempLambdaList(lst.GetSize(), [&lst](const size_t & i){return sinh(lst[i]); });
}

//DISAMBIGUATE(sinh)

//================= sqrt( x ) ==========================
//FunctionSignatures["sqrt"]={"sqrt","@SI",true};
inline OME_SCALAR sqrt(const OME_SCALAR& arg)
{
	return OMESQRT(arg);
}

inline Listable& sqrt(Listable& lst)
{
	return *NewTempLambdaList(lst.GetSize(), [&lst](const size_t & i){return sqrt(lst[i]); });
}

//DISAMBIGUATE(sqrt)

//================= step( x, y ) =======================
inline OME_SCALAR step(const OME_SCALAR & height, const OME_SCALAR & startTime, BaseManager* pBm)
{
	return pBm->GetPresentTime() >= startTime ? height : 0.0;
}

//================= stop( x ) ==========================
//FunctionSignatures["stop"]={"stop","@SI,@BM",false};
inline OME_SCALAR stop(const OME_SCALAR & arg, BaseManager* pBm)
{
	ostringstream errStrm;
		errStrm<<"User stop tripped. Error code: "<<arg;
	pBm->ReportError(errStrm.str());
    return arg;
}

//================= table( x ) =========================
//FunctionSignatures["table"]={"table","@CI,@BM,@SI,@B?",false};
inline OME_SCALAR table(Evaluable& caller,BaseManager* pBm,const unsigned int & y, const unsigned int & x,const size_t & index=0)
{
	unsigned int coords[2];
	coords[0]=x-1;
	coords[1]=y-1;

	OME_SCALAR out=0.0;
	if(!caller.GetTableValue(out,coords,2,index))
		pBm->ReportError(caller.GetName()+": Table index out of bounds");
	return out;
}

inline OME_SCALAR table(Evaluable& caller, BaseManager* pBm, unsigned int x,const size_t & index=0)
{
	x-=1;
	OME_SCALAR out = 0.0;
	if (!caller.GetTableValue(out, &x, 1,index))
		pBm->ReportError(caller.GetName() + ": Table index out of bounds");
	return out;
}

//================= tan( x ) ===========================
//FunctionSignatures["tan"]={"tan","@SI",true};
inline OME_SCALAR tan(const OME_SCALAR& arg)
{
	return OMETAN(arg);
}

inline Listable& tan(Listable & lst)
{
	return *NewTempLambdaList(lst.GetSize(), [&lst](const size_t & i){return tan(lst[i]); });
}

//DISAMBIGUATE(tan)

//================= tanh( x ) ==========================
//FunctionSignatures["tanh"]={"tanh","@SI",true};
inline OME_SCALAR tanh(const OME_SCALAR& arg)
{
	return OMETANH(arg);
}

inline Listable& tanh(Listable & lst)
{
	return *NewTempLambdaList(lst.GetSize(), [&lst](const size_t & i){return tanh(lst[i]); });
}

//DISAMBIGUATE(tanh)
//================= time() =============================
//FunctionSignatures["time"]={"@BM->GetPresentTime()","",false};
inline OME_SCALAR currenttime(BaseManager* pBm)
{
	return pBm->GetPresentTime();
}

inline OME_SCALAR currenttime(BaseManager* pBm, const OME_SCALAR & step)
{
	OME_SCALAR rawTime = currenttime(pBm);
	return rawTime;
	//simile says that it binds to nearest interval, but doesn't seem to in practice.
	//for now, disable bounding
	//OME_SCALAR upper;
	//for (upper = pBm->GetStartTime(); upper <= rawTime; upper += step);
	//OME_SCALAR lower = upper - step;

	////round to nearest step.
	//return upper - rawTime < rawTime - lower ? upper : lower;
}

//================= var_delay( x, t ) ==================
//FunctionSignatures["var_delay"]={"var_delay","@EI,@SIin",false};
inline OME_SCALAR var_delay(BaseWrap & bw, const OME_SCALAR & offset)
{
	Evaluable* pE=bw.m_pEval;
	Model* pMdl = pE->GetParentModel();

	if (bw.m_type!=BaseWrap::ALIAS)
		return pMdl->GetCurrentInstanceBackVal(pE->GetModelIndex(), offset);
	else
		return pMdl->GetCurrentInstanceBackVal(pE->GetModelIndex()+((AliasWrap&)bw).m_ind, offset);
}


inline OME_SCALAR var_delay(Evaluable& eval, const OME_SCALAR & offset)
{
	Model* pMdl = eval.GetParentModel();
	return pMdl->GetCurrentInstanceBackVal(eval.GetModelIndex(), offset);
}

inline OME_SCALAR var_delay(const SubIndexVal<ValueArchive&> & siv, const OME_SCALAR & offset)
{
	return siv.GetPrev(offset);
}

inline OME_SCALAR var_delay_withInit(BaseManager* pBm, Evaluable & caller,Evaluable & input, OME_SCALAR delay, const OME_SCALAR & initVal)
{
	if (delay < pBm->GetStepSize())
		delay = pBm->GetStepSize();
	return pBm->GetPresentTime() >= delay ? var_delay(input, delay) : (OME_SCALAR)caller;
}

inline OME_SCALAR lookupInit_var_delay_withInit(BaseManager* pBm, Evaluable & caller, const Evaluable & input, const OME_SCALAR & delay, const OME_SCALAR & initVal)
{
	return initVal;
}

//====================================================================\\
//                                                                    \\
//                         Array Funcs                                \\
//                                                                    \\
//====================================================================\\


//================= all([X]) ===========================
//FunctionSignatures["all"]={"all","@LI",false};
inline OME_SCALAR allSclr(Listable & list)
{
	return !list.TestAny(0.0);
}

inline void allAccumInit(bool & initVal)
{
	initVal = true;
}
inline void allAccum(const int & val,bool & accum)
{
	accum = accum && val;
}

inline Listable& all(Listable & list)
{

		//recurse down lists
		Listable* ret = list.SubAggregate([](Listable* pDest, Listable* pSrc, const size_t & dInd, const size_t & sInd,const size_t & rInd)
		{ (*pDest)[dInd] = (*pDest)[dInd] && (*pSrc)[sInd] != 0.0; }, 1.0);
		
	return *ret;
}

//================= any([X]) ===========================
//FunctionSignatures["any"]={"any","@LI",false};
inline OME_SCALAR anySclr(Listable & list)
{
	return !list.TestAll(0.0);
}

inline void anyAccumInit(bool & initVal)
{
	initVal = false;
}
inline void anyAccum(const int & val, bool & accum)
{
	accum = accum || val;
}


inline Listable& any(Listable & list)
{
	
		Listable* ret = list.SubAggregate([](Listable* pDest, Listable* pSrc, const size_t & dInd, const size_t & sInd,const size_t & rInd)
		{ (*pDest)[dInd] = (*pDest)[dInd] || (*pSrc)[sInd] != 0.0; }, 0.0);

	
	return *ret;
}

//================= colin([X]) =========================
//FunctionSignatures["colin"]={"colin","@LI",false};
inline unsigned int colin(Listable & list)
{
	unsigned ret = 0;
	OME_SCALAR sum = list.GetSum();

	std::uniform_real_distribution<OME_SCALAR> distrib(0.0,1.0);
	OME_SCALAR rand=distrib(sGen);
	
	//find the corresponding index
	OME_SCALAR agg = 0.0;
	int j;
	for (j = 0; j < list.GetSize() && !ret; j++)
	{
		agg += list[j]/sum;
		if(rand<agg)
			ret=j+1;
	}
	//just in case rounding errors mess with the last item
	if(!ret)
		ret=j;
	
	return ret;
}

//================= count([X]) =========================
//FunctionSignatures["count"]={"count","@SI",false};
//inline OME_SCALAR count(const OME_SCALAR & val)
//{
//	return 1;
//}

inline OME_SCALAR count(Listable & list)
{
	return list.GetSize() / list.GetLevel();
}

//================= element([X],I) =====================
//FunctionSignatures["element"]={"element","@LI,@SI",false};

inline OME_SCALAR& elementSclr(VarArray & list, const size_t & ind)
{

	return list[ind-1];
}

inline OME_SCALAR& elementSclr(Evaluable & ev, const size_t & ind)
{
	OME_SCALAR ret;
	size_t inst = ev.GetParentModel()->GetCurrentInstance();
	ev.GetParentModel()->SetCurrentInstance(ind - 1);
	ret = ev.GetValue();
	ev.GetParentModel()->SetCurrentInstance(inst);
	return ret;
}

//primitive array access
inline const OME_SCALAR& elementSclr(const OME_SCALAR* arr, const size_t & ind)
{
	return arr[ind-1];
}


inline Listable& element(Listable & list, const size_t & ind)
{

	return list.RefSubsetFromStep(ind-1);

}

inline Listable& element(Listable & list, Evaluable& eval)
{
	return element(list, eval.GetValue());
}

inline Listable& element(Listable & list, Listable & inds)
{
	return *NewTempCopyList(list,inds);	
}

inline Listable& elementSclr(Listable & list, Listable & inds)
{
	return element(list, inds);
}

inline SubIndexVal<ValueArchive&> element(Evaluable & eval, const size_t & ind,const size_t & depth)
{
	return SubIndexVal<ValueArchive&>(eval, depth).Sub(ind - 1);
}

inline SubIndexVal<ValueArchive&> elementRaw(Evaluable & eval, const size_t & ind, const size_t & depth)
{
	return SubIndexVal<ValueArchive&>(eval, depth,true).Sub(ind - 1);
}


inline SubIndexVal<OME_SCALAR*> element(OME_SCALAR* tmpList, size_t* dims, const size_t & ind, const size_t & depth)
{
	return SubIndexVal<OME_SCALAR*>(tmpList, dims, depth).Sub(ind - 1);
}

template<class TSRC>
inline SubIndexVal<TSRC> element(const SubIndexVal<TSRC> & siv, const size_t & ind)
{
	return siv.Sub(ind - 1);
}


//================= firsttrue([B]) =====================
//FunctionSignatures["firsttrue"]={"firsttrue","@LI",false};

inline void firsttrueAccumInit(int & initVal)
{
	initVal = 0;
}
inline void firsttrueAccum(Listable & list, const unsigned int & ind, int & accum)
{
	if (!accum && list[ind])
		accum = ind+1;
}

inline OME_SCALAR firsttrueSclr(Listable & list)
{
	size_t ret = 0;
	for (auto itr = list.SclrBegin(); itr.IsValid() && !ret; ++itr)
	{
		if (*itr)
			ret = itr.GetPos() + 1;
	}
	
	return ret;
}

inline Listable& firsttrue(Listable & list)
{

	
		Listable* ret = list.SubAggregate([](Listable* pDest, Listable* pSrc, const size_t & dInd, const size_t & sInd, const size_t & rInd)
		{ if (!(*pDest)[dInd] && (*pSrc)[sInd] != 0.0)
		(*pDest)[dInd] = rInd + 1; }, 0.0);

	return *ret;
}

//================= greatest([X]) ======================
//FunctionSignatures["greatest"]={"greatest","@LI",false};
inline OME_SCALAR greatestSclr(Listable & list)
{
	return list.GetMax();

}

inline void greatestAccumInit(OME_SCALAR & initVal)
{
	initVal = OME_SCALAR_MIN;
}
inline void greatestAccum(const OME_SCALAR & val, OME_SCALAR & accum)
{
	if (accum < val)
		accum = val;
}


inline Listable& greatest(Listable & list)
{
	
		Listable* ret = list.SubAggregate([](Listable* pDest, Listable* pSrc, const size_t & dInd, const size_t & sInd, const size_t & rInd)
		{ if ((*pDest)[dInd] < (*pSrc)[sInd])
		(*pDest)[dInd] = (*pSrc)[sInd]; }, OME_SCALAR_MIN);
		
	return *ret;
}

//================= howmanytrue([B]) ===================
//FunctionSignatures["howmanytrue"]={"howmanytrue","@LI",false};
inline void howmanytrueAccumInit(int & initVal)
{
	initVal = 0;
}
inline void howmanytrueAccum(const OME_SCALAR & val, int & accum)
{
	if (val)
		accum++;
}

inline OME_SCALAR howmanytrue(Listable& list)
{
	OME_SCALAR howMany=0.0;
	for (auto itr = list.SclrBegin(); itr.IsValid(); ++itr)
	{
		if (*itr)
			++howMany;
	}
	return howMany;
}
//================= least([X]) =========================
//FunctionSignatures["least"]={"least","@LI",false};

//FunctionSignatures["howmanytrue"]={"howmanytrue","@LI",false};
inline void leastAccumInit(OME_SCALAR & initVal)
{
	initVal = OME_SCALAR_MAX;
}

inline void leastAccum(const OME_SCALAR & val, OME_SCALAR & accum)
{
	if (accum>val)
		accum=val;
}

inline OME_SCALAR leastSclr(Listable& list)
{
	return list.GetMin();
}

inline Listable& least(Listable & list)
{
	
	
		Listable* ret = list.SubAggregate([](Listable* pDest, Listable* pSrc, const size_t & dInd, const size_t & sInd, const size_t & rInd)
		{ if ((*pDest)[dInd] > (*pSrc)[sInd])
		(*pDest)[dInd] = (*pSrc)[sInd]; }, OME_SCALAR_MAX);
	
	return *ret;
}

//================= makearray(X,N) =====================
//FunctionSignatures["makearray"]={"makearray","@CI,@FI,@SI,@BM,@SD",false};
inline OME_SCALAR makearray(Evaluable& caller, MADimFunc dimFunc, const OME_SCALAR & count, BaseManager* pBm, ISpatialDataProvider* pSdp)
{
	if (pBm->GetExecMode() == "Init")
	{
		std::vector<pair<size_t, size_t> >* pPosStack = GetPositionStack();
		if (caller.GetOMEType() != OME_VARARRAY)
			throw OMEFuncOpException("makearray", "caller is not VarArray.");
		VarArray& va = (VarArray&)caller;

		size_t offset = 0;
		if (!pPosStack->empty())
			offset = pPosStack->back().second;

		size_t dimSize = count;

		pPosStack->push_back(pair<size_t, size_t>(0, 0));
		if (va.GetDimCount() < pPosStack->size())
			va.AddDimFront(dimSize);
		va.Allocate();
		OME_SCALAR val = OME_NOVAL;
		for (size_t i = 0; i < dimSize; i++)
		{
			pPosStack->back().first = i;
			pPosStack->back().second = (offset * dimSize) + i;

			val = dimFunc(caller, pBm, pSdp);
			if (val != OME_NOVAL)
				va.SetValue((offset*dimSize) + i, val);
		}
		pPosStack->pop_back();
	}
	return OME_NOVAL;
}


//================= place_in(X) ========================
//FunctionSignatures["place_in"]={"place_in","@B1",false};
//presently just a placeholder
//inline OME_SCALAR place_in(const unsigned int & depth)
//{
//	OME_SCALAR ret = 0;
//
//	std::vector<std::pair<size_t, size_t> >* pStack = GetPositionStack();
//	if (!pStack->empty() && pStack->size() >= depth)
//		ret = pStack->at(pStack->size() - depth).first;
//
//	return ret;
//}


//================= posgreatest([X]) ===================
//FunctionSignatures["posgreatest"]={"posgreatest","@LI",false};

inline void posgreatestAccumInit(OME_SCALAR & initVal,int & initPos)
{
	initVal = OME_SCALAR_MIN;
	initPos = 0;
}

inline void posgreatestAccum(Listable & list, const unsigned int & ind, OME_SCALAR & lastVal,int & lastInd)
{
	if (lastVal < list[ind])
	{
		lastVal = list[ind];
		lastInd = ind + 1;

	}
}

inline unsigned int posgreatest(Listable & list)
{
	return list.GetMaxInd()+1;
}

inline unsigned int posgreatestSclr(Listable & list)
{
	return list.GetMaxInd() + 1;
}

//================= posleast([X]) ======================
//FunctionSignatures["posleast"]={"posleast","@LI",false};

inline void posleastAccumInit(OME_SCALAR & initVal, int & initPos)
{
	initVal = OME_SCALAR_MAX;
	initPos = 0;
}

inline void posleastAccum(Listable & list, const unsigned int & ind, OME_SCALAR & lastVal, int & lastInd)
{
	if (lastVal > list[ind])
	{
		lastVal = list[ind];
		lastInd = ind + 1;

	}
}
inline unsigned int posleast(Listable & list)
{
	return list.GetMinInd()+1;
}

inline unsigned int posleastSclr(Listable & list)
{
	return list.GetMaxInd() + 1;
}

//================= product([X]) =======================
//FunctionSignatures["product"]={"product","@LI",false};


inline void productAccumInit(OME_SCALAR & initVal,Listable & trgList)
{
	initVal = trgList.GetSize() ? 1.0 : 0.0;
}

inline void productAccum(const OME_SCALAR & val, OME_SCALAR & accum)
{
	accum *= val;
}

inline OME_SCALAR productSclr(Listable & list)
{
	return list.GetProduct();
}

inline Listable& product(Listable& list)
{

		Listable* ret = list.SubAggregate([](Listable* pDest, Listable* pSrc, const size_t & dInd, const size_t & sInd, const size_t & rInd)
		{ (*pDest)[dInd] *= (*pSrc)[sInd]; }, 1.0);

	return *ret;
}

//================= rankings([X]) =======================
//FunctionSignatures["rankings"]={"rankings","@LI",false};
inline Listable& rankings(Listable & list)
{
	if (list.GetLevel()>1)
	{
		throw OMESupportException("Rankings for lists with levels > 1 not supported.");
	}
	Listable* ret=NewTempList(list.GetSize(), list.GetSize());

	for (auto rItr = ret->SclrBegin(), lItr1 = list.SclrBegin(); rItr.IsValid(); ++rItr,++lItr1)
	{
		for (auto lItr2 = list.SclrBegin(); lItr2.IsValid();++lItr2)
		{
			if (lItr1 != lItr2 && *lItr1>*lItr2)
				--(*rItr);
		}
	}
	return *ret;
}

//================= size(S,I) ============================
//FunctionSignatures["size"]={"size","@MI,@SI",false};
inline OME_SCALAR size(Model* pMdl)
{
	return (OME_SCALAR)pMdl->GetNumInstances();
}
inline OME_SCALAR size(Model* pMdl, unsigned int depth)
{
	OME_SCALAR ret = 0.0;
	for (; depth && pMdl; pMdl = pMdl->GetParentModel(), ++depth);
	if (pMdl)
		ret = pMdl->GetNumInstances();
	/// @todo for fixed instances with multiple dimensions; enable if/when submodels can have multiple dimensions.
	return ret;
}

//================= subtotals([X]) =====================
//FunctionSignatures["subtotals"]={"subtotals","@LI",false};
inline void subtotalsAccum(Listable& inList,unsigned int & ind, Listable& newList)
{
	newList[ind] = inList[ind];
	if (ind > 0)
		newList[ind] += newList[ind - 1];
}

inline Listable& subtotals(Listable & list)
{
	Listable* ret=NewTempList(list.GetSize());

	size_t ind1 = 0, ind2 = 1;
	(*ret)[ind1] = list[ind1];

	for (; ind2 < ret->GetSize(); ++ind1, ++ind2)
		(*ret)[ind2] = list[ind2] + (*ret)[ind1];

	return *ret;
}

//================= sum([X]) ===========================
//FunctionSignatures["sum"]={"sum","@LI",false};
inline void sumAccumInit(OME_SCALAR & initVal)
{
	initVal =  0.0;
}

inline void sumAccum(OME_SCALAR & val, OME_SCALAR & accum)
{
	accum += val;
}

inline OME_SCALAR sumSclr(Listable & list)
{
	return list.GetSum();
}

inline Listable& sum(Listable & list)
{
	
		Listable* ret = list.SubAggregate([](Listable* pDest, Listable* pSrc, const size_t & dInd, const size_t & sInd, const size_t & rInd)
		{(*pDest)[dInd] += (*pSrc)[sInd]; }, 0.0);
		/*volatile bool dbg = false;
		if (dbg)
			for (int i = 0; i < ret->GetSize(); ++i)
				DBG_PRINTC(std::to_string((*ret)[i]), DBG_GREEN);*/
	return *ret;
}
//================= with_colin({X},{Y}) ================
//FunctionSignatures["with_colin"]={"with_colin","@LI,@LI",false};
inline OME_SCALAR with_colin(Listable & probs, Listable & vals)
{
	return vals[OMECFuncs::colin(vals)];
}

//================= with_greatest([X],[Y]) =============
//FunctionSignatures["with_greatest"]={"with_greatest","@LI,@LI",false};
inline OME_SCALAR with_greatest(Listable & probs, Listable & vals)
{
	return vals[OMECFuncs::posgreatest(probs)];
}

//================= with_least([X],[Y]) ================
//FunctionSignatures["with_least"]={"with_least","@LI,@LI",false};
inline OME_SCALAR with_least(Listable & probs, Listable & vals)
{
	return vals[OMECFuncs::posleast(probs)];
}
//====================================================================\\
//																	                   \\
//                         Undocumented Funcs                         \\
//																	                   \\
//====================================================================\\

/// \todo modify undocumented funcs to work with collections

//================= default(X) =========================
//FunctionSignatures["default"]={"default","@SI",false};
inline BaseWrap& OMEdefault(BaseWrap & arg)
{
	if (arg.m_type == BaseWrap::MENUM)
	{
		EnumWrap& ew = (EnumWrap&)arg;
		if (ew.m_pEe->pCollection->GetEnumValueCount())
			return Wrap(&ew.m_pEe->pCollection->GetEnumEntry(0));
	}
	return Wrap(0.0);
}

inline OME_SCALAR OMEdefault(Evaluable& eval)
{
	return 0.0;
}

inline OME_SCALAR OMEdefault(const OME_SCALAR & sclr)
{
	return 0.0;
}

inline int OMEdefault(const int & i)
{
	return 0;
}

inline EnumEntry OMEdefault(const EnumEntry & ee)
{
	
		return ee.pCollection->GetEnumEntry(0);
}

//================= for_members_of_type(X,[Y]) =========
//FunctionSignatures["for_members_of_type"]={"for_members_of_type","@SI,@LI",false};
inline Listable& for_members_of_type(OME_SCALAR arg, Listable & list)
{
	throw OMESupportException("for_members_of_type is not currently implemented.");
	/// \todo figure out what this function actually does
	return *NewTempCopyList(list);
}

//================= interpolate(X,[Y],[Z]) =============
//FunctionSignatures["interpolate"]={"interpolate","@SI,@LI,@LI",false};
inline OME_SCALAR interpolate(const OME_SCALAR & x, Listable & arrY, Listable & arrZ)
{
	ListableSclrItr itr;
	OME_SCALAR curr;
	unsigned int yLoc=0;
	size_t flrInd;

	//find X's relative location in [Y]
	for(itr=arrY.SclrBegin(); itr.IsValid(); ++itr)
	{
		curr=*itr;
		if(curr<x)
			yLoc++;
	}

	//find lower index for space that contains X.
	size_t len=arrY.GetSize();
	size_t minBound = len-1 < yLoc ? len-1 : yLoc;
	flrInd=1 > minBound ? 1 : minBound; //max(1,min(len-1,yLoc));

	OME_SCALAR yLow=flrInd;
	OME_SCALAR yHigh=flrInd+1;
	OME_SCALAR zLow=flrInd;
	OME_SCALAR zHigh=flrInd+1;

	//find equivalent value in [Z] - based on Simile code in the file "List Handling.pl"
	OME_SCALAR out;
	if(x<yLow)
		out=zLow;
	else if(x > yHigh)
		out=zHigh;
	else
		out= zLow+(x-yLow)*(zHigh-zLow)/(yHigh-yLow);

	return out;
}

//================= iterations( x ) ====================
//FunctionSignatures["iterations"]={"iterations","@II,"@BM",false};
inline OME_SCALAR iterations(IterConditional* pIc,BaseManager* pBm)
{
	return pIc->GetIterations(pBm)+1;
}

//================= ordinals(X) ========================
//FunctionSignatures["ordinals"]={"ordinals","@NI",false};
inline Listable& ordinals(ECollWrap & ew)
{
	//convert ECollWrap to listable by typecasting
	return ew;
}

//================= pi() ===============================
//FunctionSignatures["pi"]={"3.141592653589793238462","@NI",false};

//================= product3([[X]],[[Y]]) ==============
//FunctionSignatures["product3"]={"product3","@MI,@MI",false};
inline OME_SCALAR* product3(OME_SCALAR* matA,OME_SCALAR* matB)
{
	OME_SCALAR* result=NEWMAT3;
	
	for(unsigned short k=0; k <3; k++)
	{
		for(unsigned short m=0; m<3; m++)
		{
			//calculate specific cell, assign to outbound matrix.
			result[XY(k,m)]=(matA[XY(k,0)]*matB[XY(0,m)])+(matA[XY(k,1)]*matB[XY(1,m)])+(matA[XY(k,2)]*matB[XY(2,m)]);
		}
	}
	return result;
}

//================= transform3([[X]],[Y]) ============
//FunctionSignatures["transform3"]={"transform3","@MI,@LI",false};
inline OME_SCALAR* transform3(OME_SCALAR* mat, OME_SCALAR* vec)
{
	OME_SCALAR* result=new OME_SCALAR[3];
	for(int k=0; k<3; k++)
			result[k]=(vec[0]*mat[XY(k,0)])+(vec[1]*mat[XY(k,1)])+(vec[2]*mat[XY(k,2)]);
	return result;
}

//====================================================================\\
//                                                                    \\
//                         OME unique funcs                           \\
//                                                                    \\
//====================================================================\\

//================= valuesFromInstances(x,[y]) =======
//FunctionSignatures["valuesFromInstances"]={"valuesFromInstances","@EI,@LI",false};
inline Listable& valuesFromInstances(const Evaluable& eval, Listable & list)
{
	//still used by lua stuff
	Model* pMdl = eval.GetParentModel();
	size_t mID = eval.GetModelIndex();

	//assume and correct for 1-based indices
	return *NewTempLambdaList(list.GetSize(), [&list, pMdl, &mID](const size_t & i){return (*pMdl->GetValueArchive())[list[i] - 1 + mID]; });
}

inline void valuesFromInstances(const Evaluable& eval, size_t* inInds, OME_SCALAR* outVals,const size_t & lim)
{
	//inInds must be a 0-terminated list!!
	//outVals length must be >= inInds length
	Model* pMdl = eval.GetParentModel();
	size_t mID = eval.GetModelIndex();
	ValueArchive* pArch = pMdl->GetValueArchive();

	size_t i;
	for (i = 0; i<lim && inInds[i] != 0; ++i)
		outVals[i] = (*pArch)[inInds[i] - 1 + mID];

	if (i < lim)
	{
		outVals[i] = OME_NOVAL;
	}
}

inline OME_SCALAR valueFromInstance(const Evaluable& eval, size_t inInd)
{
	Model* pMdl = eval.GetParentModel();
	size_t mID = eval.GetModelIndex();
	ValueArchive* pArch = pMdl->GetValueArchive();

	return (*pArch)[inInd - 1 + mID];

}


//================= setEvalVal(x,y) ==================
//--not needed
//================= initEval(x) ======================
//--not needed
//================= resetEval(x) =====================
//--not needed
//================= updateTimeSeries(x) ==============
//--not needed
//================= updatePort(x) ====================
//--not needed
//================= killMarkedInstances(x) ===========
//--not needed
//================= updateSpawner(x) =================
//--not needed
//================= getAsTable(x...) =================
//FunctionSignatures["getAsTable"]={"getAsTable","@VI",false};
//inline Listable& getAsTable(initializer_list<BaseWrap*> initList)
//{
//    size_t count=initList.size();
//    Listable* result=NewTempList(count);
//    ListDims addDims;
//    
//    size_t i=0;
//    for(auto itr=initList.begin(); itr!=initList.end();++itr,++i)
//    {
//        BaseWrap* temp=(*itr);
//        
//        if(temp->m_type==BaseWrap::LIST)
//        {
//            if(itr==initList.begin() && !addDims.GetSize())
//            {
//                addDims = temp->m_pList->GetDims();
//                ListDims newDims(addDims);
//                newDims.AppendDimFront(count);
//                count = newDims.CalcRepLength();
//                DeletePoolPtr(result);
//                result = NewTempList(newDims,newDims.GetSize());
//            }
//            if(temp->m_pList->GetDims()==addDims)
//            {
//                //fix
//                for (size_t j = 0; j < temp->m_pList->GetSize(); ++j)
//                    result->SetValue(i++, temp->m_pList->GetValue(j));
//                --i;
//            }
//            else
//                throw std::out_of_range("GetAsTable: sub lists do not have matching dimensions");
//        }
//        else
//            result->SetValue(i,*temp);
//    }
//    
//    return *result;
//}

inline void getAsTable(VarArray& caller, initializer_list<OME_SCALAR> initList)
{
	size_t count = initList.size();
	caller.Allocate(count);

	size_t i = 0;
	for (auto itr = initList.begin(); itr != initList.end(); ++itr, ++i)
	{
		caller.SetValue(i, (*itr));
	}
}

inline void getAsTable(VarArray& caller,initializer_list<Listable*> initList)
{
	size_t count = initList.size();
	//pCaller->Allocate(count);

	size_t i = 0;
	for (auto itr = initList.begin(); itr != initList.end(); ++itr, ++i)
	{
		Listable* pLst = (*itr);


			if (itr == initList.begin() && !caller.GetDims().GetSize())
			{
				caller.AddDimFront(count);
				count = caller.GetDims().CalcRepLength();
				
				caller.Allocate();
			}
			if (pLst->GetDims() == caller.GetDims())
			{
				//fix
				for (size_t j = 0; j < pLst->GetSize(); ++j)
					caller.SetValue(i++, pLst->GetValue(j));
				--i;
			}
			else
				throw OMEFuncOpException("GetAsTable","sub lists do not have matching dimensions");
		
	}
}


inline void getAsTable(VarArray& caller,const size_t & count)
{
	caller.Allocate(count);
	caller.SetValues(0.0);
}

inline void getAsTable(VarArray& caller,Listable& lst)
{
	//pass through
	caller.Allocate(lst.GetDims());
	for (size_t i = 0; i < caller.GetSize(); ++i)
		caller.SetValue(i, lst[i]);
}

//================= getMCInstanceCount(x) ============
//--not needed

//================= setActiveInstance(x) =============
//--not needed

//================= omeLineInit() ====================
//--needed?
//================= upgroup(x) =======================
//FunctionSignatures["upgroup"]={"upgroup","@EI,@SI",false};

inline Listable& upgroup(VarArray& va, size_t lvl)
{

	if (lvl == 0)
	{
		throw OMEFuncOpException("upgroup","lvl cannot be 0");
		//recover with list length of 1?
	}
	Listable *ret;
	if (va.GetLevel() < lvl)
		ret = (Listable*)NewUpArray(&va, lvl);
	else
		ret = &va;
	return *ret;
}


inline Listable& upgroup(Evaluable& eval, size_t lvl)
{
	//fix for absolute instance indexes
	
	
	if (lvl == 0)
	{
		throw OMEFuncOpException("upgroup", "lvl cannot be 0");
		//recover with list length of 1?
	}
	Listable* ret;
	if (eval.GetOMEType() != OME_VARARRAY)
		ret = (Listable*)NewUpArray(&eval, lvl);
	else
	{
		return upgroup((VarArray&)eval, lvl);
	}
	return *ret;
}

//
//inline Listable& upgroup(VarArray & va, BaseWrapper & lvl)
//{
//	Listable& ret(0);
//	if ((size_t)*lvl > 1)
//	{
//		Model* pMdl = va.GetParentModel();
//		size_t instCount = 1;
//		size_t i;
//		for (i = 0; i < ((size_t)*lvl) && pMdl; i++, pMdl = pMdl->GetParentModel())
//			instCount *= pMdl->GetNumInstances();
//
//		ret = Listable&(instCount);
//
//		size_t start = pMdl->GetParentOffset()*pMdl->GetCurrentInstance();
//		for (i = start; i < start + instCount; i++)
//			ret[i](pMdl->GetRawInstance(i)->GetArrayAddr(va.GetModelIndex()));
//	}
//	else
//		ret = upgroup((Evaluable&)va, lvl);
//	return ret;
//}

//================= omecleanup() =====================
//FunctionSignatures["omecleanup"]={"omecleanup","",false};
inline void omecleanup()
{
	//cleanup stuff will go here
	//EvalCollection::ResetCacheIndex();
}

//================= setvararray(x,y) =================
inline void setvararray(VarArray& va, Listable & wrap)
{
	
	if (va.GetSize() < wrap.GetSize())
		va.Allocate(wrap.GetSize());
	for (size_t i = 0; i < wrap.GetSize(); ++i)
		va[i]=wrap[i];
}

inline void setvararray(VarArray& va, const OME_SCALAR & val)
{
	if(val != OME_NOVAL)		//noval indicates makeArray was called as argument; ignore.
		va.SetValues(val);
}

inline void setvararray(VarArray& va, AliasWrap & alias)
{
	if (alias.m_pEval->IsOMEType(OME_VARARRAY))
		setvararray(va, (VarArray&)alias.m_pEval);
	else if (alias.m_pEval==NULL)
		setvararray(va, 0.0);
	else
		setvararray(va, *alias.m_pRepVal);
}

inline void setvararray(VarArray& va, ModelEnum* val)
{
	va.SetValues(val);
}

inline void setvararray(VarArray& va, EnumEntry* val)
{
	va.SetValues(val);
}


inline void setvararray(VarArray& va, BaseWrap & bw)
{
	switch (bw.m_type)
	{
	case BaseWrap::LIST:
		setvararray(va, (Listable&)(bw));
		break;
	case BaseWrap::MENUM:
		setvararray(va, bw.m_pEe);
		break;
	case BaseWrap::ECOLL:
		setvararray(va, bw.m_pEColl);
		break;
	default:
		setvararray(va, (OME_SCALAR)(bw));

	}
}

//================= setvalue(x,y) ====================
//inline void setvalue(Evaluable& eval, BaseWrap & bw)
//{
//	switch (bw.m_type)
//	{
//	case BaseWrap::ECOLL:
//		pEval->SetValue(bw.m_pEColl);
//		break;
//	case BaseWrap::MENUM:
//		pEval->SetValue(bw.m_pEe);
//		break;
//	default:
//		pEval->SetValue((OME_SCALAR)bw);
//	}
//}

inline void setvalue(Evaluable& eval, const OME_SCALAR & val)
{
	eval.SetValue(val);
}

inline void setvalue(Evaluable& eval, Evaluable& evalIn)
{
	eval.SetValue(evalIn.GetValue());
}


inline void setvalue(Evaluable& eval, ModelEnum* val)
{
	eval.SetValue(val);
}

inline void setvalue(Evaluable& eval, EnumEntry* val)
{
	eval.SetValue(val);
}

//====================================================================\\
//                                                                    \\
//                         SDP Interface                              \\
//                                                                    \\
//====================================================================\\
//FunctionSignatures["SDPGetBooleanData"]={"SDPGetBooleanData","@SD,@SI,@SI",false};
inline bool SDPGetBooleanData(ISpatialDataProvider* pSDP,const int & row,const int & col)
{
	bool out=false;
	if(row==-1 || col==-1 || !pSDP->GetData((int)row,(int)col,out))
	{
		throw OMEFuncOpException("SDPGetBooleanData", "Invalid boolean data.");
	}
	
	return out;
}

//FunctionSignatures["SDPGetCapabilities"]={"@SD->GetCapabilities","",false};

//FunctionSignatures["SDPGetCellSize"]={"@SD->GetCellSize","",false};

//FunctionSignatures["SDPGetColumnCount"]={"@SD->GetColumnCount","",false};

//FunctionSignatures["SDPGetExtents"]={"SDPGetExtents","@SD",false};
inline Listable& SDPGetExtents(ISpatialDataProvider* pSDP)
{
	Listable* out=NewTempList(4);
	OME_SCALAR exts[4];
	pSDP->GetExtents(exts[0],exts[1],exts[2],exts[3]);
	(*out)[0] = exts[0];
	(*out)[1] = exts[1];
	(*out)[2] = exts[2];
	(*out)[3] = exts[3];
	
	return *out;
}

//FunctionSignatures["SDPGetFielCol"]={"GetFieldCol","@SD,@TI",false};
inline int SDPGetFielCol(ISpatialDataProvider* pSDP, const STLString & col)
{
	return pSDP->GetFieldCol(col.c_str());
}

//FunctionSignatures["SDPGetDataMinMax"]={"SDPGetDataMinMax","@SD,@SI",false};
inline Listable& SDPGetDataMinMax(ISpatialDataProvider* pSDP,const int & col)
{
	Listable* out = NewTempList(2);
	OME_SCALAR exts[2];
	pSDP->GetDataMinMax(col, exts[0], exts[1]);
	(*out)[0] = exts[0];
	(*out)[1] = exts[1];
	 return *out;
}


inline int SDPGetNeighborCount(ISpatialDataProvider* pSDP, const int & ind)
{
	return pSDP->GetNeighborCount(ind);
}

inline int SDPGetNeighborForDirection(ISpatialDataProvider* pSDP, const int & ind, const GRID_DIR & dir)
{
	int ret;
	if (!pSDP->GetNeighborForDirection(ind, dir, ret))
		throw OMEOOBException("No neighbor in direction specified.");
	return ret;
}

//FunctionSignatures["SDPGetIntegerData"]={"(int)SDPGetNumberData","@SD,@SI,@SI",false};

//FunctionSignatures["SDPGetNumberData"]={"SDPGetNumberData","@SD,@SI,@SI",false};
inline OME_SCALAR SDPGetNumberData(ISpatialDataProvider* pSDP, OME_SCALAR & row, OME_SCALAR & col)
{
	OME_SCALAR out=false;
	if(row==-1 || col==-1 || !pSDP->GetData(row,col,out))
	{
		throw OMEFuncOpException("SDPGetNumberData", "Invalid number data.");
	}
	
	return out;
}

//FunctionSignatures["SDPGetRowCount"]={"@SD->GetRowCount","",false};

//FunctionSignatures["SDPGetStringData"]={"SDPGetStringData","@SD,@SI,@SI,BI",false};
inline STLString SDPGetStringData(ISpatialDataProvider* pSDP,const size_t & row,const size_t & col,const size_t & lim)
{
	OMEChar* outStr=new OMEChar[lim];
	if(row==-1 || col==-1 || pSDP->GetData(row,col,outStr,lim))
	{
		throw OMEFuncOpException("SDPGetStringData", "Invalid string data.");
	}
	
	STLString out=STLString(outStr);
	delete[] outStr;
	return out;
}


inline bool SDPHasNeighborForDirection(ISpatialDataProvider* pSDP, const int & ind, const GRID_DIR & dir)
{
	return pSDP->HasNeighborForDirection(ind, dir);
}

//FunctionSignatures["SDPNextTo"]={"SDPNextTo","@SD,@SI,@TI",false};
inline bool SDPNextTo(ISpatialDataProvider* pSDP, const int & poly, STLString & query)
{
	return pSDP->NextTo(poly,query.c_str());
}

//FunctionSignatures["SDPNextToArea"]={"SDPNextToArea","@SD,@SI,@TI",false};
inline OME_SCALAR SDPNextToArea(ISpatialDataProvider* pSDP, const int & poly, STLString & query)
{
	return pSDP->NextToArea(poly,query.c_str());
}

//FunctionSignatures["SDPNextToID"]={"SDPNextToID","@SD,@SI",false};
inline Listable& SDPNextToID(ISpatialDataProvider* pSDP, const int & poly)
{
	//inbound indices are 1-based

	return *NewTempLambdaList(pSDP->GetNeighborCount(poly), [&pSDP, &poly](const size_t & i){ return pSDP->GetNeighborRecord(poly, i + 1); });

}

inline void SDPNextToID(ISpatialDataProvider* pSDP,const int & poly,size_t* outIDs)
{
	//outIDs should be the size of the max neighbor count;

	//inbound indices are 1-based
	unsigned int count = pSDP->GetNeighborCount(poly);
	unsigned int i;
	for (i = 0; i < count; ++i)
		outIDs[i] = pSDP->GetNeighborRecord(poly, i + 1);
	
	if (i < pSDP->GetMaxNeighborCount())
		outIDs[i] = 0;
}

//FunctionSignatures["SDPNextToValues"]={"SDPNextToValues","@SD,@TI,@SI",false};
inline Listable& SDPNextToValues(ISpatialDataProvider* pSDP,const int & poly,STLString & col)
{
	OME_SCALAR tmp;
	int colInd = pSDP->GetFieldCol(col.c_str());
	return *NewTempLambdaList(pSDP->GetNeighborCount(poly), [&poly, &tmp, &pSDP, &colInd](const size_t & i){ pSDP->GetData(pSDP->GetNeighborRecord(poly,i+1),colInd, tmp); return tmp; });
}

//FunctionSignatures["SDPSetData"]={"SDPSetData","@SD,@SI,@SI,@SI",false};
inline void SDPSetData(ISpatialDataProvider* pSDP,const int & row,const int & col, const OME_SCALAR & val)
{
	if(row==-1 || col==-1)
	{
		throw OMEOOBException("row or column is out of bounds.");
	}

	pSDP->SetData(row,col,val);
}

//FunctionSignatures["SDPWithin"]={"SDPWithin","@SD,@SI,@TI,@SI",false};
inline bool SDPWithin(ISpatialDataProvider* pSDP,const int & ind,STLString & query, const OME_SCALAR & dist)
{
	return pSDP->Within(ind,query.c_str(),dist);
}

//FunctionSignatures["SDPWithinArea"]={"SDPWithinArea","@SD,@SI,@TI,@SI",false};
inline OME_SCALAR SDPWithinArea(ISpatialDataProvider* pSDP,const int & ind,STLString & query, const OME_SCALAR & dist)
{
	return pSDP->WithinArea(ind,query.c_str(),dist);
}


//temporal lookup inits
inline OME_SCALAR lookupInit_const_delay(BaseManager* pBm,Evaluable& eval, const size_t & dumm, const OME_SCALAR & dum2)
{

	pBm->MarkVariableForTracking(&eval);
	return 0.0;
}

inline OME_SCALAR lookupInit_last(BaseManager* pBm, Evaluable& eval)
{
	pBm->MarkVariableForTracking(&eval);
	return 0.0;
}

inline OME_SCALAR lookupInit_prev(BaseManager* pBm,Evaluable& caller, const OME_SCALAR & dum)
{
	pBm->MarkVariableForTracking(&caller);
	return 0.0;
}

inline OME_SCALAR lookupInit_var_delay(BaseManager* pBm,Evaluable& eval, const OME_SCALAR & offset)
{
	
	pBm->MarkVariableForTracking(&eval);
	
	return 0.0;
}

//if-then-else handlers (type resolution)
//inline OME_SCALAR ifthenelse(const bool & eval, const OME_SCALAR & success, const OME_SCALAR & failure)
//{
//	return eval ? success : failure;
//}
//
//inline OME_SCALAR ifthenelse(const bool & eval, Evaluable* success, const OME_SCALAR & failure)
//{
//	return eval ? success->GetValue() : failure;
//}
//
//inline OME_SCALAR ifthenelse(const bool & eval, const OME_SCALAR & success, Evaluable* failure)
//{
//	return eval ? success : failure->GetValue();
//}
//
//inline Evaluable* ifthenelse(const bool & eval, Evaluable* success, Evaluable* failure)
//{
//	return eval ? success : failure;
//}
//
//inline Listable& ifthenelse(const bool & eval, const OME_SCALAR & success, Listable & failure)
//{
//	return eval ? *NewTempList(failure.GetSize(),success) : failure;
//}
//
//inline Listable& ifthenelse(const bool & eval, Listable & success, const OME_SCALAR & failure)
//{
//	return eval ? success : *NewTempList(success.GetSize(), failure);
//}
//
//inline Listable& ifthenelse(const bool & eval, Listable & success, Listable & failure)
//{
//	return eval ? success :failure;
//}
//
//inline Listable& ifthenelse(const bool & eval, Evaluable* success, Listable & failure)
//{
//	return eval ? *NewTempList(failure.GetSize(), success) : failure;
//}
//
//inline Listable& ifthenelse(const bool & eval, Listable & success, Evaluable* failure)
//{
//	return eval ? success : *NewTempList(success.GetSize(), failure);
//}



//====================================================================\\
//                                                                    \\
//                         OME Registry Function                      \\
//                                                                    \\
//====================================================================\\

    //custom funcs
    FuncSig::FS_ARGS GenMakeArray(const STLString & className, oSTLSStream & outBuff, Evaluable* pCaller, CPPLoopCluster* pSrc, size_t & clustInd, const STLString & call, const bool & forInit);
    FuncSig::FS_ARGS GenElement(const STLString & className, oSTLSStream & outBuff, Evaluable* pCaller, CPPLoopCluster* pSrc, size_t & clustInd, const STLString & call, const bool & forInit);
    FuncSig::FS_ARGS GenCount(const STLString & className, oSTLSStream & outBuff, Evaluable* pCaller, CPPLoopCluster* pSrc, size_t & clustInd, const STLString & call, const bool & inInit);
    FuncSig::FS_ARGS GenIndex(const STLString & className, oSTLSStream & outBuff, Evaluable* pCaller, CPPLoopCluster* pSrc, size_t & clustInd, const STLString & call, const bool & inInit);
	FuncSig::FS_ARGS GenInitOnly(const STLString & className, oSTLSStream & outBuff, Evaluable* pCaller, CPPLoopCluster* pSrc, size_t & clustInd, const STLString & call, const bool & inInit);
	FuncSig::FS_ARGS GenValuesFromInstances(const STLString & className, oSTLSStream & outBuff, Evaluable* pCaller, CPPLoopCluster* pSrc, size_t & clustInd, const STLString & call, const bool & inInit);
    FuncSig::FS_ARGS GenSDPNextToID(const STLString & className, oSTLSStream & outBuff, Evaluable* pCaller, CPPLoopCluster* pSrc, size_t & clustInd, const STLString & call, const bool & inInit);


    void LoadSigs(std::map<STLString,FuncSig> & sigs);
    void LoadAccumPatterns(std::map<STLString, std::pair<STLString, STLString> > & patterns);
	void LoadConstants(std::map<STLString, STLString > & maps);
///@endcond
};
