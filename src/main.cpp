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

	std::ifstream stream(argv[1], std::ifstream::in | std::ifstream::binary);

	TextureExtractor extractor(stream);
	try {
		extractor.Extract();
	} catch (std::exception & e) {
		std::cout << "Error: " << e.what() << '\n';
		return 1;
	}
	return 0;
}
