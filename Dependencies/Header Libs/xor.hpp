#pragma once

#ifdef _MSC_VER
#define XORSTR_FORCEINLINE __forceinline
#else
#define XORSTR_FORCEINLINE __attribute__((always_inline)) inline
#endif

#define XORSTR_DISABLE_AVX_INTRINSICS

namespace xor_str {
	namespace detail {
		template<uint32_t S>
		struct unsigned_t;

		template <>
		struct unsigned_t<1u> {
			using type = uint8_t;
		};

		template <>
		struct unsigned_t<2u> {
			using type = uint16_t;
		};

		template <>
		struct unsigned_t<4u> {
			using type = uint32_t;
		};

		template <auto T, auto...>
		struct pack_value_t {
			using type = decltype( T );
		};

		template <uint64_t V>
		struct uint64_value_t {
			constexpr static uint64_t m_value = V;
		};

		template <uint32_t S>
		XORSTR_FORCEINLINE constexpr uint32_t get_buffer_size( ) {
			return ( ( S / 16u ) + ( S % 16u != 0u ) ) * 2u;
		}

		template<auto... CS>
		struct string_t {
			using value_t = typename pack_value_t<CS...>::type;

			constexpr static uint32_t m_size = sizeof...( CS );
			constexpr static value_t  m_str[ m_size ] = { CS... };

			constexpr static uint32_t m_buffer_size = get_buffer_size<sizeof( m_str )>( );
			constexpr static uint32_t m_buffer_align =
#ifndef XORSTR_DISABLE_AVX_INTRINSICS
			( ( sizeof( m_str ) > 16u ) ? 32u : 16u );
#else
				16u;
#endif
		};

		template <uint32_t I, uint64_t K>
		struct key_t {
			constexpr static uint32_t   m_index = I;
			constexpr static uint64_t	m_key = K;
		};

		template <uint32_t S>
		XORSTR_FORCEINLINE constexpr uint32_t get_key4( ) {
			auto value = S;

			for ( auto& i : __TIME__ ) {
				value = static_cast< uint32_t >( ( value ^ i ) * 16777619ull );
			}

			return value;
		}

		template <uint32_t S>
		XORSTR_FORCEINLINE constexpr uint64_t get_key8( ) {
			constexpr auto k0 = get_key4<S + 2166136261u>( );

			return ( static_cast< uint64_t >( k0 ) << 32 ) | get_key4<k0>( );
		}

		template <typename T>
		XORSTR_FORCEINLINE constexpr uint64_t load_xored_str8( uint64_t key, uint32_t index ) {
			using cast_type = typename unsigned_t<sizeof( typename T::value_t )>::type;

			constexpr auto size = sizeof( typename T::value_t );
			constexpr auto offset = 8u / size;

			auto value = key;

			for ( auto i = 0u; i < offset && i + index * offset < T::m_size; i++ ) {
				value ^= static_cast< uint64_t >( static_cast< cast_type >( T::m_str[ i + index * offset ] ) ) << ( ( i % offset ) * 8u * size );
			}

			return value;
		}

		XORSTR_FORCEINLINE uint64_t load_from_reg( uint64_t value ) {
#if defined(__clang__) || defined(__GNUC__)
			asm( "" : "=r"( value ) : "0"( value ) : );
#endif
			return value;
		}

		XORSTR_FORCEINLINE void xor128( uint64_t* value, const uint64_t* key ) {
			_mm_store_si128( reinterpret_cast< __m128i* >( value ), _mm_xor_si128( _mm_load_si128( reinterpret_cast< const __m128i* >( value ) ), _mm_load_si128( reinterpret_cast< const __m128i* >( key ) ) ) );
		}

		XORSTR_FORCEINLINE void xor256( uint64_t* value, const uint64_t* key ) {
			_mm256_store_si256( reinterpret_cast< __m256i* >( value ), _mm256_xor_si256( _mm256_load_si256( reinterpret_cast< const __m256i* >( value ) ), _mm256_load_si256( reinterpret_cast< const __m256i* >( key ) ) ) );
		}
	}

	template <typename T, typename... K>
	struct vectorized_t {
	private:
		template <uint32_t... I>
		XORSTR_FORCEINLINE void crypt_256( const uint64_t* keys, std::index_sequence<I...> ) { ( detail::xor256( m_storage + I * 4u, keys + I * 4u ), ... ); }

		template <uint32_t... I>
		XORSTR_FORCEINLINE void crypt_128( const uint64_t* keys, std::index_sequence<I...> ) { ( detail::xor128( m_storage + I * 2u, keys + I * 2u ), ... ); }

		alignas( T::m_buffer_align ) uint64_t m_storage[ T::m_buffer_size ];
	public:
		using value_t = typename T::value_t;

		XORSTR_FORCEINLINE vectorized_t( ) : m_storage { detail::load_from_reg( detail::uint64_value_t<detail::load_xored_str8<T>( K::m_key, K::m_index )>::m_value )... } {}

		XORSTR_FORCEINLINE constexpr uint32_t size( ) const { return T::m_size - 1u; }

		XORSTR_FORCEINLINE void crypt( ) {
#if defined(__clang__)
			alignas( T::m_buffer_align ) uint64_t arr[ sizeof...( K ) ] { detail::load_from_reg( K::m_key )... };
			const auto keys = ( uint64_t* ) detail::load_from_reg( ( uint64_t ) arr );
#else
			alignas( T::m_buffer_align ) uint64_t keys[ sizeof...( K ) ] { detail::load_from_reg( K::m_key )... };
#endif

#ifndef XORSTR_DISABLE_AVX_INTRINSICS
			crypt_256( keys, std::make_index_sequence<T::m_buffer_size / 4u>( ) );

			if constexpr ( T::m_buffer_size % 4u != 0u ) {
				crypt_128( keys, std::index_sequence<T::m_buffer_size / 2u - 1u>( ) );
			}
#else
			crypt_128( keys, std::make_index_sequence<T::m_buffer_size / 2u>( ) );
#endif
		}

		XORSTR_FORCEINLINE const value_t* get_ptr( ) const { return reinterpret_cast< const value_t* >( m_storage ); }

		XORSTR_FORCEINLINE value_t* get_ptr( ) { return reinterpret_cast< value_t* >( m_storage ); }

		XORSTR_FORCEINLINE value_t* get( ) {
			crypt( );

			return get_ptr( );
		}
	};

	template <class T, uint32_t... S, uint32_t... K>
	XORSTR_FORCEINLINE constexpr auto create( T str_lambda, std::index_sequence<S...>, std::index_sequence<K...> ) { return vectorized_t<detail::string_t<str_lambda( )[ S ]...>, detail::key_t<K, detail::get_key8<K>( )>...>( ); }
}

#define CREATE_XOR_STR(txt) xor_str::create([]() { return txt; }, std::make_index_sequence<sizeof(txt) / sizeof(*txt)>(), std::make_index_sequence<xor_str::detail::get_buffer_size<sizeof(txt)>()>())

#ifdef _DEBUG
#define _(txt) txt
#else
#define _(txt) CREATE_XOR_STR(txt).get()
#endif