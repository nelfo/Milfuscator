#pragma once

enum E_ObfuscationType : int
{
	OT_NONE,
	
	OT_MOV,
	OT_MOV_RR,
	OT_MOV_ESP_AVAL,
	OT_MOV_R_ESP,
	OT_MOV_MEM_AVAL,
	OT_MOV_R_MEM,

	OT_LEA_RM,

	OT_PUSH_R,
	OT_PUSH_C,

	OT_CMP_RC,

	OT_ADD_RC,
	OT_ADD_RR,

	OT_SUB_RC,
	OT_SUB_RR,

	OT_RET_DEF,
};

enum E_Operations
{
	OP_ROL,
	OP_ROR,
	OP_BSWAP,
};

struct S_InsnObfData
{
	E_ObfuscationType m_eType;
	S_AnalyzedInstruction m_sInsn;
	asmjit::_abi_1_9::BaseNode* m_pNode;

	S_InsnObfData( ) { memset( this, 0x00, sizeof( *this ) ); }
};

class C_Obfuscator
{
public:
	E_ObfuscationType CheckInstruction( S_AnalyzedInstruction& sInsn );
	void ProcessObfuscation( S_InsnObfData& sData, asmjit::x86::Builder& sBuilder );

private:
	void GenerateDecryptCode( asmjit::x86::Builder& sBuilder, asmjit::_abi_1_9::x86::Gpd cReg, DWORD dwValue );
	void GenerateDecryptPush( asmjit::x86::Builder& sBuilder, DWORD dwValue );
	void GenerateRandomPush( asmjit::x86::Builder& sBuilder );

private:
	asmjit::_abi_1_9::x86::Gpd ZyReg2Aj( ZydisRegister eReg );
	asmjit::_abi_1_9::x86::Gpd GetRandomReg( );
	asmjit::_abi_1_9::x86::Gpd GetRandomRegNoEsp( );
};

inline C_Obfuscator* g_Obfuscator = new C_Obfuscator;