#pragma once

#include <ssxtools/Types.hpp>
#include <ssxtools/ssx3/RID.hpp>

namespace ssxtools::format::ssx3 {

	enum StreamInterleaveType : u8 {
		Unk = 0,
		Patch = 1,
		Model = 2,
		Instance = 3,

		StreamTexture = 9,
		LightmapTexture = 10,

		AudioBank = 20,

		BeforeEnd = 0x12, // 0x48 bytes of padding
		End = 0x16		  // no data
	};

	// TODO for future lily
	// not rn
	struct StreamInterleaveHeader {


	};

}
