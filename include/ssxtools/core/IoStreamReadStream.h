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
	 * A Stream which wraps a C++ IOStreams input stream for reading.
	 */
	struct IoStreamReadStream {

		// Stream concept implementation

		/**
		 * Returns whether or not this is a read stream at compile time.
		 */
		consteval static bool IsReadStream() {
			return true;
		}

		explicit IoStreamReadStream(std::istream& stream)
			: stream(stream) {
		}

		bool Byte(std::uint8_t& b);

		// TODO: SByte()?

// This template is written once and expanded,
// using IoStreamReadStream::ReadThing<Endian, T>() as a helper,
// to avoid writing the same code so many times.
//
// I'm sorry. Forgive me for my 
#define TYPE(methodName, T)    \
	template<std::endian Endian>     \
	inline bool methodName(T& t) {   \
		return ReadThing<Endian>(t); \
	}
#include <ssxtools/core/StreamTypeListing.inl>
#undef TYPE

		// FIXME: Document these methods, they are part of Stream

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


		/**
		 * Read a *null-terminated* string.
		 */
		bool String(std::string& string);

		template<class T>
		inline bool Other(T& t) {
			return t.Transform(*this);
		}

		/**
		 * Get the raw stream this IoStreamReadStream is wrapping.
		 */
		[[nodiscard]] std::istream& GetStream() const;

	   private:

		/**
		 * Internal helper for most types.
		 */
		template<std::endian Endian, class T>
		inline bool ReadThing(T& t) {
			// Don't bother.
			if(!stream)
				return false;

			std::uint8_t readin_buffer[sizeof(T)];

			// Meh... Could we use FixedArray<sizeof(T)>()?
			// that way we could catch an EOF or something a little nicer
			stream.read(reinterpret_cast<char*>(&readin_buffer[0]), sizeof(T));

			// This will reinterpret the object properly (temporarily copying it.) and swap its endian
			// if required.
			//
			// But, really, it's stack space we're copying, so it's not too big of a deal,
			// as it will be freed at the end of the day when returning.
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
