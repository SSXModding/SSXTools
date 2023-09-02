#pragma once
#include <bit>
#include <ssxtools/FixedString.hpp>
#include <ssxtools/Types.hpp>

namespace ssxtools::core {

	// TODO(lily): this.
	//enum class FourCC : u32 {};

	/**
	 * A multi-endian, compile-time FourCC generator.
	 * You love to see it.
	 */
	template <FixedString fccString, std::endian Endian = std::endian::little>
	consteval std::uint32_t FourCC() {
		static_assert(fccString.Length() == 4, "Provided string is not a FourCC");

		switch(Endian) {
			case std::endian::little:
				return (fccString[0]) | (fccString[1] << 8) | (fccString[2] << 16) | (fccString[3] << 24);

			case std::endian::big:
				return (fccString[0] << 24) | (fccString[1] << 16) | (fccString[2] << 8) | fccString[3];
		}

		// throw in this case to make this a compile time error :)
		throw 0xff'ff'ff'ff;
	}

} // namespace europa::util
