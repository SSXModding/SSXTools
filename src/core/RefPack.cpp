#include "Core.h"

namespace eagle {
	namespace core {
		namespace refpack {

			// TODO: refpack compression
			// TODO: deobfuscate

			std::vector<byte> Decompress(Span<byte> compressed) {
				const byte* in = compressed.get();
				byte byte0;
				byte byte1;
				byte byte2;
				byte byte3;
				std::vector<byte> out;

				uint32 proc_len;
				uint32 ref_len;
				byte* ref_ptr;

				// null checking
				if(!in || compressed.size() == 0)
					return {};

				uint16 signature = ((in[0] << 8) | in[1]);
				in += 2;

				// skip u24 compressed size field
				if(signature & 0x0100)
					in += 3;

				// read the uint24 decompressed size
				uint32 decompressed_size = ((in[0] << 16) | (in[1] << 8) | in[2]);
				in += 3;

				out.resize(decompressed_size);
				byte* outptr = out.data();

				while(1) {
					byte0 = *in++;

					if(!(byte0 & 0x80)) {
						// 2-byte command: 0DDRRRPP DDDDDDDD
						byte1 = *in++;

						proc_len = byte0 & 0x03;
						for(uint32 i = 0; i < proc_len; i++)
							*outptr++ = *in++;

						ref_ptr = outptr - ((byte0 & 0x60) << 3) - byte1 - 1;
						ref_len = ((byte0 >> 2) & 0x07) + 3;

						for(uint32 i = 0; i < ref_len; ++i)
							*outptr++ = *ref_ptr++;

					} else if(!(byte0 & 0x40)) {
						// 3-byte command: 10RRRRRR PPDDDDDD DDDDDDDD
						byte1 = *in++;
						byte2 = *in++;

						proc_len = byte1 >> 6;

						for(uint32 i = 0; i < proc_len; ++i)
							*outptr++ = *in++;

						ref_ptr = outptr - ((byte1 & 0x3f) << 8) - byte2 - 1;
						ref_len = (byte0 & 0x3f) + 4;

						for(uint32 i = 0; i < ref_len; ++i)
							*outptr++ = *ref_ptr++;

					} else if(!(byte0 & 0x20)) {
						// 4-byte command: 110DRRPP DDDDDDDD DDDDDDDD RRRRRRRR
						byte1 = *in++;
						byte2 = *in++;
						byte3 = *in++;

						proc_len = byte0 & 0x03;

						for(uint32 i = 0; i < proc_len; ++i)
							*outptr++ = *in++;

						ref_ptr = outptr - ((byte0 & 0x10) << 12) - (byte1 << 8) - byte2 - 1;
						ref_len = ((byte0 & 0x0c) << 6) + byte3 + 5;

						for(uint32 i = 0; i < ref_len; ++i)
							*outptr++ = *ref_ptr++;
					} else {
						// 1-byte command: 111PPPPP

						proc_len = (byte0 & 0x1f) * 4 + 4;

						if(proc_len <= 0x70) {
							// no stop flag

							for(uint32 i = 0; i < proc_len; ++i)
								*outptr++ = *in++;

						} else {
							// has a stop flag
							proc_len = byte0 & 0x3;

							for(uint32 i = 0; i < proc_len; ++i)
								*outptr++ = *in++;

							break;
						}
					}
				}

				return out;
			}

		} // namespace refpack
	}	  // namespace core
} // namespace eagle