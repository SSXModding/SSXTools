#pragma once
#include <ssxtools/Types.hpp>

namespace ssxtools {

	/// A hopefully-sane uint24 type
	template <std::endian Endian = std::endian::little>
	struct [[gnu::packed]] u24 {
	   private:
		constexpr void Set(u32 val) {
			if constexpr(Endian == std::endian::little) {
				bytes_[0] = (val >> 16) & 0xFF;
				bytes_[1] = (val >> 8) & 0xFF;
				bytes_[2] = val & 0xFF;
			} else {
				bytes_[2] = (val >> 16) & 0xFF;
				bytes_[1] = (val >> 8) & 0xFF;
				bytes_[0] = val & 0xFF;
			}
		}

		constexpr u32 Get() const {
			if constexpr(Endian == std::endian::little) {
				return bytes_[0] << 16 | bytes_[1] << 8 | bytes_[2];
			} else {
				return bytes_[2] << 16 | bytes_[1] << 8 | bytes_[0];
			}
		}

	   public:
		constexpr operator u32() const { return Get(); }

		/// Transmute a u24 read from some data to another endian.
		template <std::endian Endian2>
		constexpr u24<Endian2> Transmute() const {
			if constexpr(Endian2 == Endian)
				return *this;
			else {
				// copy the bytes over and return a new object
				u24<Endian2> ret;
				memcpy(&ret.bytes_[0], &bytes_[0], sizeof(bytes_));
				return ret;
			}
		}

		// explicitly allow copy/move
		constexpr u24() = default;
		constexpr u24(const u24&) = default;
		constexpr u24(u24&&) = default;

		constexpr u24 operator=(u32 val) {
			Set(val);
			return *this;
		}

		constexpr u24 operator=(u24 val) {
			Set(val.Get());
			return *this;
		}

		// Basic math

		constexpr u24 operator+(const u24 other) {
			Set(Get() + other.Get());
			return *this;
		}

		constexpr u24 operator-(const u24 other) {
			Set(Get() - other.Get());
			return *this;
		}

		constexpr u24 operator/(const u24 other) {
			Set(Get() / other.Get());
			return *this;
		}

		constexpr u24 operator*(const u24 other) {
			Set(Get() * other.Get());
			return *this;
		}

		// Bit operations

		constexpr u24 operator^(const u24 other) {
			Set(Get() ^ other.Get());
			return *this;
		}

		constexpr u24 operator&(const u24 other) {
			Set(Get() & other.Get());
			return *this;
		}

		constexpr u24 operator|(const u24 other) {
			Set(Get() | other.Get());
			return *this;
		}

		constexpr u24 operator~() { Set(~Get()); }

		// NO TOUCHY
		std::uint8_t bytes_[3];
	};

	// sanity checks
	static_assert(sizeof(u24<>) == 3, "Your compiler sucks and u24<> somehow isn't 3 bytes on it. Get a better one");

	/// Little endian unsigned 24-bit integer.
	using u24le = u24<std::endian::little>;

	/// Big endian unsigned 24-bit integer.
	using u24be = u24<std::endian::big>;

} // namespace ssxtools
