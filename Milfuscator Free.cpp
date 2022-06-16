#include "Classes/Classes.hpp"

#define AlignSize(Size, Align) (Size+Align-1)/Align*Align

std::map < DWORD, DWORD > g_cRelocToReloc = { };
std::map < DWORD, asmjit::_abi_1_9::Label > g_cRelocLabels = { };
std::map < DWORD, asmjit::_abi_1_9::Label > g_cGlobalLabels = { };
std::map < DWORD, S_RvaFix > g_cRvaFixes = { };

int main()
{
	srand( time( 0 ) );

	printf( _( "[+] Milfuscator Free by nelfo & colby57 \n\n" ) );

	std::string cStrExePath = "";
	std::string cStrMapPath = "";

	printf( _( "[+] Exe path: " ) );
	std::getline( std::cin, cStrExePath );

	printf( _( "[+] Map path: " ) );
	std::getline( std::cin, cStrMapPath );
	
	std::vector < BYTE > cVecMappedPE = { };

	if ( !g_PE->LoadMappedPE( cVecMappedPE, cStrExePath.c_str( ) ) )
	{
		printf( _( "[+] File load error! \n" ) );
		system( _( "pause" ) );
		exit( 0 );
	}

	std::vector < S_FunctionData > cVecFns = { };

	g_PE->LoadFunctionsList( cVecFns, cVecMappedPE, cStrMapPath.c_str( ) );

	printf( _( "\n[+] Functions loaded! \n" ) );

	asmjit::JitRuntime rt;
	asmjit::CodeHolder code;

	code.init( rt.environment( ) );

	asmjit::x86::Builder b( &code );

	auto AddRvaResolve = [&] ( const void* pAddress, DWORD dwRva ) -> void
	{
		g_cRvaFixes[ ( DWORD ) pAddress ] = S_RvaFix( ( DWORD ) cVecMappedPE.data( ) + dwRva, b.newLabel( ) );
	};

	auto pNtHeaders = g_PE->GetNtHeaders( cVecMappedPE );

	AddRvaResolve( &pNtHeaders->OptionalHeader.AddressOfEntryPoint, pNtHeaders->OptionalHeader.AddressOfEntryPoint );

	g_CodeAnalyzer->AnalyzeRelocs( b, cVecMappedPE, g_cRelocToReloc, g_cRelocLabels );

	std::map < DWORD, S_RelocIntersectionsData > сMapRelocsIntersections = { };

	g_CodeAnalyzer->FindRelocsIntersections( b, сMapRelocsIntersections, g_cRelocToReloc, g_cRelocLabels );

	std::vector < S_AnalyzedFunction > cVecAnalyzedFns = { };

	printf( _( "[+] Analyzing functions... \n" ) );

	g_CodeAnalyzer->AnalyzeAllFunctions( b, cVecFns, cVecAnalyzedFns, cVecMappedPE, g_cGlobalLabels );

	printf( _( "[+] Analyzed %d functions! \n" ), cVecAnalyzedFns.size( ) );

	std::map < DWORD, S_JumpCallInfo > cMapJumps = { };

	printf( _( "[+] Rebuilding binary... \n" ) );

	g_CodeAnalyzer->GenerateCallJumpsInfo( cVecAnalyzedFns, cVecMappedPE, cMapJumps, g_cGlobalLabels );

	DWORD dwSavedDllSize = cVecMappedPE.size( );

	std::map < DWORD, S_InsnObfData > cMapObfDatas = { };

	g_CodeAnalyzer->GenerateObfData( cMapObfDatas, cVecMappedPE, g_cRelocToReloc, cVecAnalyzedFns );

	auto cBaseLabel = b.newLabel( );

	g_Rebuilder->LiftRebuildBinary( cVecMappedPE, b, g_cRelocToReloc, g_cRelocLabels, g_cGlobalLabels, g_cRvaFixes, cMapObfDatas, сMapRelocsIntersections, cMapJumps, cBaseLabel );

	printf( _( "[+] Obfuscating binary... \n" ) );

	for ( auto& sObfData : cMapObfDatas )
		g_Obfuscator->ProcessObfuscation( sObfData.second, b );

	printf( _( "[+] Binary obfuscated...! \n" ) );

	std::string cRebuilderDataBeginStr = "";

	g_Rebuilder->InsertIntersectionsData( b, сMapRelocsIntersections, cBaseLabel, cRebuilderDataBeginStr );

	printf( _( "[+] Finalizing... \n" ) );
	b.finalize( );
	code.relocateToBase( ( uint64_t ) g_PE->GetNtHeaders( cVecMappedPE )->OptionalHeader.ImageBase );

	cVecMappedPE.clear( );
	cVecMappedPE.shrink_to_fit( );

	cVecMappedPE.insert( cVecMappedPE.end( ), code.sectionById( 0 )->buffer( ).begin( ), code.sectionById( 0 )->buffer( ).end( ) );

	g_Rebuilder->ProcessIntersectionsData( cVecMappedPE, сMapRelocsIntersections, cRebuilderDataBeginStr );

	DWORD dwBytesInserted = cVecMappedPE.size( ) - dwSavedDllSize;

	DWORD dwNewSize = AlignSize( cVecMappedPE.size( ), 0x1000 );

	while ( cVecMappedPE.size( ) != dwNewSize )
		cVecMappedPE.push_back( 0x00 );

	pNtHeaders = g_PE->GetNtHeaders( cVecMappedPE );
	auto pSectHeader = g_PE->GetSections( cVecMappedPE );

	pNtHeaders->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_BASERELOC ].VirtualAddress = 0;
	pNtHeaders->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_BASERELOC ].Size = 0;
	pNtHeaders->OptionalHeader.DllCharacteristics &= ~0x40;

	pNtHeaders->OptionalHeader.SizeOfImage = dwNewSize;
	pNtHeaders->FileHeader.NumberOfSections = 1;

	pSectHeader->Characteristics = 0xE0000060;
	pSectHeader->PointerToRawData = pSectHeader->VirtualAddress;
	pSectHeader->SizeOfRawData = dwNewSize - 0x1000;
	pSectHeader->Misc.VirtualSize = dwNewSize - 0x1000;
	
	for ( int i = IMAGE_DIRECTORY_ENTRY_EXPORT; i <= IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR; ++i )
	{
		auto& sDir = pNtHeaders->OptionalHeader.DataDirectory[ i ];

		if ( i != IMAGE_DIRECTORY_ENTRY_IMPORT && i != IMAGE_DIRECTORY_ENTRY_BASERELOC && i != IMAGE_DIRECTORY_ENTRY_IAT ) // ok
		{
			sDir.VirtualAddress = 0;
			sDir.Size = 0;
			continue;
		}

		if ( sDir.VirtualAddress )
			sDir.VirtualAddress += dwBytesInserted;
	}
	
	PIMAGE_IMPORT_DESCRIPTOR pIID = ( PIMAGE_IMPORT_DESCRIPTOR ) ( ( LPBYTE ) cVecMappedPE.data( )
		+ pNtHeaders->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_IMPORT ].VirtualAddress );

	while ( pIID->Characteristics )
	{
		pIID->OriginalFirstThunk += dwBytesInserted;
		pIID->FirstThunk += dwBytesInserted;
		pIID->Name += dwBytesInserted;

		PIMAGE_THUNK_DATA OrigFirstThunk = ( PIMAGE_THUNK_DATA ) ( ( LPBYTE ) cVecMappedPE.data( ) + pIID->OriginalFirstThunk );
		PIMAGE_THUNK_DATA FirstThunk = ( PIMAGE_THUNK_DATA ) ( ( LPBYTE ) cVecMappedPE.data( ) + pIID->FirstThunk );

		while ( OrigFirstThunk->u1.AddressOfData )
		{
			OrigFirstThunk->u1.AddressOfData += dwBytesInserted;
			FirstThunk->u1.AddressOfData += dwBytesInserted;

			OrigFirstThunk++;
			FirstThunk++;
		}
		pIID++;
	}

	printf( _( "[+] Saving... \n" ) );

	g_Files->SaveFile( _( "Milfuscated.exe" ), cVecMappedPE );

	printf( _( "[+] Saved! \n" ) );

	ShellExecuteA( 0, 0, _( "https://t.me/neru_technologies" ), 0, 0, SW_HIDE );

	while ( true );
}