#include "..\Classes.hpp"

bool C_PE::LoadMappedPE( std::vector<BYTE>& cVecOut, const char* pPePath )
{
	cVecOut.clear( );
	cVecOut.shrink_to_fit( );

	std::vector < BYTE > cVecRawPe = { };

	if ( !g_Files->LoadFile( pPePath, cVecRawPe ) )
		return false;

	PIMAGE_DOS_HEADER pDosHeader = ( PIMAGE_DOS_HEADER ) cVecRawPe.data( );
	PIMAGE_NT_HEADERS pNtHeaders = ( PIMAGE_NT_HEADERS ) ( ( LPBYTE ) pDosHeader + pDosHeader->e_lfanew );
	PIMAGE_SECTION_HEADER pSectHeader = ( PIMAGE_SECTION_HEADER ) ( pNtHeaders + 1 );

	cVecOut.insert( cVecOut.end( ), pNtHeaders->OptionalHeader.SizeOfImage, 0x00 );

	memcpy( cVecOut.data( ), cVecRawPe.data( ), pNtHeaders->OptionalHeader.SizeOfHeaders );

	for ( int i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++ )
	{
		memcpy( ( PVOID ) ( ( LPBYTE ) cVecOut.data( ) + pSectHeader[ i ].VirtualAddress ),
			( PVOID ) ( ( LPBYTE ) cVecRawPe.data( ) + pSectHeader[ i ].PointerToRawData ), pSectHeader[ i ].SizeOfRawData );
	}

	pDosHeader = ( PIMAGE_DOS_HEADER ) cVecOut.data( );
	pNtHeaders = ( PIMAGE_NT_HEADERS ) ( ( LPBYTE ) pDosHeader + pDosHeader->e_lfanew );
	pSectHeader = ( PIMAGE_SECTION_HEADER ) ( pNtHeaders + 1 );

	for ( int i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++ )
	{
		pSectHeader[ i ].PointerToRawData = pSectHeader[ i ].VirtualAddress;
		pSectHeader[ i ].SizeOfRawData = pSectHeader[ i ].Misc.VirtualSize;
	}

	cVecRawPe.clear( );
	cVecRawPe.shrink_to_fit( );

	return true;
}

void C_PE::LoadFunctionsList( std::vector<S_FunctionData>& cVecOut, std::vector < BYTE >& cVecImage, const char* szMapFilePath )
{
	MAPFile cMapFile = MAPFile( szMapFilePath );

	MAPData cMapData = { };
	memset( &cMapData, 0x00, sizeof( MAPData ) );

	cMapFile.Parse( &cMapData );

	std::sort( cMapData.functions.begin( ),
		cMapData.functions.end( ),
		[] ( Data& cData1, Data& cData2 ) -> bool { return cData1.offset < cData2.offset; } );
 
	DWORD dwTextOffset = GetSections( cVecImage )[ 0 ].VirtualAddress;

	for ( auto& cFn : cMapData.functions )
		cVecOut.push_back( S_FunctionData( cFn.name, dwTextOffset + cFn.offset, 0 ) );

	for ( int i = 0; i < cVecOut.size( ) - 1; ++i )
		cVecOut[ i ].m_dwFunctionSize = cVecOut[ i + 1 ].m_dwFunctionOffset - cVecOut[ i ].m_dwFunctionOffset;
	
	BYTE* pCode = cVecImage.data( ) + cVecOut.back( ).m_dwFunctionOffset;
	
	while ( memcmp( pCode, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 10 ) )
	{
		pCode++;
		cVecOut.back( ).m_dwFunctionSize++;
	}
}

PIMAGE_SECTION_HEADER C_PE::GetSections( std::vector<BYTE>& cVecIn )
{
	auto pDosHeader = ( PIMAGE_DOS_HEADER ) cVecIn.data( );
	auto pNtHeaders = ( PIMAGE_NT_HEADERS ) ( ( LPBYTE ) pDosHeader + pDosHeader->e_lfanew );
	auto pSectHeader = ( PIMAGE_SECTION_HEADER ) ( pNtHeaders + 1 );

	return pSectHeader;
}

PIMAGE_NT_HEADERS C_PE::GetNtHeaders( std::vector<BYTE>& cVecIn )
{
	auto pDosHeader = ( PIMAGE_DOS_HEADER ) cVecIn.data( );
	auto pNtHeaders = ( PIMAGE_NT_HEADERS ) ( ( LPBYTE ) pDosHeader + pDosHeader->e_lfanew );

	return pNtHeaders;
}