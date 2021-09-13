//
// Created by lily on 7/13/21.
//

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
	concept Stream = requires(T stream) {
		typename T::IsStream;

		// This is a consteval static trait function which returns true
		// if the stream is reading.
		{ T::IsReadStream() } -> std::same_as<bool>;
	};

	/**
	 * Handy macro to avoid writing the boilerplate to
	 * catch an error during transformation, just wrap your call in this instead!
	 */
#define SSXTOOLS_CATCH_ERROR(x) if(!(x)) return false

}

#endif //SSXTOOLS_CONCEPTS_H
