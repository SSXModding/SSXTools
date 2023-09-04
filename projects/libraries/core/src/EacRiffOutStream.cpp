#include <ssxtools/EacRiffOutStream.hpp>
#include <ssxtools/StreamUtils.hpp>

namespace ssxtools::core {

	namespace {
		template <class T>
		constexpr T AlignBy(T value, std::size_t alignment) {
			return (-value) & (alignment - 1);
		}
	} // namespace

	EacRiffOutStream::EacRiffOutStream(std::ostream& os) : outStream(os) {
	}

	void EacRiffOutStream::AddChunkImpl(FourCCT fourCC, const ChunkData& data) {
		// The size includes the size of this chunk header
		auto fullSize = static_cast<u32>(data.size() + sizeof(EacRiffChunk));
		auto eacRiff = EacRiffChunk { fourCC, fullSize };

		// Write the chunk header
		WriteStreamType(outStream, eacRiff);
		WriteBytes(outStream, data);

		// Align the next chunk to an aligned boundary
		// if an alignment was set by the user
		if(alignment)
			outStream.seekp(AlignBy(outStream.tellp(), alignment), std::ostream::cur);
	}
} // namespace ssxtools::core
