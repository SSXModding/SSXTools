#pragma once
#include "Core.h"
namespace eagle {
	namespace core {
		namespace refpack {

			/**
			 * Decompress a RefPack source.
			 * 
			 * \param[in] compressed Compressed span of data.
			 * \return A decompressed buffer.
			 */
			std::vector<byte> Decompress(Span<byte> compressed);

		} // namespace refpack
	}	  // namespace core
} // namespace eagle