#include <cstring>
#include <sstream>
#include "Util.h"
#include "EndianSwap.h"
#include "ShpsReader.h"

namespace eagle {
namespace core {

	bool ShpsReader::CheckValidHeader(const ShpsFileHeader& header) {
		if(SizedCmp(header.Magic, "SHPS")) {
			auto tellLength = [&]() {
				auto oldpos = stream.tellg();
				stream.seekg(0, std::istream::end);
	
				auto pos = stream.tellg();
				stream.seekg(oldpos, std::istream::beg);
				return pos;
			};

			if(tellLength() == header.FileLength) {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

	void ShpsReader::ReadHeader() {
		ReadFromStream(stream, header);

		// TODO: Adopt a google-style "No exception" thing
		if(!CheckValidHeader(header))
			throw std::invalid_argument("EAGL SSH header invalid");
	}

	void ShpsReader::ReadTOC() {
		for(uint32 i = 0; i < header.FileTextureCount; ++i) {
			ShpsTocEntry entry;
			ReadFromStream(stream, entry);

			toc.push_back(entry);
		}
	}

	ShpsImage ShpsReader::ReadImage(int imageIndex) {
		if(imageIndex > toc.size())
			return {};

		ShpsTocEntry& tocEntry = toc[imageIndex];

		ShpsImage image;
		image.index = imageIndex;
		image.toc_entry = tocEntry;

		uint32 size{};

		stream.seekg(tocEntry.StartOffset, std::istream::beg);

		ReadFromStream<ShpsImageHeader>(stream, image);

		// Some images are marked with 0x10(FORMAT)
		// so we manually set the image format to the paricular one
		// TODO does this mean image type is a byte?
		if(image.format == 0x1005)
			image.format = ShpsImageType::NonLut32Bpp;

		if(image.format == 0x1002)
			image.format = ShpsImageType::Lut256;

		switch(image.format) {

		case ShpsImageType::Lut128:
			size = (image.width * image.height) / 2;
			break;

		case ShpsImageType::Lut256:
			size = image.width * image.height;
			break;

		case ShpsImageType::NonLut32Bpp:
			// While * sizeof(uint32) could be used,
			// I have chosen to use ShpsRgba's size as it's supposed to be sizeof(uint32)
			size = (image.width * image.height) * sizeof(ShpsRgba);
			break;

		default:
			// Return a empty image.
			// This is used to skip images that are unsupported.
			images.push_back(image);
			return image;
			break;
		}

		image.data.resize(size);

		stream.read((char*)image.data.data(), size);

		switch(image.format) {

		case ShpsImageType::Lut128: {
			// Resize the palette to the correct size for this 
			// palettized image format.
			image.palette.resize(16);

			// Read in the palette.
			// First, we read the palette header.
			// (We should end up there!)
			ShpsPaletteHeader ph;
			ReadFromStream(stream, ph);

			// Then read in all of the colors.
			for (int i = 0; i < 16; ++i) {
				ShpsRgba color;
				ReadFromStream(stream, color);
				image.palette[i] = color;
			}
		} break;

		case ShpsImageType::Lut256: {
			// Resize the palette to the correct size for this 
			// palettized image format.
			image.palette.resize(256);

			// Read in the palette.
			// First, we read the palette header.
			// (We should end up there!)
			ShpsPaletteHeader ph;
			ReadFromStream(stream, ph);

			// Then read in all of the colors.
			for(int i = 0; i < 256; ++i) {
				ShpsRgba color;
				ReadFromStream(stream, color);
				image.palette[i] = color;
			}
		} break;

		default:
			break;
		}

		// Add the image now that we've got its data.
		images.push_back(image);
		return image;
	}

}
}