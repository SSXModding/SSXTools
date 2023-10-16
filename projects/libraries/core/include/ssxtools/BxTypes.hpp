//! BX types. These are data-storage only, and should
//! be converted into more sensible formats for display.

#pragma once
#include <ssxtools/Types.hpp>

/// ...
#define ___COMBINE(a, b) a##b
#define __COMBINE(a, b) ___COMBINE(a, b)
#define SSXTOOLS_PADDING(size) u8 __COMBINE(__ssxtools_padding_dont_introspect, __LINE__)[size];

namespace ssxtools::core {

	struct [[gnu::packed]] tVec2 {
		float x;
		float y;
	};

	struct [[gnu::packed]] tVec3 {
		float x;
		float y;
		float z;
	};

	struct [[gnu::packed]] tVec4 {
		float x;
		float y;
		float z;
		float w;
	};

	struct [[gnu::packed]] tMat4 {
		float mats[4][4];
	};

	struct [[gnu::packed]] tARGB {
		float a;
		float r;
		float g;
		float b;
	};

	// yowch

	struct [[gnu::packed]] tVec3_Aligned {
		SSXTOOLS_PADDING(8);
		float x;
		SSXTOOLS_PADDING(40);
		float y;
		SSXTOOLS_PADDING(40);
		float z;
		SSXTOOLS_PADDING(48);
	};

} // namespace ssxtools::core
