#include <cstring>
#include <sstream>
#include "Core.h"
#include "EndianSwap.h"
#include "ShpsReader.h"

// needed for refpack shape decompression
#include <bigfile/refpack.h>

namespace eagle {
	namespace core {

		bool ShpsReader::CheckValidHeader(const shps::FileHeader& header) {
			if(SizedCmp(header.Magic, "SHPS")) {
				auto length = [&]() {
					auto oldpos = reader.raw().tellg();
					reader.raw().seekg(0, std::istream::end);

					auto pos = reader.raw().tellg();
					reader.raw().seekg(oldpos, std::istream::beg);
					return pos;
				}();

				if(length == header.FileLength) {
					return true;
				} else {
					return false;
				}
			} else {
				return false;
			}
		}

		bool ShpsReader::ReadHeader() {
			reader.ReadSingleType(header);

			if(!CheckValidHeader(header))
				return false;

			return true;
		}

		void ShpsReader::ReadTOC() {
			for(uint32 i = 0; i < header.FileTextureCount; ++i) {
				shps::TocEntry entry;
				reader.ReadSingleType(entry);

				toc.push_back(entry);
			}
		}

		/**
		 * template function to read palette
		 * \tparam N amount of colors to read
		 */
		template<int N>
		constexpr void ReadPalette(std::vector<shps::Bgra8888>& palette, mco::BinaryReader& reader) {
			palette.resize(N);

			for(int i = 0; i < N; ++i)
				reader.ReadSingleType<shps::Bgra8888>(palette[i]);
		}

		/**
		 * utility to read a 24-bit LE value
		 */
		constexpr uint32 DoRead24LE(mco::BinaryReader& reader) {
			byte buf[3] {};
			reader.ReadSingleType(buf);

			return ((buf[2] << 16) | (buf[1] << 8) | buf[0]);
		}

		shps::Image ShpsReader::ReadImage(int imageIndex) {
			if(imageIndex > toc.size())
				return {};

			shps::TocEntry& tocEntry = toc[imageIndex];

			bool readclut = false;

			shps::Image image;
			image.index = imageIndex;
			image.toc_entry = tocEntry;

			uint64 size {};

			reader.raw().seekg(tocEntry.StartOffset, std::istream::beg);

			reader.ReadSingleType<shps::ShpsImageType>(image.format);

			// fix the format of refpack shapes
			// This is a hack and I really should be masking out 0x80,
			// but this should work for 95% of cases for now

#define FIX_SHAPE_FORMAT(n, type) \
	if(image.format == (shps::ShpsImageType)0x8##n)    \
		image.format = shps::ShpsImageType::type;

			FIX_SHAPE_FORMAT(2, Lut256);
			FIX_SHAPE_FORMAT(5, NonLut32Bpp);

#undef FIX_SHAPE_FORMAT

			image.clut_offset = DoRead24LE(reader);

			reader.ReadSingleType<shps::ImageHeader>(image);

			if(image.clut_offset != 0) {
				// Signal the need to read a CLUT after the image data
				readclut = true;
				size = image.clut_offset - sizeof(shps::ImageHeader);
			} else {
				size = (image.width * image.height) * sizeof(shps::Bgra8888);
			}

			image.data.resize(size);
			reader.ReadRawBuffer((char*)image.data.data(), size);
			
			// Decompress the RefPack data out into raw image data.
			if(image.data[0] == 0x10 && image.data[1] == 0xFB) {
				auto decompressed = bigfile::refpack::Decompress(bigfile::MakeSpan(image.data.data(), size));
				image.data.clear();
				image.data.resize(decompressed.size());
				memcpy(&image.data[0], &decompressed[0], decompressed.size() * sizeof(byte));
			}

			// Read in the CLUT if we need to.
			if(readclut) {
				reader.raw().seekg(tocEntry.StartOffset + image.clut_offset, std::istream::beg);
				shps::PaletteHeader ph;
				reader.ReadSingleType(ph);

				// ALL palettized shapes have a CLUT header with this magic value being the first byte.
				// So, if this isn't pressent, then we just return a empty shape and presume it's invalid.
				if(ph.unknown[0] != 0x21) {
					images.push_back({});
					return {};
				}

				switch(image.format) {
					case shps::ShpsImageType::Lut128:
						ReadPalette<16>(image.palette, reader);
						break;

					case shps::ShpsImageType::Lut256:
						ReadPalette<256>(image.palette, reader);
						break;

					default:
						break;
				}
			}

			// Add the image now that we've got its data.
			images.emplace_back(image);
			return image;
		}

	} // namespace core
} // namespace eagle