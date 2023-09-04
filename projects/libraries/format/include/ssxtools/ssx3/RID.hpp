#pragma once
#include <ssxtools/Types.hpp>
#include <ssxtools/uint24.hpp>

namespace ssxtools::format {

	/// a Resource Identifier. This is unique per-section and per resource type.
	struct [[gnu::packed]] RID {
		constexpr static RID New(u8 section, u32 uid) {
			RID r;
			r.sectionId = section;
			r.uid = uid;
			return r;
		}

		u24le uid;
		u8 sectionId;
	};

	constexpr static RID Global = RID::New(0, 0);

} // namespace ssxtools::format
