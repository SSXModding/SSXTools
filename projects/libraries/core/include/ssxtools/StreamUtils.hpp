#pragma once

#include <iostream>
#include <ssxtools/Types.hpp>
#include <string>
#include <vector>

namespace ssxtools::core {

	namespace detail {
		void ReadStreamTypeImpl(std::istream& is, char* buffer, std::size_t size);
		void WriteStreamTypeImpl(std::ostream& os, const char* buffer, std::size_t buffer_size);
	} // namespace detail

	// This is lame. But it works :)
	template <class T>
	constexpr T ReadStreamType(std::istream& is) {
		T object {};
		detail::ReadStreamTypeImpl(is, std::bit_cast<char*>(&object), sizeof(T));
		return object;
	}

	template <class T>
	constexpr void WriteStreamType(std::ostream& os, const T& object) {
		detail::WriteStreamTypeImpl(os, std::bit_cast<char*>(&object), sizeof(T));
	}

	std::vector<u8> ReadBytes(std::istream& is, std::size_t size);

	std::string ReadAsciiZString(std::istream& is);

	std::string ReadAsciiPString(std::istream& is);

} // namespace ssxtools::core
