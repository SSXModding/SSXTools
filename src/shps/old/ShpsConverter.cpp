#include <vector>
#include <sstream>
#include <ssxtools/shps/ShpsConverter.h>
#include <ssxtools/Core.h>

// TODO: Switch to a real libpng
//  (this will probably be provided by the Qt GUI, as ssxtools-cli was always sort of a test.)
#include "stb_image_write.h"

namespace ssxtools::shps {

		/**
		 * Multiply a stored byte.
		 *
		 * \param[in] val val.
		 */
		inline std::uint8_t MultiplyValue(std::uint8_t val) {
			// Multiply the stored value by 2
			// or round it directly up to 255 if it's 128.
			// We do this instead of blindly multiplying the value
			// because it could overflow and break images.
			if(val < 128)
				return val * 2;
			else if(val == 128) // TODO: is another branch needed? (useless perf improvement..?)
				return 255;

			// I don't know how you'd ever get beyond the following point
			// but OK
			return 255;
		}

		bool ShpsConverter::BuildImageBuffer(std::vector<std::uint8_t>& imageBuffer, Image& image, bool ssxHack) {
			if(image.data.empty()) {
				//logger.error("Image ", image.index, " is empty or unknown format!");
				return false;
			}

			//bool ssxHack = ShouldEnableSSXHack(image);

			//if(ssxHack)
			//	logger.info("Enabling FIXSSH.BAT hack cause alpha was <= 128");

			//logger.info("Writing image ", image.index, "...");

			//logger.info("Information on image ", image.index, ':');
			//logger.info("Width x Height: ", image.width, 'x', image.height);

			imageBuffer.resize((image.width * image.height * ShpsConverter::ChannelCount));

			switch(image.format) {
				case ShapeImageType::Lut128: {
					//logger.info("Image ", image.index, " is an 4bpp image.");
					std::uint8_t* normalizedDataPtr = imageBuffer.data();
					std::uint8_t* texPixelPtr = image.data.data();

					// Write each pixel to the image buffer that we save.
					// We do this by looking in the LUT for each pixel and setting the colors there.
					for(int i = 0; i < image.width * image.height; ++i) {
						int index = 0;

						for(int b = 0; b < 2; b++) {
							index = ((*texPixelPtr & (0x0F, (b * 4))) >> (b * 4));
						}

						*(normalizedDataPtr++) = image.palette[index].b;
						*(normalizedDataPtr++) = image.palette[index].g;
						*(normalizedDataPtr++) = image.palette[index].r;
						*(normalizedDataPtr++) = MultiplyValue(image.palette[index].a);

						texPixelPtr++;
					}
				} break;

				case ShapeImageType::Lut256: {
					//logger.info("Image ", image.index, " is an 8bpp image.");
					std::uint8_t* normalizedDataPtr = imageBuffer.data();

					// Current pixel.
					std::uint8_t* texPixelPtr = image.data.data();

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
							*(normalizedDataPtr++) = MultiplyValue(image.palette[*texPixelPtr].a);
						}

						texPixelPtr++;
					}
				} break;

				case ShapeImageType::NonLut32Bpp: {
					//logger.info("Image ", image.index, " is an 32bpp image.");

					std::uint8_t* normalizedDataPtr = imageBuffer.data();

					// We cast the image data (which is just individual bytes) to Bgra8888* because
					// non-LUT images directly use Bgra8888.
					auto* texPixelPtr = (Bgra8888*)image.data.data();

					// Write each pixel to the image buffer that we save.
					for(int i = 0; i < image.width * image.height; ++i) {
						if(ssxHack) {
							*(normalizedDataPtr++) = MultiplyValue((*texPixelPtr).b);
							*(normalizedDataPtr++) = MultiplyValue((*texPixelPtr).g);
							*(normalizedDataPtr++) = MultiplyValue((*texPixelPtr).r);
							*(normalizedDataPtr++) = MultiplyValue((*texPixelPtr).a);
						} else {
							*(normalizedDataPtr++) = (*texPixelPtr).b;
							*(normalizedDataPtr++) = (*texPixelPtr).g;
							*(normalizedDataPtr++) = (*texPixelPtr).r;
							*(normalizedDataPtr++) = MultiplyValue((*texPixelPtr).a);
						}

						texPixelPtr++;
					}
				} break;

				default:
					break;
			}
			return true;
		}

		// dead code ATM

		bool ShpsConverter::WritePNG(Image& image, const std::filesystem::path& input_path, const std::filesystem::path& output_path) {
			// avoid weird images entirely,
			// helps avoid crashing on certain things
			if(image.data.empty()) {
			//	logger.error("Image ", image.index, " is empty or unknown format!");
				return false;
			}

			auto path = output_path / input_path.filename();
			std::filesystem::create_directories(path);
			std::string outFilename = (path / std::filesystem::path(std::to_string(image.index)).replace_extension(".PNG")).string();

			std::vector<std::uint8_t> imageData;

			// build image buffer
			if(!BuildImageBuffer(imageData, image)) {
				//logger.error("Could not build image buffer...");
			}

			// Finally, write the PNG after we've made the data buffers.
			//stbi_write_png(outFilename.c_str(), image.width, image.height, ShpsConverter::ChannelCount, imageData.data(), (image.width * ShpsConverter::ChannelCount));
			//logger.info("Image ", image.index, " (" , core::EnumToString(image.format), ")", " written to \"", outFilename, "\".");

			// Clear the PNG data buffer after we're done.
			imageData.clear();

			return true;
		}

	} // namespace ssxtools