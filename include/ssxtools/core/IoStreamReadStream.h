//
// Created by lily on 7/13/21.
//

#ifndef SSXTOOLS_IOSTREAMREADSTREAM_H
#define SSXTOOLS_IOSTREAMREADSTREAM_H

#include <bit>
#include <iostream>
#include <array>

#include <ssxtools/core/EndianUtils.h>

namespace ssxtools::core {

	/**
	 * A Stream which wraps a IOStreams input stream for reading.
	 */
	struct IoStreamReadStream {
		using IsStream = void; // this class is in fact a Stream

		explicit IoStreamReadStream(std::istream& stream)
			: stream(stream) {
		}

		/**
		 * Trait function, returns whether or not this is a read stream at compile time
		 */
		consteval static bool IsReadStream() {
			return true;
		}

		bool Byte(std::uint8_t& b);

		// This template is written once and expanded.
#define TYPE(methodName, T)    \
	template<std::endian Endian>     \
	inline bool methodName(T& t) {   \
		return ReadThing<Endian>(t); \
	}
#include <ssxtools/core/StreamTypeListing.inl>
#undef TYPE

		template<std::size_t N>
		inline bool FixedSizeArray(std::uint8_t(&arr)[N]) {
			if(!stream)
				return false;

			for(std::size_t i = 0; i < N; ++i)
				if(!this->Byte(arr[i]))
					return false;

			return true;
		}

		template<std::size_t N>
		inline bool FixedString(char(&fixedStr)[N]) {
			if(!stream)
				return false;

			for(std::size_t i = 0; i < N; ++i)
				if(!this->Byte(reinterpret_cast<std::uint8_t&>(fixedStr[i])))
					return false;

			return true;
		}

		bool String(std::string& string);

		/**
		 * Get the raw stream this is wrapping.
		 */
		std::istream& GetStream() const;

		template<class T>
		inline bool Other(T& t) {
			return t.Transform(*this);
		}

	   private:
		/**
		 * Internal helper for most types.
		 */
		template<std::endian Endian, class T>
		inline bool ReadThing(T& t) {
			if(!stream)
				return false;

			// This buffer is used to read the swappable object in.
			std::uint8_t readin_buffer[sizeof(T)];
			stream.read(reinterpret_cast<char*>(&readin_buffer[0]), sizeof(T));

			// This will reinterpret the object properly with endian (temporarily copying it).
			t = core::ReadEndian<Endian, T>(&readin_buffer[0]);
			return true;
		}

		/**
		 * The backing stream.
		 */
		std::istream& stream;
	};

} // namespace ssxtools::core

#endif //SSXTOOLS_IOSTREAMREADSTREAM_H
