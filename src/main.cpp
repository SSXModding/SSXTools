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
// I want to be able to make a GUI tool with a selector and stuff for images in a bank
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
			
			// Constant amount of channels in the output PNG.
			constexpr int32 CHANNEL_COUNT = 4;

			std::string sshname = filename;
			sshname.replace(sshname.find_first_of(".SSH"), sshname.find_first_of(".SSH") - sshname.length(), "");

			// TODO: compose a path with std::filesystem instead of this garbage
			std::string outfilename = sshname + "_" + std::to_string(i) + ".png";
			std::vector<byte> imageData;

			if(image.data.empty()) {
				std::cout << "Texture " << i << " is in an unsupported format or has no data, skipping\n";
				continue;
			}

			std::cout << "Dumping texture " << i << "...\n";

			std::cout << "Texture " << i << " information:\n";
			std::cout << "WxH: " << image.width << 'x' << image.height << '\n';

			imageData.resize(image.width * image.height * CHANNEL_COUNT);

			switch(image.format) {

			case ShpsImageType::Lut256: {
				std::cout << "Image is an 8bpp image\n";
				byte* ptr = imageData.data();
				byte* texel = image.data.data();

				// Write each pixel to the image buffer that we save.
				for(int i = 0; i < image.width * image.height; ++i) {
						*(ptr++) = image.palette[*texel].color.b;
						*(ptr++) = image.palette[*texel].color.g;
						*(ptr++) = image.palette[*texel].color.r;

						byte alpha = image.palette[*texel].color.a;

						// Multiply the stored alpha by 2 
						// or round it up to 255 if it's 128.
						// (Further explaination in ShpsStructs.h)
						// We do this instead of blindly multiplying the alpha value
						// because it could overflow and break images.
						if(alpha < 128)
							alpha *= 2;
						else if(alpha == 128)
							alpha = 255;

						*(ptr++) = alpha;
						texel++;
				}
			} break;

			default:
				break;
			}

			// Write the PNG.
			stbi_write_png(outfilename.c_str(), image.width, image.height, CHANNEL_COUNT, imageData.data(), (image.width * CHANNEL_COUNT));
			std::cout << "Output PNG written to " << outfilename << ".\n";
			imageData.clear();
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
