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
		inline core::byte MultiplyValue(core::byte val) {
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

		// TODO: we should only detour to this if the user doesn't give an option
		// whether in the UI (i don't know where I'd put this) or in the cli (say --disable-ssxhack or --enable-ssxhack)
		/**
		 * Function called **ONCE** on the shps::Image to determine if it possibly
		 * went through the fabled
		 * `FIXSSH.BAT` (the Winter Jampack 2001 build of SSX Tricky accidentally shipped with this)
		 * aka `gx -pixela/2,r/2,g/2,b/2 -csm2 %1`.
		 * If it did, the hack should be enabled.
		 */
		bool ShouldEnableSSXHack(shps::Image& image) {
			// test
			auto test = [](core::byte c) {
				return c != 255 && c != 0 /* Seems to fix some other textures */ && std::max(c, (core::byte)128) == 128;
			};

			shps::Bgra8888 MaxColor{};

			if(image.palette.empty()) {
				// Detour to a much slower impl
				// that gets the max pixel in the entire image.
				// This will be far slower than if we had a palette,
				// but it's the only way we can determine it on 32bpp images.

				MaxColor = core::MaxSpanElement(mco::MakeSpan((Bgra8888*)image.data.data(), image.width * image.height), [](const Bgra8888& l, const Bgra8888& r) {
					return std::max(l.r, r.r) && std::max(l.g, r.g) && std::max(l.b, r.b) && std::max(l.a, r.a);
				});
			} else {
				// Use a faster method that just gets the max palette.

				MaxColor = core::MaxSpanElement(mco::MakeSpan(image.palette.data(), image.palette.size()), [](const Bgra8888& l, const Bgra8888& r) {
					return std::max(l.r, r.r) && std::max(l.g, r.g) && std::max(l.b, r.b) && std::max(l.a, r.a);
				});

			}

			// TODO: Fix this hack detection,
			// or have the user explicitly enable it.

			return false;
			// seems the best way to detect this hack is to test just the alpha
			return test(MaxColor.a);
		}

		bool ShpsConverter::BuildImageBuffer(std::vector<core::byte>& imageBuffer, Image& image, bool ssxHack) {
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
					core::byte* normalizedDataPtr = imageBuffer.data();
					core::byte* texPixelPtr = image.data.data();

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
					core::byte* normalizedDataPtr = imageBuffer.data();

					// Current pixel.
					core::byte* texPixelPtr = image.data.data();

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

					core::byte* normalizedDataPtr = imageBuffer.data();

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
				logger.error("Image ", image.index, " is empty or unknown format!");
				return false;
			}

			auto path = output_path / input_path.filename();
			std::filesystem::create_directories(path);
			std::string outFilename = (path / std::filesystem::path(std::to_string(image.index)).replace_extension(".PNG")).string();

			std::vector<core::byte> imageData;

			// build image buffer
			if(!BuildImageBuffer(imageData, image)) {
				logger.error("Could not build image buffer...");
			}

			// Finally, write the PNG after we've made the data buffers.
			stbi_write_png(outFilename.c_str(), image.width, image.height, ShpsConverter::ChannelCount, imageData.data(), (image.width * ShpsConverter::ChannelCount));
			logger.info("Image ", image.index, " (" , core::EnumToString(image.format), ")", " written to \"", outFilename, "\".");

			// Clear the PNG data buffer after we're done.
			imageData.clear();

			return true;
		}

	} // namespace ssxtools