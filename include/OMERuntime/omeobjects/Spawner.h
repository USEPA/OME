#pragma once
#include "Evaluable.h"
#include <random>
#include <bitset>
class BaseManager;

/** Evaluable object used to evaluate the number of submodel instances to add or remove during run. */
class __EXPORT__ Spawner : public Evaluable
{
public:
	Spawner(void);
	Spawner(STLString name, STLString desc, bool runOnce=false,bool perInst=false, bool isLoss=false, bool stoch=false,bool cond=false);
	
	/** Copy Constructor. */
	Spawner(const Spawner & spwn) { Duplicate(spwn); };

	/** XML Constructor
		@param pCurrElem The current xml Element
		@param tag Tag to identify element.
		@param filename Name of originating file.
	*/
   Spawner(TI2Element* pCurrElem,const STLString & tag, const STLString & filename) 
   {
	   OMEObject::PopulateFromXML(pCurrElem,tag,filename);
	   CheckConstant();
   }

 
  /** Assignment Operator.
		@param rhs The Spawner to use in the assignment.
		@return A reference to the current Spawner.
	*/
	Spawner & operator=(const Spawner & rhs) { Duplicate(rhs); return *this; };

	bool operator==(const Spawner & rhs);

	virtual ~Spawner(void);

	/// @return true if Spawner only runs on initialization. false otherwise.
	inline bool RunsOnce() const { return m_actionFlags[SB_INIT_ONLY]; };
	/// @param ro Whether or not to run once.
	inline void SetRunOnce(const bool ro) { m_actionFlags[SB_INIT_ONLY] = ro; };				

	/// @return true if Spawner uses Stochastic calculations. false otherwise.
	inline bool const IsStochastic() const {return m_actionFlags[SB_IS_STOCHASTIC]; };		
	/// @param iss Whether or not to use stochastic calculations.
	inline void SetIsStochastic(const bool iss) { m_actionFlags[SB_IS_STOCHASTIC]=iss; };	

	/// @return true if Spawner is run on each instance of the parent model. false otherwise.
	inline bool const GetPerInstance() const {return m_actionFlags[SB_PER_INSTANCE]; };		
	/// @param pinst Whether or not to run Spawner on a per-instance basis.
	inline void SetPerInstance(const bool pinst) { m_actionFlags[SB_PER_INSTANCE]=pinst; };	

	/// @return true if Spawner kills instances instead of creating them. false otherwise.
	inline bool const IsLoss() const {return m_actionFlags[SB_IS_LOSS]; };	
	/// @param il Whether or not Spawner causes instance losses instead of gains.
	inline void SetIsLoss(const bool il) { m_actionFlags[SB_IS_LOSS]=il; };			

	/// @return true if Spawner determines model's fate based on a conditional statement. false otherwise.
	inline bool const IsConditional() const {return m_actionFlags[SB_CONDITIONAL]; };	
	/// @param ic Whether or not Spawner causes instance losses instead of gains.
	inline void SetConditional(const bool ic) { m_actionFlags[SB_CONDITIONAL]=ic; };				

	// XML Stuff
	virtual STLString GetXMLRepresentation(const unsigned int indent=0, const unsigned int inc=4);
	virtual void GetXMLAttributes(StrArray & out) const;
	virtual void GetXMLSubNodes(StrArray & out,const unsigned int indent=0,const unsigned int inc=4);
	virtual const OMEChar* GetXMLTag() const { return "spawner"; }

	//Evaluable
	virtual OME_SCALAR Evaluate(const OME_SCALAR time,BaseManager* pBm);
	OME_SCALAR EvalWithVal(const OME_SCALAR & value);

	virtual OME_SCALAR Initialize(const OME_SCALAR time, BaseManager* pBm);
	virtual void BatchInitialize(const OME_SCALAR time,BaseManager* pBm);

	virtual void Reset(BaseManager* pBm);

	virtual void EventAction(const CentralEvent & ev, void* extra);
	virtual unsigned int Prioritize(unsigned int currPriority, const bool isInitial=false);
	//OMEObject
	virtual OMETYPE GetOMEType() const { return OME_SPAWNER; };
	virtual bool IsOMEType(OMETYPE type) const { return type == OME_SPAWNER || Evaluable::IsOMEType(type); };
	//virtual OME_SCALAR GetValue() { return (!m_initOnly || !m_hasRun) ? m_value : 0.0; }

	//factories
	/** Factory Function.
		Returns the equivalent of Simile's Initialisation channel node.
		@param name The name of the Spawner.
		@param desc The Description of the Spawner.
		@return pointer to the newly allocated Spawner.
	*/
	static Spawner* BuildInitSpawn(STLString name, STLString desc) { return new Spawner(name, desc, true,false,false,false); }
	/** Factory Function.
		Returns the equivalent of Simile's Migration channel node.
		@param name The name of the Spawner.
		@param desc The Description of the Spawner.
		@return pointer to the newly allocated Spawner.
	*/
	static Spawner* BuildAccumSpawn(STLString name, STLString desc) { return new Spawner(name, desc, false,false,false,false); }
	/** Factory Function.
		Returns the equivalent of Simile's Reproduction channel node.
		@param name The name of the Spawner.
		@param desc The Description of the Spawner.
		@return pointer to the newly allocated Spawner.
	*/
	static Spawner* BuildReproSpawn(STLString name, STLString desc) { return new Spawner(name, desc, false,true,false,false); }
	/** Factory Function.
		Returns the equivalent of Simile's Extermination channel node.
		@param name The name of the Spawner.
		@param desc The Description of the Spawner.
		@return pointer to the newly allocated Spawner.
	*/
	static Spawner* BuildDeathSpawn(STLString name, STLString desc) { return new Spawner(name, desc, false,true,true,true); }

	/** Factory Function.
		Returns the equivalent of Simile's Condition channel node.
		@param name The name of the Spawner.
		@param desc The Description of the Spawner.
		@return pointer to the newly allocated Spawner.
	*/
	static Spawner* BuildCondSpawn(STLString name, STLString desc) { return new Spawner(name, desc, false,false,false,false,true); }

	virtual Spawner* Clone() { return new Spawner(*this); }

	/** Record an Instance index as a child of this Spawner.
		@param ind The index to record as a child instance.
	*/
	inline void AddSpwnInd(const size_t & ind) { m_spawnedInstances.push_back(ind); };
	bool DidSpawnInst(const size_t & ind) const;
protected:
	virtual void GetXMLAttributeMap(XMLAttrVector & out);
	virtual int PopulateFromComplexElements(TI2Element* pCurrElem,const STLString & tag, const STLString & filename);
	void Duplicate(const Spawner & spwn);
   virtual void CheckConstant() { SetIsConstant(false); }

private:
	enum {
			SB_INIT_ONLY=0,			///< If set, spawner runs only once (ie initialization).
			SB_IS_STOCHASTIC,		///< Flag for probability-based modification to model count
			SB_PER_INSTANCE,		///< Evaluate on a per-instance basis.
			SB_IS_LOSS,				///< Represents loss if true.
			SB_CONDITIONAL,			///< Action is taken if the current Spawner evaluates to true

			SB_BITCOUNT				///< The total number of spawner states
		};

	std::bitset<SB_BITCOUNT> m_actionFlags;		///< Keeps track of all the flags which dictate how a spawner behaves.

	OME_SCALAR m_accumulator;			/**< Internal accumulator that mimics Simile behavior.
										Spawn occurs for each whole value that arises during accumulation.
									*/
	std::list<size_t> m_spawnedInstances;		///< List of indices that point to ModelInstances that were created through a given Spawner.

	static std::default_random_engine s_rEngine;		///< Random engine used for Stochastic Spawners.
	static std::normal_distribution<OME_SCALAR> s_nDist;	///< Normal Distribution used with Stochastic Spawners, ranging from 0 to 1.

	void SpawnCopies(const unsigned int & numCopies, const int & instIndex=-1);
	unsigned int GetNumCopies(const OME_SCALAR & inVal);

	void EvaluateConditional(const OME_SCALAR time,BaseManager* pBm);
};
