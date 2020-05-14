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
		image.index = imageIndex;

		uint32 size{};

		stream.seekg(e.StartOffset, std::istream::beg);

		ReadFromStream<ShpsImageHeader>(stream, image);

		switch(image.format) {

		case ShpsImageType::Lut256:
			size = image.width * image.height;
			break;

		default:
			images.push_back(image);
			return image;
			break;
		}

		image.data.resize(size);

		stream.read((char*)image.data.data(), size);

		switch(image.format) {

		case ShpsImageType::Lut256: {
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