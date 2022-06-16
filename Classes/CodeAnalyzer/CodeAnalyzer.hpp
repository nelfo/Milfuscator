#pragma once

enum E_InstructionType : int
{
	IT_DEFAULT,
	IT_NEED_TO_FIX,
};

struct S_AnalyzedInstruction
{
	ZydisDecodedInstruction m_cDecodedInstruction;
	ZydisDecodedOperand m_cDecodedOperands[ ZYDIS_MAX_OPERAND_COUNT_VISIBLE ];
	DWORD m_dwInstructionOffset;

	E_InstructionType m_eType;

	BYTE m_btInstructionBytes[ 15 ];
	char m_chInstructionString[ 256 ];

	S_AnalyzedInstruction( )
	{
		memset( this, 0x00, sizeof( *this ) );
	}
};

struct S_AnalyzedFunction
{
	std::vector < S_AnalyzedInstruction > m_cAnalyzedInstructions;
	DWORD m_dwFnOffset;

	S_AnalyzedFunction( )
	{
		m_cAnalyzedInstructions = { };
		m_dwFnOffset = 0;
	}
};

struct S_JumpCallInfo
{
	ZydisMnemonic m_eJumpMnem;
	asmjit::_abi_1_9::Label m_cJumpLabel;
	DWORD m_dwOldJumpSize;

	S_JumpCallInfo( ) { memset( this, 0x00, sizeof( *this ) ); }
	S_JumpCallInfo( ZydisMnemonic eMnem, asmjit::_abi_1_9::Label& cLabel, DWORD dwOldJumpSize )
	{
		m_eJumpMnem = eMnem;
		m_cJumpLabel = cLabel;
		m_dwOldJumpSize = dwOldJumpSize;
	}
};

struct S_RvaFix
{
	DWORD m_dwPtrTo;
	asmjit::_abi_1_9::Label m_cLabel;

	S_RvaFix( ) { memset( this, 0x00, sizeof( *this ) ); }
	S_RvaFix( DWORD dwPtrTo, asmjit::_abi_1_9::Label cLabel )
	{
		m_dwPtrTo = dwPtrTo;
		m_cLabel = cLabel;
	}
};

struct S_RelocIntersectionsData
{
	asmjit::_abi_1_9::Label m_cLabel;
	asmjit::_abi_1_9::Label m_cLabelTo;
	int m_iOffset;

	S_RelocIntersectionsData( ) { memset( this, 0x00, sizeof( *this ) ); }
	S_RelocIntersectionsData( asmjit::_abi_1_9::Label cLabel, asmjit::_abi_1_9::Label cLabelTo, int iOffset )
	{
		m_cLabel = cLabel;
		m_cLabelTo = cLabelTo;
		m_iOffset = iOffset;
	}
};

struct S_InsnObfData;

class C_CodeAnalyzer
{
public:
	void AnalyzeFunction( std::vector < BYTE >& cVecImage, S_FunctionData& sFnData, S_AnalyzedFunction& cOutData );
	void AnalyzeRelocs( asmjit::x86::Builder& b, std::vector < BYTE >& cVecImage, std::map < DWORD, DWORD >& cRelocMap, std::map < DWORD, asmjit::_abi_1_9::Label >& cRelocLabelsMap );
	void FindRelocsIntersections( asmjit::x86::Builder& b, std::map < DWORD, S_RelocIntersectionsData >& cMapOutData, std::map < DWORD, DWORD >& cRelocMap, std::map < DWORD, asmjit::_abi_1_9::Label >& cRelocLabelsMap );
	void AnalyzeAllFunctions( asmjit::x86::Builder& b, std::vector < S_FunctionData >& cVecFns, std::vector < S_AnalyzedFunction >& cVecOutData, std::vector < BYTE >& cVecImage, std::map < DWORD, asmjit::_abi_1_9::Label >& cGlobalLabels );
	void GenerateCallJumpsInfo( std::vector < S_AnalyzedFunction >& cVecAnalyzedFns, std::vector < BYTE >& cVecImage, std::map < DWORD, S_JumpCallInfo >& cMapJumps, std::map < DWORD, asmjit::_abi_1_9::Label >& cGlobalLabelsMap );
	void GenerateObfData( std::map < DWORD, S_InsnObfData >& cVecOutData, std::vector < BYTE >& cVecImage, std::map < DWORD, DWORD >& cRelocMap, std::vector < S_AnalyzedFunction >& cVecAnalyzedFns );
};

inline C_CodeAnalyzer* g_CodeAnalyzer = new C_CodeAnalyzer;