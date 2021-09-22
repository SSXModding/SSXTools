//
// Created by lily on 9/21/2021.
//

#ifndef SSXTOOLS_MEMORYREADSTREAM_H
#define SSXTOOLS_MEMORYREADSTREAM_H

#include <ssxtools/core/Stream.h>

namespace ssxtools::core {

	/**
	 * A Stream which reads from a memory buffer.
	 */
	struct MemoryReadStream {

		// constructor for an arbitrary container of arbitrary type.
		template<class Container>
		inline MemoryStream(const Container& c) :
			MemoryStream(reinterpret_cast<const std::uint8_t*>(c.data()), c.size() * sizeof(typename Container::value_type)) {

		}

		// Helper constructor. Calls Setup() automatically
		MemoryStream(const std::uint8_t* begin, std::size_t length);

		// null out buffer
		void Reset();

		bool Seek(std::int64_t pos, StreamSeekDirection dir);

	   private:
		const std::uint8_t* begin;
		const std::uint8_t* curr;
		const std::uint8_t* end;

		/**
		 * A helper to assert if we can actually read an amount of bytes.
		 *
		 * \param[in] size how many bytes requested
		 * \return True if the MemoryStream can read this much data from current position, false otherwise
		 */
		bool AssertCanReadSize(std::size_t size) const;
	};

}

#endif //SSXTOOLS_MEMORYREADSTREAM_H
