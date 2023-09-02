#pragma once

#include <ssxtools/FourCC.hpp>
#include <ssxtools/Types.hpp>

namespace ssxtools::core {

	/// A RIFF chunk.
	struct [[gnu::packed]] RiffChunk {
		u32 ident {};
		u32 size {};
	};

	static_assert(sizeof(RiffChunk) == 8, "get a compiler that doesn't suck asshole thanks");

} // namespace ssxtools::core
