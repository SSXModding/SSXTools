#include <cstring>
#include <sstream>

#include <eagle/Core.h>
#include <eagle/EndianSwap.h>
#include <eagle/ShpsReader.h>

#include "GimexCodec.h"
#include "GimexInterleavedCodec.h"

// needed for refpack shape decompression
#include <bigfile/refpack.h>

namespace eagle::core {

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

	bool ShpsReader::CheckValidHeader(const shps::FileHeader& header) {
		if(SizedCmp(header.Magic, "SHPS")) { // TODO: Investigate having this NOT be a string comparision
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
			shps::TocEntry entry {};
			reader.ReadSingleType(entry);

			toc.push_back(entry);
		}
	}

	shps::Image ShpsReader::ReadImage(int imageIndex) {
		if(imageIndex > toc.size())
			return {};

		shps::TocEntry& tocEntry = toc[imageIndex];

		// whether or not we should bother reading the
		// CLUT
		bool readclut = false;
		shps::Image image;
		std::shared_ptr<BaseGimexCodec> codec;

		image.index = imageIndex;
		image.toc_entry = tocEntry;

		uint64 size {};

		reader.raw().seekg(tocEntry.StartOffset, std::istream::beg);
		reader.ReadSingleType<shps::ImageHeader>(image);

		// fix the format of refpack shapes
		// This is a hack and I really should be masking out 0x80,
		// but this should work for 95% of cases for now
#define FIX_SHAPE_FORMAT(n, type)                   \
	if(image.format == (shps::ShpsImageType)0x8##n) \
		image.format = shps::ShpsImageType::type;

		FIX_SHAPE_FORMAT(2, Lut256);
		FIX_SHAPE_FORMAT(5, NonLut32Bpp);

#undef FIX_SHAPE_FORMAT

		// Non-CLUT shapes set the clut offset to 0.
		// In this case, we don't need to bother reading in the CLUT.
		// This also declares a previous "hack" a non-hack and actually
		// probably the intended way for non-palletized shapes to be read.
		if(image.clut_offset != 0) {
			readclut = true;
			size = image.clut_offset - sizeof(shps::ImageHeader);
		} else {
			size = (image.width * image.height) * sizeof(shps::Bgra8888);
		}

		// Make the codec beforehand if we need to
		switch(image.unknown2) { // TODO: all G357 shapes that are 8bpp seem to require this, so maybe check for THAT?
			case shps::EncodingType::Interleaved:
				codec = std::make_shared<InterleavedCodec>();
				break;

			default:
				break;
		}

		// Resize the data buffer and read the entire image's data into it.
		image.data.resize(size);
		reader.ReadRawBuffer((char*)image.data.data(), size);

		// If the image data is RefPack/FB5 compressed,
		// then decompress it.
		if(image.data[0] == 0x10 && image.data[1] == 0xFB) {
			auto decompressed = bigfile::refpack::Decompress(bigfile::MakeSpan(image.data.data(), size)); // TODO: BIGFILE SHOULD USE MCOMMON SPAN!!!

			// Resize the image buffer to the decompressed image data size.
			image.data.resize(decompressed.size());

			// Copy data back into the image data as decompressed data
			memcpy(&image.data[0], &decompressed[0], decompressed.size() * sizeof(byte));

			// Clear the temporary decompression buffer, as we no longer need it.
			//decompressed.clear();
		}

		// If we need to decode the image data using a gimex codec,
		// then do so before reading in the CLUT
		if(codec)
			image.data = codec->Decode(image);

		// Read in the CLUT if we need to.
		if(readclut) {
			reader.raw().seekg(tocEntry.StartOffset + image.clut_offset, std::istream::beg);
			shps::PaletteHeader ph {};
			reader.ReadSingleType(ph);

			// ALL palettized shapes have a CLUT header with this magic value (ascii '!') being the first byte.
			// So, if this isn't pressent, then we just return a empty shape and presume it's invalid.
			if(ph.unknown[0] != 0x21) {
				image.data.clear();
				images.push_back(image);
				return image;
			}

			// Read colors out depending on the image type.
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

} // namespace eagle::core