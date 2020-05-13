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

		ShpsTocEntry& e = toc[imageIndex];
		ShpsImage image;

		stream.seekg(e.StartOffset, std::istream::beg);

		ReadFromStream<ShpsImageHeader>(stream, image);

		uint32 size;

		if(image.format == ShpsImageType::Lut256) {
			// 1 byte per pixel so size is w*h
			size = image.width * image.height;
		}

		image.data.resize(size);

		stream.read((char*)image.data.data(), size);

		if(image.format == ShpsImageType::Lut256
				/* || image.format == ShpsImageType::Lut256Comp */) {

			image.palette.resize(256);

			// Read in the palette.
			// First, we read the palette header.
			// (We should end up there!)
			ShpsPaletteHeader ph;
			ReadFromStream(stream, ph);

			// Then read in all of the colors.
			for(int i = 0; i < 256; ++i) {
				Shps8bppRgba color;
				ReadFromStream(stream, color);
				image.palette[i] = color;
			}
		}

		// Add the image now that we've got its data.
		images.push_back(image);
		return image;
	}

}
}