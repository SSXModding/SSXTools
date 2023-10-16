#pragma once
#include <bit>
#include <ssxtools/Types.hpp>
#include <type_traits>
#include <span>

namespace ssxtools::core {

	namespace detail {

		template <class NativeT, class OffsetType>
		constexpr NativeT* CreatePointerFromAddend(void* BasePointer, OffsetType addend) noexcept {
			return std::bit_cast<NativeT*>(static_cast<char*>(BasePointer) + addend);
		}

	} // namespace detail

	/// An "auto-resolving" semi-sweet (/s) pointer type.
	/// This is designed to allow resolving offsets in data for 
	/// games written before 64-bit pointers were common/used at all.
	/// This allows looking up data a lot easier :)
	///
	/// [NativeT] is the type of data this would point to
	/// [OffsetType] is the type of data the "pointer" is repressented as
	template <class NativeT, class OffsetType = u32>
	struct OffsetPtr final {
		using Type = std::remove_cvref_t<NativeT>;
		using Pointer = Type*;
		using ConstPointer = const Type*;

		/// Set the offset. Duh!
		constexpr void Set(OffsetType newOffset) noexcept { rawOffset = newOffset; }

		[[nodiscard]] constexpr OffsetType Raw() const noexcept { return rawOffset; }

		[[nodiscard]] constexpr Pointer operator()(void* baseAddr) const noexcept {
			// While yucky, it should show problem areas which aren't checking things
			// immediately rather than read invalid data that might do much worse.
			if(rawOffset == 0)
				return nullptr;

			return detail::CreatePointerFromAddend<Type>(baseAddr, rawOffset);
		}

		template<class NativeU>
		constexpr OffsetPtr<NativeU, OffsetType>& PtrCast() {
			// Safety: The data layout of OffsetPtr<> stays
			// the exact same regardless of the result type, therefore
			// even though this is *techinically* UB (? using bit_cast it shouldn't be ?),
			// this isn't problematic
			return *std::bit_cast<OffsetPtr<NativeU, OffsetType>*>(this);
		}

	   private:
		OffsetType rawOffset;
	};

	/// Like OffsetPtr<T> but for arrays of data
	template <class NativeT, class OffsetType = u32>
	struct OffsetArrayPtr final {
		using Type = std::remove_cvref_t<NativeT>;
		using Pointer = Type*;
		using ConstPointer = const Type*;

		using Span = std::span<NativeT>;

		/// Set the offset. Duh!
		constexpr void Set(OffsetType newOffset) noexcept { rawOffset = newOffset; }

		[[nodiscard]] constexpr OffsetType Raw() const noexcept { return rawOffset; }

		[[nodiscard]] constexpr Span operator()(void* baseAddr, OffsetType length) const noexcept {
			// While yucky, it should show problem areas which aren't checking things
			// immediately rather than read invalid data that might do much worse.
			if(rawOffset == 0 || length == 0)
				return {};

			return { detail::CreatePointerFromAddend<Type>(baseAddr, rawOffset), length };
		}

	   private:
		OffsetType rawOffset;
	};

} // namespace ssxtools::core
