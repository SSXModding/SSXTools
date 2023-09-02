#pragma once

#include <ssxtools/ssx3/RID.hpp>
#include <ssxtools/Types.hpp>
#include <ssxtools/uint24.hpp>

namespace ssxtools::format::ssx3 {

	enum StreamInterleaveType : u8 {
		/// I have no idea if these are correct
		Unk = 0,
		Patch = 1,
		Model = 2,
		Instance = 3,

		/// These ones are

		StreamTexture = 9,
		LightmapTexture = 10,

		AudioBank = 20, /// An audio bank

		BeforeEnd = 0x12, // 0x48 bytes of padding, unknown purpose
		End = 0x16		  // no data?
	};

	struct StreamInterleaveHeader {
		/// The resource identifier of this interleaved resource
		/// (where it goes basically?)
		RID rid;

		/// The type of this interleaved resource
		StreamInterleaveType type;

		/// The data size of this interleaved resource.
		u24<> dataSize;
	};

} // namespace ssxtools::format::ssx3
