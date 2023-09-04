//! BX types

#pragma once
#include <ssxtools/Types.hpp>

namespace ssxtools::core {

	struct tVec2 {
		float x;
		float y;
	};

	struct tVec3 {
		float x;
		float y;
		float z;
	};

	struct tVec4 {
		float x;
		float y;
		float z;
		float w;
	};

	struct tMat4 {
		float mats[4][4];
	};

	struct tARGB {
		float a;
		float r;
		float g;
		float b;
	};

} // namespace ssxtools::core
