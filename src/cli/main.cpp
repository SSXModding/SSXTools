#include <iostream>
#include <exception>
#include <fstream>
#include <string>
#include <filesystem>

#include <modeco/Logger.h>
#include <modeco/IostreamLoggerSink.h>

#include <ssxtools/shps/ShpsReader.h>
#include <ssxtools/shps/ShpsConverter.h>

using namespace ssxtools::core;

/**
 * CLI version and usage information.
 */
void cli_usage(char* prog) {
	std::cout << "EAGLe CLI: Command-line tool to convert Gimex .SSH shape files to .PNG files.\n";
	std::cout << "(C) 2019-2020 modeco80 under the MIT License\n";
	std::cout << "Usage:\n";
	std::cout << "  " << prog << " <input file> <output directory>\n";
	std::cout << "Example:\n";
	std::cout << "  " << prog << " D:\\ssx\\textures\\aloha_ice_jam_sky.ssh D:\\extracted\\aij\n";
}

static mco::IostreamLoggerSink logger_sink;
mco::Logger logger = mco::Logger::CreateLogger("EAGLe");

int main(int argc, char** argv) {
	if(argc < 3) {
		cli_usage(argv[0]);
		return 1;
	}

	mco::Logger::SetSink(&logger_sink);

	std::string input_filename = argv[1];
	std::string output_directory = argv[2];
	std::ifstream stream(input_filename, std::ifstream::in | std::ifstream::binary);

	if(!stream) {
		logger.error("Could not open file ", input_filename, " for reading.");
		return 1;
	}

	ShpsReader reader(stream, input_filename);

	// Read the SHPS header and the image TOC
	// of the texture bank.
	if(!reader.ReadHeader()) {
		logger.error("Invalid SHPS header!");
		return 1;
	}
	reader.ReadTOC();
	shps::FileHeader& header = reader.GetHeader();

	logger.info("Shape File Information: ");

	// While we could in theory get the size of the file from the stream itself,
	// the file length member is faster to lookup and should always be correct, considering we verify
	// by getting the size of the file from the stream anyways in ReadHeader().

	logger.info("  Total File Size:", (float)header.FileLength / 1000, " kB");
	logger.info("  Image Count:", header.FileTextureCount, " files");

	// Read every image into the ShpsReader/shps format
	for(uint32 i = 0; i < header.FileTextureCount; ++i)
		reader.ReadImage(i);

	auto& images = reader.GetImages();

	ShpsConverter writer;

	// Write every image in the texture bank to a PNG file.
	for(uint32 i = 0; i < header.FileTextureCount; ++i)
		writer.WritePNG(images[i], std::filesystem::path(input_filename), std::filesystem::path(output_directory));

	// Clean up image memory.
	// TODO: won't RAII take care of this?
	images.clear();

	return 0;
}