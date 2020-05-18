#include <vector>
#include <sstream>
#include "writer.h"
#include "stb_image_write.h"

using namespace eagle::core;

namespace eagle {
namespace writer {

	ProgressFunction Progress;

	void CheckedProgressUpdate(std::string str, ProgressType type) {
		if(Progress)
			Progress(str, type);
	}

	/**
	 * Multiply an stored alpha value or clamp it to 255.
	 *
	 * \param[in] alpha Alpha value to multiply.
	 */
	inline byte MultiplyAlpha(byte alpha) {
		// Multiply the stored alpha by 2 
		// or round it directly up to 255 if it's 128.
		// (Further explaination in ShpsStructs.h)
		// We do this instead of blindly multiplying the alpha value
		// because it could overflow and break images.
		if(alpha < 128)
			return alpha * 2;
		else if(alpha == 128)
			return 255;
		
		// I don't know how you'd ever get beyond the following point
		// but OK
		return 255;
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
						*(pngDataPtr++) = MultiplyAlpha(image.palette[*currentTexPixelPtr].color.a);
						
						currentTexPixelPtr++;
				}
			} break;

			case ShpsImageType::NonLut32Bpp: {
				STREAM_PROGRESS_UPDATE(ProgressType::Info, "Image " << image.index << " is an 32bpp image.")

				byte* pngDataPtr = imageData.data();
				
				// We cast the image data (which is just individual bytes) to ShpsRgba* because
				// non-LUT images directly use ShpsRgba.
				//
				// Also saves a bit of typing, as we won't have to manually
				// advance 4 bytes and cast to ShpsRgba* each and every time.
				// We can just increment the pointer!
				ShpsRgba* texPixelPtr = (ShpsRgba*)image.data.data();

				// Write each pixel to the image buffer that we save.
				for(int i = 0; i < image.width * image.height; ++i) {
						*(pngDataPtr++) = (*texPixelPtr).color.b;
						*(pngDataPtr++) = (*texPixelPtr).color.g;
						*(pngDataPtr++) = (*texPixelPtr).color.r;
						*(pngDataPtr++) = MultiplyAlpha((*texPixelPtr).color.a);
						
						texPixelPtr++;
				}
			} break;

			default:
				break;
			}

			// Finally, write the PNG after we've made the data buffers.
			stbi_write_png(outfilename.c_str(), image.width, image.height, CHANNEL_COUNT, imageData.data(), (image.width * CHANNEL_COUNT));
			STREAM_PROGRESS_UPDATE(ProgressType::Info, "Image " << image.index << " written to \"" << outfilename << "\".")
			outfilename.clear();
			
			// Clear the PNG data buffer after we're done.
			imageData.clear();

			return true;
		}

}
}