#include <iostream>
#include <exception>
#include <fstream>
#include <string>
#include <filesystem>
#include "ShpsReader.h"
#include "writer.h"

using namespace eagle::core;
using namespace eagle::writer;

/**
 * CLI version and usage information.
 */
void cli_usage(char* prog) {
	std::cout << "EAGLe CLI: Command-line tool to convert EAGL .SSH texture banks to .PNG files.\n";
	std::cout << "(C) 2019-2020 modeco80 under the MIT License\n";
	std::cout << "Usage:\n";
	std::cout << "  " << prog << " <input file> <output directory>\n";
	std::cout << "Example:\n";
	std::cout << "  " << prog << " D:\\ssx\\textures\\aloha_ice_jam_sky.ssh D:\\extracted\\aij\n";
}

/**
 * Writer progress function for the CLI.
 */
void cli_progress(std::string progress, ProgressType type) {
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
	if (argc < 3) {
		cli_usage(argv[0]);
		return 1;
	}
	
	std::string input_filename = argv[1];
	std::string output_directory = argv[2];
	std::ifstream stream(input_filename, std::ifstream::in | std::ifstream::binary);

	if(!stream) {
		std::cout << "Could not open file for reading\n";
		return 1;
	}

	ShpsReader reader(stream, input_filename);

	SetProgressFunction(cli_progress);

	try {
		// Read the SHPS header and the image TOC
		// of the texture bank.
		reader.ReadHeader();
		reader.ReadTOC();

		ShpsFileHeader& header = reader.GetHeader();

		std::cout << "SSH Info:" << '\n';
		
		// While we could in theory get the size of the file from the stream itself,
		// the file length member is faster to lookup and should always be correct, considering we verify
		// by getting the size of the file from the stream anyways in ReadHeader().
		std::cout << "Total file size: " << (float)header.FileLength/1000 << " kBytes" << '\n';
		std::cout << "Image count: " << header.FileTextureCount << " files" << '\n';

		// Read every image into the ShpsReader/SHPSCore format
		for(uint32 i = 0; i < header.FileTextureCount; ++i)
			reader.ReadImage(i);
		
		auto& images = reader.GetImages();
		
		// Write every image in the texture bank to a PNG file.
		for(uint32 i = 0; i < header.FileTextureCount; ++i)
			WriteImage(images[i], std::filesystem::path(input_filename), std::filesystem::path(output_directory));

		std::cout << "Finished conversion, cleaning up...\n";

		// Clean up image memory.
		images.clear();
	} catch (std::exception& e) {
		// Exceptions are thrown by ShpsReader to indicate errors.
		std::cout << "Error: " << e.what() << '\n';
		return 1;
	}

	stream.close();
	return 0;
}