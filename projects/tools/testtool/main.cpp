#include <fstream>
#include <ssxtools/EacRiffInStream.hpp>
#include <ssxtools/EacRiffOutStream.hpp>

namespace ssxcore = ssxtools::core;

int main() {
	std::ifstream ifs { "/data/sda/lily/games/ssx/ssx3/data/worlds/data/worlds/bam.ssb", std::ifstream::binary };
	// std::ifstream ifs { "/data/sda/lily/games/ssx/ssx3/data/movies/eabig.mpc", std::ifstream::binary };

	std::ofstream ofs { "./the_cooler_bam.ssb", std::ofstream::binary };

	ssxcore::EacRiffInStream inStream { ifs };

	// create a output stream that can output bam.ssb
	ssxcore::EacRiffOutStream outStream { ofs };
	outStream.SetAlignment(0x8000);

	while(true) {
		bool ok = inStream.ReadChunk([&](ssxcore::FourCCT fourCC, const std::vector<u8>& data) {
			if(fourCC == ssxcore::FourCC<"CBXS">()) {
				outStream.AddChunk<"CBXS">(data);
			} else if(fourCC == ssxcore::FourCC<"CEND">()) {
				outStream.AddChunk<"CEND">(data);
			}
		});
		if(!ok)
			break;
	}

	std::cout << "done!\n";

	ofs.flush();
	return 0;
}
