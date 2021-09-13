#ifndef SSXTOOLS_UINT24_H
#define SSXTOOLS_UINT24_H

#include <cstdint>

// core::Stream concept
#include <ssxtools/core/Stream.h>

namespace ssxtools::core {

	namespace detail {

		/**
		 * A sane 24-bit integer type.
		 */
		template<bool BE> // TODO: We're C++20, we can use std::endian.
		struct basic_uint24 {
		   private: // TODO: Public?
			constexpr void Set(std::uint32_t val) {
				if constexpr(BE) {
					bytes_[0] = (val >> 16) & 0xFF;
					bytes_[1] = (val >> 8) & 0xFF;
					bytes_[2] = val & 0xFF;
				} else {
					bytes_[2] = (val >> 16) & 0xFF;
					bytes_[1] = (val >> 8) & 0xFF;
					bytes_[0] = val & 0xFF;
				}
			}

			constexpr std::uint32_t Get() {
				if constexpr(BE) {
					return bytes_[0] << 16 | bytes_[1] << 8 | bytes_[2];
				} else {
					return bytes_[2] << 16 | bytes_[1] << 8 | bytes_[0];
				}
			}

		   public:
			constexpr operator std::uint32_t() { // NOLINT (We want this to be implicitly convertiable in expressions)
				return Get();
			}

			constexpr basic_uint24& operator=(std::uint32_t val) {
				Set(val);
				return *this;
			}

#define BASICOP(op) constexpr basic_uint24& operator op (const basic_uint24& other) { Set(Get() op other.Get()); return *this; }
			BASICOP(+)
			BASICOP(-)
			BASICOP(*)
			BASICOP(/)

			// bit manip
			BASICOP(^)
			BASICOP(&)
			BASICOP(|)
#undef BASICOP

			// TODO: Implement Transform for this properly (needed for shape chunks and c0fb)
			// Since basic_uint24 does endian handling itself (unfortunately),
			// couldn't this just be FixedArray<sizeof(bytes_)>()?
			template<core::Stream Stream>
			inline bool Transform(Stream& stream) {
				return true;
			}

			// NO TOUCHY
			std::uint8_t bytes_[3];
		};

		static_assert(sizeof(basic_uint24<false>) == 3, "Your compiler sucks and basic_uint24<false> somehow is not 3 bytes in size. Get a better one");

	} // namespace detail

	// uint24 types

	/**
	 * Little endian unsigned 24-bit integer.
	 */
	using uint24le = detail::basic_uint24<false>;

	/**
	 * Big endian unsigned 24-bit integer.
	 */
	using uint24be = detail::basic_uint24<true>;

	// this using is NEVER used. Drop it?
	using uint24 = detail::basic_uint24<std::endian::native == std::endian::big>;

} // namespace ssxtools::shps

#endif //EAGLE_UINT24_H
