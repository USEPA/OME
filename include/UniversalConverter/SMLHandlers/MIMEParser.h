#pragma once
#include "Base64Decoder.h"
#include <list>

typedef std::list<STLString> KeyList;

/** Parses and stores data from a MIME file.
	Assumes unsigned chars due to direct memory access
*/
class MIMEParser
{
public:
	/** Labeled indices for parsing regex results*/
	enum {MP_ALL=0, MP_VERSION, MP_ID1, MP_FILENAME, MP_DESC, MP_ID2,MP_MOD,MP_AUTH,MP_TYPE,MP_ENCODE,MP_DATA};

	struct MIMEEntry				///< represents an entry in a MIME file.
	{
		STLString filename;			///< name of encoded file.
		STLString description;		///< description of encoded file.
		STLString dateModified;		///< last date modified date/timestamp of encoded file.
		unsigned char* rawData;		///< raw encoded data.
		int dataLength;				///< length of rawData buffer.
		STLString encoding;			///< encoding algorithm used to encode file

		/** Default Constructor. */
		MIMEEntry() :rawData(NULL),dataLength(0) {};
		/** Copy Constructor. */
		MIMEEntry(const MIMEEntry & me)
		 :filename(me.filename),description(me.description),dateModified(me.dateModified),dataLength(me.dataLength),encoding(me.encoding)
		{
			rawData=new unsigned char[dataLength];
			memcpy(rawData,me.rawData,dataLength);
		};

	};

	
	MIMEParser(void);

	/** Copy Constructor.
		@param mp The MIMEParser to copy.
	*/
	MIMEParser(const MIMEParser & mp) { Duplicate(mp); };
	/** Assignment operator.
		@param mp The MIMEParser to copy.
		@return a reference to this MIMEParser.
	*/
	MIMEParser& operator=(const MIMEParser & mp) { Duplicate(mp); return *this; };

	virtual ~MIMEParser(void);

	void ParseFile(const STLString & filePath);
	void ParseData(const STLString & mimeData);

	KeyList GetKeys();
	STLString GetDataForKey(const STLString & key);

protected:
	STLString m_boundStr;							///< string that represents boundary between files.
	Base64Decoder m_decoder;						///< base64 decoder for accessing decoded rawData.
	std::map<STLString, MIMEEntry> m_entryMap;	///< map of files stored in MIME; referenced by filename

	void ClearEntryMap();
	void LoadFileData(STLString fileData);

	void Duplicate(const MIMEParser & mp);
};

