#include <iostream>
#include <exception>
#include <fstream>
#include "TextureExtractor.h"

void cli_usage(char* prog) {
	std::cout << "EAGLe: Tool to extract EAGL .SSH textures\n";
	std::cout << "2019 modeco80 under the MIT License\n";
	std::cout << "Usage:\n";
	std::cout << "  " << prog << " <input file>\n";
}

// TODO: this CLI is mostly temporary.
// I want to be able to make a GUI tool with a selector and stuff for images in a container
// and a preview of each texture
int main(int argc, char** argv) {
	if (argc < 2) {
		cli_usage(argv[0]);
		return 1;
	}

	std::string filename(argv[1]);
	std::ifstream stream(filename, std::ifstream::in | std::ifstream::binary);

	if(!stream) {
		std::cout << "Could not open file for reading\n";
		return 1;
	}

	eagle::core::TextureExtractor extractor(stream, filename);

	try {
		extractor.ReadHeader();
		extractor.ReadTOC();

		eagle::core::ShpsFileHeader header = extractor.GetHeader();

		std::cout << "SSH Info:" << '\n';
		std::cout << "File size: " << (float)header.FileLength/1000 << " kBytes" << '\n';
		std::cout << "Image count: " << header.FileTextureCount << " files" << '\n';

		for(int i = 0; i < extractor.GetHeader().FileTextureCount; ++i) {
			extractor.ReadImage(i);
			eagle::core::ShpsImage& image = extractor.GetImages()[i];

			std::cout << "Texture information:\n";
			std::cout << "WxH: " << image.width << 'x' << image.height << '\n';
			std::cout << "Image size: " << (float)(image.width * image.height)/1000 << "kBytes\n";
			if(image.format == eagle::core::ShpsImageType::Lut256) {
				std::cout << "Image is an 8bpp image\n";
			}

			extractor.WriteImage(i);
		}

		std::cout << "Finished extraction, doing some cleanup\n";

		for(eagle::core::ShpsImage& image : extractor.GetImages()) {
			if(!image.palette.empty())
				image.palette.clear();
			
			image.data.clear();
		}
			
	} catch (std::exception & e) {
		std::cout << "Error: " << e.what() << '\n';
		return 1;
	}

	stream.close();
	return 0;
}
