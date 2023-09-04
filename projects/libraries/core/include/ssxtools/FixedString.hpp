#pragma once

#include <ssxtools/Types.hpp>

namespace ssxtools::core {

	/// A compile-time string. Usable as a C++20 cNTTP.
	template <usize N>
	struct FixedString {
		char buf[N + 1] {};

		constexpr FixedString(const char* s) {
			for(usize i = 0; i != N; ++i)
				buf[i] = s[i];
		}

		constexpr operator const char*() const {
			return buf;
		}

		[[nodiscard]] constexpr usize Length() const { return N; }
	};

	template <usize N>
	FixedString(char const (&)[N]) -> FixedString<N - 1>;

} // namespace ssxtools::core
