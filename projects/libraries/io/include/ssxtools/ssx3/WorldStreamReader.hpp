#pragma once
#include <ssxtools/ssx3/Stream.hpp>
#include <ssxtools/EacRiffInStream.hpp>
#include <vector>

namespace ssxtools::io {
	
	/// Reader for SSX 3 World Stream data.
	struct WorldStreamReader {

		/// Read.
		void Read(std::istream& is);

		usize StreamCount() const {
			return uninterleavedStreams.size();
		}

		usize StreamDataSize(usize index) const {
			return uninterleavedStreams[index].size();
		}

		/// Get a given stream (after reading)
		format::ssx3::InterleaveHeader* GetStream(usize index) {
			return std::bit_cast<format::ssx3::InterleaveHeader*>(uninterleavedStreams[index].data());
		}

	private:
		using StreamData = std::vector<u8>;
		std::vector<StreamData> uninterleavedStreams;
	};

}
