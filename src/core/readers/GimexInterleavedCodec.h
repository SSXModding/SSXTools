#pragma once

#include "../../../include/eagle/Core.h"

#include "GimexCodec.h"

namespace eagle::core {

	/**
	 * GIMEX 3 interleaved codec
	 */
	struct InterleavedCodec : public BaseGimexCodec {
		std::vector<byte> Encode(const shps::Image& image);

		std::vector<byte> Decode(const shps::Image& image);
	};
} // namespace eagle::core