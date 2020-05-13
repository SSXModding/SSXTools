#pragma once
#include <iostream>
#include <fstream>
#include <type_traits>
#include <cstdint>
#include <stdint.h>

namespace eagle {
namespace core {



	/**
	 * Static assert helper for enforcing POD types while not being ugly.
	 * Forward-compatible with C++20's deprecation of is_pod<T>.
	 *
	 * \tparam T type to assert is POD.
	 */
	template<class T>
	struct PodStaticAssert {
#if __cplusplus >= 201704L
		static_assert(std::is_standard_layout<T>::value, "type not POD");
#else
		static_assert(std::is_pod<T>::value, "type not POD");
#endif
	};

	/**
	 * Macro to easily assert a type is POD.
	 */
	#define REQUIRES_POD(T) static PodStaticAssert<T> pod__##T;


#ifdef __GNUC__
	#define PACK_STRUCT __attribute__((packed))
	#define RESET_PACKING
#elif defined(_MSC_VER)
	#define PACK_STRUCT_INTERNAL(N) __pragma(pack(N))
	#define PACK_STRUCT PACK_STRUCT_INTERNAL(1)
	#define RESET_PACKING __pragma(pack(pop))
#endif

	/**
	 * Define a getter function.
	 */
	#define EAGLE_GETTER(Type, funName) inline Type funName()

	
	/**
	 * Define a setter function.
	 */
	#define EAGLE_SETTER(Type, funName) inline void funName(Type& New)

	typedef std::uint8_t byte;
	typedef std::int8_t sbyte;
	
	typedef std::int16_t int16;
	typedef std::uint16_t uint16;

	typedef std::int32_t int32;
	typedef std::uint32_t uint32;

	typedef std::int64_t int64;
	typedef std::uint64_t uint64;


	/**
	 * \defgroup FunTemp Function templates
	 * @{
	 */

	/** Reads value of T from a open stream
	 * T can be assumed, but, if you're doing
	 * ```cpp
	 * struct ThingHeader { things in the file ... }
	 * struct Thing : public ThingHeader { things not in the file ... }
	 * ```
	 * you can specify what T should be as well.
	 *
	 * \tparam T Type to read.
	 *
	 * \param[in] stream Stream to read from.
	 * \param[out] output Output variable.
	 */
	template<class T>
	inline bool ReadFromStream(std::istream& stream, T& output) {
		REQUIRES_POD(T);

		if (!stream)
			return false;

		stream.read((char*)&output, sizeof(T));
		return true;
	}

	/**
	 * Read a value of type T from a buffer.
	 *
	 * Follows the same rules as ReadFromStream(),
	 * only that this is a buffer instead of a file, and
	 * each additional index travels a stride of sizeof(T) bytes.
	 *
	 * \tparam T Type to read.
	 *
	 * \param[in] buf Stream to read from.
	 * \param[in] index Index in buffer.
	 * \param[out] output Output variable.
	 */
	template<typename T>
	inline bool ReadFromBuffer(char* buf, size_t index, T& out) {
		REQUIRES_POD(T);

		if(!buf)
			return false;

		memcpy(&buf[(index * sizeof(T))], &out, sizeof(T));
		return true;
	}

	/**
	 * Print a fixed array of type T.
	 *
	 * \tparam T Type of the array.
	 * \tparam size Size of the array.
	 *
	 * \param[out] os Stream to write to.
	 * \param[in] arr Array.
	 */
	template<class T, uint32 size = 0>
	inline void PrintFixedArray(std::ostream& os, T arr[size]) {
		std::stringstream ss;
		for(uint32 i = 0; i < size; ++i)
			ss << (char)arr[i];

		os << ss.str();
		ss.clear();
	}
	
	/**
	 * Compare two objects of type T or 
	 * two objects of type T and T2.
	 * 
	 * \tparam T First type.
	 * \tparam T2 Second type.
	 *
	 * \param[in] a First object to compare.
	 * \param[in] b Second object to compare.
	 */
	template<typename T, typename T2>
	inline bool SizedCmp(T* a, T2* b) {
		// We require both operand types to be POD
		REQUIRES_POD(T);
		REQUIRES_POD(T2);

		return !memcmp(a, b, sizeof(T));
	}

	
	/**
	 * Compare two objects of type T or 
	 * two objects of type T and T2, with an additional offset.
	 * 
	 * \tparam T First type.
	 * \tparam T2 Second type.
	 * \tparam Offset Offset.
	 *
	 * \param[in] a First object to compare.
	 * \param[in] b Second object to compare.
	 */
	template<typename T, typename T2, std::size_t Offset = 0>
	inline bool SizedCmpOff(T* a, T2* b) {
		// We require both operand types to be POD
		REQUIRES_POD(T);
		REQUIRES_POD(T2);

		return !memcmp(a, b, sizeof(T) - Offset);
	}

	/**
	 * }@
	 */

#undef REQUIRES_POD

}
}


