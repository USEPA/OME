#include "SMLHandlers/MIMEParser.h"
#include <iostream>
#include <fstream>
#include <regex>

MIMEParser::MIMEParser(void)
{
}


MIMEParser::~MIMEParser(void)
{
	ClearEntryMap();
}

/** Deallocate all rawData fields in the EntryMap
*/
void MIMEParser::ClearEntryMap()
{
		//delete all allocated buffers
	std::map<STLString, MIMEEntry>::const_iterator itr;
	for(itr=m_entryMap.begin(); itr!=m_entryMap.end();++itr)
		delete[] (*itr).second.rawData;
}

/** Grab keys for represented files in MIME archive
	@return a sorted list of keys that can be used with GetDataForKey().
	@see GetDataForKey()
*/
KeyList MIMEParser::GetKeys()
{
	KeyList out;

	//iterate through the map, grabbing each key.
	std::map<STLString, MIMEEntry>::const_iterator itr;
	for(itr=m_entryMap.begin(); itr!=m_entryMap.end();++itr)
		out.push_back((*itr).first);

	//sort keys
	out.sort();
	return out;
}

/** Get decoded string representation of entry at key.
	@param key filename of file to grab.
	@return String containing decoded file data; empty string otherwise.
	@see GetKeys()
*/
STLString MIMEParser::GetDataForKey(const STLString & key)
{
	STLString ret="";
	if(m_entryMap.find(key)!=m_entryMap.end())
	{
		MIMEEntry entry=m_entryMap[key];

		if(entry.encoding=="base64")
		{
			ret=m_decoder.Decode((const char*)entry.rawData, entry.dataLength);
		}
		else
		{
			DBG_COLOR(DBG_PARTIAL_PRINT("ERR: "),DBG_RED);
			DBG_COLOR(DBG_PRINT("encoding "+entry.encoding+" not supported."),DBG_WHITE);
		}
	}
	else
	{
		DBG_COLOR(DBG_PARTIAL_PRINT("ERR: "),DBG_RED);
		DBG_COLOR(DBG_PRINT("file "+key+" not found."),DBG_WHITE);
	}
	return ret;
}

/** Parse a MIME file.
	Parsing will populate the values stored by the parser, replacing any previous data.
	@param filePath path to the MIME file.
*/
void MIMEParser::ParseFile(const STLString & filePath)
{
	std::ifstream fIn;
	fIn.open(filePath,std::ios::binary);
	if(fIn.good())
	{
		fIn.seekg(0,std::ios::end);
		int size=fIn.tellg();
		fIn.seekg(0,std::ios::beg);

		char* fileBuff=new char[size];

		fIn.read(fileBuff, size);
		if(!fIn.eof())
		{
			STLString fileStr(fileBuff,size);

			delete[] fileBuff;
			fIn.close();
			ParseData(fileStr);
		}
		else
		{
			DBG_COLOR(DBG_PARTIAL_PRINT("ERR: "),DBG_RED);
			DBG_COLOR(DBG_PRINT("copying data from file to buffer unsuccessful."),DBG_WHITE);
		}
	}
}

/**	Parse out Data passed in as a string.
	Workhorse function; slices the files out along designated boundaries and catalogs them.
	@param mimeData string of data to be parsed.
*/
void MIMEParser::ParseData(const STLString & mimeData)
{
	ClearEntryMap();

	//grab file boundary delimiter
	STLRegex boundReg("boundary=\"(.*)\"");
	STLsmatch boundMatch;
	std::regex_search(mimeData,boundMatch,boundReg);

	m_boundStr=boundMatch[1];

	//process individual file chunks
	size_t found,pos=0;
	STLString sub;

	found=mimeData.find(m_boundStr); //definition
	//first few matches will be misses

	while(found!=std::string::npos)
	{
		sub=mimeData.substr(pos,found-pos);
		LoadFileData(sub);
		pos=found+m_boundStr.length()+1;
		found=mimeData.find(m_boundStr,found+1); //definition
	}

}

/** Store individual file data in parser
	Extracts relevant information for MIMEEntry struct through regex.
	@param fileData string of data to be parsed.
*/
void MIMEParser::LoadFileData(STLString fileData)
{
	/// \todo refactor for speed improvements.
	const OMEChar* patterns[] = {
						"MIME-Version: (.*?)\n",
						"Content-ID: <(.*?)>\n",
						"Content-Disposition: .* filename=\"(.*?)\"\n",
						"Content-Description: (.*?)\n",
						"Content-ID: <(.*?)>\n",
						"Date-Modified: (.*?)\n",
						"Authentication-Code: (.*?)\n",
						"Content-Type: (.*?)\n",
						"Content-Transfer-Encoding: (.*)\n\n",
						//"((?:.|\n)*)",
						"\0"};
	//STLRegex fileReg(	"^"
	//					"MIME-Version: (.*?)\n"
	//					"(?:Content-ID: <(.*?)>\n)?"
	//					"(?:Content-Disposition: .* filename=\"(.*?)\"\n)?"
	//					"Content-Description: (.*?)\n"
	//					"(?:Content-ID: <(.*?)>\n)?"
	//					"(?:Date-Modified: (.*?)\n)?"
	//					"(?:Authentication-Code: (.*?)\n)?"
	//					"Content-Type: (.*?)\n"
	//					"Content-Transfer-Encoding: (.*)\n\n"
	//					"((?:.|\n)*)"
	//				);

	//STLsmatch fileMatch;
	//try
	//{
	//	regex_match(fileData, fileMatch, fileReg);
	//}
	//catch (std::regex_error & err)
	//{
	//	DBG_PRINTC(err.what(), DBG_RED);

	//}

	StrArray fileMatch=BatchRegex(fileData, patterns,1,true);
	if(!fileMatch.empty())
	{
		//if section matches file pattern, build a new MIMEEntry
		MIMEEntry entry;
		entry.dateModified=fileMatch[MP_MOD];
		entry.description=fileMatch[MP_DESC];
		entry.encoding=fileMatch[MP_ENCODE];
		entry.filename=fileMatch[MP_FILENAME];

		DBG_PRINT("Unpacking file"+entry.filename);
		//rawdata will likely be broken up by newlines; we need to remove them for the decoders to work.
		//step through the string and only copy non-newline characters.
		
		//use only block after header
		size_t start = fileData.find("\n\n");
		if (start != STLString::npos)
		{
			STLString preFiltStr = fileData.substr(start+2);
			unsigned char *tempBuff = new unsigned char[preFiltStr.length()];
			unsigned int ind = 0;
			for (STLString::const_iterator it = preFiltStr.begin(); it != preFiltStr.end(); it++)
			{
				if ((*it) != '\n')
					tempBuff[ind++] = (*it);
			}

			//temp buff will be a little bigger than size, but that should be ok when we delete[]
			entry.rawData = tempBuff;
			entry.dataLength = ind;

			//store entry under captured description. - will need to barrier this operation for OMP
			m_entryMap[entry.description] = entry;
		}
	}
}

/** Duplicate values from another parser
	@param mp the MIMEParser to copy.
*/
void MIMEParser::Duplicate(const MIMEParser & mp)
{
	m_boundStr=mp.m_boundStr;

	ClearEntryMap();
	std::map<STLString, MIMEEntry>::const_iterator itr;
	for(itr=mp.m_entryMap.begin(); itr!=mp.m_entryMap.end();++itr)
		m_entryMap[(*itr).first]=MIMEEntry((*itr).second);
}