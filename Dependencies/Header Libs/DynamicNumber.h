#pragma once

class C_DynamicNumber
{
public:
	template <typename T>
	C_DynamicNumber( T value )
	{
		m_dwValue = ( unsigned int ) value;
	}

	template<typename _T>
	_T Get( )
	{
		return ( _T ) m_dwValue;
	}

	template<typename _T>
	C_DynamicNumber Add( _T value )
	{
		return C_DynamicNumber( m_dwValue + ( unsigned int ) value );
	}

	C_DynamicNumber Add( C_DynamicNumber value )
	{
		return C_DynamicNumber( m_dwValue + value.m_dwValue );
	}

	template<typename _T>
	C_DynamicNumber Sub( _T value )
	{
		return C_DynamicNumber( m_dwValue - ( unsigned int ) value );
	}

	C_DynamicNumber Sub( C_DynamicNumber value )
	{
		return C_DynamicNumber( m_dwValue - value.m_dwValue );
	}

	template<typename _T>
	_T Read( )
	{
		return *( _T* ) m_dwValue;
	}

	template<typename _T>
	C_DynamicNumber Write( _T value )
	{
		*( _T* ) m_dwValue = value;
		return C_DynamicNumber( m_dwValue );
	}

	C_DynamicNumber Write( C_DynamicNumber value )
	{
		*( unsigned int* ) m_dwValue = value.m_dwValue;
		return C_DynamicNumber( m_dwValue );
	}

	unsigned int m_dwValue;
};