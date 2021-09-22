// This header will probably be phased out soon.
// Mcommon usage is going to be removed and bigfile/makecbxs
// will be integrated. This header's days are not long.

#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <type_traits>
#include <functional>
#include <cstdint>
#include <memory>

namespace ssxtools::core {

#if defined(__GNUC__) || defined(__clang__)
	#define SSXTOOLS_UNUSED __attribute__((unused))
#elif defined(_MSC_VER)
	// msvc doesn't care about unused values
	// at least it doesnt screech
	#define SSXTOOLS_UNUSED
#endif


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
	 * Turn a enum constant into a string.
	 * Needs to be specialized for each enum.
	 */
	template<class T>
	constexpr const char* EnumToString(T) {
		// TODO: this should be static_assert'd at compile time
		return "no";
	}

	/**
	 * }@
	 */

} // namespace ssxtools::shps
