#include <ostream>
#include <ssxtools/EacRiffChunk.hpp>
#include <vector>

namespace ssxtools::core {

	/// An output stream adapter which creates EAC-RIFF chunk format data.
	struct EacRiffOutStream {
		// TODO(lily): ...? should be a span<u8>.
		using ChunkData = std::vector<u8>;
		explicit EacRiffOutStream(std::ostream& os);

		template <FixedString fcc>
		void AddChunk(const ChunkData& data) {
			// forward to the impl
			AddChunkImpl(FourCC<fcc>(), data);
		}

		void SetAlignment(u32 newAlignment) { alignment = newAlignment; }

	   private:
		void AddChunkImpl(FourCCT fcc, const ChunkData& data);

		std::ostream& outStream;

		u32 alignment {};
	};

} // namespace ssxtools::core
