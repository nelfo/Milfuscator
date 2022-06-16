#pragma once

class C_Rebuilder
{
public:
	void LiftRebuildBinary( std::vector < BYTE >& cVecMappedImage, asmjit::x86::Builder& b, std::map < DWORD, DWORD >& cRelocMap, std::map < DWORD, asmjit::_abi_1_9::Label >& cRelocLabelsMap, std::map < DWORD, asmjit::_abi_1_9::Label >& cGlobalLabelsMap, std::map < DWORD, S_RvaFix >& cRvaFixesMap, std::map < DWORD, S_InsnObfData >& cObfDatasMap, std::map < DWORD, S_RelocIntersectionsData >& cRelocIntersectionsMap, std::map < DWORD, S_JumpCallInfo >& cMapJumps, asmjit::_abi_1_9::Label& cBaseLabel );
	void InsertIntersectionsData( asmjit::x86::Builder& b, std::map < DWORD, S_RelocIntersectionsData >& cRelocIntersectionsMap, asmjit::_abi_1_9::Label& cBaseLabel, std::string& cRebuilderDataBeginStr );
	void ProcessIntersectionsData( std::vector < BYTE >& cVecMappedImage, std::map < DWORD, S_RelocIntersectionsData >& cRelocIntersectionsMap, std::string& cRebuilderDataBeginStr );
};

inline C_Rebuilder* g_Rebuilder = new C_Rebuilder;