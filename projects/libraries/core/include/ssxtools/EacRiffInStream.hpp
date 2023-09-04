#pragma once

#include <iostream>
#include <ssxtools/EacRiffChunk.hpp>
#include <vector>

#include "ssxtools/FourCC.hpp"
#include "ssxtools/Types.hpp"

namespace ssxtools::core {

	/// This class reads individual EAC-RIFF chunks from a input stream and essentially functions like
	/// a depacketizer, allowing the data to be processed elsewhere.
	struct EacRiffInStream {
		explicit EacRiffInStream(std::istream& is);

		/// Read a chunk from the given input stream.
		///
		/// ChunkCallback is expected to be a callable that has an overload
		/// for the given signature: `void(FourCCT fourCC, const std::vector<u8>& data)`
		///
		/// This function returns false once the stream has hit the end of the file.
		template <class ChunkCallback>
		bool ReadChunk(ChunkCallback&& chunkCallback) {
			FourCCT currChunkFcc {};
			std::vector<u8> currChunkData {};

			if(ReadChunkImpl(currChunkFcc, currChunkData)) {
				chunkCallback(currChunkFcc, currChunkData);
			} else {
				return false;
			}

			return true;
		}

	   private:
		bool ReadChunkImpl(FourCCT& fourCC, std::vector<u8>& data);

		std::istream& stream;
	};

} // namespace ssxtools::core
