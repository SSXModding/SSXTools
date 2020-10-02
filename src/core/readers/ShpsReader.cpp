#include <cstring>
#include <sstream>
#include "Core.h"
#include "EndianSwap.h"
#include "ShpsReader.h"

namespace eagle {
	namespace core {

		bool ShpsReader::CheckValidHeader(const shps::FileHeader& header) {
			if(SizedCmp(header.Magic, "SHPS")) {
				auto length = [&]() {
					auto oldpos = reader.raw().tellg();
					 reader.raw().seekg(0, std::istream::end);

					auto pos =  reader.raw().tellg();
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

		shps::Image ShpsReader::ReadImage(int imageIndex) {
			if(imageIndex > toc.size())
				return {};

			shps::TocEntry& tocEntry = toc[imageIndex];

			shps::Image image;
			image.index = imageIndex;
			image.toc_entry = tocEntry;

			uint32 size {};

			reader.raw().seekg(tocEntry.StartOffset, std::istream::beg);

			reader.ReadSingleType<shps::ImageHeader>(image);

			// Some images are marked with 0x10(FORMAT)
			// so we manually set the image format to the paricular one
			// TODO does this mean image type is a byte?
			if(image.format == 0x1005)
				image.format = shps::ShpsImageType::NonLut32Bpp;

			if(image.format == 0x1002 || image.format == 0x5002 /* huh? I only see this in AiJ textures */)
				image.format = shps::ShpsImageType::Lut256;

			switch(image.format) {
				case shps::ShpsImageType::Lut128:
					size = (image.width * image.height) / 2;
					break;

				case shps::ShpsImageType::Lut256:
					size = image.width * image.height;
					break;

				case shps::ShpsImageType::NonLut32Bpp:
					// While * sizeof(uint32) could be used,
					// I have chosen to use Bgra8888's size as it's supposed to be sizeof(uint32)
					size = (image.width * image.height) * sizeof(shps::Bgra8888);
					break;

				default:
					// Return a empty image.
					// This is used to skip images that are unsupported.
					images.push_back(image);
					return image;
					break;
			}

			image.data.resize(size);

			reader.ReadRawBuffer((char*)image.data.data(), size);

			// TODO: Now that we have bigfile we can use its RefPack decompression code to deal with the agonizing pain that is
			// refpack shape textures.

			switch(image.format) {
				case shps::ShpsImageType::Lut128: {
					// Resize the palette to the correct size for this
					// palettized image format.
					image.palette.resize(16);

					// Read in the palette.
					// First, we read the palette header.
					// (We should end up there!)
					shps::PaletteHeader ph;
					reader.ReadSingleType(ph);

					// Then read in all of the colors.
					for(int i = 0; i < 16; ++i)
						reader.ReadSingleType(image.palette[i]);
				} break;

				case shps::ShpsImageType::Lut256: {
					// Resize the palette to the correct size for this
					// palettized image format.
					image.palette.resize(256);

					// Read in the palette.
					// First, we read the palette header.
					// (We should end up there!)
					shps::PaletteHeader ph;
					reader.ReadSingleType(ph);

					// Then read in all of the colors.
					for(int i = 0; i < 256; ++i)
						reader.ReadSingleType(image.palette[i]);
				} break;

				default:
					break;
			}

			// Add the image now that we've got its data.
			images.push_back(image);
			return image;
		}

	} // namespace core
} // namespace eagle