#pragma once

#include <iostream>
#include <ssxtools/RiffChunk.hpp>
#include <vector>

#include "ssxtools/Types.hpp"

namespace ssxtools::core {

	/// This class reads individual RIFF chunks from a stream and essentially functions like
	/// a depacketizer, allowing the data to be processed elsewhere.
	struct EacRiffStream {
		explicit EacRiffStream(std::istream& is);

		/// Read a chunk from the given input stream.
		///
		/// ChunkCallback is expected to be a callable that has an overload
		/// for the given signature: `void(u32 fourCC, const std::vector<u8>& data)`
		///
		/// This function returns false once the stream has hit the end of the file.
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
		
		std::istream& stream;
	};

} // namespace ssxtools::core
