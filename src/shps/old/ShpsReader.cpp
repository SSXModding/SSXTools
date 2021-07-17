#include <cstring>
#include <sstream>

#include <ssxtools/Core.h>
#include <ssxtools/EndianSwap.h>
#include <ssxtools/shps/ShpsReader.h>

// needed for refpack shape decompression
#include <bigfile/refpack.h>

namespace ssxtools::shps {

	/**
	 * Template function to read palette so I don't repeat myself as much
	 * \tparam N amount of colors to read
	 */
	template<core::uint16 N>
	constexpr void ReadPalette(std::vector<Bgra8888>& palette, mco::BinaryReader& reader) {
		palette.resize(N);

		for(int i = 0; i < N; ++i)
			reader.ReadSingleType<Bgra8888>(palette[i]);
	}

	bool ShpsReader::CheckValidHeader(const FileHeader& header) {
		if(core::EndianSwap(header.Magic) == core::MakeFourCCValue("SHPS")) {
			auto length = [&]() {
				auto oldpos = reader.raw().tellg();
				reader.raw().seekg(0, std::istream::end);

				auto pos = reader.raw().tellg();
				reader.raw().seekg(oldpos, std::istream::beg);
				return pos;
			}();

			// the game doesn't verify this, however when writing we probably should.
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
		for(core::uint32 i = 0; i < header.FileTextureCount; ++i) {
			shps::TocEntry entry {};
			reader.ReadSingleType(entry);

			toc.push_back(entry);
		}
	}

	shps::Image ShpsReader::ReadImage(int imageIndex) {
		if(imageIndex > toc.size())
			return {};

		shps::TocEntry& tocEntry = toc[imageIndex];

		// whether or not we should bother reading the CLUT
		bool readclut = false;
		Image image;

		auto error = [&image]() {
			// clear fields on error
			image.data.clear();
			image.palette.clear();
		};

		image.index = imageIndex;
		image.toc_entry = tocEntry;

		core::uint64 size {};

		reader.raw().seekg(tocEntry.StartOffset, std::istream::beg);
		reader.ReadSingleType<ImageHeader>(image);

		// fix the format of refpack shapes
		// This is a hack and I really should be testing out bits,
		// but this should work for right now.
		// If this ends up requiring more I'll fix it.

#define FIX_SHAPE_FORMAT(n, type)                    \
	if(image.format == (shps::ShapeImageType)0x8##n) \
		image.format = shps::ShapeImageType::type;

		FIX_SHAPE_FORMAT(2, Lut256);
		FIX_SHAPE_FORMAT(5, NonLut32Bpp);

#undef FIX_SHAPE_FORMAT

		// Non-CLUT shapes set the clut offset to 0.
		// In this case, we don't need to bother reading in the CLUT.
		// This also declares a previous "hack" a non-hack and actually
		// probably the intended way for non-palletized shapes to be read.
		if(image.clut_offset != 0) {
			readclut = true;
			size = image.clut_offset - sizeof(ImageHeader);
		} else {
			size = (image.width * image.height) * sizeof(Bgra8888);
		}

		// Resize the data buffer and read the entire image's data into it.
		image.data.resize(size);
		reader.ReadRawBuffer((char*)image.data.data(), size);

		// If the image data is RefPack/FB5 compressed,
		// then decompress it.
		if(image.data[0] == 0x10 && image.data[1] == 0xFB) {
			auto decompressed = bigfile::refpack::Decompress(bigfile::MakeSpan(image.data.data(), size)); // TODO: BIGFILE SHOULD USE MCOMMON SPAN!!!

			// refpack::Decompress returns an empty vector on decompression failure.
			if(decompressed.empty()) {
				error();
				return image;
			}

			// Resize the image buffer to fit decompressed data.
			image.data.resize(decompressed.size());

			// Copy the decompressed data into the image.
			memcpy(&image.data[0], &decompressed[0], decompressed.size() * sizeof(core::byte));
		}

		// Read in the CLUT header if we are a palettized shape.
		if(readclut) {
			reader.raw().seekg(tocEntry.StartOffset + image.clut_offset, std::istream::beg);
			shps::PaletteHeader ph {};
			reader.ReadSingleType(ph);

			// ALL palettized shapes have a CLUT header with this magic value (ascii '!') being the first byte.
			// So if this isn't present, then we just return a empty shape and presume it's invalid.
			if(ph.unknown[0] != 0x21) {
				error();
				//images.push_back(image);
				return image;
			}

			// Read colors out depending on the image type.
			switch(image.format) {
				case shps::ShapeImageType::Lut128:
					ReadPalette<16>(image.palette, reader);
					break;

				case shps::ShapeImageType::Lut256:
					ReadPalette<256>(image.palette, reader);
					break;

				default:
					break;
			}

			// In a old version I had this explicitly checking for the Gimex version 3 shapes have
			// ... i should probably do that still ....
			auto IsSSX3Shape = [&]() -> bool {
				// TODO: don't endian swap where not needed (portability matters...)
				return !(core::EndianSwap(header.creator) == shps::GimexVersion_SSX
					   || core::EndianSwap(header.creator) == shps::GimexVersion_SSXT);
			};

			// SSX3 shape images with a CLUT tile the colors for some reason.
			if(IsSSX3Shape() && image.format == shps::ShapeImageType::Lut256) {
				std::vector<shps::Bgra8888> palette_copy { image.palette.size() };
				constexpr int tile_x = 8;
				constexpr int tile_y = 2;

				// this is a certified hack... But it works..
				// maybe it isn't a hack.. Who knows?
				int ntx = 16 / tile_x;
				int nty = 16 / tile_y;
				int i = 0;

				for(int ty = 0; ty < nty; ty++)
					for(int tx = 0; tx < ntx; tx++)
						for(int y = 0; y < tile_y; y++)
							for(int x = 0; x < tile_x; x++)
								palette_copy[(ty * tile_y + y) * 16 + (tx * tile_x + x)] = image.palette[i++];

				image.palette = palette_copy;
			}

		}

		// Add the image now that we've got its data.
		images.emplace_back(image);
		return image;
	}

} // namespace ssxtools::shps