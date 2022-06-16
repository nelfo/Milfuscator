#pragma once

struct S_FunctionData
{
	std::string m_cFnName {};
	DWORD m_dwFunctionOffset {};
	DWORD m_dwFunctionSize {};
	
	S_FunctionData( std::string& cFnName, DWORD dwOffset, DWORD dwSize )
	{
		m_cFnName = cFnName;
		m_dwFunctionOffset = dwOffset;
		m_dwFunctionSize = dwSize;
	}
};

class C_PE
{
public:
	bool LoadMappedPE( std::vector < BYTE >& cVecOut, const char* pPePath );
	void LoadFunctionsList( std::vector<S_FunctionData>& cVecOut, std::vector < BYTE >& cVecImage, const char* szMapFilePath );
	PIMAGE_SECTION_HEADER GetSections( std::vector < BYTE >& cVecIn );
	PIMAGE_NT_HEADERS GetNtHeaders( std::vector < BYTE >& cVecIn );
};

inline C_PE* g_PE = new C_PE;