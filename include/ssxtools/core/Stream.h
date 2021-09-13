#ifndef SSXTOOLS_CONCEPTS_H
#define SSXTOOLS_CONCEPTS_H

#include <concepts>
#include <bit> // Uint* methods use std::endian, so include it here..

namespace ssxtools::core {

	/**
	 * Constrains a template function to working with streams.
	 * Streams in the SSXTools codebase are not like C++ IOStreams,
	 * rather, they're a simplified version of... say, MFC's CArchive.
	 * Only, they don't work with some specific data set,
	 * and can be molded into whatever the stream user wants.
	 */
	template<class T>
	concept Stream = requires(T stream, std::uint8_t byte){
		// This is a consteval static trait function which returns true
		// if the stream is a read stream.
		//
		// If the stream is in fact reading, then all methods on it will go from,
		// say "Byte(const std::uint8_t&)" to "Byte(std::uint8_t&)" and the read result
		// will go into the passed reference.
		{ T::IsReadStream() } -> std::same_as<bool>;
		
		// Byte() is one of the few functions we can test for.
		{ stream.Byte(byte) } -> std::same_as<bool>;
	};

	/**
	 * Handy macro to avoid writing the boilerplate to
	 * catch an error during transformation, just wrap your call in this instead!
	 */
#define SSXTOOLS_CATCH_ERROR(x) if(!(x)) return false

}

#endif //SSXTOOLS_CONCEPTS_H
