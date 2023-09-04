#pragma once
#include <ssxtools/Types.hpp>
#include <bit>
#include <type_traits>

namespace ssxtools::core {

	namespace detail {

		template<class NativeT, class OffsetType>
		constexpr NativeT* CreatePointerFromAddend(void* BasePointer, OffsetType addend) noexcept {
			return std::bit_cast<NativeT*>(static_cast<char*>(BasePointer) + addend);
		}

	}

	/// An "auto-resolving" semi-sweet (/s) pointer type.
	/// [NativeT] is the type of data this would point to
	/// [OffsetType] is the type of data the "pointer" is repressented as
	template<class NativeT, class OffsetType = u32>
	struct OffsetPtr {
		using Type = std::remove_cvref_t<NativeT>;
		using Pointer = Type*;
		using ConstPointer = const Type*;

		[[nodiscard]] constexpr OffsetType Raw() const noexcept {
			return rawOffset;
		}

		[[nodiscard]] constexpr Pointer operator()(void* baseAddr) const noexcept {
			// While yucky, it should show problem areas which aren't checking things
			// immediately rather than read invalid data that might do much worse.
			if(rawOffset == 0)
				return nullptr;

			return detail::CreatePointerFromAddend<Type>(baseAddr, rawOffset);
		}

	   private:
		OffsetType rawOffset;
	};


}
