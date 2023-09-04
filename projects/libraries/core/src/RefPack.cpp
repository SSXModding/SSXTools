#include <ssxtools/RefPack.hpp>

namespace ssxtools::core {

	std::optional<std::vector<u8>> RefPackDecompress(const std::vector<u8>& compressed) {
		const u8* in = compressed.data();

		// Command variables
		u8 first;
		u8 second;
		u8 third;
		u8 fourth;

		// output buffer
		std::vector<u8> out;

		u32 proc_len;
		u32 ref_run;
		u8* ref_ptr;

		if(compressed.empty())
			return {};

		u16 signature = ((in[0] << 8) | in[1]);

		// a bit of robustness that was integrated into SSX Tricky's RefPack mechanism
		// but not integrated into TSO (which probably came from the EAC library directly like C&C)
		// is to check if the 0xFB magic exists before trying to decompress what could be
		// malformed data.
		//
		// We do that. It's probably a good idea(TM).
		if(in[1] != 0xFB) {
			return std::nullopt;
		}

		in += sizeof(u16);

		// skip uint24 compressed size field
		if(signature & 0x0100)
			in += 3;

		// read the uint24 decompressed size (if it exists)
		// TODO: see if we can use uint24_le_t directly!
		u32 decompressed_size = ((in[0] << 16) | (in[1] << 8) | in[2]);
		in += 3;

		// then resize output buffer

		out.resize(decompressed_size);
		u8* outptr = out.data();

		while(true) {
			// Retrive the first command byte
			first = *in++;

			if(!(first & 0x80)) {
				// 2-byte command: 0DDRRRPP DDDDDDDD
				second = *in++;

				proc_len = first & 0x03;

				for(u32 i = 0; i < proc_len; i++)
					*outptr++ = *in++;

				ref_ptr = outptr - ((first & 0x60) << 3) - second - 1;
				ref_run = ((first >> 2) & 0x07) + 3;

				for(u32 i = 0; i < ref_run; ++i)
					*outptr++ = *ref_ptr++;

			} else if(!(first & 0x40)) {
				// 3-byte command: 10RRRRRR PPDDDDDD DDDDDDDD
				second = *in++;
				third = *in++;

				proc_len = second >> 6;

				for(u32 i = 0; i < proc_len; ++i)
					*outptr++ = *in++;

				ref_ptr = outptr - ((second & 0x3f) << 8) - third - 1;
				ref_run = (first & 0x3f) + 4;

				for(u32 i = 0; i < ref_run; ++i)
					*outptr++ = *ref_ptr++;

			} else if(!(first & 0x20)) {
				// 4-byte command: 110DRRPP DDDDDDDD DDDDDDDD RRRRRRRR
				second = *in++;
				third = *in++;
				fourth = *in++;

				proc_len = first & 0x03;

				for(u32 i = 0; i < proc_len; ++i)
					*outptr++ = *in++;

				ref_ptr = outptr - ((first & 0x10) << 12) - (second << 8) - third - 1;
				ref_run = ((first & 0x0c) << 6) + fourth + 5;

				for(u32 i = 0; i < ref_run; ++i)
					*outptr++ = *ref_ptr++;
			} else {
				// 1-byte command: 111PPPPP

				proc_len = (first & 0x1f) * 4 + 4;

				if(proc_len <= 0x70) {
					// no stop flag

					for(u32 i = 0; i < proc_len; ++i)
						*outptr++ = *in++;

				} else {
					// has a stop flag
					proc_len = first & 0x3;

					for(u32 i = 0; i < proc_len; ++i)
						*outptr++ = *in++;

					break;
				}
			}
		}

		return out;
	}

} // namespace ssxtools::core
