#ifndef SSXTOOLS_ENDIANUTILS_H
#define SSXTOOLS_ENDIANUTILS_H

#include <bit>
#include <climits>
#include <cstdint>

// In this case, the compiler detection in this file is intended
// to condense intrinsics into a single portable set of macros, 
// so that the code is more readable, and does not have lots of #ifdef garbage all over the place.
// Basically, a "get the ugliness done and over with so we can replace it with better code" strategy.

#ifdef _MSC_VER // MSVC
	#include <intrin.h>

	// Prefer the intrinsic function versions of the following functions, if available.
	#pragma intrinsic(_byteswap_ushort)
	#pragma intrinsic(_byteswap_ulong)
	#pragma intrinsic(_byteswap_uint64)

	#define SSXTOOLS_BYTESWAP16(x) _byteswap_ushort(x)
	#define SSXTOOLS_BYTESWAP32(x) _byteswap_ulong(x)
	#define SSXTOOLS_BYTESWAP64(x) _byteswap_uint64(x)
#else
	#ifdef __GNUC__ // GCC & Clang (including Emscripten)
		// Builtin functions with GNU C & Clang get turned into (sometimes single-instruction) intrinsics
		// usually by default if the target supports them.
		// Otherwise, they become inline functions
		// (which still *have* a speed penalty, but far less then if it had to make a call into the C runtime)
		#define SSXTOOLS_BYTESWAP16(x) __builtin_bswap16(x)
		#define SSXTOOLS_BYTESWAP32(x) __builtin_bswap32(x)
		#define SSXTOOLS_BYTESWAP64(x) __builtin_bswap64(x)
	#else
		#error Unsupported compiler. Check in support for it?
	#endif
#endif

namespace ssxtools::core {
	/**
	 * Concept constraining a type which is byte-swappable by detail::Swap<T>().
	 * This type must be:
	 *
	 *  - Bigger than or equal to sizeof(std::uint16_t)...
	 *  - BUT smaller than or equal to sizeof(std::uint64_t).
	 *  - It must also be a "trivial" type i.e std::is_trivial_v<T> must be true.
	 */
	template<class T>
	concept IsSwappable = (sizeof(T) >= sizeof(std::uint16_t)) && (sizeof(T) <= sizeof(std::uint64_t)) && std::is_trivial_v<T>;

	namespace detail {

		/**
		 * Do a swap of a thing of type T.
		 * Uses the above portability macros to use intrinsics.
		 */
		template<class T>
		constexpr T Swap(const T& val) requires(IsSwappable<T>) {
			switch(sizeof(T)) {
				case sizeof(std::uint16_t):
					return SSXTOOLS_BYTESWAP16(val);

				case sizeof(std::uint32_t):
					return SSXTOOLS_BYTESWAP32(val);

				case sizeof(std::uint64_t):
					return SSXTOOLS_BYTESWAP64(val);

				// TODO: A worst-case path which swaps
				//  sizeof(std::uint16_t) aligned types.
				//  Once applied here, IsSwappable will need to be altered to remove the second size check,
				//  and replace it with a size *alignment* check.
			}
		}

// Once this function is done these "portability" macros are NOT needed anymore and can be undefined.
#undef SSXTOOLS_BYTESWAP16
#undef SSXTOOLS_BYTESWAP32
#undef SSXTOOLS_BYTESWAP64

		/**
		 * Perform an endian swap if the host compile endian
		 * is equal to the Endian template parameter, otherwise,
		 * simply return the value. 
		 *
		 * Does unfortunately copies the value,
		 * however swappable things are trivial, so it shouldn't be too terrible.
		 *
		 * This is intended to be used with the OppositeEndian trait.
		 */
		template<std::endian Endian, class T>
		inline T SwapIfEndian(const T& val) requires(IsSwappable<T>) {
			if constexpr(std::endian::native == Endian)
				return Swap(val);
			return val;
		}

		/**
		 * Get a reference as T to a buffer.
		 */
		template<class T>
		constexpr T& PointerTo(void* ptr) {
			return *static_cast<T*>(ptr);
		}

		/**
		 * Get a const reference as T to a const buffer.
		 *
		 * Exactly the same as the above PointerTo, however
		 * participates in overload resolution only if T is const.
		 */
		template<class T>
		constexpr T& PointerTo(const void* ptr) requires(std::is_const_v<T>) {
			return *static_cast<T*>(ptr);
		}

		/**
		 * Trait function to return the opposite endian to the one which is given.
		 */
		consteval std::endian OppositeEndian(std::endian e) {
			switch(e) {
				case std::endian::little:
					return std::endian::big;
				case std::endian::big:
					return std::endian::little;
			}
		}

	} // namespace detail

	/**
	 * Read a value from a buffer, possibly swapping the endian only if required.
	 *
	 * On systems where Endian is the host compile endian already, then,
	 * this essentially becomes a return of detail::PointerTo<const T>().
	 *
	 * \tparam Endian the endian of the value.
	 * \tparam T the value type. Must follow IsSwappable<T>.
	 */
	template<std::endian Endian, class T>
	std::remove_cvref_t<T> ReadEndian(const std::uint8_t* base) requires(IsSwappable<std::remove_cvref_t<T>>) {
		const auto& ref = detail::PointerTo<const std::remove_cvref_t<T>>(base);
		return detail::SwapIfEndian<detail::OppositeEndian(Endian), std::remove_cvref_t<T>>(ref);
	}

	/**
	 * Write a value to a buffer, possibly swapping the endian only if required.
	 * 
	 * On systems where Endian is the host compile endian already, then,
	 * this essentially is a store of the value directly. Nothing else.
	 *
	 * \tparam Endian The endian of the value.
	 * \tparam T The value type. Must follow IsSwappable<T>.
	 */
	template<std::endian Endian, class T>
	void WriteEndian(std::uint8_t* base, const std::remove_cvref_t<T>& val) requires(IsSwappable<std::remove_cvref_t<T>>) {
		auto& i = detail::PointerTo<std::remove_cvref_t<T>>(base);
		i = detail::SwapIfEndian<detail::OppositeEndian(Endian), std::remove_cvref_t<T>>(val);
	}

} // namespace ssxtools::core

#endif //SSXTOOLS_ENDIANUTILS_H
