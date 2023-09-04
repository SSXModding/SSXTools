#include <fstream>
//#include <ssxtools/EacRiffInStream.hpp>
//#include <ssxtools/EacRiffOutStream.hpp>
#include <ssxtools/ssx3/WorldStreamReader.hpp>

#include <format>

namespace ssxcore = ssxtools::core;
namespace ssxio = ssxtools::io;

int main() {
	std::ifstream ifs { "/data/sda/lily/games/ssx/ssx3/data/worlds/data/worlds/bam.ssb", std::ifstream::binary };
	
	ssxio::WorldStreamReader streamReader{};

	streamReader.Read(ifs);

	std::cout << "Read " << streamReader.StreamCount() << " streams\n";

	for(usize i = 0; i < streamReader.StreamCount(); ++i) {
		auto size = streamReader.StreamDataSize(i);
		auto begin = streamReader.GetStream(i);
		auto p = streamReader.GetStream(i);

		auto j = 0;
		while(p != nullptr) {
			//std::cout << std::format("travelled {}/{} bytes\n", static_cast<usize>(p - begin), size);
			if(static_cast<usize>(p - begin) >= size)
				break;

			//std::cout << std::format("{:08x}\n", *std::bit_cast<u32*>(p));
			//std::cout << std::format("Stream {}/File {}: RID {}.{}, Type {}, {} bytes\n", i, j, static_cast<u32>(p->rid.uid), p->rid.sectionId, static_cast<u8>(p->type), static_cast<u32>(p->dataSize));

			p = p->NextEntry();
			j++;
		}
	}
	
	return 0;
}
