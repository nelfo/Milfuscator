#include "..\Classes.hpp"

void C_Rebuilder::LiftRebuildBinary( std::vector < BYTE >& cVecMappedImage, asmjit::x86::Builder& b, std::map < DWORD, DWORD >& cRelocMap, std::map < DWORD, asmjit::_abi_1_9::Label >& cRelocLabelsMap, std::map < DWORD, asmjit::_abi_1_9::Label >& cGlobalLabelsMap, std::map < DWORD, S_RvaFix >& cRvaFixesMap, std::map < DWORD, S_InsnObfData >& cObfDatasMap, std::map < DWORD, S_RelocIntersectionsData >& cRelocIntersectionsMap, std::map < DWORD, S_JumpCallInfo >& cMapJumps, asmjit::_abi_1_9::Label& cBaseLabel )
{
	int iMinEmitIndex = 0;
	int iBytesEmited = 0;
	DWORD dwDataRva = g_PE->GetSections( cVecMappedImage )[ 1 ].VirtualAddress;

	for ( int i = 0; i < cVecMappedImage.size( ); ++i )
	{
		if ( !i )
			b.bind( cBaseLabel );

		if ( /* ok */ ( DWORD ) i == dwDataRva )
			b.align( asmjit::_abi_1_9::AlignMode::kData, 16 );

		BYTE* pCode = cVecMappedImage.data( ) + i;
		DWORD dwCode = ( DWORD ) pCode;

		for ( auto& sRvaData : cRvaFixesMap )
		{
			if ( sRvaData.second.m_dwPtrTo == dwCode )
			{
				b.bind( sRvaData.second.m_cLabel );
			}
		}

		if ( cRelocIntersectionsMap.find( dwCode ) != cRelocIntersectionsMap.end( ) )
		{
			b.bind( cRelocIntersectionsMap[ dwCode ].m_cLabel );
		}

		if ( cRvaFixesMap.find( dwCode ) != cRvaFixesMap.end( ) )
		{
			b.embedLabelDelta( cRvaFixesMap[ dwCode ].m_cLabel, cBaseLabel );
			iMinEmitIndex = i + 4;
		}

		if ( cRelocLabelsMap.find( dwCode ) != cRelocLabelsMap.end( ) )
		{
			b.bind( cRelocLabelsMap[ dwCode ] );
		}

		if ( cGlobalLabelsMap.find( dwCode ) != cGlobalLabelsMap.end( ) )
		{
			b.bind( cGlobalLabelsMap[ dwCode ] );
		}

		if ( cObfDatasMap.find( dwCode ) != cObfDatasMap.end( ) )
		{
			auto& sObfData = cObfDatasMap[ dwCode ];

			b.bind( b.newLabel( ) );

			sObfData.m_pNode = b.cursor( );

			iMinEmitIndex = i + sObfData.m_sInsn.m_cDecodedInstruction.length;
		}

		if ( cRelocMap.find( dwCode ) != cRelocMap.end( ) )
		{
			b.embedLabel( cRelocLabelsMap[ cRelocMap[ dwCode ] ] );
			iMinEmitIndex = i + 4;
		}

		if ( i >= iMinEmitIndex && cMapJumps.find( dwCode ) != cMapJumps.end( ) )
		{
			auto& sJumpData = cMapJumps[ dwCode ];
			auto& sLabel = sJumpData.m_cJumpLabel;

			switch ( sJumpData.m_eJumpMnem )
			{
			case ZYDIS_MNEMONIC_CALL: b.call( sLabel ); break;
			case ZYDIS_MNEMONIC_JB: b.jb( sLabel ); break;
			case ZYDIS_MNEMONIC_JBE: b.jbe( sLabel ); break;
				//case ZYDIS_MNEMONIC_JCXZ: a.j( sLabel ); break;
			case ZYDIS_MNEMONIC_JECXZ: b.jecxz( sLabel ); break;
				//case ZYDIS_MNEMONIC_JKNZD: a.j( sLabel ); break;
				//case ZYDIS_MNEMONIC_JKZD: a.j( sLabel ); break;
			case ZYDIS_MNEMONIC_JL: b.jl( sLabel ); break;
			case ZYDIS_MNEMONIC_JLE: b.jle( sLabel ); break;
				//case ZYDIS_MNEMONIC_JMP: a.jmp( sLabel ); break;
			case ZYDIS_MNEMONIC_JMP: b.stc( ); b.jb( sLabel ); break;
			case ZYDIS_MNEMONIC_JNB: b.jnb( sLabel ); break;
			case ZYDIS_MNEMONIC_JNBE: b.jnbe( sLabel ); break;
			case ZYDIS_MNEMONIC_JNL: b.jnl( sLabel ); break;
			case ZYDIS_MNEMONIC_JNLE: b.jnle( sLabel ); break;
			case ZYDIS_MNEMONIC_JNO: b.jno( sLabel ); break;
			case ZYDIS_MNEMONIC_JNP: b.jnp( sLabel ); break;
			case ZYDIS_MNEMONIC_JNS: b.jns( sLabel ); break;
			case ZYDIS_MNEMONIC_JNZ: b.jnz( sLabel ); break;
			case ZYDIS_MNEMONIC_JO: b.jo( sLabel ); break;
			case ZYDIS_MNEMONIC_JP: b.jp( sLabel ); break;
				//case ZYDIS_MNEMONIC_JRCXZ: a.j( sLabel ); break;
			case ZYDIS_MNEMONIC_JS: b.js( sLabel ); break;
			case ZYDIS_MNEMONIC_JZ: b.jz( sLabel ); break;

			default:
				printf( _( "[+] Unknown mnemonic! | %d\n" ), sJumpData.m_eJumpMnem );
				system( _( "pause" ) );
			}

			iMinEmitIndex = i + sJumpData.m_dwOldJumpSize;
		}

		if ( i >= iMinEmitIndex )
			b.db( *pCode );
	}
}

void C_Rebuilder::InsertIntersectionsData( asmjit::x86::Builder& b, std::map<DWORD, S_RelocIntersectionsData>& cRelocIntersectionsMap, asmjit::_abi_1_9::Label& cBaseLabel, std::string& cRebuilderDataBeginStr )
{
	auto sLabelColbaGay = b.newLabel( );

	b.jmp( sLabelColbaGay );

	for ( int i = 0; i < 8; ++i )
	{
		cRebuilderDataBeginStr.push_back( "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[ rand( ) % 26 ] );
		b.db( cRebuilderDataBeginStr.back( ) );
	}

	for ( auto& cShiftData : cRelocIntersectionsMap )
	{
		b.embedLabelDelta( cShiftData.second.m_cLabel, cBaseLabel );
		b.embedLabelDelta( cShiftData.second.m_cLabelTo, cBaseLabel );
	}

	b.bind( sLabelColbaGay );
}

void C_Rebuilder::ProcessIntersectionsData( std::vector<BYTE>& cVecMappedImage, std::map<DWORD, S_RelocIntersectionsData>& cRelocIntersectionsMap, std::string& cRebuilderDataBeginStr )
{
	BYTE* pLastByte = cVecMappedImage.data( ) + cVecMappedImage.size( ) - 1;

	while ( memcmp( pLastByte, cRebuilderDataBeginStr.c_str( ), 8 ) )
		pLastByte--;

	pLastByte += 8;
	DWORD* pRvasRelocstoShift = ( DWORD* ) pLastByte;

	int i = 0;
	for ( auto& cShiftData : cRelocIntersectionsMap )
	{
		*( DWORD* ) ( cVecMappedImage.data( ) + pRvasRelocstoShift[ i * 2 ] ) -= cShiftData.second.m_iOffset;
		//*( DWORD* ) ( cVecMappedPE.data( ) + pRvasRelocstoShift[ i * 2 ] ) = g_PE->GetNtHeaders( cVecMappedPE )->OptionalHeader.ImageBase + pRvasRelocstoShift[ ( i * 2 ) + 1 ] - cShiftData.second.m_iOffset;

		//printf( "%p   %p   %p \n", pRvasRelocstoShift[ i * 2 ], pRvasRelocstoShift[ ( i * 2 ) + 1 ], cShiftData.second.m_iOffset );
		i++;
	}
}