#pragma once
#include <ssxtools/Types.hpp>

namespace ssxtools::format {

	/// a Resource Identifier. This is unique per-section and per resource type.
	struct RID {
		constexpr RID() = default;

		/// The section ID.
		constexpr u8 SectionId() const { return storage & 0xff; }

		/// The unique ID as a part of this section
		constexpr u32 Uid() const { return storage >> 24; }

	   private:
		u32 storage;
	};

} // namespace ssxtools::format
