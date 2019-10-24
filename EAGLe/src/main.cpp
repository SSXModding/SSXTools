#include <iostream>
#include <exception>
#include <fstream>
#include "TextureExtractor.h"

int main(int argc, char** argv) {
	std::ifstream stream("FLEX16.SSH");
	TextureExtractor extractor(stream);
	try {
		extractor.Extract();
	} catch (std::exception& e) {
		std::cout << "Caught exception: " << e.what() << '\n';
	}
}
