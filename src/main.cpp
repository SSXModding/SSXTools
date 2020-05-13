#include <iostream>
#include <exception>
#include <fstream>
#include <string>
#include "ShpsReader.h"

#include "stb_image_write.h"


using namespace eagle::core;

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

	ShpsReader reader(stream, filename);

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

			std::cout << "Dumping texture " << i << "...\n";

			std::cout << "Texture " << i << " information:\n";
			std::cout << "WxH: " << image.width << 'x' << image.height << '\n';

			if(image.format == eagle::core::ShpsImageType::Lut256) {
				std::cout << "Image is an 8bpp image\n";
			}

			constexpr int32 CHANNEL_COUNT = 3;

			std::string sshname = filename;
			sshname.replace(sshname.find_first_of(".SSH"), sshname.find_first_of(".SSH") - sshname.length(), "");

			// TODO: compose a path with std::filesystem instead of this garbage
			std::string outfilename = sshname + "_" + std::to_string(i) + ".png";
			std::vector<byte> imageData;

			imageData.resize(image.width * image.height * CHANNEL_COUNT);

			// Specific to lut256 images
			if(image.format == ShpsImageType::Lut256) {
				byte* ptr = imageData.data();
				byte* texel = image.data.data();

				// Write the data to the image data we are going to write to the PNG
				for(int i = 0; i < image.width * image.height; ++i) {
						*(ptr++) = image.palette[*(texel)].color.b;
						*(ptr++) = image.palette[*(texel)].color.g;
						*(ptr++) = image.palette[*(texel)].color.r;
						texel++;
				}	
			}

			// Write the PNG.
			stbi_write_png(outfilename.c_str(), image.width, image.height, 3, imageData.data(), image.width * CHANNEL_COUNT);
			std::cout << "PNG written at " << outfilename << ".\n";
			imageData.clear();
		}

		std::cout << "Finished extraction, cleaning up...\n";

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
