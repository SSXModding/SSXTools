#include <iostream>
#include <exception>
#include <fstream>
#include <string>
#include "ShpsReader.h"
#include "writer.h"

using namespace eagle::core;
using namespace eagle::writer;

void cli_usage(char* prog) {
	std::cout << "EAGLe CLI: Command-line tool to convert EAGL .SSH texture banks to .PNG files.\n";
	std::cout << "(C) 2019-2020 modeco80 under the MIT License\n";
	std::cout << "Usage:\n";
	std::cout << "  " << prog << " <input file>\n";
}

/**
 * Progress function for the CLI.
 */
void cli_progress(std::string& progress, ProgressType type) {
	std::string type_str;

	switch(type) {
	case ProgressType::Info:
		type_str = "[Writer Info]";
		break;
		
	case ProgressType::Error:
		type_str = "[Writer Error]";
		break;

	default:
		type_str = "[Writer Unknown]";
		break;
	}

	std::cout << type_str << ' ' << progress << '\n';
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

	ShpsReader reader(stream, filename);

	SetProgressFunction(cli_progress);

	try {
		reader.ReadHeader();
		reader.ReadTOC();

		ShpsFileHeader& header = reader.GetHeader();

		std::cout << "SSH Info:" << '\n';
		std::cout << "File size: " << (float)header.FileLength/1000 << " kBytes" << '\n';
		std::cout << "Image count: " << header.FileTextureCount << " files" << '\n';

		for(uint32 i = 0; i < header.FileTextureCount; ++i) {
			reader.ReadImage(i);
		}

		for(uint32 i = 0; i < header.FileTextureCount; ++i) {
			auto& images = reader.GetImages();
			ShpsImage& image = images[i];
			
			WriteImage(image, filename);
		}

		std::cout << "Finished conversion, cleaning up...\n";

		for(ShpsImage& image : reader.GetImages()) {
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
