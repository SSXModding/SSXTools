//! EAC-RIFF types.
#pragma once

#include <ssxtools/FourCC.hpp>
#include <ssxtools/Types.hpp>

namespace ssxtools::core {

	/// A EAC-RIFF chunk.
	struct [[gnu::packed]] EacRiffChunk {
		FourCCT ident {};
		u32 size {};
	};

	static_assert(sizeof(EacRiffChunk) == 8, "get a compiler that doesn't suck asshole thanks");

} // namespace ssxtools::core
