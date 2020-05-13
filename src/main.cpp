#include <iostream>
#include <exception>
#include <fstream>
#include "TextureExtractor.h"

void cli_usage(char* prog) {
	std::cout << "EAGLe: WIP tool to extract EAGL PS2 .SSH textures\n";
	std::cout << "2019 modeco80 under the MIT License\n";
	std::cout << "Usage:\n";
	std::cout << "  " << prog << " <input file>\n";
}

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

		for(int i = 0; i < extractor.GetHeader().FileTextureCount; ++i) {
			extractor.ReadImage(i);
			extractor.WriteImage(i);
		}
			
	} catch (std::exception & e) {
		std::cout << "Error: " << e.what() << '\n';
		return 1;
	}

	stream.close();
	return 0;
}
