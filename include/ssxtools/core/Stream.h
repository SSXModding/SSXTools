#ifndef SSXTOOLS_CONCEPTS_H
#define SSXTOOLS_CONCEPTS_H

#include <cstdint>
#include <concepts>
#include <bit> // Uint* methods use std::endian, so include it here..

namespace ssxtools::core {

	/**
	 * Stream seek direction.
	 *
	 * Maps directly to the IOStreams seek enum, or C file io.
	 */
	enum class StreamSeekDirection {
		Begin,
		Current,
		End
	};

	/**
	 * Stream offset type.
	 */
	using StreamOff = std::int64_t;

	/**
	 * Constrains a template function to working with a SSXTools stream.
	 * Streams in the SSXTools codebase are not like C++ IOStreams,
	 * rather, they're a simplified version of... say, MFC's CArchive.
	 *
	 * Only, they don't work with some specific data set,
	 * and can be molded into whatever the stream user wants.
	 *
	 * One design goal of this is to avoid virtual dispatch.
	 * Any functions desiring to interact with a Stream should be statically
	 * dispatching to a given stream type at compile time.
	 *
	 * HOWEVER, this shouldn't result in (so much) extra pain on the programmer's behalf.
	 */
	template<class T>
	concept Stream = requires(T stream, std::uint8_t byte) {
		// This is a consteval static trait function which returns true
		// if the stream is a read stream.
		//
		// If the stream is in fact reading, then all methods on it will go from
		// say "Byte(const std::uint8_t&)" to "Byte(std::uint8_t&)" and the read result
		// will go into the passed reference.
		{ T::IsReadStream() } -> std::same_as<bool>;
		
		// Byte() is one of the few functions we can test for.
		{ stream.Byte(byte) } -> std::same_as<bool>;

		{ stream.Seek(0, StreamSeekDirection::Begin) } -> std::same_as<bool>;
		{ stream.Tell() } -> std::same_as<StreamOff>;
	};

	/**
	 * Handy macro to avoid writing the boilerplate to
	 * catch an error during transformation, just wrap your call in this instead!
	 */
#define SSXTOOLS_CATCH_ERROR(x) if(!(x)) return false

}

#endif //SSXTOOLS_CONCEPTS_H
