#include <eagle/Core.h>
#include "GimexCodec.h"

namespace eagle::core {

	// local coord
	struct Coord {
		uint16 x;
		uint16 y;
	};

	// todo: There's probably a way we can do away with this
	// clang-format off
		const static std::vector<std::tuple<Coord, Coord>> DecodeMap { // NOLINT
			{ { 0, 0 }, { 0, 0 } },
			{ { 0, 1 }, { 0, 4 } },
			{ { 0, 2 }, { 0, 8 } },
			{ { 0, 3 }, { 0, 12 } },
			{ { 0, 4 }, { 0, 16 } },
			{ { 0, 5 }, { 0, 20 } },
			{ { 0, 6 }, { 0, 24 } },
			{ { 0, 7 }, { 0, 28 } },
			{ { 1, 0 }, { 2, 0 } },
			{ { 1, 1 }, { 2, 4 } },
			{ { 1, 2 }, { 2, 8 } },
			{ { 1, 3 }, { 2, 12 } },
			{ { 1, 4 }, { 2, 16 } },
			{ { 1, 5 }, { 2, 20 } },
			{ { 1, 6 }, { 2, 24 } },
			{ { 1, 7 }, { 2, 28 } },
			{ { 2, 0 }, { 0, 17 } },
			{ { 2, 1 }, { 0, 21 } },
			{ { 2, 2 }, { 0, 25 } },
			{ { 2, 3 }, { 0, 29 } },
			{ { 2, 4 }, { 0, 1 } },
			{ { 2, 5 }, { 0, 5 } },
			{ { 2, 6 }, { 0, 9 } },
			{ { 2, 7 }, { 0, 13 } },
			{ { 3, 0 }, { 2, 17 } },
			{ { 3, 1 }, { 2, 21 } },
			{ { 3, 2 }, { 2, 25 } },
			{ { 3, 3 }, { 2, 29 } },
			{ { 3, 4 }, { 2, 1 } },
			{ { 3, 5 }, { 2, 5 } },
			{ { 3, 6 }, { 2, 9 } },
			{ { 3, 7 }, { 2, 13 } },
			{ { 4, 0 }, { 4, 16 } },
			{ { 4, 1 }, { 4, 20 } },
			{ { 4, 2 }, { 4, 24 } },
			{ { 4, 3 }, { 4, 28 } },
			{ { 4, 4 }, { 4, 0 } },
			{ { 4, 5 }, { 4, 4 } },
			{ { 4, 6 }, { 4, 8 } },
			{ { 4, 7 }, { 4, 12 } },
			{ { 5, 0 }, { 6, 16 } },
			{ { 5, 1 }, { 6, 20 } },
			{ { 5, 2 }, { 6, 24 } },
			{ { 5, 3 }, { 6, 28 } },
			{ { 5, 4 }, { 6, 0 } },
			{ { 5, 5 }, { 6, 4 } },
			{ { 5, 6 }, { 6, 8 } },
			{ { 5, 7 }, { 6, 12 } },
			{ { 6, 0 }, { 4, 1 } },
			{ { 6, 1 }, { 4, 5 } },
			{ { 6, 2 }, { 4, 9 } },
			{ { 6, 3 }, { 4, 13 } },
			{ { 6, 4 }, { 4, 17 } },
			{ { 6, 5 }, { 4, 21 } },
			{ { 6, 6 }, { 4, 25 } },
			{ { 6, 7 }, { 4, 29 } },
			{ { 7, 0 }, { 6, 1 } },
			{ { 7, 1 }, { 6, 5 } },
			{ { 7, 2 }, { 6, 9 } },
			{ { 7, 3 }, { 6, 13 } },
			{ { 7, 4 }, { 6, 17 } },
			{ { 7, 5 }, { 6, 21 } },
			{ { 7, 6 }, { 6, 25 } },
			{ { 7, 7 }, { 6, 29 } }
		};
	// clang-format on


	constexpr std::size_t GetIndexForCoord(const shps::Image& image, Coord location) {
		return location.y * image.width + location.x;
	}

	Coord FindCoordInDecodeMap(Coord encoded) {
		for(auto [lv, rv] : DecodeMap) {
				if(encoded.x == lv.x && encoded.y == lv.y)
					return rv;
		}
		return {};
	}

	byte GetDecodedPixel(const shps::Image& image, Coord location) {
		//auto decoded = FindCoordInDecodeMap()
	}

	/**
	 * GIMEX 3 interleaved codec
	 */
	struct InterleavedCodec : public BaseGimexCodec {
			std::tuple<CodecResult, std::optional<shps::Image>> Encode(const shps::Image& image) override {
				// currently unsupported
				// TODO i should implement when EAGLe gets writing...
				return std::make_tuple(CodecResult::OK, shps::Image(image));
			}

			CodecResult Decode(shps::Image& image) override {
				// Bail if the image is not Lut256,
				// cause 32-bpp G3 shapes are not interleaved.

				if(image.format != shps::ShpsImageType::Lut256)
					return CodecResult::ErrorDecoding;

				// We intentionally make a copy of the image we are given,
				// so if something breaks
				shps::Image copy{image};

				for(int x = 0; x < image.width; ++x) {
					for(int y = 0; y < image.height; ++y) {
						const Coord c = {(uint16)x, (uint16)y};
						copy.data[GetIndexForCoord(image, c)] = GetDecodedPixel(image, c);
					}
				}

				return CodecResult::OK;
			}
	};

	// moved this to a factory style thing cause its cleaner that way

	std::shared_ptr<BaseGimexCodec> MakeInterleavedCodec() {
		return std::make_shared<InterleavedCodec>();
	}

} // namespace eagle::core