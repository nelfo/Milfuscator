#include "MAPFile.hpp"

VOID MAPFile::Parse(MAPData* data)
{
	lastError = MAPERROR_NONE;

	if(Open() == false) {
		lastError = MAPERROR_NO_FILE;

		return;
	}

	DWORD dwFileSize = 0;

	content = ReadToEnd(&dwFileSize);

	Close();

	std::istringstream i;

	i.str(content);

	readMode = MAPMODE_NONE;

	int idx = 0;
	for(std::string line; std::getline(i, line);) {
		if(line.length() == 0)
			continue;

		if(line.substr(line.length() - 1).compare("\x0D") == 0) {
			line.pop_back(); // Remove junk (if needed? Might want to check...)
		}

		MAPParseLineResult r;

		ParseLine(data, idx, line, &r);

		// Line numbers and beyond is not done yet by me.
		// I don't really need them, maybe some other time
		if(readMode == MAPMODE_LINENUMBERS)
			break;

		if(r == MAPPARSE_OK) {
			idx++;
		} else if(r == MAPPARSE_FAILURE) {
			// Ignore the failure? or break?
			break;
		}
	}
}

VOID MAPFile::ParseLine(MAPData *data, int index, std::string line, MAPParseLineResult* result)
{
	*result = MAPPARSE_OK;

	if(line.length() == 0)
		return;

	if(line.substr(0, 1).compare("\x20") == 0) {
		line = line.substr(1);
	}

	if(line.length() == 0)
		return;

	std::vector<std::string> linedata = ParseNonWhiteSpace(line);

	if(linedata.size() == 0)
		return;

	if(readMode == MAPMODE_NONE && index == 0) {
		data->modulename = line;
		readMode = MAPMODE_TIMESTAMP;
		return;
	}

	if(readMode == MAPMODE_TIMESTAMP) {
		if(linedata[0].compare("Timestamp") == 0 && linedata[1].compare("is") == 0) {
			HexStringToDword(linedata[2], &data->timestamp); // I thought this was decimal at first... silly

			data->time = line.substr(line.find_last_of("(") + 1, line.length());
			data->time = data->time.substr(0, data->time.find_last_of(")"));

			readMode = MAPMODE_LOADADDRESS;
			return;
		}
	}

	if(readMode == MAPMODE_LOADADDRESS) {
		if(linedata[0].compare("Preferred") == 0 && linedata[1].compare("load") == 0) {
			HexStringToDword(linedata[linedata.size() - 1], &data->loadaddress);
			readMode = MAPMODE_SECTION;
			return;
		}
	}

	if(readMode == MAPMODE_SECTION) {
		if(linedata[0].compare("Start") == 0) {
			return; // return OK, but parse nothing
		}

		if(linedata[0].compare("Address") == 0) {
			readMode = MAPMODE_PUBLIC_SYMBOLS;
			return;
		}

		Section ns;
		AddressStringToDword(linedata[0], NULL, &ns.offset); // Do we need the data from the first bit? [0001:00000000]
		HexStringToDword(linedata[1], &ns.length);
		ns.name = ns.cname = linedata[2];
		ns.library.clear();
		ns.object.clear();
		ns.type = DataType::DT_DATA;
		ns.typeclass = (linedata[3].compare("CODE") == 0) ? TypeClass::CLASS_CODE : TypeClass::CLASS_DATA;

		data->sections.push_back(ns);
		return;
	}

	if(readMode == MAPMODE_PUBLIC_SYMBOLS) {
		if(linedata[0].compare("entry") == 0 && linedata[1].compare("point") == 0) {
			AddressStringToDword(linedata[linedata.size() - 1], NULL, &data->entrypoint);
			readMode = MAPMODE_STATIC_SYMBOLS;
			return;
		}

		ParseDataLine(data, line, linedata);
		return;
	}

	if(readMode == MAPMODE_STATIC_SYMBOLS) {
		if(linedata[0].compare("Static") == 0 && linedata[1].compare("symbols") == 0) {
			return;
		}

		if(linedata[0].compare("Line") == 0 && linedata[1].compare("numbers") == 0) {
			readMode = MAPMODE_LINENUMBERS;
			return;
		}

		ParseDataLine(data, line, linedata);
		return;
	}

	*result = MAPPARSE_FAILURE;
}

VOID MAPFile::ParseDataLine(MAPData *data, std::string line, std::vector<std::string> linedata)
{
	Data ds;

	bool is_func = (linedata[3].compare("f") == 0);

	// There's also an "i" flag on some projects after "f"
	// I have no idea what it means, no documentation, no spec
	// It keeps me up some nights

	AddressStringToDword(linedata[0], NULL, &ds.offset);
	ds.name = linedata[1];
	ds.cname = ""; // We need a demangler for this
	ds.type = (is_func) ? DataType::DT_FUNCTION : DataType::DT_DATA;
	ds.length = 0;

	std::string libdata = linedata[linedata.size() - 1];

	if(libdata.compare("module") == 0 && linedata[linedata.size() - 2].compare("CIL") == 0) {
		ds.library = "CIL library: CIL module"; // Would have messed up parsing
	} else {
		size_t libsplit = libdata.find_first_of(":");

		if(libsplit != -1) {
			ds.object = libdata.substr(0, libsplit);
			ds.library = libdata.substr(libsplit + 1);
		} else {
			ds.library = libdata;
		}
	}

	if(is_func) {
		data->functions.push_back(ds);
	}

	// We want to push functions AND data to the data vector
	// Why? Because.
	data->data.push_back(ds);
}

VOID MAPFile::HexStringToDword(std::string s, DWORD* out)
{
	std::stringstream ss;
	ss << std::hex << s;
	ss >> *out;
}

VOID MAPFile::AddressStringToDword(std::string s, DWORD* out1, DWORD* out2)
{
	if(out1 != NULL) 
		HexStringToDword(s.substr(0, 4), out1);

	if(out2 != NULL)
		HexStringToDword(s.substr(5), out2);
}

std::vector<std::string> MAPFile::ParseNonWhiteSpace(std::string line)
{
	std::vector<std::string> v;

	std::istringstream ss(line);

	for(std::string word; ss >> word;) {
		v.push_back(word);
	}

	return v;
}