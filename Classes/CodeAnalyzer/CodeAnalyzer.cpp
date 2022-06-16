#include "..\Classes.hpp"

void C_CodeAnalyzer::AnalyzeFunction( std::vector<BYTE>& cVecImage, S_FunctionData& sFnData, S_AnalyzedFunction& cOutData )
{
	cOutData = S_AnalyzedFunction( );

	ZydisDecoder cZyDecoder;
	ZydisDecoderInit( &cZyDecoder, ZYDIS_MACHINE_MODE_LONG_COMPAT_32, ZYDIS_STACK_WIDTH_32 );

	ZydisFormatter cZyFormatter;
	ZydisFormatterInit( &cZyFormatter, ZYDIS_FORMATTER_STYLE_INTEL );

	C_DynamicNumber dnStart = cVecImage.data( ) + sFnData.m_dwFunctionOffset;
	C_DynamicNumber dnSize = sFnData.m_dwFunctionSize;
	C_DynamicNumber dnOffset = 0;

	while ( dnSize.m_dwValue > 0 )
	{
		auto& sInstruction = cOutData.m_cAnalyzedInstructions.emplace_back( );

		memset( &sInstruction, 0x00, sizeof( sInstruction ) );

		ZydisDecoderDecodeFull( &cZyDecoder, dnStart.Add( dnOffset ).Get<const void*>( ), dnSize.m_dwValue,
			&sInstruction.m_cDecodedInstruction, sInstruction.m_cDecodedOperands, ZYDIS_MAX_OPERAND_COUNT_VISIBLE,
			ZYDIS_DFLAG_VISIBLE_OPERANDS_ONLY );

		sInstruction.m_dwInstructionOffset = dnOffset.m_dwValue;

		memcpy( sInstruction.m_btInstructionBytes, dnStart.Add( dnOffset ).Get<const void*>( ), sInstruction.m_cDecodedInstruction.length );
	
		ZydisFormatterFormatInstruction( &cZyFormatter, &sInstruction.m_cDecodedInstruction, sInstruction.m_cDecodedOperands,
			sInstruction.m_cDecodedInstruction.operand_count_visible, sInstruction.m_chInstructionString, sizeof( sInstruction.m_chInstructionString ), 0 );

		dnOffset = dnOffset.Add( sInstruction.m_cDecodedInstruction.length );
		dnSize = dnSize.Sub( sInstruction.m_cDecodedInstruction.length );

		//if ( sInstruction.m_cDecodedInstruction.mnemonic == ZYDIS_MNEMONIC_INT3 )
		//	break;
	}

	for ( auto& sInsn : cOutData.m_cAnalyzedInstructions )
	{
		auto& sDecodedInstruction = sInsn.m_cDecodedInstruction;
		auto& sDecodedOperands = sInsn.m_cDecodedOperands;

		if ( sDecodedInstruction.mnemonic >= ZYDIS_MNEMONIC_JB &&
			sDecodedInstruction.mnemonic <= ZYDIS_MNEMONIC_JZ &&
			sDecodedOperands[ 0 ].type == ZYDIS_OPERAND_TYPE_IMMEDIATE )
		{
			sInsn.m_eType = IT_NEED_TO_FIX;
		}
		else if ( sDecodedInstruction.mnemonic == ZYDIS_MNEMONIC_CALL &&
			sDecodedOperands[0].type == ZYDIS_OPERAND_TYPE_IMMEDIATE )
		{
			sInsn.m_eType = IT_NEED_TO_FIX;
		}
	}
}

void C_CodeAnalyzer::AnalyzeRelocs( asmjit::x86::Builder& b, std::vector < BYTE >& cVecImage, std::map < DWORD, DWORD >& cRelocMap, std::map < DWORD, asmjit::_abi_1_9::Label >& cRelocLabelsMap )
{
	auto pNtHeaders = g_PE->GetNtHeaders( cVecImage );
	auto dwBaseCompiledAt = g_PE->GetNtHeaders( cVecImage )->OptionalHeader.ImageBase;

	PIMAGE_BASE_RELOCATION pIBR = ( PIMAGE_BASE_RELOCATION ) ( ( LPBYTE ) cVecImage.data( )
		+ pNtHeaders->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_BASERELOC ].VirtualAddress );

	while ( pIBR->VirtualAddress )
	{
		if ( pIBR->SizeOfBlock >= sizeof( IMAGE_BASE_RELOCATION ) )
		{
			int count = ( pIBR->SizeOfBlock - sizeof( IMAGE_BASE_RELOCATION ) ) / sizeof( WORD );
			PWORD list = ( PWORD ) ( pIBR + 1 );

			for ( int i = 0; i < count; i++ )
			{
				if ( list[ i ] && ( list[ i ] >> 12 ) & IMAGE_REL_BASED_HIGHLOW )
				{
					PDWORD pValue = ( PDWORD ) ( ( LPBYTE ) cVecImage.data( ) + ( pIBR->VirtualAddress + ( list[ i ] & 0xFFF ) ) );
					DWORD dwValue = ( *pValue - dwBaseCompiledAt ) + ( DWORD ) cVecImage.data( );

					cRelocMap[ ( DWORD ) pValue ] = dwValue;

					if ( cRelocLabelsMap.find( dwValue ) == cRelocLabelsMap.end( ) )
						cRelocLabelsMap[ dwValue ] = b.newLabel( );
				}
			}
		}

		pIBR = ( PIMAGE_BASE_RELOCATION ) ( ( LPBYTE ) pIBR + pIBR->SizeOfBlock );
	}
}

void C_CodeAnalyzer::FindRelocsIntersections( asmjit::x86::Builder& b, std::map<DWORD, S_RelocIntersectionsData>& cMapOutData, std::map<DWORD, DWORD>& cRelocMap, std::map < DWORD, asmjit::_abi_1_9::Label >& cRelocLabelsMap )
{
	for ( auto& sRelocFrom : cRelocMap )
	{
		for ( auto& sRelocLabel : cRelocMap )
		{
			DWORD dwRelocValue = sRelocFrom.second;
			DWORD dwRelocInsert = sRelocLabel.first;

			if ( dwRelocValue >= dwRelocInsert + 1 && dwRelocValue <= dwRelocInsert + 3 )
			{
				cMapOutData[ sRelocFrom.first ] = S_RelocIntersectionsData( b.newLabel( ), cRelocLabelsMap[ sRelocFrom.second ], ( int ) ( dwRelocValue - dwRelocInsert ) );
			}
		}
	}
}

void C_CodeAnalyzer::AnalyzeAllFunctions( asmjit::x86::Builder& b, std::vector < S_FunctionData >& cVecFns, std::vector<S_AnalyzedFunction>& cVecOutData, std::vector<BYTE>& cVecImage, std::map<DWORD, asmjit::_abi_1_9::Label>& cGlobalLabels )
{
	for ( auto& cFn : cVecFns )
	{
		auto& cData = cVecOutData.emplace_back( );

		AnalyzeFunction( cVecImage, cFn, cData );

		cData.m_dwFnOffset = cFn.m_dwFunctionOffset;

		for ( auto& sInsn : cData.m_cAnalyzedInstructions )
		{
			if ( sInsn.m_eType == IT_NEED_TO_FIX )
			{
				DWORD dwPtr = ( DWORD ) cVecImage.data( ) + cData.m_dwFnOffset + sInsn.m_dwInstructionOffset + sInsn.m_cDecodedOperands[ 0 ].imm.value.u + sInsn.m_cDecodedInstruction.length;

				if ( cGlobalLabels.find( dwPtr ) == cGlobalLabels.end( ) )
				{
					cGlobalLabels[ dwPtr ] = b.newLabel( );
				}
			}
		}
	}
}

void C_CodeAnalyzer::GenerateCallJumpsInfo( std::vector < S_AnalyzedFunction >& cVecAnalyzedFns, std::vector < BYTE >& cVecImage, std::map < DWORD, S_JumpCallInfo >& cMapJumps, std::map < DWORD, asmjit::_abi_1_9::Label >& cRelocLabelsMap )
{
	for ( auto& sAnalData : cVecAnalyzedFns )
	{
		for ( auto& sInsn : sAnalData.m_cAnalyzedInstructions )
		{
			if ( sInsn.m_eType == IT_NEED_TO_FIX )
			{
				DWORD dwPtrJumpTo = ( DWORD ) cVecImage.data( ) + sAnalData.m_dwFnOffset + sInsn.m_dwInstructionOffset + sInsn.m_cDecodedOperands[ 0 ].imm.value.u + sInsn.m_cDecodedInstruction.length;
				DWORD dwPtr = ( DWORD ) cVecImage.data( ) + sAnalData.m_dwFnOffset + sInsn.m_dwInstructionOffset;

				/*if ( g_cGlobalLabels.find( dwPtrJumpTo ) == g_cGlobalLabels.end( ) )
				{
					printf( _( "Wha... | 0x%p \n" ), dwPtr );
				}*/

				cMapJumps[ dwPtr ] = S_JumpCallInfo( sInsn.m_cDecodedInstruction.mnemonic,
					cRelocLabelsMap[ dwPtrJumpTo ],
					sInsn.m_cDecodedInstruction.length
				);
			}
		}
	}
}

void C_CodeAnalyzer::GenerateObfData( std::map<DWORD, S_InsnObfData>& cVecOutData, std::vector<BYTE>& cVecImage, std::map<DWORD, DWORD>& cRelocMap, std::vector<S_AnalyzedFunction>& cVecAnalyzedFns )
{
	for ( auto& sAnalData : cVecAnalyzedFns )
	{
		for ( auto& sInsn : sAnalData.m_cAnalyzedInstructions )
		{
			DWORD dwInsnOffset = sAnalData.m_dwFnOffset + sInsn.m_dwInstructionOffset;
			DWORD dwPtr = ( DWORD ) cVecImage.data( ) + dwInsnOffset;

			if ( cRelocMap.find( ( DWORD ) cVecImage.data( ) + dwInsnOffset ) != cRelocMap.end( ) )
				break;

			bool bCont = false;

			for ( auto& sRelocData : cRelocMap )
			{
				if ( sRelocData.first >= dwPtr && sRelocData.first < dwPtr + sInsn.m_cDecodedInstruction.length )
				{
					bCont = true;
					break;
				}
			}

			if ( bCont )
				continue;

			auto eResult = g_Obfuscator->CheckInstruction( sInsn );

			if ( eResult != OT_NONE )
			{
				S_InsnObfData sData = { };

				sData.m_eType = eResult;
				sData.m_sInsn = sInsn;

				cVecOutData[ ( DWORD ) cVecImage.data( ) + dwInsnOffset ] = sData;
			}
		}
	}
}