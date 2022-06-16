#include "..\Classes.hpp"

E_ObfuscationType C_Obfuscator::CheckInstruction( S_AnalyzedInstruction& sInsn )
{
	auto& sDecodedInsn = sInsn.m_cDecodedInstruction;
	auto& sDecodedOperands = sInsn.m_cDecodedOperands;

	if ( sDecodedInsn.mnemonic == ZYDIS_MNEMONIC_MOV &&
		sDecodedOperands[ 0 ].type == ZYDIS_OPERAND_TYPE_REGISTER &&
		ZydisRegisterGetWidth( ZYDIS_MACHINE_MODE_LONG_COMPAT_32, sDecodedOperands[ 0 ].reg.value ) == 32 &&
		sDecodedOperands[ 1 ].type == ZYDIS_OPERAND_TYPE_IMMEDIATE )
		return OT_MOV;
	
	/*if ( sDecodedInsn.mnemonic == ZYDIS_MNEMONIC_PUSH &&
		sDecodedOperands[ 0 ].type == ZYDIS_OPERAND_TYPE_REGISTER )
		return OT_PUSH_R;*/

	
	if ( sDecodedInsn.mnemonic == ZYDIS_MNEMONIC_MOV &&
		sDecodedOperands[ 0 ].type == ZYDIS_OPERAND_TYPE_REGISTER &&
		sDecodedOperands[ 1 ].type == ZYDIS_OPERAND_TYPE_REGISTER &&
		ZydisRegisterGetWidth( ZYDIS_MACHINE_MODE_LONG_COMPAT_32, sDecodedOperands[ 0 ].reg.value ) == 32 &&
		ZydisRegisterGetWidth( ZYDIS_MACHINE_MODE_LONG_COMPAT_32, sDecodedOperands[ 1 ].reg.value ) == 32 )
		return OT_MOV_RR;

	
	if ( sDecodedInsn.mnemonic == ZYDIS_MNEMONIC_PUSH &&
		sDecodedOperands[ 0 ].type == ZYDIS_OPERAND_TYPE_IMMEDIATE )
		return OT_PUSH_C;

	
	if ( sDecodedInsn.mnemonic == ZYDIS_MNEMONIC_CMP &&
		sDecodedOperands[ 0 ].type == ZYDIS_OPERAND_TYPE_REGISTER &&
		ZydisRegisterGetWidth( ZYDIS_MACHINE_MODE_LONG_COMPAT_32, sDecodedOperands[ 0 ].reg.value ) == 32 &&
		sDecodedOperands[ 1 ].type == ZYDIS_OPERAND_TYPE_IMMEDIATE )
		return OT_CMP_RC;
	
	
	if ( sDecodedInsn.mnemonic == ZYDIS_MNEMONIC_ADD &&
		sDecodedOperands[ 0 ].type == ZYDIS_OPERAND_TYPE_REGISTER &&
		ZydisRegisterGetWidth( ZYDIS_MACHINE_MODE_LONG_COMPAT_32, sDecodedOperands[ 0 ].reg.value ) == 32 &&
		sDecodedOperands[ 1 ].type == ZYDIS_OPERAND_TYPE_IMMEDIATE )
		return OT_ADD_RC;

	
	if ( sDecodedInsn.mnemonic == ZYDIS_MNEMONIC_ADD &&
		sDecodedOperands[ 0 ].type == ZYDIS_OPERAND_TYPE_REGISTER &&
		sDecodedOperands[ 1 ].type == ZYDIS_OPERAND_TYPE_REGISTER &&
		ZydisRegisterGetWidth( ZYDIS_MACHINE_MODE_LONG_COMPAT_32, sDecodedOperands[ 0 ].reg.value ) == 32 &&
		ZydisRegisterGetWidth( ZYDIS_MACHINE_MODE_LONG_COMPAT_32, sDecodedOperands[ 1 ].reg.value ) == 32 )
		return OT_ADD_RR;

	
	if ( sDecodedInsn.mnemonic == ZYDIS_MNEMONIC_SUB &&
		sDecodedOperands[ 0 ].type == ZYDIS_OPERAND_TYPE_REGISTER &&
		ZydisRegisterGetWidth( ZYDIS_MACHINE_MODE_LONG_COMPAT_32, sDecodedOperands[ 0 ].reg.value ) == 32 &&
		sDecodedOperands[ 1 ].type == ZYDIS_OPERAND_TYPE_IMMEDIATE )
		return OT_SUB_RC;

	if ( sDecodedInsn.mnemonic == ZYDIS_MNEMONIC_SUB &&
		sDecodedOperands[ 0 ].type == ZYDIS_OPERAND_TYPE_REGISTER &&
		sDecodedOperands[ 1 ].type == ZYDIS_OPERAND_TYPE_REGISTER &&
		ZydisRegisterGetWidth( ZYDIS_MACHINE_MODE_LONG_COMPAT_32, sDecodedOperands[ 0 ].reg.value ) == 32 &&
		ZydisRegisterGetWidth( ZYDIS_MACHINE_MODE_LONG_COMPAT_32, sDecodedOperands[ 1 ].reg.value ) == 32 )
		return OT_SUB_RR;

	if ( sDecodedInsn.mnemonic == ZYDIS_MNEMONIC_MOV &&
		sDecodedOperands[0].type == ZYDIS_OPERAND_TYPE_MEMORY &&
		sDecodedOperands[0].size == 32 &&
		sDecodedOperands[ 0 ].mem.base == ZYDIS_REGISTER_ESP &&
		sDecodedOperands[ 0 ].mem.index == ZYDIS_REGISTER_NONE &&
		sDecodedOperands[ 0 ].mem.scale == 0
		)
		return OT_MOV_ESP_AVAL;

	
	if ( sDecodedInsn.mnemonic == ZYDIS_MNEMONIC_MOV &&
		sDecodedOperands[ 0 ].type == ZYDIS_OPERAND_TYPE_REGISTER &&
		ZydisRegisterGetWidth( ZYDIS_MACHINE_MODE_LONG_COMPAT_32, sDecodedOperands[ 0 ].reg.value ) == 32 &&
		sDecodedOperands[ 1 ].type == ZYDIS_OPERAND_TYPE_MEMORY &&
		sDecodedOperands[ 1 ].mem.base == ZYDIS_REGISTER_ESP &&
		sDecodedOperands[ 1 ].mem.index == ZYDIS_REGISTER_NONE &&
		sDecodedOperands[ 1 ].mem.scale == 0 )
		return OT_MOV_R_ESP;

	if ( sDecodedInsn.mnemonic == ZYDIS_MNEMONIC_LEA &&
		sDecodedOperands[ 0 ].type == ZYDIS_OPERAND_TYPE_REGISTER &&
		ZydisRegisterGetWidth( ZYDIS_MACHINE_MODE_LONG_COMPAT_32, sDecodedOperands[ 0 ].reg.value ) == 32 &&
		sDecodedOperands[ 1 ].type == ZYDIS_OPERAND_TYPE_MEMORY &&
		sDecodedOperands[ 1 ].mem.base != ZYDIS_REGISTER_NONE &&
		sDecodedOperands[ 1 ].mem.index == ZYDIS_REGISTER_NONE &&
		sDecodedOperands[ 1 ].mem.scale == 0 &&
		sDecodedOperands[ 1 ].mem.base != sDecodedOperands[ 0 ].reg.value )
		return OT_LEA_RM;

	if ( sDecodedInsn.mnemonic == ZYDIS_MNEMONIC_MOV &&
		sDecodedOperands[ 0 ].type == ZYDIS_OPERAND_TYPE_MEMORY &&
		sDecodedOperands[ 0 ].size == 32 &&
		sDecodedOperands[ 0 ].mem.base != ZYDIS_REGISTER_NONE &&
		sDecodedOperands[ 0 ].mem.base != ZYDIS_REGISTER_ESP &&
		sDecodedOperands[ 0 ].mem.scale == 0 &&
		sDecodedOperands[ 0 ].mem.index == ZYDIS_REGISTER_NONE )
		return OT_MOV_MEM_AVAL;

	if ( sDecodedInsn.mnemonic == ZYDIS_MNEMONIC_MOV &&
		sDecodedOperands[ 0 ].type == ZYDIS_OPERAND_TYPE_REGISTER &&
		ZydisRegisterGetWidth( ZYDIS_MACHINE_MODE_LONG_COMPAT_32, sDecodedOperands[ 0 ].reg.value ) == 32 &&
		sDecodedOperands[ 1 ].type == ZYDIS_OPERAND_TYPE_MEMORY &&
		sDecodedOperands[ 1 ].mem.base != ZYDIS_REGISTER_ESP &&
		sDecodedOperands[ 1 ].mem.base != ZYDIS_REGISTER_NONE &&
		sDecodedOperands[ 1 ].mem.index == ZYDIS_REGISTER_NONE &&
		sDecodedOperands[ 1 ].mem.scale == 0 && 
		sDecodedOperands[ 1 ].mem.base != sDecodedOperands[ 0 ].reg.value )
		return OT_MOV_R_MEM;

	if ( sDecodedInsn.mnemonic == ZYDIS_MNEMONIC_RET &&
		sDecodedOperands[ 0 ].type != ZYDIS_OPERAND_TYPE_IMMEDIATE )
		return OT_RET_DEF;

	return OT_NONE;
}

void C_Obfuscator::ProcessObfuscation( S_InsnObfData& sData, asmjit::x86::Builder& sBuilder )
{
	sBuilder.setCursor( sData.m_pNode );
	auto eType = sData.m_eType;

	if ( eType == OT_MOV )
	{
		auto eRegToDec = ZyReg2Aj( sData.m_sInsn.m_cDecodedOperands[ 0 ].reg.value );
		GenerateDecryptCode( sBuilder, eRegToDec, sData.m_sInsn.m_cDecodedOperands[ 1 ].imm.value.u );
	}
	else if ( eType == OT_PUSH_R )
	{
		GenerateRandomPush( sBuilder );
		sBuilder.mov( asmjit::x86::dword_ptr( asmjit::x86::esp ), ZyReg2Aj( sData.m_sInsn.m_cDecodedOperands[ 0 ].reg.value ) );
	}
	else if ( eType == OT_MOV_RR )
	{
		auto eRegFrom = sData.m_sInsn.m_cDecodedOperands[ 1 ].reg.value;
		auto eRegTo = sData.m_sInsn.m_cDecodedOperands[ 0 ].reg.value;

		sBuilder.push( ZyReg2Aj( eRegFrom ) );
		sBuilder.pop( ZyReg2Aj( eRegTo ) );
	}
	else if ( eType == OT_PUSH_C )
	{
		auto eRandReg = GetRandomRegNoEsp( );
		DWORD dwValue = sData.m_sInsn.m_cDecodedOperands[ 0 ].imm.value.u;

		sBuilder.push( eRandReg );
		sBuilder.pushfd( );
		GenerateDecryptCode( sBuilder, eRandReg, dwValue );
		sBuilder.popfd( );
		sBuilder.xchg( asmjit::x86::dword_ptr( asmjit::x86::esp ), eRandReg );
	}
	else if ( eType == OT_CMP_RC )
	{
		auto cRegToCmp = ZyReg2Aj( sData.m_sInsn.m_cDecodedOperands[ 0 ].reg.value );
		DWORD dwValToCheck = sData.m_sInsn.m_cDecodedOperands[ 1 ].imm.value.u;

		if ( cRegToCmp == asmjit::x86::esp )
		{
			sBuilder.cmp( asmjit::x86::esp, dwValToCheck );
		}
		else
		{
			auto cReg2 = GetRandomRegNoEsp( );

			while ( cRegToCmp == cReg2 )
				cReg2 = GetRandomRegNoEsp( );

			sBuilder.push( cReg2 );
			GenerateDecryptCode( sBuilder, cReg2, dwValToCheck );
			sBuilder.cmp( cRegToCmp, cReg2 );
			sBuilder.pop( cReg2 );
		}
	}
	else if ( eType == OT_ADD_RC )
	{
		auto cRegToAdd = ZyReg2Aj( sData.m_sInsn.m_cDecodedOperands[ 0 ].reg.value );
		DWORD dwValToAdd = sData.m_sInsn.m_cDecodedOperands[ 1 ].imm.value.u;

		if ( cRegToAdd == asmjit::x86::esp )
		{
			sBuilder.add( asmjit::x86::esp, dwValToAdd );
		}
		else
		{
			sBuilder.push( cRegToAdd );
			GenerateDecryptCode( sBuilder, cRegToAdd, dwValToAdd );

			sBuilder.add( asmjit::x86::dword_ptr( asmjit::x86::esp ), cRegToAdd );
			sBuilder.pop( cRegToAdd );
		}
	}
	else if ( eType == OT_ADD_RR )
	{
		auto cRegToAdd1 = ZyReg2Aj( sData.m_sInsn.m_cDecodedOperands[ 0 ].reg.value );
		auto cRegToAdd2 = ZyReg2Aj( sData.m_sInsn.m_cDecodedOperands[ 1 ].reg.value );

		if ( cRegToAdd1 == asmjit::x86::esp || cRegToAdd2 == asmjit::x86::esp )
		{
			sBuilder.add( cRegToAdd1, cRegToAdd2 );
		}
		else
		{
			sBuilder.push( cRegToAdd1 );
			sBuilder.add( asmjit::x86::dword_ptr( asmjit::x86::esp ), cRegToAdd2 );
			sBuilder.pop( cRegToAdd1 );
		}
	}
	else if ( eType == OT_SUB_RC )
	{
		auto cRegToAdd = ZyReg2Aj( sData.m_sInsn.m_cDecodedOperands[ 0 ].reg.value );
		DWORD dwValToAdd = sData.m_sInsn.m_cDecodedOperands[ 1 ].imm.value.u;

		if ( cRegToAdd == asmjit::x86::esp )
		{
			sBuilder.sub( asmjit::x86::esp, dwValToAdd );
		}
		else
		{
			sBuilder.push( cRegToAdd );
			GenerateDecryptCode( sBuilder, cRegToAdd, dwValToAdd );

			sBuilder.sub( asmjit::x86::dword_ptr( asmjit::x86::esp ), cRegToAdd );
			sBuilder.pop( cRegToAdd );
		}
	}
	else if ( eType == OT_SUB_RR )
	{
		auto cRegToAdd1 = ZyReg2Aj( sData.m_sInsn.m_cDecodedOperands[ 0 ].reg.value );
		auto cRegToAdd2 = ZyReg2Aj( sData.m_sInsn.m_cDecodedOperands[ 1 ].reg.value );

		if ( cRegToAdd1 == asmjit::x86::esp || cRegToAdd2 == asmjit::x86::esp )
		{
			sBuilder.sub( cRegToAdd1, cRegToAdd2 );
		}
		else
		{
			sBuilder.push( cRegToAdd1 );
			sBuilder.sub( asmjit::x86::dword_ptr( asmjit::x86::esp ), cRegToAdd2 );
			sBuilder.pop( cRegToAdd1 );
		}
	}
	else if ( eType == OT_MOV_ESP_AVAL )
	{
		DWORD dwOffset = ( DWORD ) ( sData.m_sInsn.m_cDecodedOperands[ 0 ].mem.disp.value );

		auto sMem = asmjit::x86::dword_ptr( 0xCC );
		sMem.setSegment( asmjit::x86::fs );

		sBuilder.pushfd( );
		sBuilder.pop( sMem );

		sBuilder.add( asmjit::x86::esp, dwOffset + 4 );

		if ( sData.m_sInsn.m_cDecodedOperands[ 1 ].type == ZYDIS_OPERAND_TYPE_REGISTER )
			sBuilder.push( ZyReg2Aj( sData.m_sInsn.m_cDecodedOperands[ 1 ].reg.value ) );
		else
		{
			//sBuilder.push( sData.m_sInsn.m_cDecodedOperands[ 1 ].imm.value.u );
			GenerateDecryptPush( sBuilder, sData.m_sInsn.m_cDecodedOperands[ 1 ].imm.value.u );
		}

		if ( dwOffset )
			sBuilder.sub( asmjit::x86::esp, dwOffset );

		sBuilder.push( sMem );
		sBuilder.popfd( );
	}
	else if ( eType == OT_MOV_R_ESP )
	{
		DWORD dwOffset = ( DWORD ) ( sData.m_sInsn.m_cDecodedOperands[ 1 ].mem.disp.value );

		auto sMem = asmjit::x86::dword_ptr( 0xCC );
		sMem.setSegment( asmjit::x86::fs );

		sBuilder.pushfd( );
		sBuilder.pop( sMem );

		sBuilder.add( asmjit::x86::esp, dwOffset );

		sBuilder.pop( ZyReg2Aj( sData.m_sInsn.m_cDecodedOperands[ 0 ].reg.value ) );

		if ( dwOffset )
			sBuilder.sub( asmjit::x86::esp, dwOffset + 4 );

		sBuilder.push( sMem );
		sBuilder.popfd( );
	}
	else if ( eType == OT_LEA_RM )
	{
		DWORD dwOffset = ( DWORD ) ( sData.m_sInsn.m_cDecodedOperands[ 1 ].mem.disp.value );
		auto cRegTo = ZyReg2Aj( sData.m_sInsn.m_cDecodedOperands[ 0 ].reg.value );
		auto cRegBase = ZyReg2Aj( sData.m_sInsn.m_cDecodedOperands[ 1 ].mem.base );
		
		auto sMem = asmjit::x86::dword_ptr( 0xCC );
		sMem.setSegment( asmjit::x86::fs );

		sBuilder.pushfd( );
		sBuilder.pop( sMem );

		GenerateDecryptCode( sBuilder, cRegTo, dwOffset );
		sBuilder.add( cRegTo, cRegBase );

		sBuilder.push( sMem );
		sBuilder.popfd( );
	}
	else if ( eType == OT_MOV_MEM_AVAL )
	{
		auto sMem = asmjit::x86::dword_ptr( 0xCC );
		sMem.setSegment( asmjit::x86::fs );

		sBuilder.pushfd( );
		sBuilder.pop( sMem );

		if ( sData.m_sInsn.m_cDecodedOperands[ 1 ].type == ZYDIS_OPERAND_TYPE_IMMEDIATE )
		{
			DWORD dwValue = sData.m_sInsn.m_cDecodedOperands[ 1 ].imm.value.u;

			auto cRegBase = ZyReg2Aj( sData.m_sInsn.m_cDecodedOperands[ 0 ].mem.base );
			
			auto cRegDecValue = GetRandomRegNoEsp( );;
			
			while ( cRegDecValue == cRegBase )
				cRegDecValue = GetRandomRegNoEsp( );

			auto cRegOffset = GetRandomRegNoEsp( );

			while ( cRegOffset == cRegBase || cRegOffset == cRegDecValue )
				cRegOffset = GetRandomRegNoEsp( );

			sBuilder.push( cRegOffset );

			auto cRegAdditional = GetRandomRegNoEsp( );

			while ( cRegAdditional == cRegOffset || cRegAdditional == cRegBase || cRegAdditional == cRegDecValue )
				cRegAdditional = GetRandomRegNoEsp( );

			sBuilder.push( cRegAdditional );
			sBuilder.mov( cRegAdditional, cRegBase );

			GenerateDecryptCode( sBuilder, cRegOffset, ( DWORD ) ( sData.m_sInsn.m_cDecodedOperands[ 0 ].mem.disp.value ) );

			sBuilder.add( cRegAdditional, cRegOffset );

			sBuilder.push( cRegDecValue );

			GenerateDecryptCode( sBuilder, cRegDecValue, dwValue );

			sBuilder.mov( asmjit::x86::dword_ptr( cRegAdditional ), cRegDecValue );

			sBuilder.pop( cRegDecValue );
			sBuilder.pop( cRegAdditional );
			sBuilder.pop( cRegOffset );
		}
		else
		{
			auto cRegWrite = ZyReg2Aj( sData.m_sInsn.m_cDecodedOperands[ 1 ].reg.value );

			auto cRegBase = ZyReg2Aj( sData.m_sInsn.m_cDecodedOperands[ 0 ].mem.base );
			auto cRegOffset = GetRandomRegNoEsp( );

			while ( cRegOffset == cRegBase || cRegOffset == cRegWrite )
				cRegOffset = GetRandomRegNoEsp( );

			sBuilder.push( cRegOffset );

			auto cRegAdditional = GetRandomRegNoEsp( );

			while ( cRegAdditional == cRegOffset || cRegAdditional == cRegBase || cRegAdditional == cRegWrite )
				cRegAdditional = GetRandomRegNoEsp( );

			sBuilder.push( cRegAdditional );
			sBuilder.mov( cRegAdditional, cRegBase );

			GenerateDecryptCode( sBuilder, cRegOffset, ( DWORD ) ( sData.m_sInsn.m_cDecodedOperands[ 0 ].mem.disp.value ) );

			sBuilder.add( cRegAdditional, cRegOffset );
			sBuilder.mov( asmjit::x86::dword_ptr( cRegAdditional ), cRegWrite );
			
			sBuilder.pop( cRegAdditional );
			sBuilder.pop( cRegOffset );
		}

		sBuilder.push( sMem );
		sBuilder.popfd( );
	}
	else if ( eType == OT_MOV_R_MEM )
	{
		auto sMem = asmjit::x86::dword_ptr( 0xCC );
		sMem.setSegment( asmjit::x86::fs );

		sBuilder.pushfd( );
		sBuilder.pop( sMem );

		auto cRegTo = ZyReg2Aj( sData.m_sInsn.m_cDecodedOperands[ 0 ].reg.value );
		DWORD dwOffset = sData.m_sInsn.m_cDecodedOperands[ 1 ].mem.disp.value;

		GenerateDecryptCode( sBuilder, cRegTo, dwOffset );
		sBuilder.add( cRegTo, ZyReg2Aj( sData.m_sInsn.m_cDecodedOperands[ 1 ].mem.base ) );
		sBuilder.mov( cRegTo, asmjit::x86::dword_ptr( cRegTo ) );


		sBuilder.push( sMem );
		sBuilder.popfd( );
	}
	else if ( eType == OT_RET_DEF )
	{
		sBuilder.add( asmjit::x86::esp, 4 );
		sBuilder.jmp( asmjit::x86::dword_ptr( asmjit::x86::esp, -4 ) );
	}
}

void C_Obfuscator::GenerateDecryptCode( asmjit::x86::Builder& sBuilder, asmjit::_abi_1_9::x86::Gpd cReg, DWORD dwValue )
{
	int iNumberOfInstructions = ( rand( ) % 5 ) + 4;
	DWORD dwValToCrypt = dwValue;

	std::vector < std::pair < E_Operations, DWORD > > cVecOps = { };

	E_Operations eLastOp = ( E_Operations ) -1;

	for ( int i = 0; i < iNumberOfInstructions; ++i )
	{
		auto eOp = ( E_Operations ) ( rand( ) % 3 );

		while( eOp == eLastOp )
			eOp = ( E_Operations ) ( rand( ) % 3 );

		eLastOp = eOp;

		auto dwValue = rand( ) * 10000;

		switch ( eOp )
		{
			case OP_ROL: dwValToCrypt = _rotl( dwValToCrypt, dwValue % 31 ); break;
			case OP_ROR: dwValToCrypt = _rotr( dwValToCrypt, dwValue % 31 ); break;
			case OP_BSWAP: dwValToCrypt = _byteswap_ulong( dwValToCrypt ); break;
		}

		cVecOps.push_back( { eOp, dwValue } );
	}

	sBuilder.mov( cReg, dwValToCrypt );

	for ( int i = cVecOps.size( ) - 1; i >= 0; --i )
	{
		switch ( cVecOps[ i ].first )
		{
			case OP_ROL: sBuilder.ror( cReg, cVecOps[ i ].second % 31 ); break;
			case OP_ROR: sBuilder.rol( cReg, cVecOps[ i ].second % 31 ); break;
			case OP_BSWAP: sBuilder.bswap( cReg ); break;
		}
	}

	sBuilder.stc( );
}

void C_Obfuscator::GenerateDecryptPush( asmjit::x86::Builder& sBuilder, DWORD dwValue )
{
	int iNumberOfInstructions = ( rand( ) % 5 ) + 4;
	DWORD dwValToCrypt = dwValue;

	std::vector < std::pair < E_Operations, DWORD > > cVecOps = { };

	E_Operations eLastOp = ( E_Operations ) -1;

	for ( int i = 0; i < iNumberOfInstructions; ++i )
	{
		auto eOp = ( E_Operations ) ( rand( ) % 2 );

		while ( eOp == eLastOp )
			eOp = ( E_Operations ) ( rand( ) % 2 );

		eLastOp = eOp;

		auto dwValue = rand( ) * 10000;

		switch ( eOp )
		{
		case OP_ROL: dwValToCrypt = _rotl( dwValToCrypt, dwValue % 31 ); break;
		case OP_ROR: dwValToCrypt = _rotr( dwValToCrypt, dwValue % 31 ); break;
		//case OP_BSWAP: dwValToCrypt = _byteswap_ulong( dwValToCrypt ); break;
		}

		cVecOps.push_back( { eOp, dwValue } );
	}

	sBuilder.push( dwValToCrypt );

	for ( int i = cVecOps.size( ) - 1; i >= 0; --i )
	{
		switch ( cVecOps[ i ].first )
		{
		case OP_ROL: sBuilder.ror( asmjit::x86::dword_ptr( asmjit::x86::esp ), cVecOps[ i ].second % 31 ); break;
		case OP_ROR: sBuilder.rol( asmjit::x86::dword_ptr( asmjit::x86::esp ), cVecOps[ i ].second % 31 ); break;
		//case OP_BSWAP: sBuilder.bswap( cReg ); break;
		}
	}

	sBuilder.stc( );
}

void C_Obfuscator::GenerateRandomPush( asmjit::x86::Builder& sBuilder )
{
	if ( rand( ) % 2 == 0 )
		sBuilder.push( rand( ) * 10000 );
	else
		sBuilder.push( GetRandomReg( ) );
}

asmjit::_abi_1_9::x86::Gpd C_Obfuscator::ZyReg2Aj( ZydisRegister eReg )
{
	switch ( eReg )
	{
	case ZYDIS_REGISTER_EAX: return asmjit::_abi_1_9::x86::eax;
	case ZYDIS_REGISTER_EBX: return asmjit::_abi_1_9::x86::ebx;
	case ZYDIS_REGISTER_ECX: return asmjit::_abi_1_9::x86::ecx;
	case ZYDIS_REGISTER_EDX: return asmjit::_abi_1_9::x86::edx;
	case ZYDIS_REGISTER_EBP: return asmjit::_abi_1_9::x86::ebp;
	case ZYDIS_REGISTER_ESP: return asmjit::_abi_1_9::x86::esp;
	case ZYDIS_REGISTER_ESI: return asmjit::_abi_1_9::x86::esi;
	case ZYDIS_REGISTER_EDI: return asmjit::_abi_1_9::x86::edi;
	}

	return asmjit::_abi_1_9::x86::eax;
}

asmjit::_abi_1_9::x86::Gpd C_Obfuscator::GetRandomReg( )
{
	return ZyReg2Aj( ( ZydisRegister ) ( ZYDIS_REGISTER_EAX + ( rand( ) % 8 ) ) );
}

asmjit::_abi_1_9::x86::Gpd C_Obfuscator::GetRandomRegNoEsp( )
{
	int iMas[] = {
		ZYDIS_REGISTER_EAX,
		ZYDIS_REGISTER_ECX,
		ZYDIS_REGISTER_EDX,
		ZYDIS_REGISTER_EBX,
		ZYDIS_REGISTER_EBP,
		ZYDIS_REGISTER_ESI,
		ZYDIS_REGISTER_EDI,
	};

	return ZyReg2Aj( ( ZydisRegister ) iMas[ rand( ) % 7 ] );
}