#pragma once
#include <ssxtools/Types.hpp>
#include <ssxtools/FixedString.hpp>
#include <string_view>

namespace ssxtools::core {

	/// BX implementation of the ELF variant of the PJW string hash.
	constexpr u32 BxStringHash(std::string_view input) {
		auto hash = 0u;

		for(auto c : input) {
			hash = (hash << 4) + c;
			auto high = hash & 0xf0000000;

			if(high)
				hash ^= high >> 23;

			hash &= ~high;
		}

		return hash;
	}

	/// BxStringHash() but always at compile time. No buts.
	template<FixedString str>
	consteval u32 BxStringHash() {
		return BxStringHash(str);
	}

	static_assert(BxStringHash("mdl_medal_03") == 0x05d26e53, "BxStringHash is not hashing properly.");
	static_assert(BxStringHash<"mdl_medal_03">() == 0x05d26e53, "BxStringHash (compile-time version) is not hashing properly.");

}
