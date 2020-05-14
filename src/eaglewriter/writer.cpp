#include <vector>
#include <sstream>
#include "writer.h"
#include "stb_image_write.h"

using namespace eagle::core;

namespace eagle {
namespace writer {

	ProgressFunction Progress;

	void CheckedProgressUpdate(std::string& str, ProgressType type) {
		if(Progress)
			Progress(str, type);
	}

	/**
	 * Macro to use a stringstream to compose progress messages.
	 */
#define STREAM_PROGRESS_UPDATE(type, ...) { std::stringstream ss; ss << __VA_ARGS__; CheckedProgressUpdate(ss.str(), type); ss.clear(); }

	void SetProgressFunction(ProgressFunction new_func) {
		Progress = new_func;
	}

	bool WriteImage(ShpsImage& image, const std::string& filename) {
			// Constant amount of channels in the output PNG.
			constexpr int32 CHANNEL_COUNT = 4;


			std::string sshname = filename;
			sshname.replace(sshname.find_first_of(".SSH"), sshname.find_first_of(".SSH") - sshname.length(), "");

			// TODO: compose a path with std::filesystem instead of this garbage
			std::string outfilename = sshname + "_" + std::to_string(image.index) + ".png";
			std::vector<byte> imageData;

			if(image.data.empty()) {
				STREAM_PROGRESS_UPDATE(ProgressType::Error, "Image " << image.index << " is in a unknown format or is empty.")
				return false;
			}

			STREAM_PROGRESS_UPDATE(ProgressType::Info, "Writing image " << image.index << "...")
			
			STREAM_PROGRESS_UPDATE(ProgressType::Info, "Information on image " << image.index << ':')
			STREAM_PROGRESS_UPDATE(ProgressType::Info, "Width x Height: " << image.width << 'x' << image.height)

			imageData.resize(image.width * image.height * CHANNEL_COUNT);

			switch(image.format) {

			case ShpsImageType::Lut256: {
				STREAM_PROGRESS_UPDATE(ProgressType::Info, "Image " << image.index << " is an 8bpp image.")
				byte* pngDataPtr = imageData.data();
				byte* currentTexPixelPtr = image.data.data();

				// Write each pixel to the image buffer that we save.
				for(int i = 0; i < image.width * image.height; ++i) {
						*(pngDataPtr++) = image.palette[*currentTexPixelPtr].color.b;
						*(pngDataPtr++) = image.palette[*currentTexPixelPtr].color.g;
						*(pngDataPtr++) = image.palette[*currentTexPixelPtr].color.r;

						byte alpha = image.palette[*currentTexPixelPtr].color.a;

						// Multiply the stored alpha by 2 
						// or round it up to 255 if it's 128.
						// (Further explaination in ShpsStructs.h)
						// We do this instead of blindly multiplying the alpha value
						// because it could overflow and break images.
						if(alpha < 128)
							alpha *= 2;
						else if(alpha == 128)
							alpha = 255;

						*(pngDataPtr++) = alpha;
						currentTexPixelPtr++;
				}
			} break;

			default:
				break;
			}

			// Write the PNG.
			stbi_write_png(outfilename.c_str(), image.width, image.height, CHANNEL_COUNT, imageData.data(), (image.width * CHANNEL_COUNT));
			STREAM_PROGRESS_UPDATE(ProgressType::Info, "Image " << image.index << " written to \"" << outfilename << "\".")
			outfilename.clear();
			imageData.clear();

			return true;
		}

}
}