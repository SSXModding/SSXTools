#pragma once

#include <eagle/Core.h>
#include <eagle/ShpsStructs.h>

namespace eagle::core {

	// TODO: Is this a internal header?

	enum class CodecResult : byte {
		OK,
		ErrorDecoding,
		ErrorEncoding,
		ErrorInputInvalid
	};

	/**
	 * Gimex codec interface.
	 */
	struct BaseGimexCodec {
		/**
		 * Encode the image data.
		 * 
		 * Doesn't need to be implemented yet
		 */
		virtual std::tuple<CodecResult, std::optional<shps::Image>> Encode(const shps::Image& image) = 0;

		/**
		 * Decode the image data.
		 *
		 * \return CodecResult::OK on success
		 */
		virtual CodecResult Decode(shps::Image& image) = 0;
	};

	/**
	 * Create a GIMEX interleaved shape codec.
	 * @return
	 */
	std::shared_ptr<BaseGimexCodec> MakeInterleavedCodec();

} // namespace eagle::core