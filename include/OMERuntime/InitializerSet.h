#pragma once
#include "OMEDefines.h"
#include <map>
#include "tinyxml2.h"

typedef std::map<STLString,OME_SCALAR> SimpleValMap;


class Model;

/** Entry for a model to be initialized*/
struct ISModelEntry
{
	STLString m_modelName;									///< Name of model to be initialized.
	SimpleValMap m_values;									///< Name-value pairs of initial values to be applied
	DSEntryMap m_dataSources;								///< Map of data sources.
	std::map<STLString,ISModelEntry> m_subEntries;	///< Subentries to be applied to submodels.

};

typedef std::map<STLString,ISModelEntry> ISModelMap;

/** Holds initialization information for models.

*/
class InitializerSet
{


public:
	InitializerSet();
	InitializerSet(const STLString filepath);

	bool LoadFromFile(const STLString filepath);
	/** @return true if there are no entries in the InitializerSet; false otherwise. */
	inline bool IsEmpty() const { return !m_isPopulated || m_mappings.empty(); }
	/** @return true if field entries were loaded successfully; false otherwise. */
	inline bool LoadSuccessful() const { return m_isPopulated; };

	void ApplySinglesToModel(Model* pModel);
	void ApplyColumnsToModel(Model* pModel);

	bool InitializeEval(Evaluable* pEval);
private:
	STLString m_dir;			///< Relative directory where param file is found.
	bool m_isPopulated;			///< Flag that indicates that InitilizerSet has been populated with initial values.
	ISModelMap m_mappings;		///< Map of values and where to direct them.

	int ProcessSPF(const STLString filepath);
	void ProcessSPFLevel(TI2Element *pCurrElem, ISModelMap & currMap);
	static void ApplyISLevel(const ISModelEntry & entry, Model* pModel,const bool colPass);

	/** Utility function that makes removes illegal characters and whitespaces from labels.
		@param inLabel The label to scrub.
		@return A STLString containing the scrubbed label.
	*/
	static inline STLString ScrubLabel(const OMEChar* inLabel)
	{
				//replace spaces with underscores
		STLString label=inLabel;
		//std::replace_if(label.begin(),label.end(),[](char val){return val==' ';},'_');
		static STLRegex reg("(?:\\r\\n)|[\\n\\r]");
		label=std::regex_replace(label, reg, STLString(" "));
		
		//prepend name that starts with number with n_
		/*if (std::isdigit(label[0]))
			label = STLString("n_") + label;
		*///chop any newline chars off of the end
		//unsigned int resize=label.size();
		//for(; resize>=0; resize--)
		//{
		//	if(label[resize-1]!='\n' && label[resize-1]!='\l' && label[resize-1]!='\r')
		//		break;
		//}
		//label.resize(resize);

		return label;
	}
};
