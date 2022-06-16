#pragma once
#include <Windows.h>
#include <vector>
#include <sstream>
#include <string>

#include "DebugTypes.hpp"
#include "File.hpp"

struct MAPData
{
	std::string modulename;
	DWORD timestamp;
	DWORD loadaddress;
	DWORD entrypoint;
	std::string time;
	std::vector<Data> data;
	std::vector<Data> functions;
	std::vector<Section> sections;
};

class MAPFile : public File
{
public:
	enum MAPError
	{
		MAPERROR_NONE = 0,
		MAPERROR_NO_FILE,
		MAPERROR_READFILE
	};

	enum MAPParseLineResult
	{
		MAPPARSE_OK = 0,
		MAPPARSE_FAILURE
	};

	enum MAPReadMode
	{
		MAPMODE_NONE = 0,
		MAPMODE_TIMESTAMP,
		MAPMODE_LOADADDRESS,
		MAPMODE_SECTION,
		MAPMODE_PUBLIC_SYMBOLS,
		MAPMODE_STATIC_SYMBOLS,
		MAPMODE_LINENUMBERS,
		MAPMODE_FIXUPS
	};

	MAPFile(std::string filename) : File(filename, FILE_GENERIC_READ, OPEN_EXISTING) {}

	VOID Parse(MAPData *data);
	VOID ParseLine(MAPData *data, int index, std::string line, MAPParseLineResult* result);
	VOID ParseDataLine(MAPData *data, std::string line, std::vector<std::string> linedata);
	VOID ParsePublicLine(int index, std::string line);
	VOID HexStringToDword(std::string s, DWORD* out);
	VOID AddressStringToDword(std::string s, DWORD* out1, DWORD* out2);
	std::vector<std::string> ParseNonWhiteSpace(std::string line);

private:
	std::string content;
	MAPReadMode readMode;
	MAPError lastError;
};