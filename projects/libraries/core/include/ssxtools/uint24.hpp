#include <ssxtools/Types.hpp>

namespace ssxtools {

	// TODO: instead of encoding the endian as a type signature
	// maybe I can do .Get<Endian> to have some sort of runtime dispatch
	// .. but then stuff would be very very verbose so idk
	// guess it's trading off some demons for other worse demons

	/// A hopefully-sane uint24 type
	template <std::endian Endian = std::endian::little>
	struct [[gnu::packed]] u24 {
	   private:
		constexpr void Set(std::uint32_t val) {
			if constexpr(Endian == std::endian::big) {
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
			if constexpr(Endian == std::endian::big) {
				return bytes_[0] << 16 | bytes_[1] << 8 | bytes_[2];
			} else {
				return bytes_[2] << 16 | bytes_[1] << 8 | bytes_[0];
			}
		}

	   public:
		constexpr operator std::uint32_t() const { return Get(); }

		constexpr u24& operator=(std::uint32_t val) {
			Set(val);
			return *this;
		}

		constexpr u24& operator=(u24 val) {
			Set(val.Get());
			return *this;
		}

		// Basic math

		constexpr u24& operator+(const u24& other) {
			Set(Get() + other.Get());
			return *this;
		}

		constexpr u24& operator-(const u24& other) {
			Set(Get() - other.Get());
			return *this;
		}

		constexpr u24& operator/(const u24& other) {
			Set(Get() / other.Get());
			return *this;
		}

		constexpr u24& operator*(const u24& other) {
			Set(Get() * other.Get());
			return *this;
		}

		// Bit operations

		constexpr u24& operator^(const u24& other) {
			Set(Get() ^ other.Get());
			return *this;
		}

		constexpr u24& operator&(const u24& other) {
			Set(Get() & other.Get());
			return *this;
		}

		constexpr u24& operator|(const u24& other) {
			Set(Get() | other.Get());
			return *this;
		}

		constexpr u24& operator~() { Set(~Get()); }

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
