#include <cstring>
#include <ssxtools/EacRiffInStream.hpp>
#include <ssxtools/RefPack.hpp>
#include <ssxtools/ssx3/WorldStreamReader.hpp>

namespace ssxtools::io {

	namespace {
		/// this weaves a chunk together
		struct ChunkWeaverState {
			explicit ChunkWeaverState(std::istream& is, std::vector<std::vector<u8>>& dataBuffers)
				: inputStream(is), stream(inputStream), dataBuffers(dataBuffers) {}

			void WeaverGotChunk(core::FourCCT fourCC, const std::vector<u8>& data) {
				// unpack the refpack data
				auto uncompressed = core::RefPackDecompress(data);
				if(!uncompressed.has_value()) {
					fail = true;
					return;
				}

				auto& uncompressedData = uncompressed.value();

				if(fourCC == core::FourCC<"CEND">()) {
					end = true;
				}

				workBuffer.resize(workBuffer.size() + uncompressedData.size());
				memcpy(&workBuffer[workBufferIndex], &uncompressedData[0], uncompressedData.size());

				workBufferIndex += uncompressedData.size();
			}

			void ResetWorkBuffer() {
				// Reserve ~1mb so that we don't allocate often while
				// weaving the world stream data together.
				//workBuffer.reserve(1024 * 1024);
				workBufferIndex = 0;
			}

			void ReadImpl() {
				ResetWorkBuffer();
				while(true) {
					auto res = stream.ReadChunk([&](auto fcc, const auto& data) { WeaverGotChunk(fcc, data); });
					if(!res)
						break;

					if(fail)
						break;

					if(end) {
						end = false;
						// move the stream into the vector
						dataBuffers.emplace_back(std::move(workBuffer));
						ResetWorkBuffer();
					}
				}
			}

		   private:
			std::istream& inputStream;
			core::EacRiffInStream stream;

			std::vector<u8> workBuffer;
			usize workBufferIndex { 0 };

			std::vector<std::vector<u8>>& dataBuffers;

			bool fail = false; // invalid
			bool end = false;
		};
	} // namespace

	void WorldStreamReader::Read(std::istream &is) {
		ChunkWeaverState weaver {is, uninterleavedStreams};
		weaver.ReadImpl();
	}

} // namespace ssxtools::io
