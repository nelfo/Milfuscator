#include "..\Classes.hpp"

bool C_Files::LoadFile( const char* szName, std::vector<BYTE>& cVecData )
{
	FILE* pFp = fopen( szName, "rb" );

	if ( !pFp )
		return false;

	fseek( pFp, 0L, SEEK_END );
	DWORD dwFileSize = ftell( pFp );
	fseek( pFp, 0L, SEEK_SET );

	cVecData.resize( dwFileSize );

	fread( cVecData.data( ), 1, cVecData.size( ), pFp );
	fclose( pFp );

	return true;
}

void C_Files::SaveFile( const char* szName, std::vector<BYTE>& cVecData )
{
	FILE* pFp = fopen( szName, "wb" );

	fwrite( cVecData.data( ), 1, cVecData.size( ), pFp );
	fclose( pFp );
}