#pragma once

class C_Files
{
public:
	bool LoadFile( const char* szName, std::vector < BYTE >& cVecData );
	void SaveFile( const char* szName, std::vector < BYTE >& cVecData );
};

inline C_Files* g_Files = new C_Files;