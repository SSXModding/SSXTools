#include <ssxtools/EacRiffStream.hpp>
#include <ssxtools/StreamUtils.hpp>

namespace ssxtools::core {

	EacRiffStream::EacRiffStream(std::istream& is) : stream(is) {
	}

	bool EacRiffStream::ReadChunkImpl(FourCCT& fourCC, std::vector<u8>& data) {
		try {
			auto riffHeader = ReadStreamType<RiffChunk>(stream);
			auto size = riffHeader.size - sizeof(RiffChunk);

			if(stream.eof())
				return false;

			// should probably idiot check this at some point but yknow living dangerous
			data = ReadBytes(stream, size);
			fourCC = riffHeader.ident;

			return true;
		} catch(std::runtime_error& re) {
			// probably trying to keep reading a EOF'd stream or something
			// tell our code that in a less "invasive" fashion
			return false;
		}
	}

} // namespace ssxtools::core
