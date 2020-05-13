#include <cstring>
#include <sstream>
#include "Util.h"
#include "TextureExtractor.h"
#include "EndianSwap.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace eagle {
namespace core {

	bool TextureExtractor::CheckValidHeader(const ShpsFileHeader& header) {
		if(SizedCmp(header.Magic, "SHPS")) {
			auto tellLength = [&]() -> uint32 {
				uint32 oldpos = stream.tellg();
				stream.seekg(0, std::istream::end);
	
				uint32 pos = stream.tellg();
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

	void TextureExtractor::ReadHeader() {
		ReadFromStream(stream, header);

		if(!CheckValidHeader(header))
			throw std::invalid_argument("EAGL SSH header invalid");
	}

	void TextureExtractor::ReadTOC() {
		for(uint32 i = 0; i < header.FileTextureCount; ++i) {
			ShpsTocEntry entry;
			ReadFromStream(stream, entry);

			toc.push_back(entry);
		}
	}

	ShpsImage TextureExtractor::ReadImage(int imageIndex) {
		if(imageIndex > toc.size())
			return {};

		ShpsTocEntry& e = toc[imageIndex];
		ShpsImage image;

		stream.seekg(e.StartOffset, std::istream::beg);

		ReadFromStream<ShpsImageHeader>(stream, image);

		uint32 start = stream.tellg();
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

	// TODO(modeco80): See if I can get alpha to work
	// Alpha is stored in every palette but adding it to the image makes it break
	void TextureExtractor::WriteImage(int index) {
		if(index > toc.size())
			return;

		constexpr int32 CHANNEL_COUNT = 3;
		ShpsImage image = images[index];

		std::string sshname = GetFileName();
		sshname.replace(sshname.find_first_of(".SSH"), sshname.find_first_of(".SSH") - sshname.length(), "");

		// TODO: compose a path with std::filesystem instead of this garbage
		std::string filename = sshname + "_" + std::to_string(index) + ".png";
		std::vector<byte> imageData;

		imageData.resize(image.width * image.height * CHANNEL_COUNT);

		// Specific to lut256 images
		if(image.format == ShpsImageType::Lut256) {

			byte* ptr = imageData.data();
			byte* texel = image.data.data();
			for(int i = 0; i < image.width * image.height; ++i) {
					*(ptr++) = image.palette[*(texel)].color.b;
					*(ptr++) = image.palette[*(texel)].color.g;
					*(ptr++) = image.palette[*(texel)].color.r;
					texel++;
			}	
		}

		stbi_write_png(filename.c_str(), image.width, image.height, 3, imageData.data(), image.width * CHANNEL_COUNT);
		imageData.clear();
	}

}
}