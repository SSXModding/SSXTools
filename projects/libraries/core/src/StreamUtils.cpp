#include <ssxtools/StreamUtils.hpp>

#include <cstdint>

namespace ssxtools::core {

	namespace detail {

		void ReadStreamTypeImpl(std::istream& is, char* buffer, std::size_t size) {
			if(!is || is.eof())
				throw std::runtime_error("stream is bad");

			is.read(&buffer[0], size);
		}

		void WriteStreamTypeImpl(std::ostream& os, const char* buffer, std::size_t buffer_size) {
			os.write(&buffer[0], buffer_size);
		}

	} // namespace detail

	std::vector<u8> ReadBytes(std::istream& is, std::size_t size) {
		std::vector<u8> buffer(size);
		is.read(std::bit_cast<char*>(&buffer[0]), size);
		return buffer;
	}

	std::string ReadAsciiZString(std::istream& is) {
		std::string s;
		char c;

		if(!is)
			return "";

		while(true) {
			c = static_cast<char>(is.get());

			if(c == '\0')
				return s;

			s.push_back(c);
		}
	}

	std::string ReadAsciiPString(std::istream& is) {
		std::string s;
		char c;

		if(!is)
			return "";

		// should be just resizing, and refactor this loop to not do this,
		// but .... meh. I'll get to it if it's a problem
		std::uint8_t length = is.get();
		s.reserve(length);

		while(true) {
			c = static_cast<char>(is.get());

			if(c == '\0')
				return s;

			s.push_back(c);
		}
	}

	void WriteBytes(std::ostream &os, const std::vector<u8> &bytes) {
		os.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
	}

} // namespace ssxtools::core
