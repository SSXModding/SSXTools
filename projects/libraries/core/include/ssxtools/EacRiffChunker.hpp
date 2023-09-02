#pragma once

#include <iostream>
#include <ssxtools/RiffChunk.hpp>
#include <vector>

#include "ssxtools/Types.hpp"

namespace ssxtools::core {

	/// This class reads individual RIFF chunks from a stream and essentially functions like
	/// a depacketizer, allowing the data to be processed elsewhere.
	struct EacRiffChunker {
		explicit EacRiffChunker(std::istream& is);

		/// ChunkCallback is expected to be a callable that has an overload
		/// for the given signature: `void(u32 fourCC, const std::vector<u8>& data)`
		template <class ChunkCallback>
		bool ReadChunk(ChunkCallback&& chunkCallback) {
			u32 currChunkFcc {};
			std::vector<u8> currChunkData {};

			if(ReadChunkImpl(currChunkFcc, currChunkData)) {
				chunkCallback(currChunkFcc, currChunkData);
			} else {
				return false;
			}

			return true;
		}

	   private:
		bool ReadChunkImpl(u32& fourCC, std::vector<u8>& data);

		/// the stream we are reading from
		/// should last longer than this obviously
		std::istream& stream;
	};

} // namespace ssxtools::core
