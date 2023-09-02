#pragma once
#include <bit>
#include <ssxtools/FixedString.hpp>
#include <ssxtools/Types.hpp>

namespace ssxtools::core {

	/// strong type for FourCCs
	enum class FourCCT : u32 {};

	/// Compile-time FourCC generation
	template <FixedString fccString, std::endian Endian = std::endian::big>
	consteval FourCCT FourCC() {
		static_assert(fccString.Length() == 4, "Provided string is not a FourCC");
		switch(Endian) {
			case std::endian::little: return static_cast<FourCCT>((fccString[0] << 24) | (fccString[1] << 16) | (fccString[2] << 8) | fccString[3]);
			case std::endian::big: return static_cast<FourCCT>((fccString[0]) | (fccString[1] << 8) | (fccString[2] << 16) | (fccString[3] << 24));
		}
	}

} // namespace ssxtools::core
