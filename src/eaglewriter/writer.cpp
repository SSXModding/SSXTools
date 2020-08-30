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

	// TODO replace this with a non inlining garbage fire
	/**
	 * Macro to use a stringstream to compose progress messages.
	 */
#define STREAM_PROGRESS_UPDATE(type, ...) do { std::stringstream ss; ss << __VA_ARGS__; CheckedProgressUpdate(ss.str(), type); ss.clear(); } while(0)

	void SetProgressFunction(ProgressFunction new_func) {
		Progress = new_func;
	}


	/**
	 * Multiply an stored byte.
	 *
	 * \param[in] val val.
	 */
	inline byte MultiplyValue(byte val) {
		// Multiply the stored value by 2 
		// or round it directly up to 255 if it's 128.
		// We do this instead of blindly multiplying the value
		// because it could overflow and break images.
		if(val < 128)
			return val * 2;
		else if(val == 128)
			return 255;
		
		// I don't know how you'd ever get beyond the following point
		// but OK
		return 255;
	}

	/**
	 * Function called **ONCE** on the ShpsImage to determine if it possibly
	 * went through `FIXSSH.BAT`, aka `gx -pixela/2,r/2,g/2,b/2 -csm2 %1`. 
	 * If it did, the hack should be enabled.
	 */
	bool ShouldEnableSSXHack(core::ShpsImage& image) {
		// test
		auto test = [](byte c) {
			return c != 255 && std::max(c, (byte)128) == 128;
		};

		ShpsRgba MaxColor;

		if(image.palette.empty()) {
			// Detour to a much slower function
			// that gets the max pixel in the entire image.
			// This will be far slower than if we had a palette,
			// but it's the only way we can determine it on 32bpp images...
			
			STREAM_PROGRESS_UPDATE(ProgressType::Info, "Oh no");

			// Convert the raw image data...
			auto pixels = ConvertTypes<byte, ShpsRgba>(image.data);

			auto it = std::max_element(pixels.begin(), pixels.end(), [](const ShpsRgba& l, const ShpsRgba& r) {
				return std::max(l.r, r.r) && std::max(l.g, r.g) 
						&& std::max(l.b, r.b) && std::max(l.a, r.a);
			});

			if(it == pixels.end())
				return false; // What

			MaxColor = *it;
		} else {
			// Use a faster method that just gets the max palette.

			auto it = std::max_element(image.palette.begin(), image.palette.end(), [](const ShpsRgba& l, const ShpsRgba& r) {
				return std::max(l.r, r.r) && std::max(l.g, r.g) 
						&& std::max(l.b, r.b) && std::max(l.a, r.a);
			});

			if(it == image.palette.end())
				return false; // What

			MaxColor = *it;
		}

		// debug stuff:tm:
		STREAM_PROGRESS_UPDATE(ProgressType::Info, "Color: R: " << (int)MaxColor.r << " G: " << (int)MaxColor.g << " B: " << (int)MaxColor.b << " A: " << (int)MaxColor.a);

		// seems the best way to detect this hack is to test just the alpha
		return test(MaxColor.a);
	}

	// Constant amount of channels in the output PNG.
	constexpr int32 CHANNEL_COUNT = 4;

	// build a image buffer that is in a "standard"
	// format (that something can read)
	bool BuildImageBuffer(std::vector<byte>& imageBuffer, core::ShpsImage& image) {
			if(image.data.empty()) {
				STREAM_PROGRESS_UPDATE(ProgressType::Error, "Image " << image.index << " is in a unknown format or is empty.");
				return false;
			}

			bool ssxHack = ShouldEnableSSXHack(image);
			
			if(ssxHack)
				STREAM_PROGRESS_UPDATE(ProgressType::Info, "Enabling FIXSSH.BAT hack cause alpha was <= 128");

			STREAM_PROGRESS_UPDATE(ProgressType::Info, "Writing image " << image.index << "...");
			
			STREAM_PROGRESS_UPDATE(ProgressType::Info, "Information on image " << image.index << ':');
			STREAM_PROGRESS_UPDATE(ProgressType::Info, "Width x Height: " << image.width << 'x' << image.height);

			imageBuffer.resize((image.width * image.height * CHANNEL_COUNT));

			switch(image.format) {

			case ShpsImageType::Lut128: {
				STREAM_PROGRESS_UPDATE(ProgressType::Info, "Image " << image.index << " is an 4bpp image.");
				byte* normalizedDataPtr = imageBuffer.data();

				// Current pixel.
				byte* texPixelPtr = image.data.data();

				// Write each pixel to the image buffer that we save.
				// We do this by looking in the LUT for each pixel and setting the colors there.
				for (int i = 0; i < image.width * image.height; ++i) {
					int index = 0;

					for (int b = 0; b < 2; b++) {

						 index = ((*texPixelPtr & (0x0F << (b * 4))) >> (b * 4));
					}

					*(normalizedDataPtr++) = image.palette[index].b;
					*(normalizedDataPtr++) = image.palette[index].g;
					*(normalizedDataPtr++) = image.palette[index].r;
					*(normalizedDataPtr++) = MultiplyValue(image.palette[index].a);


					texPixelPtr++;
				}
			} break;

			case ShpsImageType::Lut256: {
				STREAM_PROGRESS_UPDATE(ProgressType::Info, "Image " << image.index << " is an 8bpp image.");
				byte* normalizedDataPtr = imageBuffer.data();

				// Current pixel.
				byte* texPixelPtr = image.data.data();

				// Write each pixel to the image buffer that we save.
				// We do this by looking in the LUT for each pixel and setting the colors there.
				for(int i = 0; i < image.width * image.height; ++i) {
					if(ssxHack) {
						*(normalizedDataPtr++) = MultiplyValue(image.palette[*texPixelPtr].b);
						*(normalizedDataPtr++) = MultiplyValue(image.palette[*texPixelPtr].g);
						*(normalizedDataPtr++) = MultiplyValue(image.palette[*texPixelPtr].r);
						*(normalizedDataPtr++) = MultiplyValue(image.palette[*texPixelPtr].a);
					} else {
						*(normalizedDataPtr++) = image.palette[*texPixelPtr].b;
						*(normalizedDataPtr++) = image.palette[*texPixelPtr].g;
						*(normalizedDataPtr++) = image.palette[*texPixelPtr].r;
						*(normalizedDataPtr++) = image.palette[*texPixelPtr].a;
					}
						
					texPixelPtr++;
				}
			} break;

			case ShpsImageType::NonLut32Bpp: {
				STREAM_PROGRESS_UPDATE(ProgressType::Info, "Image " << image.index << " is an 32bpp image.");

				byte* normalizedDataPtr = imageBuffer.data();
				
				// We cast the image data (which is just individual bytes) to ShpsRgba* because
				// non-LUT images directly use ShpsRgba.
				//
				// Also saves a bit of typing, as we won't have to manually
				// advance 4 bytes and cast to ShpsRgba* each and every time.
				// We can just increment the pointer!
				ShpsRgba* texPixelPtr = (ShpsRgba*)image.data.data();

				// Write each pixel to the image buffer that we save.
				for(int i = 0; i < image.width * image.height; ++i) {
					if(ssxHack) {
						STREAM_PROGRESS_UPDATE(ProgressType::Info, "Billbert mays here" << i);
						*(normalizedDataPtr++) = MultiplyValue((*texPixelPtr).b);
						*(normalizedDataPtr++) = MultiplyValue((*texPixelPtr).g);
						*(normalizedDataPtr++) = MultiplyValue((*texPixelPtr).r);
						*(normalizedDataPtr++) = MultiplyValue((*texPixelPtr).a);
					} else {
						*(normalizedDataPtr++) = (*texPixelPtr).b;
						*(normalizedDataPtr++) = (*texPixelPtr).g;
						*(normalizedDataPtr++) = (*texPixelPtr).r;
						*(normalizedDataPtr++) = (*texPixelPtr).a;
					}

					texPixelPtr++;
				}
			} break;

			default:
				break;
			}
	}

	bool WriteImage(core::ShpsImage& image, const std::filesystem::path& input_path, const std::filesystem::path& output_path) {
			auto path = output_path / input_path.filename();
			std::filesystem::create_directories(path);
			std::string outFilename = (path / std::filesystem::path(std::to_string(image.index)).replace_extension(".PNG")).string();

			std::vector<byte> imageData;

			// build image buffer
			BuildImageBuffer(imageData, image);
			
			// Finally, write the PNG after we've made the data buffers.
			stbi_write_png(outFilename.c_str(), image.width, image.height, CHANNEL_COUNT, imageData.data(), (image.width * CHANNEL_COUNT));
			STREAM_PROGRESS_UPDATE(ProgressType::Info, "Image " << image.index << " written to \"" << outFilename << "\".");
			
			// Clear the PNG data buffer after we're done.
			imageData.clear();

			return true;
		}

}
}