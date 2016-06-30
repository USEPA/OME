#pragma once

#include "omeobjects/Model.h"
#include "omeobjects/Spawner.h"
#include "omeobjects/IterConditional.h"
#include "omeobjects/VarArray.h"
#include "UpdateContainers/Listable.h"
#include "UpdateContainers/UpdateContainers.h"
#include <exception>

namespace OMECFuncs
{
	/** Base class for evaluative Wrapper objects.*/
	struct __EXPORT__ 	BaseWrap
	{
		/** Scalar Wrapper Constructor.
			@param sclr The scalar to wrap.
		*/
		BaseWrap(const OME_SCALAR & sclr) :m_type(SCALAR), m_scalar(sclr){};
		/** Evaluable Wrapper constructor.
			@param pEval pointer to Evaluable object to wrap.
		*/
		BaseWrap(Evaluable* pEval) :m_type(EVAL), m_pEval(pEval) {};
		/** Evaluable Wrapper constructor.
			@param pEval pointer to Evaluable object to wrap.
			@param offs The instance or array offset.
			@param pRep Pointer to the specifically enumerated value.
		*/
		BaseWrap(Evaluable* pEval, const size_t & offs, OME_SCALAR* pRep) :m_type(ALIAS), m_pEval(pEval), m_ind(offs), m_pRepVal(pRep){};
		/** List Wrapper constructor. 
			@param lst Reference to the list to wrap.
		*/
		BaseWrap(Listable & lst) :m_type(LIST), m_pList(&lst) {};
		/** EnumEntry Wrapper constructor.
			@param pEe Pointer to the EnumEntry to wrap.
		*/
		BaseWrap(EnumEntry* pEe) :m_type(MENUM), m_pEe(pEe){};
		/** ModelEnum Wrapper constructor.
			@param pEc Pointer to the ModelEnum to wrap.
		*/
		BaseWrap(ModelEnum* pEc) :m_type(ECOLL), m_pEColl(pEc){};
		/** Wrapper type specification flag.*/
		enum TYPE : short { SCALAR = 0, ///< Scalar Wrapper.
							EVAL, 		///< Evaluable Wrapper.
							LIST, 		///< Listable Wrapper.
							ALIAS, 		///< EvalAlias Wrapper.
							MENUM, 		///< EnumEntry Wrapper.
							ECOLL, 		///< ModelEnum Wrapper.
							TYPECOUNT	///< Total count of flags.
		};

		union
		{
			Evaluable* m_pEval;	 ///< Pointer to Evaluable Object.
			Listable* m_pList;	 ///< Pointer to Listable Object.
			EnumEntry* m_pEe;	 ///< Pointer to EnumEntry Object.
			ModelEnum* m_pEColl; ///< Pointer to ModelEnum Object.
		};

		union
		{
			OME_SCALAR m_scalar; ///< Represented scalar.
			size_t m_ind;		 ///< Represented index.
		};
		OME_SCALAR* m_pRepVal;	 ///< Pointer to Represented Scalar value.
		TYPE m_type;			 ///< Flag indicating the type of the wrapped object.

		/** Listable Reference cast operator. */
		virtual operator Listable&() = 0;
		/** Evaluable Pointer cast operator. */
		virtual operator Evaluable*() = 0;
		/** ModelEnum Pointer cast operator. */
		virtual operator ModelEnum*() = 0;
		/** EnumEntry Pointer cast operator. */
		virtual operator EnumEntry*() = 0;

		//put explicit bool here since vs 2013 cannot do virtual explicit
		/** bool cast operator. */
		explicit operator bool(){ return this->ToBool() != 0.0; }
		/** Scalar cast operator. */
		operator const OME_SCALAR() { return this->ToSclr(); }
	private:
		/** @return Boolean representation of object. Used for bool cast operator. */
		virtual bool ToBool() = 0;
		/** @return Scalar representation of object. Used for Scalar cast operator. */
		virtual OME_SCALAR ToSclr() = 0;
	};

	void __EXPORT__ ResizeTempPools(const size_t & sSize, const size_t & aSize);
	void __EXPORT__ AdjustTempPool();

	void __EXPORT__ ResetLine();
	__EXPORT__ std::vector<std::pair<size_t, size_t> >* GetPositionStack();

	__EXPORT__ void DumpProfile(const char* file);

	__EXPORT__ Listable* NewTempList(const size_t & len, Evaluable* pEval = NULL);
	__EXPORT__ Listable* NewTempList(const size_t & len, const size_t & fill, Evaluable* pEval = NULL);
	__EXPORT__ Listable* NewTempList(const ListDims & dims, const size_t & lvl, Evaluable* pEval = NULL);
	__EXPORT__ Listable* NewTempCopyList(Listable & lst);
	__EXPORT__ Listable* NewTempCopyList(Listable & lst, Listable & inds);

	/** Allocate a new temporary list initialized with a custome function.
		@param len The Length of the temporary list.
		@param InitFunc The initialization function to call on each slot.
		@return The newly allocated temporary func.
	*/
	template<class LambdaFunc> Listable* NewTempLambdaList(const size_t & len, LambdaFunc InitFunc)
	{
		return TEMP_LIST_TYPE::NewTempLambda(len, InitFunc);
	}

	struct AliasWrap;
	__EXPORT__ AliasWrap* NewAlias(Evaluable* pEval, OME_SCALAR* pRep, const size_t & index);
	__EXPORT__ StaticUpArray* NewUpArray(Evaluable* pEval, const size_t & lvl);
	__EXPORT__ StaticUpArray* NewUpArray(VarArray* pVa, const size_t & lvl);

	__EXPORT__ void DeletePoolPtr(Listable* il);
	//	__EXPORT__ void DeletePoolPtr(AliasWrap* ea);
	__EXPORT__ void DeletePoolPtr(StaticUpArray* sua);
	__EXPORT__ void DeletePoolPtr(DynamicUpArray* dua);
	__EXPORT__ void DeletePoolPtr(ShuffledList* sl);
	__EXPORT__ void DeletePoolPtr(BaseWrap* ifw);

	/** Wrapper for scalar values. */
	struct __EXPORT__ SclrWrap : public BaseWrap
	{
		/** Simple constructor. 
			@param sclr The scalar value to wrap.
		*/
		SclrWrap(const OME_SCALAR & sclr) :BaseWrap(sclr){};
		virtual operator Evaluable*() { return NULL; }
		virtual operator Listable&() { return *NewTempList(1, m_scalar); }
		virtual operator ModelEnum*(){ return NULL; };
		virtual operator EnumEntry*(){ return NULL; };

	private:
		virtual bool ToBool() { return m_scalar != 0.0; };
		virtual OME_SCALAR ToSclr() { return m_scalar; }
	};

	/** Wrapper for Evaluable objects.*/
	struct __EXPORT__ EvalWrap : public BaseWrap
	{
		/** Simple Constructor.
			@param pEval Pointer to Evaluable object to wrap.
		*/
		EvalWrap(Evaluable* pEval) :BaseWrap(pEval) {};
		virtual operator Evaluable*() { return m_pEval; }
		virtual operator Listable&() { return *NewTempList(1, m_pEval->GetValue(), m_pEval); }
		virtual operator ModelEnum*(){ return m_pEval->GetEnumClass(); };
		virtual operator EnumEntry*(){ return m_pEval->GetEnumEntry(); };

	private:
		virtual bool ToBool() { return m_pEval->GetValue() != 0.0; };
		virtual OME_SCALAR ToSclr() { return m_pEval->GetValue(); }
	};

	/** Wrapper for a subvalue of a collection of values.*/
	struct __EXPORT__ AliasWrap : public BaseWrap
	{
		/** Simple Constructor.
			@param pEval Pointer to the source Evaluable object.
			@param pRep Pointer to the represented Scalar value.
			@param offs The offset index to the represented object.
		*/
		AliasWrap(Evaluable* pEval, OME_SCALAR* pRep, const size_t & offs) : BaseWrap(pEval, offs, pRep) {};
		/** Simple Wrapper constructor.
			@param bw The wrapper to alias.
		*/
		AliasWrap(const BaseWrap & bw) :BaseWrap(bw.m_pEval, bw.m_ind, bw.m_pRepVal){};

		//for Null value equivalent
		AliasWrap() :BaseWrap(NULL, 0, NULL) {}

		virtual operator Evaluable*() { return m_pEval; }
		virtual operator Listable&() { return *NewTempList(1, *m_pRepVal, m_pEval); }
		virtual operator ModelEnum*(){ return m_pEval->GetEnumClass(); };
		virtual operator EnumEntry*(){ return m_pEval->GetEnumClass()->AddrForIndex(*m_pRepVal); };

		OME_SCALAR operator[](const size_t & i) const
		{
			if (!m_pEval || !m_pEval->IsOMEType(OME_VARARRAY))
				throw OMEFuncOpException("AliasWrap::operator[]","not an alias of a list type.");
			return ((const VarArray*)m_pEval)->GetValue(i);

		}

		/** @return Represented value.*/
		OME_SCALAR GetValue() { return *m_pRepVal; }
	private:
		virtual bool ToBool() { return *m_pRepVal != 0.0; };
		virtual OME_SCALAR ToSclr() { return *m_pRepVal; }
	};

	/** Wrapper for Listable object.*/
	struct __EXPORT__ ListWrap : public BaseWrap
	{
		/** Simple Constructor.
			@param lst reference to listable to wrap.
		*/
		ListWrap(Listable & lst) :BaseWrap(lst) {};
		virtual operator Listable&() { return *m_pList; }
		virtual operator Evaluable*() { return m_pList->GetRepEval(); }
		virtual operator ModelEnum*(){ return m_pList->GetRepEval()->GetEnumClass(); };
		virtual operator EnumEntry*(){ return m_pList->GetRepEval()->GetEnumEntry(); };

	private:
		virtual bool ToBool() { throw OMESupportException("Boolean cast not supported."); return false; };
		virtual OME_SCALAR ToSclr() { return m_pList->operator[](0); }
	};

	/** Wrapper for EnumEntry object.*/
	struct __EXPORT__ EnumWrap : public BaseWrap
	{
		/** Simple Constructor.
			@param pEe Pointer to EnumEntry.
		*/
		EnumWrap(EnumEntry* pEe) :BaseWrap(pEe){};
		virtual operator Evaluable*() { return NULL; }
		virtual operator Listable&() { return *NewTempList(1, m_pEe->ind); }
		virtual operator ModelEnum*(){ return m_pEe->pCollection; };
		virtual operator EnumEntry*(){ return m_pEe; };

	private:
		virtual bool ToBool() { return true; };
		virtual OME_SCALAR ToSclr() { return m_pEe->ind; }
	};

	/** Wrapper for ModelEnum object. */
	struct __EXPORT__ ECollWrap : public BaseWrap
	{
		/**Simple Constructor.
		   @param pEc Pointer to ModelEnum to wrap.
		*/
		ECollWrap(ModelEnum* pEc) :BaseWrap(pEc){};
		//virtual operator OME_SCALAR() const { return m_pEColl->GetEnumValueCount(); }
		virtual operator Evaluable*() { return NULL; }
		virtual operator Listable&()
		{
			Listable* pList = NewTempList(m_pEColl->GetEnumValueCount(), (size_t)0);
			for (unsigned int i = 1; i <= m_pEColl->GetEnumValueCount(); ++i)
				pList->operator[](i - 1) = i;
			return *pList;
		}
		virtual operator ModelEnum*(){ return m_pEColl; };
		virtual operator EnumEntry*(){ return NULL; };

	private:
		virtual bool ToBool() { return m_pEColl->GetEnumValueCount(); };
		virtual OME_SCALAR ToSclr() { return m_pEColl->GetEnumValueCount(); };

	};

	//=========== alloc templates =====================================
	/// @cond DOX_NO_DOC
	typedef BaseWrap*(*BWNew)(void*);
	typedef void(*BWRelease)(BaseWrap*, void*);
	typedef void(*BWClearTemps)(void*);
	typedef void(*BWAdjustStorage)(void*);
	/// @endcond

	//=========== arg adapters ========================================

	__EXPORT__ BaseWrap& Wrap(const OME_SCALAR & scalar);
	__EXPORT__ BaseWrap& Wrap(Evaluable* pEval);
	__EXPORT__ BaseWrap& Wrap(VarArray* pVa);
	__EXPORT__ BaseWrap& Wrap(Listable& list);
	__EXPORT__ BaseWrap& Wrap(EnumEntry* pEe);
	/** Passthrough function.
		@param bw The wrapper to pass through.
		@return Reference to bw.
	*/
	__EXPORT__ inline BaseWrap& Wrap(BaseWrap& bw) { return bw; };


	/// @cond DOX_NO_DOC
	//wrap operators
#define WRAP_BINOPDEFINE(OP)																                            \
	__EXPORT__ inline BaseWrap& operator OP ( BaseWrap & lhs, BaseWrap & rhs)		                                            \
		   {																					                                    \
		using namespace OMECFuncs;																		                        \
		enum {SS,SE,SL,SA,SM,SC,                                                                                                \
            ES,EE,EL,EA,EM,EC,                                                                                                  \
            LS,LE,LL,LA,LM,LC,                                                                                                  \
            AS,AE,AL,AA,AM,AC,                                                                                                  \
            MS,ME,ML,MA,MM,MC,                                                                                                  \
            CS,CE,CL,CA,CM,CC};							                                                                        \
		unsigned short action = 0;										                                                        \
		action = lhs.m_type*BaseWrap::TYPECOUNT;					                                                            \
		action += rhs.m_type;											                                                        \
																				                                                \
		switch (action)													                                                        \
	      {									                                                                                        \
		case SS: return Wrap(lhs.m_scalar OP rhs.m_scalar); break;						                                        \
		case SE: return Wrap(lhs.m_scalar OP rhs.m_pEval->GetValue()); break;		                                            \
		case SL: return Wrap(lhs.m_scalar OP *rhs.m_pList); break;						                                        \
		case SA: return Wrap(lhs.m_scalar OP *((AliasWrap&)rhs).m_pRepVal); break;	                                            \
      case SM: return Wrap(lhs.m_scalar OP rhs.m_pEe->ind); break;                                                              \
      case SC: return Wrap(lhs.m_scalar OP rhs.m_pEColl->GetEnumValueCount()); break;                                           \
		case ES: return Wrap(lhs.m_pEval->GetValue() OP rhs.m_scalar); break;			                                        \
		case EE: return Wrap(lhs.m_pEval->GetValue() OP rhs.m_pEval->GetValue()); break;                                        \
		case EL: return Wrap(lhs.m_pEval->GetValue() OP *rhs.m_pList); break;			                                        \
		case EA: return Wrap(lhs.m_pEval->GetValue() OP *((AliasWrap&)rhs).m_pRepVal); break;	                                \
      case EM: return Wrap(lhs.m_pEval->GetValue() OP rhs.m_pEe->ind); break;                                                   \
      case EC: return Wrap(lhs.m_pEval->GetValue() OP rhs.m_pEColl->GetEnumValueCount()); break;                                \
		case LS: return Wrap(*lhs.m_pList OP rhs.m_scalar); break;						                                        \
		case LE: return Wrap(*lhs.m_pList OP rhs.m_pEval->GetValue()); break;			                                        \
		case LL: return Wrap(*lhs.m_pList OP *rhs.m_pList); break;						                                        \
		case LA: return Wrap(*lhs.m_pList OP *((AliasWrap&)rhs).m_pRepVal); break;			                                    \
      case LM: return Wrap(*lhs.m_pList OP rhs.m_pEe->ind); break;                                                              \
      case LC: return Wrap(*lhs.m_pList OP rhs.m_pEColl->GetEnumValueCount()); break;                                           \
		case AS: return Wrap(*((AliasWrap&)lhs).m_pRepVal OP rhs.m_scalar); break;			                                    \
		case AE: return Wrap(*((AliasWrap&)lhs).m_pRepVal OP rhs.m_pEval->GetValue()); break;			                        \
		case AL: return Wrap(*((AliasWrap&)lhs).m_pRepVal OP *rhs.m_pList); break;						                        \
		case AA: return Wrap(*((AliasWrap&)lhs).m_pRepVal OP *((AliasWrap&)rhs).m_pRepVal); break;	                            \
      case AM: return Wrap(*((AliasWrap&)lhs).m_pRepVal OP rhs.m_pEe->ind); break;                                              \
      case AC: return Wrap(*((AliasWrap&)lhs).m_pRepVal OP rhs.m_pEColl->GetEnumValueCount()); break;                           \
      case MS: return Wrap(lhs.m_pEe->ind OP rhs.m_scalar); break;						                                        \
      case ME: return Wrap(lhs.m_pEe->ind OP rhs.m_pEval->GetValue()); break;		                                            \
      case ML: return Wrap(lhs.m_pEe->ind OP *rhs.m_pList); break;						                                        \
      case MA: return Wrap(lhs.m_pEe->ind OP *((AliasWrap&)rhs).m_pRepVal); break;	                                            \
      case MM: return Wrap(lhs.m_pEe->ind OP rhs.m_pEe->ind); break;                                                            \
      case MC: return Wrap(lhs.m_pEe->ind OP rhs.m_pEColl->GetEnumValueCount()); break;                                         \
      case CS: return Wrap(lhs.m_pEColl->GetEnumValueCount() OP rhs.m_scalar); break;						                    \
      case CE: return Wrap(lhs.m_pEColl->GetEnumValueCount() OP rhs.m_pEval->GetValue()); break;		                        \
      case CL: return Wrap(lhs.m_pEColl->GetEnumValueCount() OP *rhs.m_pList); break;						                    \
      case CA: return Wrap(lhs.m_pEColl->GetEnumValueCount() OP *((AliasWrap&)rhs).m_pRepVal); break;	                        \
      case CM: return Wrap(lhs.m_pEColl->GetEnumValueCount() OP rhs.m_pEe->ind); break;                                         \
      default:/*case CC:*/ return Wrap(lhs.m_pEColl->GetEnumValueCount() OP rhs.m_pEColl->GetEnumValueCount()); break;          \
	      };		                                    											                                \
	}																					                            

#define WRAP_UNOPDEFINE(OP)														         					   \
   __EXPORT__ inline BaseWrap& operator OP (BaseWrap & lhs)		                         					   \
	      {																			         					   \
      switch (lhs.m_type)																					   \
	      {																				     					   \
         case BaseWrap::SCALAR: return Wrap(OP lhs.m_scalar); break;										   \
         case BaseWrap::EVAL: return Wrap(OP lhs.m_pEval->GetValue()); break;								   \
         case BaseWrap::LIST: return Wrap(OP *lhs.m_pList); break;											   \
         case BaseWrap::ALIAS: return Wrap(OP *((AliasWrap&)lhs).m_pRepVal); break;							   \
         case BaseWrap::MENUM: return Wrap(OP (int)lhs.m_pEe->ind); break;              							   \
          default:/*case BaseWrap::ECOLL:*/ return Wrap(OP (int)lhs.m_pEColl->GetEnumValueCount()); break;          \
					};																				                \
				}

	WRAP_BINOPDEFINE(+)
	WRAP_BINOPDEFINE(-)
	__EXPORT__ inline BaseWrap& operator * (BaseWrap & lhs, BaseWrap & rhs)		                                            
	{																					                                    
	using namespace OMECFuncs;																		                        
	enum {
		SS, SE, SL, SA, SM, SC, 
		ES, EE, EL, EA, EM, EC, 
		LS, LE, LL, LA, LM, LC, 
		AS, AE, AL, AA, AM, AC, 
		MS, ME, ML, MA, MM, MC, 
		CS, CE, CL, CA, CM, CC
	};							                                                                        
	unsigned short action = 0;										                                                        
	action = lhs.m_type*BaseWrap::TYPECOUNT;					                                                            
	action += rhs.m_type;											                                                        
	
	switch (action)													                                                        
		{									                                                                                        
	case SS: return Wrap(lhs.m_scalar * rhs.m_scalar); break;						                                        
	case SE: return Wrap(lhs.m_scalar * rhs.m_pEval->GetValue()); break;		                                            
	case SL: return Wrap(lhs.m_scalar * *rhs.m_pList); break;						                                        
	case SA: return Wrap(lhs.m_scalar * *((AliasWrap&)rhs).m_pRepVal); break;	                                            
	case SM: return Wrap(lhs.m_scalar * rhs.m_pEe->ind); break;                                                              
	case SC: return Wrap(lhs.m_scalar * rhs.m_pEColl->GetEnumValueCount()); break;                                           
	case ES: return Wrap(lhs.m_pEval->GetValue() * rhs.m_scalar); break;			                                        
	case EE: return Wrap(lhs.m_pEval->GetValue() * rhs.m_pEval->GetValue()); break;                                        
	case EL: return Wrap(lhs.m_pEval->GetValue() * *rhs.m_pList); break;			                                        
	case EA: return Wrap(lhs.m_pEval->GetValue() * *((AliasWrap&)rhs).m_pRepVal); break;	                                
	case EM: return Wrap(lhs.m_pEval->GetValue() * rhs.m_pEe->ind); break;                                                   
	case EC: return Wrap(lhs.m_pEval->GetValue() * rhs.m_pEColl->GetEnumValueCount()); break;                                
	case LS: return Wrap(*lhs.m_pList * rhs.m_scalar); break;						                                        
	case LE: return Wrap(*lhs.m_pList * rhs.m_pEval->GetValue()); break;			                                        
	case LL: return Wrap(*lhs.m_pList * *rhs.m_pList); break;						                                        
	case LA: return Wrap(*lhs.m_pList * *((AliasWrap&)rhs).m_pRepVal); break;			                                    
	case LM: return Wrap(*lhs.m_pList * rhs.m_pEe->ind); break;                                                              
	case LC: return Wrap(*lhs.m_pList * rhs.m_pEColl->GetEnumValueCount()); break;                                           
	case AS: return Wrap(*((AliasWrap&)lhs).m_pRepVal * rhs.m_scalar); break;			                                    
	case AE: return Wrap(*((AliasWrap&)lhs).m_pRepVal * rhs.m_pEval->GetValue()); break;			                        
	case AL: return Wrap(*((AliasWrap&)lhs).m_pRepVal * *rhs.m_pList); break;						                        
	case AA: return Wrap(*((AliasWrap&)lhs).m_pRepVal * *((AliasWrap&)rhs).m_pRepVal); break;	                            
	case AM: return Wrap(*((AliasWrap&)lhs).m_pRepVal * rhs.m_pEe->ind); break;                                              
	case AC: return Wrap(*((AliasWrap&)lhs).m_pRepVal * rhs.m_pEColl->GetEnumValueCount()); break;                           
	case MS: return Wrap(lhs.m_pEe->ind * rhs.m_scalar); break;						                                        
	case ME: return Wrap(lhs.m_pEe->ind * rhs.m_pEval->GetValue()); break;		                                            
	case ML: return Wrap(lhs.m_pEe->ind * *rhs.m_pList); break;						                                        
	case MA: return Wrap(lhs.m_pEe->ind * *((AliasWrap&)rhs).m_pRepVal); break;	                                            
	case MM: return Wrap(lhs.m_pEe->ind * rhs.m_pEe->ind); break;                                                            
	case MC: return Wrap(lhs.m_pEe->ind * rhs.m_pEColl->GetEnumValueCount()); break;                                         
	case CS: return Wrap(lhs.m_pEColl->GetEnumValueCount() * rhs.m_scalar); break;						                    
	case CE: return Wrap(lhs.m_pEColl->GetEnumValueCount() * rhs.m_pEval->GetValue()); break;		                        
	case CL: return Wrap(lhs.m_pEColl->GetEnumValueCount() * *rhs.m_pList); break;						                    
	case CA: return Wrap(lhs.m_pEColl->GetEnumValueCount() * *((AliasWrap&)rhs).m_pRepVal); break;	                        
	case CM: return Wrap(lhs.m_pEColl->GetEnumValueCount() * rhs.m_pEe->ind); break;                                         
	default:/*case CC:*/ return Wrap(lhs.m_pEColl->GetEnumValueCount() * rhs.m_pEColl->GetEnumValueCount()); break;          
		};		                                    											                                
	}

	WRAP_BINOPDEFINE(/ )
	WRAP_BINOPDEFINE(== )
	WRAP_BINOPDEFINE(!= )
	WRAP_BINOPDEFINE(<)
	WRAP_BINOPDEFINE(<= )
	WRAP_BINOPDEFINE(>)
	WRAP_BINOPDEFINE(>= )

	WRAP_UNOPDEFINE(-)
	WRAP_UNOPDEFINE(!)

///@endcond
	//=========== alloc configuration =================================
	void UseDefaultWrapAllocator();
	void OverrideWrapPoolAllocator(BWNew newFunc, BWRelease releaseFunc, BWClearTemps clearFunc = NULL, BWAdjustStorage adjustFunc = NULL, void* extra = NULL);

};