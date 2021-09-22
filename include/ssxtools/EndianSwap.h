// Replaced by ssxtools/core/EndianUtils.h (which is far better, and a lot more ergonomic.)

#ifndef SSXTOOLS_ENDIANSWAP_H
#define SSXTOOLS_ENDIANSWAP_H

#include "Core.h"

#ifdef _MSC_VER
	#include <intrin.h>
	// Prefer the intrinsic function versions
	// of the following functions, if they
	// exist on the target. These will be similar to GCC
	// intrinisics (we never call into the CRT anyways, so, it should still be relatively quick)
	#pragma intrinsic(_byteswap_ushort)
	#pragma intrinsic(_byteswap_ulong)
	#pragma intrinsic(_byteswap_uint64)

	#define BYTESWAP16(x) _byteswap_ushort(x)
	#define BYTESWAP32(x) _byteswap_ulong(x)
	#define BYTESWAP64(x) _byteswap_uint64(x)
#elif defined(__GNUC__)
	// Builtin functions with GNU C get turned into (sometimes single-instruction) intrinisics
	// usually by default if the target supports them. Otherwise,
	// they become inline functions (which still *have* a speed penalty,
	// but far less then if it had to make a call into the C runtime)
	#define BYTESWAP16(x) __builtin_bswap16(x)
	#define BYTESWAP32(x) __builtin_bswap32(x)
	#define BYTESWAP64(x) __builtin_bswap64(x)
#else
	#error Unsupported compiler.
#endif

namespace ssxtools::core {

	/**
	 * Swap the endian of a provided value.
	 *
	 * \tparam T Type
	 * \param[in] value value to swap endian of
	 */
	template<typename T>
	inline T EndianSwap(T value) {
		static_assert(sizeof(T) > sizeof(std::uint16_t), "Structure needs to be at least 2 bytes");

		if constexpr(sizeof(T) == 2) {
			return BYTESWAP16((std::uint16_t)value);
		} else if constexpr(sizeof(T) == 4) {
			return BYTESWAP32((std::uint32_t)value);
		} else if constexpr(sizeof(T) == 8) {
			return BYTESWAP64((std::uint64_t)value);
		} else {
			// swap any structure like it's an array of shorts
			T temp {};
			memcpy(&temp, &value, sizeof(T));

			for(std::size_t i = 0; i < sizeof(T); i += sizeof(std::uint16_t))
				((std::uint16_t*)&temp)[i] = BYTESWAP16(((std::uint16_t*)&temp)[i]);

			return temp;
		}
	}

} // namespace ssxtools::core

#undef BYTESWAP16
#undef BYTESWAP32
#undef BYTESWAP64
#endif