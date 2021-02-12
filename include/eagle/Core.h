#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <type_traits>
#include <functional>
#include <cstdint>
#include <memory>

#include <modeco/types.h>
#include <modeco/span.h>

namespace eagle::core {

#if defined(__GNUC__) || defined(__clang__)
	#define EAGLE_UNUSED __attribute__((unused))
#elif defined(_MSC_VER)
	// msvc doesn't care about unused values
	// at least it doesnt screech
	#define EAGLE_UNUSED
#endif

	using mco::byte;
	using mco::sbyte;

	using mco::int16;
	using mco::uint16;

	using mco::int32;
	using mco::uint32;

	using mco::int64;
	using mco::uint64;

	/**
	 * \defgroup FunTemp Function templates
	 * @{
	 */

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
	template<typename T, typename T2 = T>
	inline bool SizedCmp(T* a, T2* b) {
		static_assert(sizeof(T2) == sizeof(T), "T and T2 need to be the same size..");
		return !memcmp(a, b, sizeof(T));
	}

	/**
	 * std::max_element for spans
	 */
	template<typename T, class F>
	inline T MaxSpanElement(mco::Span<T> memory, F comparator) {
		// a simple idiot check
		if(!memory.get() || memory.size() == 0)
			return {};

		// cache some frequently used things so that
		// we don't have to fetch them from the span so often
		T* ptr = memory.get();
		std::size_t size = memory.size();
		T* max = ptr;

		for(std::size_t i = 0; i < size; ++i) {
			if(comparator(*max, *ptr))
				max = ptr;
			++ptr;
		}

		return *max;
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
	 * \param[in] buf Buffer to read from.
	 * \param[in] index Index in buffer.
	 * \param[out] output Output variable.
	 */
	template<typename T>
	inline bool ReadFromBuffer(mco::Span<byte> buf, size_t index, T& out) {
		if(!buf.get() || buf.size() == 0)
			return false;

		if(buf.size() < index * sizeof(T))
			return false;

		memcpy(&buf.get()[(index * sizeof(T))], &out, sizeof(T));
		return true;
	}

	/**
	 * Make a fourcc value at compile time
	 */
	constexpr uint32 MakeFourCCValue(const char p[5]) {
		return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
	}

	/**
	 * Turn a enum constant into a string.
	 * Needs to be specialized.
	 */
	template<class T>
	constexpr const char* EnumToString(T) {
		//static_assert(false, "EnumToString not specialized for type T!");
		return "no";
	}

	/**
	 * }@
	 */

} // namespace eagle::core
